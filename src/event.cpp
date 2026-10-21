// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"
#include "event.h"
#include "dbgprint.h"
#include "globals.h"

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
		Frame = ::Frame;
	} else {
		ID = -1;
		Type = EMPTY;
		Frame = ::Frame;
	}
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

// SYNTHETIC: GAMEMD 0x004C81D0 SYMBOL
// _$E4
struct ZeroQuad {
	int A, B, C, D;
	ZeroQuad(void) : A(0), B(0), C(0), D(0) {}
};
ZeroQuad DAT_008B3A08;
