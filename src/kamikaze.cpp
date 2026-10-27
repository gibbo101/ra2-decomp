// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#define INCLUDE_COM
#include "always.h"
#include "geometry.h"
#include "kamikaze.h"
#include "aircraft.h"
#include "airctype.h"
#include "cell.h"
#include "map.h"
#include "swizzle.h"

// SYNTHETIC: GAMEMD 0x0054E050 SYMBOL
// _$E2
// SYNTHETIC: GAMEMD 0x0054E080 SYMBOL
// _$E5
// SYNTHETIC: GAMEMD 0x0054E0A0 SYMBOL
// _$E8
// SYNTHETIC: GAMEMD 0x0054E0C0 SYMBOL
// _$E11
// SYNTHETIC: GAMEMD 0x0054E0E0 SYMBOL
// _$E14
// SYNTHETIC: GAMEMD 0x0054E100 SYMBOL
// _$E17
// SYNTHETIC: GAMEMD 0x0054E120 SYMBOL
// _$E20
// SYNTHETIC: GAMEMD 0x0054E160 SYMBOL
// _$E23
// SYNTHETIC: GAMEMD 0x0054E190 SYMBOL
// _$E26
// SYNTHETIC: GAMEMD 0x0054E1C0 SYMBOL
// _$E29
// SYNTHETIC: GAMEMD 0x0054E1F0 SYMBOL
// _$E32
// SYNTHETIC: GAMEMD 0x0054E200 SYMBOL
// _$E35
// SYNTHETIC: GAMEMD 0x0054E220 SYMBOL
// _$E38
// SYNTHETIC: GAMEMD 0x0054E240 SYMBOL
// _$E47
// SYNTHETIC: GAMEMD 0x0054E260 SYMBOL
// _$E119
Kamikaze KamikazeInstance;

// SYNTHETIC: GAMEMD 0x0054E2D0 SYMBOL
// _$E117

// FUNCTION: GAMEMD 0x0054E360
Kamikaze::Kamikaze(void) :
	UpdateTimer(),
	Nodes()
{
	UpdateTimer = 100;
}

// FUNCTION: GAMEMD 0x0054E3B0
void Kamikaze::Add(AircraftClass * aircraft, AbstractClass * target)
{
	if (!aircraft->Class->MissileSpawn) {
		aircraft->Crash(0);
		return;
	}
	KamikazeControl * control = new KamikazeControl;
	control->Item = aircraft;
	if (!target) {
		control->Cell = &aircraft->Get_Cell_Ptr()->Adjacent_Cell(aircraft->PrimaryFacing.Current().As_Dir8());
	} else {
		Cell cell = target->Center_Coord().As_Cell();
		control->Cell = &Map[cell];
	}
	aircraft->IsKamikaze = true;
	aircraft->Ammo = 1;
	Nodes.Add(control);
}

// FUNCTION: GAMEMD 0x0054E4D0
void Kamikaze::Update(void)
{
	if (UpdateTimer == 0) {
		UpdateTimer = 30;
		for (int i = 0; i < Nodes.Count(); i++) {
			KamikazeControl * control = Nodes[i];
			CellClass * cell = control->Cell;
			AircraftClass * aircraft = control->Item;
			aircraft->Ammo = 1;
			if (!cell) {
				CellClass * adj = &aircraft->Get_Cell_Ptr()->Adjacent_Cell(aircraft->PrimaryFacing.Current().As_Dir8());
				aircraft->Assign_Target(adj);
				aircraft->QueueMission(MISSION_ATTACK, false);
			} else {
				aircraft->Assign_Target(cell);
				aircraft->QueueMission(MISSION_ATTACK, false);
			}
		}
	}
}

// FUNCTION: GAMEMD 0x0054E590
void Kamikaze::Remove(AbstractClass * object)
{
	for (int i = Nodes.Count() - 1; i >= 0; i--) {
		if (object == Nodes[i]->Item) {
			delete Nodes[i];
			Nodes.Delete_Index(i);
			return;
		}
		if (object == Nodes[i]->Cell) {
			Cell cell = object->Center_Coord().As_Cell();
			CellClass * newcell = &Map[cell];
			Nodes[i]->Item->Assign_Target(newcell);
			Nodes[i]->Item->QueueMission(MISSION_ATTACK, false);
			Nodes[i]->Cell = newcell;
			return;
		}
	}
}

// FUNCTION: GAMEMD 0x0054E690
Kamikaze::~Kamikaze(void)
{
	for (int i = Nodes.Count() - 1; i >= 0; i--) {
		delete Nodes[i];
	}
	Nodes.Delete_All();
}

// FUNCTION: GAMEMD 0x0054E6F0
void Kamikaze::Clear(void)
{
	for (int i = Nodes.Count() - 1; i >= 0; i--) {
		delete Nodes[i];
	}
	Nodes.Delete_All();
	UpdateTimer = 1;
}

// FUNCTION: GAMEMD 0x0054E750
long Kamikaze::Save(IStream * stream)
{
	int count = Nodes.Count();
	long result = stream->Write(&count, sizeof(count), 0);
	if (result >= 0) {
		for (int i = 0; i < count; i++) {
			result = stream->Write(Nodes[i], sizeof(KamikazeControl), 0);
			if (result < 0) {
				break;
			}
		}
	}
	return result;
}

// FUNCTION: GAMEMD 0x0054E7B0
long Kamikaze::Load(IStream * stream)
{
	int count;
	long result = stream->Read(&count, sizeof(count), 0);
	if (result < 0) {
		return result;
	}
	for (int i = 0; i < count; i++) {
		KamikazeControl * control = new KamikazeControl;
		result = stream->Read(control, sizeof(KamikazeControl), 0);
		if (result < 0) {
			return result;
		}
		Swizzle_Pointer(&control->Item);
		Swizzle_Pointer(&control->Cell);
		Nodes.Add(control);
	}
	return result;
}

// FUNCTION: GAMEMD 0x0054E890
// ?Clear@?$DynamicVectorClass@PAUKamikazeControl@@@@UAEXXZ

// FUNCTION: GAMEMD 0x0054E8C0
// ?ID@?$DynamicVectorClass@PAUKamikazeControl@@@@UAEHPBQAUKamikazeControl@@@Z

template class DynamicVectorClass<KamikazeControl *>;
