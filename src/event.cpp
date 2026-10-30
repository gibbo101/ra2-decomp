// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"
#include "geometry.h"
#include "rect.h"
#include "event.h"
#include "dbgprint.h"
#include "globals.h"
#include "_map.h"
#include "_rules.h"
#include "anim.h"
#include "bomb.h"
#include "building.h"
#include "builtype.h"
#include "cell.h"
#include "conquer.h"
#include "foot.h"
#include "goptions.h"
#include "house.h"
#include "ipxmgr.h"
#include "msgbox.h"
#include "ownertalk.h"
#include "queue.h"
#include "rules.h"
#include "saveload.h"
#include "scenario.h"
#include "session.h"
#include "slavemgr.h"
#include "spawnmgr.h"
#include <stddef.h>
#include "stringtable.h"
#include "surface.h"
#include "team.h"
#include "techno.h"
#include "techtype.h"
#include "unit.h"
#include "unittype.h"

// SYNTHETIC: GAMEMD 0x004C6360 SYMBOL
// _$E2
// SYNTHETIC: GAMEMD 0x004C6390 SYMBOL
// _$E5
// SYNTHETIC: GAMEMD 0x004C63B0 SYMBOL
// _$E8
// SYNTHETIC: GAMEMD 0x004C63D0 SYMBOL
// _$E11
// SYNTHETIC: GAMEMD 0x004C63F0 SYMBOL
// _$E14
// SYNTHETIC: GAMEMD 0x004C6410 SYMBOL
// _$E17
// SYNTHETIC: GAMEMD 0x004C6430 SYMBOL
// _$E20
// SYNTHETIC: GAMEMD 0x004C6470 SYMBOL
// _$E23
// SYNTHETIC: GAMEMD 0x004C64A0 SYMBOL
// _$E26
// SYNTHETIC: GAMEMD 0x004C64D0 SYMBOL
// _$E29
// SYNTHETIC: GAMEMD 0x004C6500 SYMBOL
// _$E32
// SYNTHETIC: GAMEMD 0x004C6510 SYMBOL
// _$E35
// SYNTHETIC: GAMEMD 0x004C6530 SYMBOL
// _$E38
// SYNTHETIC: GAMEMD 0x004C6550 SYMBOL
// _$E41

// FUNCTION: GAMEMD 0x004C65A0
EventClass::EventClass(int index, SpecialClass data)
{
	if (index >= 0) {
		ID = index;
		Type = SPECIAL;
		Data.Options.Data = data;
		Frame = ::Frame;
	} else {
		ID = -1;
		Type = EMPTY;
		Frame = ::Frame;
	}
}

// FUNCTION: GAMEMD 0x004C65E0
EventClass::EventClass(int index, unsigned char type, TargetClass target)
{
	DebugString("Adding event %s\n", EventNames[type]);
	if (index >= 0) {
		ID = index;
		Type = type;
		Data.Target.Whom = target;
		Frame = ::Frame;
	} else {
		ID = -1;
		Type = EMPTY;
		Frame = ::Frame;
	}
}

// FUNCTION: GAMEMD 0x004C6650
EventClass::EventClass(int index, unsigned char type, Cell const & cell)
{
	DebugString("Adding event %s\n", EventNames[type]);
	if (index >= 0) {
		ID = index;
		Type = type;
		Data.SellCell.Where = cell;
		Frame = ::Frame;
	} else {
		ID = -1;
		Type = EMPTY;
		Frame = ::Frame;
	}
}

// FUNCTION: GAMEMD 0x004C66C0
EventClass::EventClass(int index, unsigned char type)
{
	DebugString("Adding event %s\n", EventNames[type]);
	if (index >= 0) {
		ID = index;
		Type = type;
	} else {
		ID = -1;
		Type = EMPTY;
	}
	Frame = ::Frame;
}

