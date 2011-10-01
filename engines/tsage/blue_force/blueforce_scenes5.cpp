/* ScummVM - Graphic Adventure Engine
 *
 * ScummVM is the legal property of its developers, whose names
 * are too numerous to list here. Please refer to the COPYRIGHT
 * file distributed with this source distribution.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.

 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	 See the
 * GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 *
 */

#include "tsage/blue_force/blueforce_scenes5.h"
#include "tsage/blue_force/blueforce_dialogs.h"
#include "tsage/scenes.h"
#include "tsage/tsage.h"
#include "tsage/staticres.h"

namespace TsAGE {

namespace BlueForce {

/*--------------------------------------------------------------------------
 * Scene 550 - Outside Bikini Hut
 *
 *--------------------------------------------------------------------------*/

void Scene550::Action1::signal() {
	Scene550 *scene = (Scene550 *)BF_GLOBALS._sceneManager._scene;

	switch (_actionIndex++) {
	case 0:
		BF_GLOBALS._player.disableControl();
		setDelay(5);
		break;
	case 1:
		scene->_stripManager.start(scene->_sceneMode, this);
		break;
	case 2:
		BF_GLOBALS._player.enableControl();
		remove();
		break;
	}
}

/*--------------------------------------------------------------------------*/

bool Scene550::Object1::startAction(CursorType action, Event &event) {
	Scene550 *scene = (Scene550 *)BF_GLOBALS._sceneManager._scene;

	switch (action) {
	case CURSOR_TALK:
		if ((BF_INVENTORY.getObjectScene(INV_SCHEDULE) == 1) || 
				(BF_INVENTORY.getObjectScene(INV_9MM_BULLETS) == 1)) {
			if ((BF_INVENTORY.getObjectScene(INV_SCHEDULE) == 1) &&
					(BF_INVENTORY.getObjectScene(INV_9MM_BULLETS) == 1)) {
				BF_GLOBALS.setFlag(fToldLyleOfSchedule);
				BF_GLOBALS._player.disableControl();
				scene->_sceneMode = 5501;
				scene->setAction(&scene->_sequenceManager, scene, 5514, &BF_GLOBALS._player, this, NULL);
			} else {
				scene->_sceneMode = 0;
				scene->_stripManager.start(5509, scene);
			}
		} else if (BF_GLOBALS._sceneManager._previousScene == 930) {
			scene->_sceneMode = 5512;
			scene->setAction(&scene->_action1);
		} else {
			scene->_sceneMode = BF_INVENTORY.getObjectScene(BF_LAST_INVENT) == 1 ? 5513 : 5512;
			scene->_stripManager.setAction(&scene->_action1);
		}
		return true;
	case INV_SCHEDULE:
		BF_GLOBALS.setFlag(fToldLyleOfSchedule);
		BF_GLOBALS._player.disableControl();
		scene->_sceneMode = 5501;
		scene->setAction(&scene->_sequenceManager, scene, 5514, &BF_GLOBALS._player, this, NULL);
		return true;
	default:
		return NamedObject::startAction(action, event);
	}
}

bool Scene550::CaravanDoor::startAction(CursorType action, Event &event) {
	Scene550 *scene = (Scene550 *)BF_GLOBALS._sceneManager._scene;

	switch (action) {
	case CURSOR_LOOK:
		SceneItem::display2(550, 0);
		return true;
	case CURSOR_USE:
		SceneItem::display2(550, 7);
		return true;
	case BF_LAST_INVENT:
		if ((BF_GLOBALS._dayNumber == 3) || !BF_GLOBALS.getFlag(fWithLyle))
			SceneItem::display2(550, 33);
		else {
			BF_GLOBALS._player.disableControl();
			scene->_sceneMode = 5500;
			scene->setAction(&scene->_sequenceManager, scene, 5500, &BF_GLOBALS._player, this, NULL);
		}
		return true;
	default:
		return NamedObject::startAction(action, event);
	}
}

bool Scene550::Vechile::startAction(CursorType action, Event &event) {
	Scene550 *scene = (Scene550 *)BF_GLOBALS._sceneManager._scene;

	switch (action) {
	case CURSOR_LOOK:
		SceneItem::display2(550, 3);
		return true;
	case CURSOR_USE:
		if (!BF_GLOBALS.getFlag(fWithLyle)) {
			BF_GLOBALS._player.disableControl();
			scene->_sceneMode = 5510;
			scene->setAction(&scene->_sequenceManager, scene, BF_GLOBALS.getFlag(fWithLyle) ? 5510 : 5515,
				&BF_GLOBALS._player, this, NULL);
		} else if (BF_GLOBALS.getFlag(fToldLyleOfSchedule)) {
			BF_GLOBALS._player.disableControl();
			scene->_sceneMode = 2;
			scene->setAction(&scene->_sequenceManager, scene, 5501, &BF_GLOBALS._player, NULL);
		} else if ((BF_INVENTORY.getObjectScene(INV_SCHEDULE) == 1) || 
					(BF_INVENTORY.getObjectScene(INV_9MM_BULLETS) == 1)) {
			if (BF_INVENTORY.getObjectScene(INV_9MM_BULLETS) == 1) {
				scene->_sceneMode = 5501;
				scene->_stripManager.start(5511, scene);
			} else {
				scene->_sceneMode = 0;
				scene->_stripManager.start(5509, scene);
			}
		} else if (BF_GLOBALS._sceneManager._previousScene == 930) {
			scene->_sceneMode = 5512;
			scene->setAction(&scene->_action1);
		} else {
			BF_GLOBALS._player.disableControl();
			scene->_sceneMode = 2;
			scene->setAction(&scene->_sequenceManager, scene, 5501, &BF_GLOBALS._player, NULL);
		}
		return true;
	default:
		return NamedObject::startAction(action, event);
	}
}

/*--------------------------------------------------------------------------*/

void Scene550::postInit(SceneObjectList *OwnerList) {
	BF_GLOBALS._sound1.fadeSound(16);

	if ((BF_GLOBALS._bookmark == bInspectionDone) && !BF_GLOBALS.getFlag(fHasDrivenFromDrunk)) {
		_sceneMode = 1;
		signal();
		return;
	} 

	SceneExt::postInit();
	loadScene(550);
	
	_stripManager.addSpeaker(&_gameTextSpeaker);
	_stripManager.addSpeaker(&_lyleHatSpeaker);
	_stripManager.addSpeaker(&_jakeJacketSpeaker);

	BF_GLOBALS._player.postInit();
	BF_GLOBALS._player.animate(ANIM_MODE_1, NULL);
	BF_GLOBALS._player.setObjectWrapper(new SceneObjectWrapper());
	BF_GLOBALS._player.setPosition(Common::Point(160, 100));
	BF_GLOBALS._player._moveDiff = Common::Point(2, 1);
	BF_GLOBALS._player.enableControl();

	_caravanDoor.postInit();
	_caravanDoor.setVisage(550);
	_caravanDoor.setPosition(Common::Point(34, 66));
	BF_GLOBALS._sceneItems.push_back(&_caravanDoor);

	_vechile.postInit();
	_vechile.fixPriority(70);

	if (BF_GLOBALS.getFlag(fWithLyle)) {
		BF_GLOBALS._walkRegions.proc1(10);
		BF_GLOBALS._walkRegions.proc1(11);

		_vechile.setVisage(444);
		_vechile.setStrip(4);
		_vechile.setFrame2(2);
		_vechile.setPosition(Common::Point(110, 85));
		_vechile.fixPriority(76);

		_object1.postInit();
		_object1.setVisage(835);
		_object1.setPosition(Common::Point(139, 83));
		_object1.setDetails(550, 29, 30, 31, 1, NULL);
		_object1.setStrip(8);

		BF_GLOBALS._player.setVisage(303);
		BF_GLOBALS._player.setPosition(Common::Point(89, 76));
		BF_GLOBALS._player.updateAngle(_object1._position);
	} else {
		BF_GLOBALS._walkRegions.proc1(12);

		_vechile.setPosition(Common::Point(205, 77));
		_vechile.changeZoom(80);

		if (BF_GLOBALS.getFlag(onDuty)) {
			_vechile.setVisage(301);
			_vechile.setStrip(1);
			
			BF_GLOBALS._player.setVisage(304);
		} else {
			_vechile.setVisage(580);
			_vechile.setStrip(2);
			_vechile.setFrame(2);

			BF_GLOBALS._player.setVisage(303);
		}		

		BF_GLOBALS._sceneItems.push_back(&_vechile);

		if (BF_GLOBALS._sceneManager._previousScene == 930) {
			_caravanDoor.setFrame(_caravanDoor.getFrameCount());
			BF_GLOBALS._player.disableControl();

			_sceneMode = 0;
			setAction(&_sequenceManager, this, 5512, &BF_GLOBALS._player, &_caravanDoor, NULL);
		} else if (BF_GLOBALS.getFlag(onDuty)) {
			BF_GLOBALS._player.disableControl();
			_sceneMode = 0;
			setAction(&_sequenceManager, this, 5502, &BF_GLOBALS._player, &_vechile, NULL);
		} else if (!BF_GLOBALS.getFlag(fWithLyle)) {
			BF_GLOBALS._player.setPosition(Common::Point(185, 70));
		} else if (BF_GLOBALS._bookmark == bFlashBackOne) {
			BF_GLOBALS._player.disableControl();
			_sceneMode = 0;
			setAction(&_sequenceManager, this, 5513, &_object1, NULL);
		} else {
			_sceneMode = 0;
		}
	}

	_item2.setDetails(Rect(0, 26, 53, 67), 550, 1, -1, 2, 1, NULL);
	_item3.setDetails(Rect(53, 12, 173, 65), 550, 4, -1, 5, 1, NULL);
	_item1.setDetails(Rect(0, 0, 320, 170), 550, 6, -1, -1, 1, NULL);
}

void Scene550::signal() {
	switch (_sceneMode) {
	case 0:
		BF_GLOBALS._player.enableControl();
		break;
	case 1:
		BF_GLOBALS._sceneManager.changeScene(551);
		break;
	case 2:
	case 5510:
		BF_GLOBALS._sceneManager.changeScene(60);
		break;
	case 3:
		BF_GLOBALS._driveFromScene = 16;
		BF_GLOBALS._driveToScene = 128;
		BF_GLOBALS._mapLocationId = 128;
		BF_GLOBALS._sound1.fadeOut2(NULL);
		BF_GLOBALS._sceneManager.changeScene(800);
		break;
	case 5500:
		BF_GLOBALS._sceneManager.changeScene(930);
		break;
	case 5501:
		BF_GLOBALS._player.disableControl();
		_sceneMode = 3;
		setAction(&_sequenceManager, this, 5501, &BF_GLOBALS._player, NULL);
		break;
	default:
		break;
	}
}

/*--------------------------------------------------------------------------
 * Scene 551 - Outside Bikini Hut (Drunk Stop)
 *
 *--------------------------------------------------------------------------*/

void Scene551::Action2::signal() {
	Scene551 *scene = (Scene551 *)BF_GLOBALS._sceneManager._scene;

	switch (_actionIndex++) {
	case 0:
		BF_GLOBALS._walkRegions.proc2(18);
		BF_GLOBALS._walkRegions.proc2(4);
		scene->_field1CD2 = 1;
		
		scene->_harrison.setObjectWrapper(new SceneObjectWrapper());
		scene->_harrison.animate(ANIM_MODE_1, NULL);

		BF_GLOBALS._player.setVisage(304);
		BF_GLOBALS._player.animate(ANIM_MODE_1, NULL);

		if (scene->_drunkStanding._flag != 1) {
			ADD_PLAYER_MOVER_THIS(scene->_harrison, 126, 85);
		} else {
			ADD_PLAYER_MOVER_THIS(scene->_harrison, 88, 91);
		}
		break;
	case 1:
		scene->_harrison.updateAngle(BF_GLOBALS._player._position);

		if (scene->_drunkStanding._flag == 1) {
			BF_GLOBALS._walkRegions.proc1(4);
			ADD_PLAYER_MOVER(71, 97);
		} else {
			ADD_PLAYER_MOVER(141, 87);
		}
		break;
	case 2:
		scene->_harrison.updateAngle(BF_GLOBALS._player._position);
		BF_GLOBALS._player.updateAngle(scene->_harrison._position);
		setDelay(10);
		break;
	case 3:
		scene->_stripManager.start(scene->_harrison._flag, this);
		break;
	case 4:
		scene->_field1CD2 = 0;
		BF_GLOBALS._player.enableControl();
		remove();
		break;
	default:
		break;
	}
}

/*--------------------------------------------------------------------------*/

bool Scene551::Vechile::startAction(CursorType action, Event &event) {
	Scene551 *scene = (Scene551 *)BF_GLOBALS._sceneManager._scene;

	switch (action) {
	case CURSOR_LOOK:
		SceneItem::display2(550, 3);
		return true;
	case CURSOR_USE:
		if (!BF_GLOBALS.getFlag(didDrunk)) {
			BF_GLOBALS._player.disableControl();
			scene->_sceneMode = 1;
			scene->setAction(&scene->_sequenceManager, scene, 5510, &BF_GLOBALS._player, this, NULL);
		} else if (BF_INVENTORY.getObjectScene(INV_CENTER_PUNCH) == 1) {
			BF_INVENTORY.setObjectScene(INV_CENTER_PUNCH, 0);
			scene->_harrison._flag = 5505;
			scene->setAction(&scene->_action2, scene);
			scene->_sceneMode = 5509;
		} else {
			BF_GLOBALS._player.disableControl();
			scene->_sceneMode = 1;
			scene->setAction(&scene->_sequenceManager, scene, 5510, &BF_GLOBALS._player, this, NULL);
		}
		return true;
	default:
		return NamedObject::startAction(action, event);
	}
}

bool Scene551::DrunkStanding::startAction(CursorType action, Event &event) {
	Scene551 *scene = (Scene551 *)BF_GLOBALS._sceneManager._scene;

	switch (action) {
	case CURSOR_LOOK:
		SceneItem::display2(550, 15);
		return true;
	case CURSOR_USE:
		SceneItem::display2(550, 16);
		return true;
	case CURSOR_TALK:
		SceneItem::display2(550, 17);
		return true;
	case INV_HANDCUFFS:
		scene->_harrison.animate(ANIM_MODE_1, NULL);
		BF_GLOBALS.set2Flags(f1098Drunk);
		BF_GLOBALS.setFlag(didDrunk);
		_flag = 2;
		BF_GLOBALS._player.disableControl();
		scene->_sceneMode = 5509;

		scene->setAction(&scene->_sequenceManager, scene, 5509, &BF_GLOBALS._player, this, &scene->_harrison, NULL);
		BF_GLOBALS.set2Flags(f1015Drunk);
		return true;
	default:
		return NamedObjectExt::startAction(action, event);
	}
}

bool Scene551::Drunk::startAction(CursorType action, Event &event) {
	Scene551 *scene = (Scene551 *)BF_GLOBALS._sceneManager._scene;

	switch (action) {
	case CURSOR_LOOK:
		switch (_flag) {
		case 0:
			BF_GLOBALS._player.disableControl();
			scene->_sceneMode = 0;
			scene->setAction(&scene->_sequenceManager, scene, 5505, &BF_GLOBALS._player, &scene->_object15, NULL);
			break;
		case 1:
			SceneItem::display2(550, 11);
			break;
		case 2:
			SceneItem::display2(550, 12);
			break;
		case 3:
			SceneItem::display2(550, 27);
			break;
		}
		return true;
	case CURSOR_USE:
		switch (_flag) {
		case 0:
			BF_GLOBALS._player.disableControl();
			if (BF_GLOBALS.getFlag(fTalkedToDrunkInCar)) {
				scene->_sceneMode = 5508;
				scene->setAction(&scene->_sequenceManager, scene, 5508, &BF_GLOBALS._player, NULL);
			} else {
				BF_GLOBALS.setFlag(fTalkedToDrunkInCar);
				scene->_sceneMode = 16;
				scene->setAction(&scene->_sequenceManager, scene, 5504, &BF_GLOBALS._player, &scene->_object15, NULL);
			}
			break;
		case 2:
			SceneItem::display2(550, 13);
			break;
		case 3:
			SceneItem::display2(550, 27);
			break;
		default:
			break;
		}
		return true;
	case CURSOR_TALK:
		if (_flag)
			break;
		BF_GLOBALS._player.disableControl();

		if (BF_GLOBALS.getFlag(fTalkedToDrunkInCar)) {
			scene->_sceneMode = 5508;
			scene->setAction(&scene->_sequenceManager, scene, 5508, &BF_GLOBALS._player, NULL);
		} else {
			BF_GLOBALS.setFlag(fTalkedToDrunkInCar);
			scene->_sceneMode = 0;
			scene->setAction(&scene->_sequenceManager, scene, 5504, &BF_GLOBALS._player, &scene->_object15, NULL);
		}
		return true;
	case INV_CENTER_PUNCH:
		if (_flag) {
			SceneItem::display2(550, 14);
		} else {
			BF_GLOBALS.set2Flags(f1098Drunk);
			BF_GLOBALS.setFlag(didDrunk);
			_flag = 1;
			BF_GLOBALS._uiElements.addScore(30);

			BF_GLOBALS._player.disableControl();
			scene->_sceneMode = 5506;
			scene->setAction(&scene->_sequenceManager, scene, 5506, &BF_GLOBALS._player, this,
				&scene->_object12, NULL);
		}
		return true;
	default:
		break;
	}

	return NamedObjectExt::startAction(action, event);
}

bool Scene551::PatrolCarTrunk::startAction(CursorType action, Event &event) {
	Scene551 *scene = (Scene551 *)BF_GLOBALS._sceneManager._scene;

	switch (action) {
	case CURSOR_USE:
		if (BF_INVENTORY.getObjectScene(INV_CENTER_PUNCH) != 1) {
			BF_GLOBALS._walkRegions.proc2(18);
			BF_GLOBALS._player.disableControl();
			scene->_sceneMode = 2;
			scene->setAction(&scene->_sequenceManager, scene, 5503, &BF_GLOBALS._player, &scene->_harrison, this, NULL);
			return true;
		}
		break;
	default:
		break;
	}

	return NamedObject::startAction(action, event);
}

void Scene551::TrunkInset::remove() {
	Scene551 *scene = (Scene551 *)BF_GLOBALS._sceneManager._scene;
	scene->_trunkKits.remove();
	BF_GLOBALS._player.disableControl();

	scene->_sceneMode = 0;
	scene->setAction(&scene->_sequenceManager, scene, 5516, &scene->_harrison, 
		&scene->_patrolCarTrunk, NULL);

	FocusObject::remove();
}

bool Scene551::TrunkInset::startAction(CursorType action, Event &event) {
	switch (action) {
	case CURSOR_LOOK:
		SceneItem::display2(550, 18);
		return true;
	case CURSOR_USE:
		return true;
	default:
		return NamedObject::startAction(action, event);
	}
}

bool Scene551::TrunkKits::startAction(CursorType action, Event &event) {
	Scene551 *scene = (Scene551 *)BF_GLOBALS._sceneManager._scene;

	Rect tempRect = _bounds;
	tempRect.collapse(10, 6);
	if (!tempRect.contains(event.mousePos))
		return false;

	switch (action) {
	case CURSOR_LOOK:
		switch (_frame) {
		case 1:
			SceneItem::display2(550, 19);
			break;
		case 2:
			SceneItem::display2(550, 20);
			break;
		case 3:
			SceneItem::display2(550, 21);
			break;
		default:
			break;
		}
		return true;
	case CURSOR_USE:
		if (event.mousePos.y < _bounds.top) {
			setFrame(1);
		} else if ((event.mousePos.x < (_bounds.left - 25)) && (_frame > 1)) {
			SceneItem::display2(550, 28);
		} else {
			switch (_frame) {
			case 1:
				setFrame(BF_INVENTORY.getObjectScene(INV_CENTER_PUNCH) == 1 ? 3 : 2);
				break;
			case 2:
				if (!BF_GLOBALS.getFlag(fShowedBluePrint)) {
					BF_GLOBALS._uiElements.addScore(30);
					BF_GLOBALS.setFlag(fShowedBluePrint);
				}

				BF_GLOBALS._player.disableControl();
				scene->_sceneMode = 5520;
				scene->setAction(&scene->_sequenceManager, scene, 5520, &BF_GLOBALS._player, NULL);
				BF_INVENTORY.setObjectScene(INV_CENTER_PUNCH, 1);
				setFrame(3);
				break;
			case 3:
				setFrame(1);
				break;
			default:
				break;
			}
		}
		return true;
	case INV_CENTER_PUNCH:
		SceneItem::display2(550, 22);
		BF_INVENTORY.setObjectScene(INV_CENTER_PUNCH, 0);
		setFrame(2);
		return true;
	default:
		return NamedObject::startAction(action, event);
	}
}

bool Scene551::Harrison::startAction(CursorType action, Event &event) {
	Scene551 *scene = (Scene551 *)BF_GLOBALS._sceneManager._scene;

	switch (action) {
	case CURSOR_LOOK:
		SceneItem::display2(550, 23);
		return true;
	case CURSOR_USE:
		SceneItem::display2(550, 24);
		return true;
	case CURSOR_TALK:
		BF_GLOBALS._player.disableControl();

		if (scene->_drunkStanding._flag == 1) {
			_flag = 5508;
		} else if (!scene->_field1CD0) {
			_flag = 5502;
		} else {
			scene->_field1CD0 = 1;
			_flag = 5500;
		}
		scene->setAction(&scene->_action2);
		return true;
	default:
		return NamedObjectExt::startAction(action, event);
	}
}

/*--------------------------------------------------------------------------*/

bool Scene551::Item4::startAction(CursorType action, Event &event) {
	Scene551 *scene = (Scene551 *)BF_GLOBALS._sceneManager._scene;

	switch (action) {
	case CURSOR_LOOK:
		SceneItem::display2(550, 34);
		BF_GLOBALS._player.disableControl();
		scene->_sceneMode = 1;
		scene->setAction(&scene->_sequenceManager, scene, 5510, &BF_GLOBALS._player, &scene->_vechile2, NULL);
		return true;
	default:
		return false;
	}
}

/*--------------------------------------------------------------------------*/

Scene551::Scene551(): Scene550() {
	_field1CD0 = _field1CD2 = 0;
}

void Scene551::synchronize(Serializer &s) {
	Scene550::synchronize(s);
	s.syncAsSint16LE(_field1CD0);
	s.syncAsSint16LE(_field1CD2);
}

void Scene551::postInit(SceneObjectList *OwnerList) {
	SceneExt::postInit();
	loadScene(550);
	BF_GLOBALS._walkRegions.load(551);
	BF_GLOBALS._sound1.fadeSound(16);

	_stripManager.addSpeaker(&_gameTextSpeaker);
	_stripManager.addSpeaker(&_jakeUniformSpeaker);
	_stripManager.addSpeaker(&_gigglesSpeaker);
	_stripManager.addSpeaker(&_drunkSpeaker);

	if (BF_GLOBALS.getFlag(fHasLeftDrunk)) {
		_item4.setBounds(Rect(0, 0, 320, 170));
		BF_GLOBALS._sceneItems.push_back(&_item4);
	}

	BF_GLOBALS._player.postInit();
	BF_GLOBALS._player.animate(ANIM_MODE_1, NULL);
	BF_GLOBALS._player.setObjectWrapper(new SceneObjectWrapper());
	BF_GLOBALS._player.setPosition(Common::Point(160, 100));
	BF_GLOBALS._player._moveDiff = Common::Point(2, 1);
	BF_GLOBALS._player.enableControl();

	_caravanDoor.postInit();
	_caravanDoor.setVisage(550);
	_caravanDoor.setPosition(Common::Point(34, 66));
	BF_GLOBALS._sceneItems.push_back(&_caravanDoor);

	_vechile2.postInit();
	_vechile2.setVisage(301);
	_vechile2.setStrip(1);
	_vechile2.setPosition(Common::Point(205, 77));
	_vechile2.fixPriority(70);
	_vechile2.changeZoom(80);
	BF_GLOBALS._sceneItems.push_back(&_vechile2);

	BF_GLOBALS._walkRegions.proc1(14);

	_drunk.postInit();
	_drunk.setVisage(550);
	_drunk.setStrip(3);
	_drunk.fixPriority(84);
	_drunk.setPosition(Common::Point(29, 92));
	_drunk._flag = 0;
	BF_GLOBALS._sceneItems.push_back(&_drunk);

	_object12.postInit();
	_object12.setVisage(550);
	_object12.setStrip(7);
	_object12.setPosition(Common::Point(29, 92));
	_object12.hide();

	_object13.postInit();
	_object13.setVisage(550);
	_object13.setStrip(2);
	_object13.setPosition(Common::Point(29, 92));
	_object13.fixPriority(82);
	_object13.setDetails(550, 8, -1, 9, 1, NULL);

	if (BF_GLOBALS.getFlag(didDrunk)) {
		_drunk._flag = 3;
		_drunk.setStrip(3);

		_object12.show();
		_object12.setDetails(550, 25, -1, 26, 1, NULL);
		BF_GLOBALS._sceneItems.push_front(&_object12);
		
		_harrison.postInit();
		_harrison.setVisage(304);
		_harrison.setPosition(Common::Point(67, 102));
		_harrison.setStrip(8);
		_harrison.setFrame(1);
		_harrison._flag = 1;
		BF_GLOBALS._sceneItems.push_back(&_harrison);

		_object14.postInit();
		_object14.setVisage(550);
		_object14.setStrip(5);
		_object14.animate(ANIM_MODE_2, NULL);
		_object14.fixPriority(80);
		_object14.setPosition(Common::Point(122, 57));

		_object11.postInit();
		_object11.setVisage(550);
		_object11.setStrip(2);
		_object11.setFrame(2);
		_object11.setPosition(Common::Point(116, 84));
		_object11.fixPriority(77);
		_object11.setDetails(550, 32, -1, 10, 1, NULL);

		_drunkStanding.postInit();
		_drunkStanding._flag = 0;
		_drunkStanding.setVisage(554);
		_drunkStanding.setStrip(7);
		_drunkStanding.setFrame(8);
		_drunkStanding.fixPriority(83);
		_drunkStanding.setPosition(Common::Point(57, 99));
	} else {
		_harrison.postInit();
		_harrison.setVisage(304);
		_harrison.setPosition(Common::Point(126, 83));
		_harrison.setObjectWrapper(new SceneObjectWrapper());
		_harrison._flag = 0;
		_harrison._moveDiff = Common::Point(2, 1);
		BF_GLOBALS._sceneItems.push_back(&_harrison);

		_object14.postInit();
		_object14.setVisage(550);
		_object14.setStrip(5);
		_object14.animate(ANIM_MODE_2, NULL);
		_object14.fixPriority(80);
		_object14.setPosition(Common::Point(122, 57));

		_patrolCarTrunk.postInit();
		_patrolCarTrunk.setVisage(550);
		_patrolCarTrunk.setStrip(4);
		_patrolCarTrunk.setFrame(1);
		_patrolCarTrunk.setPosition(Common::Point(149, 69));
		_patrolCarTrunk.fixPriority(79);
		_patrolCarTrunk.setDetails(550, 18, -1, 9, 1, NULL);

		_object11.postInit();
		_object11.setVisage(550);
		_object11.setStrip(2);
		_object11.setFrame(2);
		_object11.setPosition(Common::Point(116, 84));
		_object11.fixPriority(77);
		_object11.setDetails(550, 32, -1, 10, 1, NULL);

		_drunkStanding.postInit();
		_drunkStanding._flag = 0;
		_drunkStanding.setVisage(554);
		_drunkStanding.fixPriority(83);
		_drunkStanding.setPosition(Common::Point(47, 93));

		_object15.postInit();
		_object15.setVisage(552);
		_object15.setPosition(Common::Point(59, 94));
		_object15.fixPriority(200);
		_object15.hide();
	}

	BF_GLOBALS._player.disableControl();
	_sceneMode = 3;
	setAction(&_sequenceManager, this, 5502, &BF_GLOBALS._player, &_vechile2, NULL);

	_item2.setDetails(Rect(0, 26, 53, 67), 550, 1, -1, 2, 1, NULL);
	_item3.setDetails(Rect(53, 12, 173, 65), 550, 4, -1, 5, 1, NULL);
	_item1.setDetails(Rect(0, 0, 320, 170), 550, 6, -1, -1, 1, NULL);
}

void Scene551::signal() {
	switch (_sceneMode) {
	case 0:
		BF_GLOBALS._player.enableControl();
		break;
	case 1:
		BF_GLOBALS._sound1.fadeOut2(NULL);
		BF_GLOBALS.set2Flags(f1027Drunk);
		BF_GLOBALS.set2Flags(f1097Drunk);

		if (BF_GLOBALS.getFlag(didDrunk))
			BF_GLOBALS.setFlag(f1098Drunk);

		BF_GLOBALS._sceneManager.changeScene(60);
		break;
	case 2:
		BF_GLOBALS._walkRegions.proc1(18);

		_trunkInset.postInit();
		_trunkInset.setVisage(553);
		_trunkInset.setPosition(Common::Point(59, 92));
		_trunkInset.fixPriority(252);
		BF_GLOBALS._sceneItems.push_front(&_trunkInset);

		_trunkKits.postInit();
		_trunkKits.setVisage(553);
		_trunkKits.setStrip(2);
		_trunkKits.setPosition(Common::Point(57, 73));
		_trunkKits.fixPriority(254);
		BF_GLOBALS._sceneItems.push_front(&_trunkKits);
		BF_GLOBALS._player.enableControl();
		break;
	case 3:
		if (BF_GLOBALS.getFlag(talkedToHarrisAboutDrunk)) {
			BF_GLOBALS._player.enableControl();
		} else {
			BF_GLOBALS.setFlag(talkedToHarrisAboutDrunk);
			_harrison._flag = 5500;
			setAction(&_action2);
		}
		break;
	case 5506:
		_harrison.setObjectWrapper(new SceneObjectWrapper());
		ADD_PLAYER_MOVER_NULL(_harrison, 88, 91);

		_object12.show();
		_object12.setDetails(550, 25, -1, 26, 1, NULL);
		BF_GLOBALS._sceneItems.push_front(&_object12);
		BF_GLOBALS._player.enableControl();

		_sceneMode = 5507;
		setAction(&_sequenceManager, this, 5507, &BF_GLOBALS._player, &_drunk, &_drunkStanding, NULL);
		break;
	case 5507:
		BF_GLOBALS._walkRegions.proc1(2);
		BF_GLOBALS._walkRegions.proc1(4);

		_drunkStanding._flag = 1;
		BF_GLOBALS._sceneItems.push_front(&_drunkStanding);
		BF_GLOBALS._player.enableControl();
		break;
	case 5508:
		BF_GLOBALS._player.disableControl();
		_sceneMode = 0;
		setAction(&_sequenceManager, this, 5505, &BF_GLOBALS._player, &_object15, NULL);
		break;
	case 5509:
		BF_INVENTORY.setObjectScene(INV_CENTER_PUNCH, 0);
		BF_GLOBALS._player.disableControl();
		_sceneMode = 5510;
		setAction(&_sequenceManager, this, 5510, &BF_GLOBALS._player, &_vechile2, NULL);
		break;
	case 5510:
		BF_GLOBALS._bookmark = bCalledToDrunkStop;
		if (BF_GLOBALS.getFlag(didDrunk))
			BF_GLOBALS.setFlag(fHasLeftDrunk);

		BF_GLOBALS._sceneManager.changeScene(60);
		break;
	case 5520:
		BF_GLOBALS._player.animate(ANIM_MODE_1, NULL);
		_harrison.animate(ANIM_MODE_1, NULL);
		BF_GLOBALS._player.enableControl();
		break;
	default:
		BF_GLOBALS._player.enableControl();
		break;
	}
}

void Scene551::dispatch() {
	SceneExt::dispatch();

	if ((_drunkStanding._flag != 2) && !_harrison._mover && !_field1CD2)
		_harrison.updateAngle(BF_GLOBALS._player._position);
}

/*--------------------------------------------------------------------------
 * Scene 550 - Study
 *
 *--------------------------------------------------------------------------*/

void Scene560::Action1::signal() {
	Scene560 *scene = (Scene560 *)BF_GLOBALS._sceneManager._scene;

	switch (_actionIndex++) {
	case 0:
		setDelay(10);
		break;
	case 1:
		 ADD_MOVER(BF_GLOBALS._player, 105, 140);
		 break;
	case 2:
		scene->_deskChair.hide();

		BF_GLOBALS._player.changeZoom(81);
		BF_GLOBALS._player.setVisage(561);
		BF_GLOBALS._player.setStrip(2);
		BF_GLOBALS._player.setFrame(1);
		BF_GLOBALS._player.setPosition(Common::Point(96, 138));
		BF_GLOBALS._player.animate(ANIM_MODE_5, this);
		break;
	case 3:
		scene->_deskChair.setVisage(561);
		scene->_deskChair.setFrame(BF_GLOBALS._player._frame);
		scene->_deskChair.setStrip(BF_GLOBALS._player._strip);
		scene->_deskChair.setPosition(BF_GLOBALS._player._position);

		scene->_field380 = 1;
		BF_GLOBALS._player.enableControl();
		remove();
		break;
	default:
		break;
	}
}

void Scene560::Action2::signal() {
	Scene560 *scene = (Scene560 *)BF_GLOBALS._sceneManager._scene;

	switch (_actionIndex++) {
	case 0:
		BF_GLOBALS._player.disableControl();
		setDelay(10);
		break;
	case 1:
		scene->_deskChair.hide();

		BF_GLOBALS._player.setVisage(561);
		BF_GLOBALS._player.setStrip(2);
		BF_GLOBALS._player.setFrame(BF_GLOBALS._player.getFrameCount());
		BF_GLOBALS._player.animate(ANIM_MODE_6, this);
		break;
	case 2:
		scene->_field380 = 0;
		scene->_deskChair.setPosition(Common::Point(81, 149));
		scene->_deskChair.setVisage(561);
		scene->_deskChair.setStrip(3);
		scene->_deskChair.setFrame(1);
		scene->_deskChair.fixPriority(151);
		scene->_deskChair.show();

		BF_GLOBALS._player.setVisage(563);
		BF_GLOBALS._player.setPosition(Common::Point(105, 140));
		BF_GLOBALS._player.fixPriority(-1);
		BF_GLOBALS._player.changeZoom(-1);
		BF_GLOBALS._player.animate(ANIM_MODE_1, this);
		BF_GLOBALS._player.setObjectWrapper(new SceneObjectWrapper());
		BF_GLOBALS._player.enableControl();
		ADD_PLAYER_MOVER(scene->_destPosition.x, scene->_destPosition.y);
		remove();
		break;
	default:
		break;
	}
}

void Scene560::Action3::signal() {
	Scene560 *scene = (Scene560 *)BF_GLOBALS._sceneManager._scene;

	switch (_actionIndex++) {
	case 0:
		BF_GLOBALS._player.disableControl();
		setDelay(10);
		break;
	case 1:
		if (scene->_field380) {
			setDelay(10);
		} else {
			setAction(&scene->_action1, this);
		}
		break;
	case 2:
		BF_GLOBALS._sceneManager.changeScene(570);
		break;
	default:
		break;
	}
}

/*--------------------------------------------------------------------------*/

bool Scene560::PicturePart::startAction(CursorType action, Event &event) {
	Scene560 *scene = (Scene560 *)BF_GLOBALS._sceneManager._scene;

	switch (action) {
	case CURSOR_LOOK:
		if (scene->_field380) {
			SceneItem::display2(560, 54);
		} else if ((scene->_sceneMode != 4) && (scene->_sceneMode != 3)) {
			scene->_sceneMode = _flag + 4;
			PlayerMover *mover = new PlayerMover();
			Common::Point destPos(139, 106);
			BF_GLOBALS._player.addMover(mover, &destPos, scene);
		}
		return true;
	default:
		return NamedHotspotExt::startAction(action, event);
	}
}

/*--------------------------------------------------------------------------*/

bool Scene560::DeskChair::startAction(CursorType action, Event &event) {
	Scene560 *scene = (Scene560 *)BF_GLOBALS._sceneManager._scene;

	if ((action == CURSOR_USE) && !scene->_field380) {
		scene->setAction(&scene->_action1);
		return true;
	} else {
		return NamedObject::startAction(action, event);
	}
}

bool Scene560::Object2::startAction(CursorType action, Event &event) {
	Scene560 *scene = (Scene560 *)BF_GLOBALS._sceneManager._scene;

	switch (action) {
	case CURSOR_USE:
		if (scene->_field380) {
			SceneItem::display2(560, 54);
		} else {
			ADD_PLAYER_MOVER(289, 108);
		}
		return true;
	default:
		return NamedObjectExt::startAction(action, event);
	}
}

void Scene560::SafeInset::postInit(SceneObjectList *OwnerList) {
	Scene560 *scene = (Scene560 *)BF_GLOBALS._sceneManager._scene;
	FocusObject::postInit();

	_cursorVisage.setVisage(1, 5);

	if (BF_GLOBALS._safeCombination == 172) {
		setFrame(2);
		if (BF_INVENTORY.getObjectScene(INV_NICKEL) == 560) {
			scene->_nickel.postInit();
			scene->_nickel.setVisage(560);
			scene->_nickel.setStrip(2);
			scene->_nickel.setFrame(3);
			scene->_nickel.fixPriority(252);
			scene->_nickel.setPosition(Common::Point(181, 140));
			scene->_nickel.setDetails(560, 47, 48, -1, 1, NULL);
			BF_GLOBALS._sceneItems.remove(&scene->_nickel);
			BF_GLOBALS._sceneItems.push_front(&scene->_nickel);
		}
	} else {
		setFrame(1);
		_item1.setDetails(Rect(143, 68, 159, 85), 560, 49, 50, -1, 1, NULL);
		_item2.setDetails(Rect(159, 68, 175, 85), 560, 49, 50, -1, 1, NULL);
		_item3.setDetails(Rect(175, 68, 191, 85), 560, 49, 50, -1, 1, NULL);
		_item4.setDetails(Rect(143, 86, 159, 102), 560, 49, 50, -1, 1, NULL);
		_item5.setDetails(Rect(159, 86, 175, 102), 560, 49, 50, -1, 1, NULL);
		_item6.setDetails(Rect(175, 86, 191, 102), 560, 49, 50, -1, 1, NULL);
		
		BF_GLOBALS._sceneItems.remove(&_item1);
		BF_GLOBALS._sceneItems.remove(&_item2);
		BF_GLOBALS._sceneItems.remove(&_item3);
		BF_GLOBALS._sceneItems.remove(&_item4);
		BF_GLOBALS._sceneItems.remove(&_item5);
		BF_GLOBALS._sceneItems.remove(&_item6);
		BF_GLOBALS._sceneItems.push_front(&_item6);
		BF_GLOBALS._sceneItems.push_front(&_item5);
		BF_GLOBALS._sceneItems.push_front(&_item4);
		BF_GLOBALS._sceneItems.push_front(&_item3);
		BF_GLOBALS._sceneItems.push_front(&_item2);
		BF_GLOBALS._sceneItems.push_front(&_item1);

		_item1._flag = 1;
		_item2._flag = 2;
		_item3._flag = 3;
		_item4._flag = 4;
		_item5._flag = 5;
		_item6._flag = 6;

		_digit2.postInit();
		_digit2.setVisage(560);
		_digit2.setStrip(3);
		_digit2.setPosition(Common::Point(151, 94));
		_digit2.fixPriority(252);

		_digit1.postInit();
		_digit1.setVisage(560);
		_digit1.setStrip(3);
		_digit1.setPosition(Common::Point(167, 94));
		_digit1.fixPriority(252);

		_digit0.postInit();
		_digit0.setVisage(560);
		_digit0.setStrip(3);
		_digit0.setPosition(Common::Point(183, 94));
		_digit0.fixPriority(252);
		
		int amount = (BF_GLOBALS._safeCombination != 0) ? BF_GLOBALS._safeCombination : 1000;
		
		// Get digit 0 portion
		int remainder = amount % 10;
		amount /= 10;
		_digit0.setFrame(!remainder ? 10 : remainder);

		// Get digit 1 portion
		remainder = amount % 10;
		amount /= 10;
		_digit1.setFrame(!remainder ? 10 : remainder);

		// Get digit 2 portion
		remainder = amount % 10;
		_digit2.setFrame(!remainder ? 10 : remainder);
	}
}

void Scene560::SafeInset::remove() {
	Scene560 *scene = (Scene560 *)BF_GLOBALS._sceneManager._scene;
	
	_item1.remove();
	_item2.remove();
	_item3.remove();
	_item4.remove();
	_item5.remove();
	_item6.remove();
	_digit2.remove();
	_digit1.remove();
	_digit0.remove();
	
	scene->_nickel.remove();

	if (BF_GLOBALS._events.getCursor() == CURSOR_USE) {
		GfxSurface cursor = surfaceFromRes(1, 1, 2);
		BF_GLOBALS._events.setCursor(cursor);
	}

	FocusObject::remove();
}

void Scene560::SafeInset::signal() {
	Scene560 *scene = (Scene560 *)BF_GLOBALS._sceneManager._scene;

	// Recalculate the new total
	int total = 0;
	if (_digit2._frame < 10)
		total = _digit2._frame * 100;
	if (_digit1._frame < 10)
		total += _digit1._frame * 10;
	if (_digit0._frame < 10)
		total += _digit0._frame;

	BF_GLOBALS._safeCombination = !total ? 1000 : total;

	// Check if the combination is correct
	if (total == 172) {
		BF_GLOBALS._sceneObjects->draw();

		if (!BF_GLOBALS.getFlag(fGotPointsForBank)) {
			BF_GLOBALS._uiElements.addScore(50);
			BF_GLOBALS.setFlag(fGotPointsForBank);
		}

		GfxSurface cursor = surfaceFromRes(1, 1, 2);
		BF_GLOBALS._events.setCursor(cursor);

		_item1.remove();
		_item2.remove();
		_item3.remove();
		_item4.remove();
		_item5.remove();
		_item6.remove();
		_digit2.remove();
		_digit1.remove();
		_digit0.remove();

		if (BF_INVENTORY.getObjectScene(INV_NICKEL) == 560) {
			// Nickel is still in the safe, show it
			scene->_nickel.postInit();
			scene->_nickel.setVisage(560);
			scene->_nickel.setStrip(2);
			scene->_nickel.setFrame(3);
			scene->_nickel.fixPriority(252);
			scene->_nickel.setPosition(Common::Point(181, 140));
			scene->_nickel.setDetails(560, 47, 48, -1, 1, NULL);
			BF_GLOBALS._sceneItems.remove(&scene->_nickel);
			BF_GLOBALS._sceneItems.push_front(&scene->_nickel);
		}
	}
}
void Scene560::SafeInset::process(Event &event) {
	if (_bounds.contains(event.mousePos)) {
		if (BF_GLOBALS._events.getCursor() == CURSOR_USE) {
			GfxSurface cursor = _cursorVisage.getFrame(6);
			BF_GLOBALS._events.setCursor(cursor);
		} else {
			BF_GLOBALS._events.proc1();
		}

		if ((event.eventType == EVENT_BUTTON_DOWN) && (BF_GLOBALS._events.getCursor() == CURSOR_WALK) &&
				(event.btnState == 3)) {
			BF_GLOBALS._events.setCursor(CURSOR_USE);
			event.handled = true;
		}
	} else {
		GfxSurface cursor = _cursorVisage.getFrame(7);
		BF_GLOBALS._events.setCursor(cursor);

		if ((event.mousePos.y < BF_INTERFACE_Y) && (event.eventType == EVENT_BUTTON_DOWN)) {
			// Leave the safe view
			event.handled = true;
			remove();
		}
	}
}

bool Scene560::SafeInset::Item::startAction(CursorType action, Event &event) {
	Scene560 *scene = (Scene560 *)BF_GLOBALS._sceneManager._scene;

	switch (action) {
	case CURSOR_USE:
		switch (_flag) {
		case 0: {
			int newFrame = scene->_safeInset._digit2._frame + 1;
			if (newFrame == 11)
				newFrame = 1;

			scene->_safeInset._digit2.setFrame(newFrame);
			break;
		}
		case 1: {
			int newFrame = scene->_safeInset._digit1._frame + 1;
			if (newFrame == 11)
				newFrame = 1;

			scene->_safeInset._digit1.setFrame(newFrame);
			break;
		}
		case 2: {
			int newFrame = scene->_safeInset._digit0._frame + 1;
			if (newFrame == 11)
				newFrame = 1;

			scene->_safeInset._digit0.setFrame(newFrame);
			break;
		}
		case 3: {
			int newFrame = scene->_safeInset._digit2._frame - 1;
			if (newFrame == 0)
				newFrame = 10;

			scene->_safeInset._digit2.setFrame(newFrame);
			break;
		}
		case 4: {
			int newFrame = scene->_safeInset._digit1._frame - 1;
			if (newFrame == 0)
				newFrame = 10;

			scene->_safeInset._digit1.setFrame(newFrame);
			break;
		}
		case 5: {
			int newFrame = scene->_safeInset._digit0._frame - 1;
			if (newFrame == 0)
				newFrame = 10;

			scene->_safeInset._digit0.setFrame(newFrame);
			break;
		}
		default:
			break;
		}
		
		scene->_safeInset.signal();
		scene->_sound1.play(75);
		return true;
	default:
		return NamedHotspotExt::startAction(action, event);
	}
}

bool Scene560::Nickel::startAction(CursorType action, Event &event) {
	if (action == CURSOR_USE) {
		BF_INVENTORY.setObjectScene(INV_NICKEL, 1);
		BF_GLOBALS._uiElements.addScore(10);
		remove();
		return true;
	} else {
		return NamedObject::startAction(action, event);
	}
}

void Scene560::Object5::postInit(SceneObjectList *OwnerList) {
	FocusObject::postInit();
	_item1.setDetails(Rect(110, 48, 189, 102), 560, 43, 44, -1, 1, NULL);
	BF_GLOBALS._sceneItems.remove(&_item1);
	BF_GLOBALS._sceneItems.push_front(&_item1);
}

void Scene560::Object5::remove() {
	Scene560 *scene = (Scene560 *)BF_GLOBALS._sceneManager._scene;

	if (scene->_sceneMode != 3)
		scene->_sceneMode = 0;

	_item1.remove();
	FocusObject::remove();
}

bool Scene560::Object5::Item1::startAction(CursorType action, Event &event) {
	Scene560 *scene = (Scene560 *)BF_GLOBALS._sceneManager._scene;

	if (action == CURSOR_USE) {
		if (!BF_GLOBALS.getFlag(fGotPointsForBox)) {
			BF_GLOBALS._uiElements.addScore(10);
			BF_GLOBALS.setFlag(fGotPointsForBox);
		}

		scene->_safeInset.postInit();
		scene->_safeInset.setVisage(560);
		scene->_safeInset.setStrip(2);
		scene->_safeInset.setPosition(Common::Point(160, 141));
		scene->_safeInset.fixPriority(251);
		scene->_safeInset.setDetails(560, 45, 46, -1);
		
		scene->_sceneMode = 3;
		scene->_object5.remove();

		GfxSurface cursor = surfaceFromRes(1, 5, 6);
		BF_GLOBALS._events.setCursor(cursor);
		return true;
	} else {
		return NamedHotspot::startAction(action, event);
	}
}

/*--------------------------------------------------------------------------*/

bool Scene560::Computer::startAction(CursorType action, Event &event) {
	Scene560 *scene = (Scene560 *)BF_GLOBALS._sceneManager._scene;

	switch (action) {
	case CURSOR_USE:
		if (BF_GLOBALS._bookmark < bTalkedToGrannyAboutSkipsCard) {
			SceneItem::display2(666, 24);
		} else {
			scene->setAction(&scene->_action3);
		}
		return true;
	default:
		return NamedHotspot::startAction(action, event);
	}
}

/*--------------------------------------------------------------------------*/

Scene560::Scene560(): SceneExt() {
	_field380 = _field11EA = 0;
}

void Scene560::postInit(SceneObjectList *OwnerList) {
	SceneExt::postInit();
	loadScene(560);
	setZoomPercents(100, 60, 168, 90);
	_stripManager.addSpeaker(&_gameTextSpeaker);

	_picture1.setDetails(Rect(121, 16, 139, 35), 560, -1, -1, -1, 1, NULL);
	_picture2.setDetails(Rect(139, 16, 157, 35), 560, -1, -1, -1, 1, NULL);
	_picture3.setDetails(Rect(121, 35, 139, 54), 560, -1, -1, -1, 1, NULL);
	_picture4.setDetails(Rect(139, 35, 157, 54), 560, -1, -1, -1, 1, NULL);
	_picture1._flag = 3;
	_picture2._flag = 4;
	_picture3._flag = 2;
	_picture4._flag = 1;

	if (BF_GLOBALS._dayNumber == 0)
		BF_GLOBALS._dayNumber = 3;

	if (BF_GLOBALS._bookmark >= bTalkedToGrannyAboutSkipsCard) {
		_object2.postInit();
		_object2.setVisage(560);
		_object2.setStrip(4);
		_object2.setFrame(1);
		_object2.setPosition(Common::Point(295, 37));
		_object2.setDetails(560, 41, 42, -1, 1, NULL);
	}

	_deskChair.postInit();
	_deskChair.setVisage(561);
	_deskChair.setStrip(3);
	_deskChair.setPosition(Common::Point(81, 149));
	_deskChair.fixPriority(151);
	_deskChair.changeZoom(81);
	
	if (BF_GLOBALS._sceneManager._previousScene == 570) {
		// Returning from using computer
		BF_GLOBALS._events.setCursor(CURSOR_USE);

		_deskChair.hide();

		BF_GLOBALS._player.postInit();
		BF_GLOBALS._player.setVisage(561);
		BF_GLOBALS._player.setStrip(2);
		BF_GLOBALS._player.setFrame(BF_GLOBALS._player.getFrameCount());
		BF_GLOBALS._player._moveDiff.x = 11;
		BF_GLOBALS._player.setPosition(Common::Point(96, 138));
		BF_GLOBALS._player.changeZoom(81);

		_deskChair.setFrame(BF_GLOBALS._player._frame);
		_deskChair.setStrip(BF_GLOBALS._player._strip);
		_deskChair.setPosition(BF_GLOBALS._player._position);

		_field11EA = 0;
		_field380 = 1;
	} else {
		// Entering study through doorway
		_field11EA = 0;
		_field380 = 0;

		BF_GLOBALS._player.postInit();
		BF_GLOBALS._player.setVisage(563);
		BF_GLOBALS._player.animate(ANIM_MODE_1, NULL);
		BF_GLOBALS._player.setObjectWrapper(new SceneObjectWrapper());
		BF_GLOBALS._player.setPosition(Common::Point(79, 110));
		BF_GLOBALS._player._moveDiff.x = 11;
		BF_GLOBALS._player.changeZoom(-1);
		BF_GLOBALS._player.disableControl();
		
		_sceneMode = 10;
		ADD_MOVER(BF_GLOBALS._player, 85, 115);
	}

	_computer.setDetails(Rect(16, 77, 58, 107), 560, 2, -1, -1, 1, NULL);
	_deskChair.setDetails(560, 3, -1, -1, 1, NULL);
	_chair.setDetails(Rect(163, 64, 196, 102), 560, 13, 25, 36, 1, NULL);
	_lamp.setDetails(Rect(197, 43, 214, 56), 560, 7, 19, 30, 1, NULL);
	_item4.setDetails(Rect(121, 18, 156, 54), 560, 8, 20, 31, 1, NULL);
	_trophy.setDetails(Rect(259, 52, 275, 63), 560, 10, 22, 33, 1, NULL);
	_watercolours.setDetails(Rect(214, 48, 239, 64), 560, 12, 24, 35, 1, NULL);
	_fileCabinets.setDetails(Rect(0, 47, 49, 100), 560, 14, 26, 37, 1, NULL);
	_certificate.setDetails(Rect(280, 51, 292, 62), 560, 11, 23, 34, 1, NULL);
	_bookcase.setDetails(Rect(176, 0, 319, 103), 560, 9, 21, 32, 1, NULL);
	_desk.setDetails(1, 560, 6, 10, 29, 1);
	_item12.setDetails(Rect(62, 16, 96, 101), 560, 51, 52, 53, 1, NULL);
	_carpet.setDetails(Rect(0, 103, 319, 167), 560, 5, 16, 28, 1, NULL);
	_office.setDetails(Rect(0, 0, 320, 168), 560, 4, -1, -1, 1, NULL);

	BF_GLOBALS._player.enableControl();
}

void Scene560::signal() {
	switch (_sceneMode) {
	case 1:
		_object6.hide();
		_object6.remove();
		BF_GLOBALS._player.enableControl();
		break;
	case 5:
	case 6:
	case 7:
	case 8:
		BF_GLOBALS._player._strip = 4;
		BF_GLOBALS._player._frame = 1;
		BF_GLOBALS._player.disableControl();

		_object6.postInit();
		_object6.setVisage(560);
		_object6.setStrip(1);
		_object6.setFrame(_sceneMode - 4);
		_object6.setPosition(Common::Point(160, 141));
		_object6.fixPriority(250);
		_stripManager.start(_sceneMode + 5595, this);
		_sceneMode = 1;
		break;
	case 9:
		_object6._strip = 4;
		_object6._frame = 1;

		if (BF_GLOBALS._safeCombination == 172) {
			if (_sceneMode != 3) {
				_safeInset.postInit();
				_safeInset.setVisage(560);
				_safeInset.setStrip(2);
				_safeInset.setPosition(Common::Point(160, 141));
				_safeInset.fixPriority(251);
				_safeInset.setDetails(560, 45, 46, -1);

				_sceneMode = 3;
			}
		} else if ((_sceneMode != 3) && (_sceneMode != 4)) {
			if (!BF_GLOBALS.getFlag(fGotPointsForPunch)) {
				BF_GLOBALS._uiElements.addScore(10);
				BF_GLOBALS.setFlag(fGotPointsForPunch);
			}
			
			_object5.postInit();
			_object5.setVisage(560);
			_object5.setStrip(2);
			_object5.setFrame(4);
			_object5.setPosition(Common::Point(160, 141));
			_object5.fixPriority(251);
			_object5.setDetails(560, 43, 44, -1);

			_sceneMode = 4;
		}
		break;
	case 10:
		_field11EA = 0;
		BF_GLOBALS._player.enableControl();
		break;
	case 11:
		BF_GLOBALS._sceneManager.changeScene(270);
		break;
	}
}

void Scene560::process(Event &event) {
	if ((event.eventType == EVENT_BUTTON_DOWN) && (BF_GLOBALS._events.getCursor() == CURSOR_WALK) &&
			(_field380 == 1) && !_action) {
		_destPosition = event.mousePos;
		BF_GLOBALS._player.disableControl();
		setAction(&_action2);

		event.handled = true;
	}

	SceneExt::process(event);
}

void Scene560::dispatch() {
	if (!_field11EA && (BF_GLOBALS._player._position.y < 105)) {
		_field11EA = 1;
		BF_GLOBALS._player.disableControl();
		BF_GLOBALS._sceneManager.changeScene(270);
	}

	SceneExt::dispatch();
}

} // End of namespace BlueForce

} // End of namespace TsAGE
