/* ScummVM - Scumm Interpreter
 * Copyright (C) 2002-2003 The ScummVM project
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 *
 * $Header$
 */

#include "stdafx.h"
#include "about.h"
#include "newgui.h"
#include "base/engine.h"
#include "common/str.h"

AboutDialog::AboutDialog(NewGui *gui)
	: Dialog(gui, 10, 20, 300, 124) {
	addButton((_w - kButtonWidth)/2, 100, "OK", kCloseCmd, '\r');	// Close dialog - FIXME
	
	ScummVM::String version("ScummVM ");
	version += gScummVMVersion;
	new StaticTextWidget(this, 10, 10, _w - 20, kLineHeight, version, kTextAlignCenter);

	ScummVM::String date("(built on ");
	date += gScummVMBuildDate;
	date += ')';
	new StaticTextWidget(this, 10, 20, _w - 20, kLineHeight, date, kTextAlignCenter);

	// TODO: Probably should display something regarding GPL
	new StaticTextWidget(this, 10, 35, _w - 20, kLineHeight, "Copyright (C) 2002-2003 The ScummVM project", kTextAlignCenter);
	new StaticTextWidget(this, 10, 45, _w - 20, kLineHeight, "http://www.scummvm.org", kTextAlignCenter);

	new StaticTextWidget(this, 10, 65, _w - 20, kLineHeight, "SCUMM Games (C) LucasArts", kTextAlignCenter);
	new StaticTextWidget(this, 10, 75, _w - 20, kLineHeight, "Simon the Sorcerer (C) Adventure Soft", kTextAlignCenter);
	new StaticTextWidget(this, 10, 85, _w - 20, kLineHeight, "Beneath a Steel Sky (C) Revolution", kTextAlignCenter);
}