// FUNCTION: GAMEMD 0x004C6720
EventClass::EventClass(int index, unsigned char type, int val)
{
	DebugString("Adding event %s\n", EventNames[type]);
	if (index >= 0) {
		ID = index;
		Type = type;
		Data.General.Value = val;
		Frame = ::Frame;
	} else {
		ID = -1;
		Type = EMPTY;
		Frame = ::Frame;
	}
}

// FUNCTION: GAMEMD 0x004C6780
EventClass::EventClass(int index, unsigned char type, TargetClass src, TargetClass dest)
{
	DebugString("Adding event %s\n", EventNames[type]);
	if (index >= 0) {
		ID = index;
		Type = type;
		Data.NavCom.Whom = src;
		Data.NavCom.Where = dest;
		Frame = ::Frame;
	} else {
		ID = -1;
		Type = EMPTY;
		Frame = ::Frame;
	}
}

// FUNCTION: GAMEMD 0x004C6800
EventClass::EventClass(int index, AnimType anim, HousesType owner, Coord const & coord)
{
	DebugString("Adding event ANIMATION\n");
	if (index >= 0) {
		ID = index;
		Type = ANIMATION;
		Data.Anim.What = anim;
		Data.Anim.Owner = owner;
		Data.Anim.Where = coord;
		Frame = ::Frame;
	} else {
		ID = -1;
		Type = EMPTY;
		Frame = ::Frame;
	}
}

// FUNCTION: GAMEMD 0x004C6860
EventClass::EventClass(int index, TargetClass src, MissionType mission, TargetClass target, TargetClass destination, TargetClass follow)
{
	if (index >= 0) {
		ID = index;
		Type = MEGAMISSION;
		Data.MegaMission.IsPlanningEvent = false;
		Data.MegaMission.Whom = src;
		Data.MegaMission.Mission = mission;
		Data.MegaMission.Target = target;
		Data.MegaMission.Destination = destination;
		Data.MegaMission.Follow = follow;
		Frame = ::Frame;
	} else {
		ID = -1;
		Type = EMPTY;
		Frame = ::Frame;
	}
}

// FUNCTION: GAMEMD 0x004C68E0
EventClass::EventClass(int index, TargetClass src, MissionType mission, TargetClass target, TargetClass destination, SpeedType speed, MPHType maxspeed)
{
	DebugString("Adding event MEGAMISSION_F\n");
	if (index >= 0) {
		ID = index;
		Type = MEGAMISSION_F;
		Data.MegaMission_F.Whom = src;
		Data.MegaMission_F.Mission = mission;
		Data.MegaMission_F.Target = target;
		Data.MegaMission_F.Destination = destination;
		Data.MegaMission_F.Speed = speed;
		Data.MegaMission_F.MaxSpeed = maxspeed;
		Frame = ::Frame;
	} else {
		ID = -1;
		Type = EMPTY;
		Frame = ::Frame;
	}
}

// FUNCTION: GAMEMD 0x004C6970
EventClass::EventClass(int index, unsigned char type, RTTIType object, int id, BOOL naval)
{
	DebugString("Adding event %s\n", EventNames[type]);
	if (index >= 0) {
		ID = index;
		Type = type;
		Data.Specific.Type = object;
		Data.Specific.ID = id;
		Data.Specific.IsNaval = naval;
		Frame = ::Frame;
	} else {
		ID = -1;
		Type = EMPTY;
		Frame = ::Frame;
	}
}

// FUNCTION: GAMEMD 0x004C69E0
EventClass::EventClass(int index, unsigned char type, RTTIType object, Cell const & cell)
{
	DebugString("Adding event %s\n", EventNames[type]);
	if (index >= 0) {
		ID = index;
		Type = type;
		Data.Place.Type = object;
		Data.Place.ID = -1;
		Data.Place.IsNaval = 0;
		Data.Place.Where = cell;
		Frame = ::Frame;
	} else {
		ID = -1;
		Type = EMPTY;
		Frame = ::Frame;
	}
}

