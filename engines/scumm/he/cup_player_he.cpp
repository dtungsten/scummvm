/* ScummVM - Scumm Interpreter
 * Copyright (C) 2006 The ScummVM project
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.

 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 *
 * $URL$
 * $Id$
 *
 */

#include "common/stdafx.h"
#include "common/system.h"
#include "sound/mixer.h"
#include "scumm/scumm.h"
#include "scumm/util.h"
#include "scumm/he/intern_he.h"
#include "scumm/he/cup_player_he.h"

namespace Scumm {

bool CUP_Player::open(const char *filename) {
	bool opened = false;
	debug(1, "opening '%s'", filename);
	if (_fd.open(filename)) {
		uint32 tag = _fd.readUint32BE();
		_fd.readUint32BE();
		if (tag == MKID_BE('BEAN')) {
			_playbackRate = 66;
			_width = 640;
			_height = 480;
			_offscreenBuffer = (uint8 *)malloc(_width * _height);
			memset(_offscreenBuffer, 0, _width * _height);
			_paletteChanged = false;
			_sfxCount = 0;
			_sfxBuffer = 0;
			_sfxHandleTable = 0;
			memset(_sfxQueue, 0, sizeof(_sfxQueue));
			_sfxQueuePos = 0;
			parseHeaderTags();
			debug(1, "rate %d width %d height %d", _playbackRate, _width, _height);
			opened = true;
		}
	}
	return opened;
}

void CUP_Player::close() {
	_fd.close();
	free(_offscreenBuffer);
	free(_sfxBuffer);
	delete[] _sfxHandleTable;
}

uint32 CUP_Player::loadNextChunk() {
	uint32 tag = _fd.readUint32BE();
	uint32 size = _fd.readUint32BE() - 8;
	if (_currentChunkSize < size) {
		free(_currentChunkData);
		_currentChunkSize = 0;
		_currentChunkData = (uint8 *)malloc(size);
	}
	if (_currentChunkData) {
		_currentChunkSize = size;
		_fd.read(_currentChunkData, _currentChunkSize);
	}
	return tag;
}

void CUP_Player::parseHeaderTags() {
	_dataSize = 0;
	while (_dataSize == 0 && !_vm->_quit && !_fd.ioFailed()) {
		uint32 tag = loadNextChunk();
		switch (tag) {
		case MKID_BE('HEAD'):
			handleHEAD(_currentChunkData, _currentChunkSize);
			break;
		case MKID_BE('SFXB'):
			handleSFXB(_currentChunkData, _currentChunkSize);
			break;
		case MKID_BE('RGBS'):
			handleRGBS(_currentChunkData, _currentChunkSize);
			break;
		case MKID_BE('DATA'):
			_dataSize = _currentChunkSize;
			break;
		case MKID_BE('GFXB'):
			// this is never triggered
		default:
			warning("unhandled tag %s", tag2str(tag));
			break;
		}
	}
}

void CUP_Player::play() {
	int ticks = _system->getMillis();
	while (_currentChunkSize != 0 && !_vm->_quit) {
		uint32 tag, size;
		parseNextTag(_currentChunkData, tag, size);
		if (tag == MKID_BE('BLOK')) {
			bool fastMode = false;
			int diff = _system->getMillis() - ticks;
			if (diff >= 0 && diff <= _playbackRate && !fastMode) {
				_system->delayMillis(_playbackRate - diff);
			} else {
				_system->delayMillis(1);
			}
			updateSfx();
			updateScreen();
			_vm->parseEvents();

			ticks = _system->getMillis();
			size = 8;
		}
		_currentChunkData += size;
		_currentChunkSize -= size;
	}
}

void CUP_Player::setDirtyScreenRect(const Common::Rect &r) {
	const uint8 *src = _offscreenBuffer + r.top * _width + r.left;
	_system->copyRectToScreen(src, _width, r.left, r.top, r.width() + 1, r.height() + 1);
}

void CUP_Player::updateScreen() {
	if (_paletteChanged) {
		_system->setPalette(_paletteData, 0, 256);
		_paletteChanged = false;
	}
	_system->updateScreen();
}

void CUP_Player::updateSfx() {
	for (int i = 0; i < _sfxQueuePos; ++i) {
		const CUP_Sfx *sfx = &_sfxQueue[i];
		int index;
		if (sfx->num == -1) {
			for (index = 0; index < _sfxCount; ++index) {
				if (_mixer->isSoundHandleActive(_sfxHandleTable[index])) {
					break;
				}
			}
			if (index == _sfxCount) {
				continue;
			}
		} else {
			index = sfx->num - 1;
		}
		assert(index >= 0 && index < _sfxCount);
		if (!_mixer->isSoundHandleActive(_sfxHandleTable[index]) || (sfx->mode & 2) != 0) {
			if ((sfx->flags & 0x8000) == 0) {
				warning("Unhandled Sfx looping");
				continue;
			}
			uint32 offset = READ_LE_UINT32(_sfxBuffer + index * 4) - 8;
			uint8 *soundData = _sfxBuffer + offset;
			if (READ_BE_UINT32(soundData) == MKID_BE('DATA')) {
				uint32 soundSize = READ_BE_UINT32(soundData + 4);
				_mixer->playRaw(&_sfxHandleTable[index], soundData + 8, soundSize - 8, 11025, Audio::Mixer::FLAG_UNSIGNED);
			}
		}
	}
	_sfxQueuePos = 0;
}

void CUP_Player::parseNextTag(const uint8 *data, uint32 &tag, uint32 &size) {
	tag = READ_BE_UINT32(data);
	size = READ_BE_UINT32(data + 4);
	data += 8;
	switch (tag) {
	case MKID_BE('FRAM'):
		handleFRAM(_offscreenBuffer, data, size);
		break;
	case MKID_BE('LZSS'):
		data = handleLZSS(data, size);
		if (data) {
			uint32 t, s;
			parseNextTag(data, t, s);
		}
		break;
	case MKID_BE('RATE'):
		handleRATE(data, size);
		break;
	case MKID_BE('RGBS'):
		handleRGBS(data, size);
		break;
	case MKID_BE('SNDE'):
		handleSNDE(data, size);
		break;
	case MKID_BE('TOIL'):
		handleTOIL(data, size);
		break;
	case MKID_BE('BLOK'):
		// not handled here
		break;
	case MKID_BE('SRLE'):
		handleSRLE(_offscreenBuffer, data, size);
		break;
	case MKID_BE('WRLE'):
		// this is never triggered
	default:
		warning("unhandled tag %s", tag2str(tag));
		break;
	}
}

void CUP_Player::handleHEAD(const uint8 *data, uint32 dataSize) {
	_playbackRate = READ_LE_UINT16(data);
	_width = READ_LE_UINT16(data + 2);
	_height = READ_LE_UINT16(data + 4);
}

void CUP_Player::handleSFXB(const uint8 *data, uint32 dataSize) {
	if (dataSize > 16) { // WRAP and OFFS chunks
		if (READ_BE_UINT32(data) == MKID_BE('WRAP')) {
			data += 8;
			if (READ_BE_UINT32(data) == MKID_BE('OFFS')) {
				_sfxCount = (READ_BE_UINT32(data + 4) - 8) / 4;
				_sfxBuffer = (uint8 *)malloc(dataSize - 16);
				if (_sfxBuffer) {
					memcpy(_sfxBuffer, data + 8, dataSize - 16);
				}
				_sfxHandleTable = new Audio::SoundHandle[_sfxCount];
			}
		}
	}
}

void CUP_Player::handleRGBS(const uint8 *data, uint32 dataSize) {
	int i;
	uint8 *ptr = _paletteData;

	for (i = 0; i < 256; i++) {
		*ptr++ = *data++;
		*ptr++ = *data++;
		*ptr++ = *data++;
		*ptr++ = 0;
	}
	_paletteChanged = true;
}

void CUP_Player::handleFRAM(uint8 *dst, const uint8 *data, uint32 size) {
	const uint8 flags = *data++;
	int type = 256;
	if (flags & 1) {
		type = *data++;
	}
	Common::Rect r;
	if (flags & 2) {
		r.left   = READ_LE_UINT16(data); data += 2;
		r.top    = READ_LE_UINT16(data); data += 2;
		r.right  = READ_LE_UINT16(data); data += 2;
		r.bottom = READ_LE_UINT16(data); data += 2;
	}
	if (flags & 0x80) {
		decodeFRAM(dst, r, data, type);
		setDirtyScreenRect(r);
	}
}

void CUP_Player::decodeFRAM(uint8 *dst, Common::Rect &dstRect, const uint8 *data, int type) {
	if (type == 256) {
		dst += dstRect.top * _width + dstRect.left;
		int h = dstRect.bottom - dstRect.top + 1;
		int w = dstRect.right - dstRect.left + 1;
		while (h--) {
			uint16 lineSize = READ_LE_UINT16(data); data += 2;
			uint8 *dstNextLine = dst + _width;
			const uint8 *dataNextLine = data + lineSize;
			if (lineSize != 0) {
				uint8 *dstEnd = dst + w;
				while (dst < dstEnd) {
					int code = *data++;
					if (code & 1) { // skip
						code >>= 1;
						dst += code;
					} else if (code & 2) { // set
						code = (code >> 2) + 1;
						const int sz = MIN<int>(code, dstEnd - dst);
						memset(dst, *data++, sz);
						dst += sz;
					} else { // copy
						code = (code >> 2) + 1;
						const int sz = MIN<int>(code, dstEnd - dst);
						memcpy(dst, data, sz);
						dst += sz;
						data += sz;
					}
				}
			}
			dst = dstNextLine;
			data = dataNextLine;
		}
	}
}

void CUP_Player::handleSRLE(uint8 *dst, const uint8 *data, uint32 size) {
	Common::Rect r;
	r.left   = READ_LE_UINT16(data); data += 2;
	r.top    = READ_LE_UINT16(data); data += 2;
	r.right  = READ_LE_UINT16(data); data += 2;
	r.bottom = READ_LE_UINT16(data); data += 2;
	const uint8 *colorMap = data;
	int unpackedSize = READ_LE_UINT32(data + 32);
	decodeSRLE(dst, colorMap, data + 36, unpackedSize);
	setDirtyScreenRect(r);
}

void CUP_Player::decodeSRLE(uint8 *dst, const uint8 *colorMap, const uint8 *data, int unpackedSize) {
	while (unpackedSize > 0) {
		int size, code = *data++;
		if ((code & 1) == 0) {
			if ((code & 2) == 0) {
				size = (code >> 2) + 1;
				dst += size;
				unpackedSize -= size;
			} else {
				if ((code & 4) == 0) {
					*dst++ = colorMap[code >> 3];
					--unpackedSize;
				} else {
					code >>= 3;
					if (code == 0) {
						size = 1 + *data++;
					} else {
						size = code;
					}
					memset(dst, *data++, MIN(unpackedSize, size));
					dst += size;
					unpackedSize -= size;
				}
			}
		} else {
			code >>= 1;
			if (code == 0) {
				code = 1 + READ_LE_UINT16(data); data += 2;
			}
			dst += code;
			unpackedSize -= code;
		}
	}
}

uint8 *CUP_Player::handleLZSS(const uint8 *data, uint32 dataSize) {
	uint32 compressionType = 0;
	uint32 compressionSize = 0;
	uint32 tag, size;

	tag = READ_BE_UINT32(data);
	size = READ_BE_UINT32(data + 4);
	if (tag == MKID_BE('LZHD')) {
		compressionType = READ_LE_UINT32(data + 8);
		compressionSize = READ_LE_UINT32(data + 12);
	}
	data += size;

	tag = READ_BE_UINT32(data);
	size = READ_BE_UINT32(data + 4);
	if (tag == MKID_BE('DATA')) {
		if (compressionType == 0x2000) {
			if (_bufferLzssSize < compressionSize) {
				_bufferLzssSize = compressionSize;
				free(_bufferLzssData);
				_bufferLzssData = (uint8 *)malloc(_bufferLzssSize);
			}
			data += 8;
			uint32 offset1 = READ_LE_UINT32(data);
			uint32 offset2 = READ_LE_UINT32(data + 4);
			decodeLzssData(_bufferLzssData, data + 8, data + offset1, data + offset2, _tempLzssBuffer);
			return _bufferLzssData;
		}
	}
	return 0;
}

void CUP_Player::decodeLzssData(uint8 *dst1, const uint8 *src1, const uint8 *src2, const uint8 *src3, uint8 *dst2) {
	int index = 1;
	while (1) {
		int code = *src1++;
		for (int b = 0; b < 8; ++b) {
			if (code & (1 << b)) {
				*dst1++ = dst2[index] = *src2++;
				++index;
				index &= 0xFFF;
			} else {
				int cmd = READ_LE_UINT16(src3); src3 += 2;
				int count = (cmd >> 0xC) + 2;
				int offs = cmd & 0xFFF;
				if (offs == 0) {
					return;
				}
				while (count--) {
					*dst1++ = dst2[index] = dst2[offs];
					++index;
					index &= 0xFFF;
					++offs;
					offs &= 0xFFF;
				}
			}
		}
	}
}

void CUP_Player::handleRATE(const uint8 *data, uint32 dataSize) {
	const int rate = (int16)READ_LE_UINT16(data);
	_playbackRate = CLIP(rate, 1, 4000);
}

void CUP_Player::handleSNDE(const uint8 *data, uint32 dataSize) {
	if (_sfxQueuePos < ARRAYSIZE(_sfxQueue)) {
		CUP_Sfx *sfx = &_sfxQueue[_sfxQueuePos];
		sfx->mode = READ_LE_UINT32(data);
		sfx->num = (int16)READ_LE_UINT16(data + 4);
		// READ_LE_UINT16(data + 6); // unused
		sfx->flags = READ_LE_UINT16(data + 8);
		++_sfxQueuePos;
	}
}

void CUP_Player::handleTOIL(const uint8 *data, uint32 dataSize) {
	int codesCount = READ_LE_UINT16(data); data += 2;
	if (dataSize > 0) {
		while (codesCount != 0) {
			int codeSize = *data++;
			if (codeSize == 0) {
				codeSize = READ_LE_UINT16(data); data += 2;
			}
			int code = *data++;
			if (code == 0) {
				code = READ_LE_UINT16(data); data += 2;
			}
			switch (code) {
			case 1:
				_vm->_quit = true;
				break;
			case 7: { // pause playback
					int sfxSync = READ_LE_UINT32(data);
					warning("Unhandled playback pause %d", sfxSync);
				}
				break;
			case 2: // display copyright/information messagebox
			case 3: // no-op in the original
			case 4: // restart playback
			case 5: // disable normal screen update
			case 6: // perform offscreen buffers swapping
				// these are never triggered
			default:
				warning("Unhandled TOIL code=%d", code);
				break;
			}
			dataSize -= codeSize;
			data += codeSize;
			--codesCount;
		}
	}
}

} // End of namespace Scumm