// FUNCTION: GAMEMD 0x004C6A60
EventClass::EventClass(int index, unsigned char type, RTTIType object, int id, Cell const & cell)
{
	DebugString("Adding event %s\n", EventNames[type]);
	if (index >= 0) {
		ID = index;
		Type = type;
		Data.Place.Type = object;
		Data.Place.ID = id;
		Data.Place.IsNaval = 0;
		Data.Place.Where = cell;
		Frame = ::Frame;
	} else {
		ID = -1;
		Type = EMPTY;
		Frame = ::Frame;
	}
}

// FUNCTION: GAMEMD 0x004C6AE0
EventClass::EventClass(int index, unsigned char type, RTTIType object, int id, BOOL naval, Cell const & cell)
{
	DebugString("Adding event %s\n", EventNames[type]);
	if (index >= 0) {
		ID = index;
		Type = type;
		Data.Place.Type = object;
		Data.Place.ID = id;
		Data.Place.IsNaval = naval;
		Data.Place.Where = cell;
		Frame = ::Frame;
	} else {
		ID = -1;
		Type = EMPTY;
		Frame = ::Frame;
	}
}

// FUNCTION: GAMEMD 0x004C6B60
EventClass::EventClass(int index, unsigned char type, int id, Cell const & cell)
{
	DebugString("Adding event %s\n", EventNames[type]);
	if (index >= 0) {
		ID = index;
		Type = type;
		Data.Special.ID = id;
		Data.Special.Where = cell;
		Frame = ::Frame;
	} else {
		ID = -1;
		Type = EMPTY;
		Frame = ::Frame;
	}
}

// FUNCTION: GAMEMD 0x004C6BE0
EventClass::EventClass(int index, unsigned char type, RTTIType object, int id)
{
	DebugString("Adding event %s\n", EventNames[type]);
	if (index >= 0) {
		ID = index;
		Type = type;
		Data.Specific.Type = object;
		Data.Specific.ID = id;
		Frame = ::Frame;
	} else {
		ID = -1;
		Type = EMPTY;
		Frame = ::Frame;
	}
}

// FUNCTION: GAMEMD 0x004C6C50
EventClass::EventClass(int index, IPXAddressClass & addr, signed char value)
{
	NetNumType net;
	NetNodeType node;

	if (index >= 0) {
		ID = index;
		Type = ADDRESS_CHANGE;
		addr.Get_Address(net, node);
		Data.Address.Node = *(unsigned int *)node;
		Data.Address.Value = value;
		Frame = ::Frame;
	} else {
		ID = -1;
		Type = EMPTY;
		Frame = ::Frame;
	}
}

void __fastcall FUN_00637E00(EventClass * event);
void __fastcall FUN_006386E0(TechnoClass * techno, EventClass * event);
namespace Game { namespace Network { extern int Tournament; } }

// FUNCTION: GAMEMD 0x004C6CB0
void EventClass::Execute(void)
{
	TechnoClass * techno = NULL;
	BuildingClass * building = NULL;
	AnimClass * anim = NULL;
	HouseClass * house = Houses[ID];
	HouseClass * hptr = NULL;
	wchar_t const * str = NULL;
	wchar_t txt[80];
	int i;
	int index;
	unsigned int ul;

	switch (Type) {

		case PLAN_CONNECT:
		case PLAN_COMMIT:
		case PLAN_NODE_DELETE:
			FUN_00637E00(this);
			break;

		case POWERON:
			building = Data.Target.Whom.As_Building();
			FUN_006386E0(building, NULL);
			if (building && building->IsActive && !building->IsInLimbo && !building->Class->IsFirestormWall) {
				building->Turn_On();
			}
			break;

		case POWEROFF:
			building = Data.Target.Whom.As_Building();
			FUN_006386E0(building, NULL);
			if (building && building->IsActive && !building->IsInLimbo && !building->Class->IsFirestormWall) {
				building->Turn_Off();
			}
			break;

		case ARCHIVE:
			techno = Data.NavCom.Whom.As_Techno();
			FUN_006386E0(techno, NULL);
			if (techno && techno->IsActive) {
				techno->ArchiveTarget = Data.NavCom.Where.As_Abstract();
			}
			break;

		case ALLY:
			hptr = Houses[Data.General.Value];
			if (house->Is_Ally(hptr)) {
				house->Make_Enemy((HousesType)Data.General.Value, true);
			} else {
				house->Make_Ally((HousesType)Data.General.Value, true);
			}
			break;

		case ALL_CHEER:
			if (house != NULL) {
				house->Cheer();
			}
			break;

		case DESTRUCT:
			house->Flag_To_Die();
			break;

		case SPECIAL:
			if (house != NULL) {
				Special = Data.Options.Data;
				Scen->Special = Data.Options.Data;

				swprintf(txt, StringTable::LoadString("TXT_SPECIAL_WARNING", NULL, "D:\\ra2mdpost\\Event.CPP", 953), house->UIName);
				Session.Messages.Add_Message(NULL, 0, txt, house->Scheme, TextPrintType(TPF_6PT_GRAD|TPF_USE_GRAD_PAL|TPF_FULLSHADOW), 1200, false);
				Map.Flag_To_Redraw();
			}
			break;

		case REPAIR:
			techno = Data.Target.Whom.As_Techno();
			FUN_006386E0(techno, NULL);
			if (techno && techno->IsActive) {
				techno->Repair(-1);
			}
			break;

		case SELL:
			techno = Data.Target.Whom.As_Techno();
			FUN_006386E0(techno, NULL);
			if (techno && techno->IsActive && techno->House->HeapID == ID) {
				if (techno->Fetch_RTTI() == RTTI_BUILDING || ((techno->Fetch_RTTI() == RTTI_UNIT || techno->Fetch_RTTI() == RTTI_AIRCRAFT) && Map[techno->Center_Coord()].Cell_Building() != NULL)) {
					techno->Sell_Back(-1);
				}
			}
			break;

		case SELLCELL:
			house->Sell_Wall(Cell(Data.SellCell.Where.X, Data.SellCell.Where.Y), false);
			break;

		case ANIMATION:
		{
			Coord coord(Data.Anim.Where.X, Data.Anim.Where.Y, 0);
			coord.Z = Map.Get_Height_GL(coord);
			if (Map[coord].IsUnderBridge) {
				coord.Z += BRIDGE_LEPTON_HEIGHT;
			}
			if (Data.Anim.What == ANIM_NONE) {
				anim = new AnimClass(Rule->MoveFlash, coord);
			} else {
				anim = new AnimClass(AnimTypes[Data.Anim.What], coord);
			}
			if (anim) {
				if (Data.Anim.Owner != HOUSE_NONE && !Houses[Data.Anim.Owner]->Is_Player_Control()) {
					anim->Make_Invisible();
				}
			}
			break;
		}

		case PLACE:
			house->Place_Object(Data.Place.Type, Data.Place.ID, Data.Place.IsNaval, Cell(Data.Place.Where.X, Data.Place.Where.Y));
			break;

		case PRODUCE:
			house->Begin_Production(Data.Specific.Type, Data.Specific.ID, Data.Specific.IsNaval, false);
			break;

		case SUSPEND:
			house->Suspend_Production(Data.Specific.Type, Data.Specific.ID, Data.Specific.IsNaval);
			break;

		case ABANDON:
			house->Abandon_Production(Data.Specific.Type, Data.Specific.ID, Data.Specific.IsNaval, false);
			break;

		case ABANDON_ALL:
			house->Abandon_Production(Data.Specific.Type, Data.Specific.ID, Data.Specific.IsNaval, true);
			break;

		case PRIMARY:
			building = Data.Target.Whom.As_Building();
			FUN_006386E0(building, NULL);
			if (building && building->IsActive) {
				building->Toggle_Primary();
			}
			break;

		case MEGAMISSION_F:
		case MEGAMISSION:
			techno = Data.MegaMission.Whom.As_Techno();
			if (Data.MegaMission.IsPlanningEvent == true) {
				FUN_00637E00(this);
				break;
			}
			FUN_006386E0(techno, this);
			if (techno != NULL && techno->IsActive && techno->Strength > 0 && !techno->IsInLimbo) {

				ObjectClass * object = NULL;
				if (Data.MegaMission.Target.Is_Valid()) {
					object = Data.MegaMission.Target.As_Object();
					if (object != NULL && (!object->IsActive || object->Strength == 0 || object->IsInLimbo)) {
						break;
					}
				}

				if (Data.MegaMission.Destination.Is_Valid()) {
					object = Data.MegaMission.Destination.As_Object();
					if (object != NULL && (!object->IsActive || object->Strength == 0 || object->IsInLimbo)) {
						break;
					}
				}

				if (Data.MegaMission.Mission == MISSION_ENTER && techno->CurrentMission == MISSION_ENTER && techno->Is_Foot()) {
					AbstractClass * target = Data.MegaMission.Destination.As_Abstract();
					if (target != NULL && target->Fetch_RTTI() == RTTI_BUILDING && techno->FUN_0065AD30(0) == target) {
						break;
					}
				}

				if (!techno->IsTethered) {
					techno->Transmit_Message(RADIO_OVER_OUT);
				} else {
					TechnoClass * radio = techno->FUN_0065AD30(0);
					if (radio != NULL && radio->IsActive && radio->Fetch_RTTI() == RTTI_BUILDING && ((BuildingClass *)radio)->Class->IsDockUnload) {
						techno->Transmit_Message(RADIO_OVER_OUT);
						techno->IsTethered = false;
					}
				}
				if (techno->QueueUpToEnter != NULL) {
					techno->QueueUpToEnter = NULL;
				}
				if (techno->Is_Foot()) {
					if (((FootClass *)techno)->Team && Data.MegaMission.Mission != MISSION_UNLOAD) {
						((FootClass *)techno)->Team->Remove((FootClass *)techno, -1, true);
					}
				}

				if (object != NULL) {
					if (PlayerPtr->Is_Ally(techno)) {
						object->Clicked_As_Target();
					}
				}

				MissionType mission = (MissionType)techno->RespondMegaEventMission(this);
				techno->QueueMission(mission, false);

				FootClass * foot;
				if (techno->AbstractFlags & 4) {
					foot = (FootClass *)techno;
				} else {
					foot = NULL;
				}
				if (foot != NULL) {
					foot->SuspendedNavCom = NULL;
				}
				techno->SuspendedTarCom = NULL;

				if (techno->SlaveManager != NULL && mission != MISSION_ATTACK) {
					techno->SlaveManager->AllGuard();
				}

				if (Data.MegaMission.Mission == MISSION_GUARD_AREA && techno->Is_Foot()) {
					techno->Assign_Target(NULL);
					techno->Assign_Destination(Data.MegaMission.Target.As_Abstract());
					techno->ArchiveTarget = Data.MegaMission.Target.As_Abstract();
				} else {
					if (foot != NULL) {
						foot->ArchiveTarget = NULL;
						foot->Clear_Navigation_List();
					}
					AbstractClass * target = Data.MegaMission.Target.As_Abstract();
					techno->Assign_Target(target);
					techno->Assign_Destination(Data.MegaMission.Destination.As_Abstract());
					if (techno->Techno_Type_Class()->OpenTopped && target != NULL) {
						techno->SetTargetForPassengers(target);
					}
					if (Data.MegaMission.Follow.Is_Valid()) {
						techno->vt_entry_47C(Data.MegaMission.Follow.As_Abstract());
					}
				}
			}
			break;

		case IDLE:
			techno = Data.Target.Whom.As_Techno();
			FUN_006386E0(techno, NULL);
			if (techno != NULL && techno->IsActive && !techno->IsInLimbo && !techno->IsTethered) {

				if (techno->CurrentMission == MISSION_CONSTRUCTION || techno->CurrentMission == MISSION_DECONSTRUCTION) {
					break;
				}

				if (!techno->IsOnBridge && Map[(Coord const &)techno->PositionCoord].Ramp == 0 && techno->Is_On_Elevation()) {
					break;
				}

				if (techno->Is_Foot()) {
					((FootClass *)techno)->NavQueue.Clear();
					((FootClass *)techno)->Clear_Navigation_List();
					((FootClass *)techno)->CurrentPath = PATH_NONE;
					((FootClass *)techno)->NextWaypoint = 0;
					((FootClass *)techno)->WaypointOffsetCell = Cell(0, 0);
					((FootClass *)techno)->WaypointTargetCell = Cell(0, 0);
				}
				techno->SendToEachLink(RADIO_OVER_OUT);
				techno->Assign_Destination(NULL);
				techno->Assign_Target(NULL);
				if (techno->Techno_Type_Class()->BalloonHover) {
					techno->Assign_Destination(NULL);
					techno->Assign_Target(NULL);
				}
				if (techno->SpawnManager != NULL) {
					techno->SpawnManager->ResetTarget();
				}
				if (techno->Techno_Type_Class()->OpenTopped) {
					techno->SetTargetForPassengers(NULL);
				}
				if (techno->Fetch_RTTI() == RTTI_UNIT) {
					if (((UnitClass *)techno)->Class->Harvester && (techno->CurrentMission == MISSION_HARVEST || techno->CurrentMission == MISSION_RETURN)) {
						techno->QueueMission(MISSION_GUARD, false);
						techno->Commence();
					}
					if (techno->SlaveManager != NULL) {
						techno->SlaveManager->AllGuard();
					}
				}
			}
			break;

		case DEPLOY:
			techno = Data.Target.Whom.As_Techno();
			FUN_006386E0(techno, NULL);
			if (techno != NULL && techno->IsActive && !techno->IsInLimbo && !techno->IsTethered && techno->EMPLockRemaining == 0) {

				if (!techno->IsOnBridge) {
					if (Map[(Coord const &)techno->PositionCoord].Ramp == 0 && techno->Is_On_Elevation()) {
						break;
					}
				}

				if (techno->CurrentMission == MISSION_CONSTRUCTION || techno->CurrentMission == MISSION_DECONSTRUCTION || techno->Fetch_RTTI() == RTTI_AIRCRAFT) {
					break;
				}

				bool should_unload = false;
				Cell cell = techno->PositionCell;
				if (cell == CELL_NONE) {
					should_unload = true;
				} else {
					building = Map[cell].Cell_Building();
					if (building == NULL || !building->Class->IsWeaponsFactory) {
						should_unload = true;
					}
				}

				if (should_unload) {
					techno->Transmit_Message(RADIO_OVER_OUT);
					techno->Assign_Destination(NULL);
					techno->Assign_Target(NULL);
					techno->QueueMission(MISSION_UNLOAD, false);
				}
			}
			break;

		case DETONATE:
			techno = Data.Target.Whom.As_Techno();
			FUN_006386E0(techno, NULL);
			if (techno != NULL && techno->AttachedBomb != NULL) {
				techno->AttachedBomb->Detonate();
			}
			break;

		case SCATTER:
			techno = Data.Target.Whom.As_Techno();
			FUN_006386E0(techno, NULL);
			if (techno != NULL && techno->Is_Foot() && techno->IsActive && !techno->IsInLimbo && !techno->IsTethered) {
				((FootClass *)techno)->IsScattering = true;
				techno->Scatter(COORD_NONE, true, false);
			}
			break;

		case SPECIAL_PLACE:
			house->Place_Special_Blast((SuperWeaponType)Data.Special.ID, Cell(Data.Special.Where.X, Data.Special.Where.Y));
			break;

		case EXIT:
			DebugString("Processing EXIT event on frame %d\n", ::Frame);
			PlayerAborts = true;
			break;

		case OPTIONS:
			if (!Session.Play) {
				SpecialDialog = SDLG_OPTIONS;
			}
			break;

		case GAMESPEED:
		{
			wchar_t msg[256];
			Options.GameSpeed = Data.General.Value;

			house = Houses[ID];
			if (house != PlayerPtr && house != NULL) {
				str = StringTable::LoadString("TXT_PLAYER_CHANGED_SPEED", NULL, "D:\\ra2mdpost\\Event.CPP", 1427);
				if (str != NULL && wcslen(str) != 0) {
					swprintf(msg, str, house->UIName);
					Session.Messages.Add_Message(NULL, 0, msg, house->Scheme, TextPrintType(TPF_6PT_GRAD|TPF_USE_GRAD_PAL|TPF_FULLSHADOW), Rule->MessageDelay * TICKS_PER_MINUTE, false);
				}
			}
			break;
		}

		case RESPONSE_TIME:
			Session.MaxAhead = Data.FrameInfo.Delay;
			break;

		case SAVEGAME:
			if (SpecialDialog == SDLG_NONE) {
				CDTimerClass<SystemTimerClass> timer;
				timer = TICKS_PER_SECOND * 4;

				WWMessageBox().Process(StringTable::LoadString("TXT_SAVING_GAME", NULL, "D:\\ra2mdpost\\Event.CPP", 1460), StringTable::LoadString("TXT_NONE", NULL, "D:\\ra2mdpost\\Event.CPP", 1460), NULL, NULL);

				Save_Game("SAVEGAME.NET", StringTable::LoadString("TXT_MULTIPLAYER_GAME", NULL, "D:\\ra2mdpost\\Event.CPP", 1462));

				while (timer > 0) {
					Call_Back();
				}

				HiddenSurface->Fill(0);
				Map.Flag_To_Redraw(GS_REDRAW_ALL);
				Map.Render();
			} else {
				Save_Game("SAVEGAME.NET", StringTable::LoadString("TXT_MULTIPLAYER_GAME", NULL, "D:\\ra2mdpost\\Event.CPP", 1473));
			}
			break;

		case ADDPLAYER:
			if (ID != PlayerPtr->HeapID) {
				delete [] Data.Variable.Pointer;
			}
			break;

		case REMOVEPLAYER:
			DebugString("Executing REMOVEPLAYER event. Frame is %d\n", ::Frame);
			index = Data.General.Value;

			house = Houses[index];
			if (Session.Type == GAME_INTERNET && Session.ViaPacketRouter && house->FUN_0050B730()) {
				OwnerTalk.Send_Remove_Player(index);
			}
			if ((Session.Type == GAME_INTERNET && Game::Network::Tournament) || house->FUN_0050B730()) {
				house->Flag_To_Die();
			}
			break;

		case FALLBACK_HOST:
			DebugString("Received FALLBACKHOST event\n");
			Session.unknown_3094 = Data.General.Value;
			break;

		case ADDRESS_CHANGE:
		{
			DebugString("Executing ADDRESSCHANGE event for player %d\n", Data.Address.Value);
			NetNodeType node;
			NetNumType net;
			memset(node, 0, sizeof(node));
			memset(net, 0, sizeof(net));
			memcpy(node, &Data.Address.Node, sizeof(Data.Address.Node));
			IPXAddressClass address(net, node, 0);
			DebugString("New address is %s\n", address.As_String(&address));
			Ipx.Set_Connection_Address(Data.Address.Value, &address);
			OwnerTalk.Set_Address(Data.Address.Value, &address);
			for (i = 0; i < Session.Players.Count(); i++) {
				if (Session.Players[i]->Player.ID == Data.Address.Value) {
					if (i < Session.Players.Count()) {
						Session.Players[i]->Address = address;
					}
					break;
				}
			}
			break;
		}

		case ABOUT_TO_EXIT:
		{
			DebugString("Processing ABOUTTOEXIT event on frame %d\n", ::Frame);
			if (Session.Am_I_Master()) {
				OutList.Add(EventClass(PlayerPtr->HeapID, EXIT));
				Session.unknown_3078 = true;
			}
			Session.unknown_3098 = true;
			index = OwnerTalk.Update_Master();
			if (index != -1) {
				hptr = Houses[index];
				if (hptr != NULL) {
					if (Session.Players.Count() > 2) {
						IPXAddressClass * addr = NULL;
						for (i = 0; i < Session.Players.Count(); i++) {
							if (Session.Players[i]->Player.ID == index) {
								addr = &Session.Players[i]->Address;
								Session.PacketRouterPort = Session.PlayerPorts[i];
								break;
							}
						}
						NetNumType net;
						NetNodeType node;
						addr->Get_Address(net, node);
						Session.PacketRouterAddress.Set_Address(net, node, 0);
						DebugString("Session.PacketRouterAddress = %s\n", Session.PacketRouterAddress.As_String(NULL));
						DebugString("Session.PacketRouterPort = %d\n", Session.PacketRouterPort);
						if (!Session.Am_I_Master()) {
							Ipx.FUN_00543050(&Session.PacketRouterAddress);
						}
					}
					wcscpy(Session.MasterPlayerName, hptr->UIName);
					DebugString("New game host is %ls\n", Session.MasterPlayerName);
					Session.MasterPlayerID = index;
					if (Session.Am_I_Master()) {
						OwnerTalk.Send_Player_List();
					}
				}
				Session.unknown_30A0 = ::Frame;
				Session.unknown_30A8 = index;
				Session.unknown_3094 = -1;
				Session.unknown_30A4 = ((Session.MaxAhead + ::Frame + 10) / 10) * 10;
			}
			Session.unknown_3098 = false;
			break;
		}

		case LATENCYFUDGE:
		{
			wchar_t msg[256];
			DebugString("Executing LATENCYFUDGE event. Frame is %d\n", ::Frame);
			Session.LatencyFudge = Data.General.Value;
			DebugString("LatencyFudge is %d\n", Session.LatencyFudge);

			house = Houses[ID];
			if (house != PlayerPtr && house != NULL) {
				str = StringTable::LoadString("TXT_PLAYER_CHANGED_LATENCY", NULL, "D:\\ra2mdpost\\Event.CPP", 1744);
				if (str != NULL && wcslen(str) != 0) {
					swprintf(msg, str, house->UIName);
					Session.Messages.Add_Message(NULL, 0, msg, house->Scheme, TextPrintType(TPF_6PT_GRAD|TPF_USE_GRAD_PAL|TPF_FULLSHADOW), Rule->MessageDelay * TICKS_PER_MINUTE, false);
				}
			}
			break;
		}

		case TIMING:
			Data.Timing.MaxAhead -= Scen->Special.IsFogOfWar ? 10 : 0;

			if (Data.Timing.MaxAhead > Session.MaxAhead || Data.Timing.FrameSendRate > Session.FrameSendRate) {
				NewMaxAheadFrame1 = Frame;
				NewMaxAheadFrame2 = Data.Timing.FrameSendRate * ((Data.Timing.FrameSendRate + Data.Timing.MaxAhead + Frame - 1) / Data.Timing.FrameSendRate);
			} else {
				NewMaxAheadFrame1 = 0;
				NewMaxAheadFrame2 = 0;
			}

			Session.DesiredFrameRate = Data.Timing.DesiredFrameRate;
			ul = Session.MaxMaxAhead;
			Session.MaxAhead = Data.Timing.MaxAhead;

			if (ul <= Session.MaxAhead) {
				Session.MaxMaxAhead = Session.MaxAhead;
			}

			Session.FrameSendRate = Data.Timing.FrameSendRate;
			break;

		case PROCESS_TIME:
			for (i = 0; i < Session.Players.Count(); i++) {
				if (ID == Session.Players[i]->Player.ID) {
					Session.Players[i]->Player.ProcessTime = Data.ProcessTime.AverageTicks;
					break;
				}
			}
			break;

		case PAGEUSER:
			if (!Session.Play) {
				SpecialDialog = SDLG_WOL_OPTIONS;
			}
			break;

		default:
			break;
	}
}
