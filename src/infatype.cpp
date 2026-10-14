// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#define INCLUDE_COM
#include "always.h"
#include "infatype.h"
#include "_map.h"
#include "_rules.h"
#include "cell.h"
#include "findmake.h"
#include "globals.h"
#include "infantry.h"
#include "mouse.h"
#include "rules.h"
#include "sun.h"
#include "tracker.h"
#include <cstdio>

enum { DOINFO_COUNT = 42 };

static inline void Init_Controls(InfantryTypeClass * type)
{
	type->DoControls = new DoInfoStruct[DOINFO_COUNT];
	for (int i = 0; i < DOINFO_COUNT; i++) {
		((DoInfoStruct *)type->DoControls)[i].Frame = 0;
		((DoInfoStruct *)type->DoControls)[i].Count = 0;
		((DoInfoStruct *)type->DoControls)[i].Jump = 0;
		((DoInfoStruct *)type->DoControls)[i].Facing = FACING_NONE;
		((DoInfoStruct *)type->DoControls)[i].SoundCount = 0;
	}
}

// FUNCTION: 0x005236A0
InfantryTypeClass::InfantryTypeClass(char const * ininame) :
	BASECLASS(ininame, SPEED_FOOT),
	HeapID(INFANTRY_NONE),
	Pip(PIP_GREEN),
	OccupyPip(PIP_PERSON_GREEN),
	DoControls(NULL),
	FireLaunch(0),
	ProneLaunch(0),
	SecondaryFire(0),
	SecondaryProne(0),
	DeadBodies(),
	DeathAnims(),
	VoiceComment(),
	EnterWaterSound(-1),
	LeaveWaterSound(-1),
	IsCyborg(false),
	NotHuman(false),
	Ivan(false),
	DirectionDistance(0),
	Occupier(false),
	Assaulter(false),
	HarvestRate(1),
	IsFearless(false),
	IsCrawling(true),
	IsCapture(false),
	IsFraidyCat(false),
	IsTiberiumProof(false),
	IsCivilian(false),
	IsBomber(false),
	IsEngineer(false),
	IsAgent(false),
	IsThief(false),
	IsVehicleThief(false),
	IsDoggie(false),
	Deployer(false),
	DeployedCrushable(true),
	UseOwnName(false),
	JumpJetTurn(false)
{
	Create_ID();
	InfantryTypes.Add(this);
	HeapID = (InfantryType)InfantryTypes.ID(this);

	OccupyWeapon.Weapon = NULL;
	OccupyWeapon.FireFLH = Coord(0, 0, 0);
	OccupyWeapon.BarrelLength = 0;
	OccupyWeapon.BarrelThickness = 0;
	OccupyWeapon.TurretLocked = false;
	EliteOccupyWeapon.Weapon = NULL;
	EliteOccupyWeapon.FireFLH = Coord(0, 0, 0);
	EliteOccupyWeapon.BarrelLength = 0;
	EliteOccupyWeapon.BarrelThickness = 0;
	EliteOccupyWeapon.TurretLocked = false;

	Rotation = FACING_COUNT;
	IsRadarVisible = false;
	IsCrushable = true;
	IsRepairable = false;
	IsCrew = false;
	ImmuneToPsionics = false;
	ImmuneToPsionicWeapons = false;
	ImmuneToPoison = false;
	Parasiteable = true;
	Organic = true;
	ConsideredAircraft = false;
	Bunkerable = false;

	Init_Controls(this);
}

// FUNCTION: 0x00523980
InfantryTypeClass::InfantryTypeClass(NoInitClass const & x) :
	BASECLASS(x),
	DeadBodies(x),
	DeathAnims(x),
	VoiceComment(x)
{
}

// FUNCTION: 0x005239D0
InfantryTypeClass::~InfantryTypeClass(void)
{
	Detach_This_From_All(this);
	InfantryTypes.Delete(this);
	delete (DoInfoStruct *)DoControls;
	DoControls = NULL;
}

// FUNCTION: 0x00523B10
ObjectClass * InfantryTypeClass::Create_One_Of(HouseClass * house) const
{
	return(new InfantryClass(this, house));
}

// FUNCTION: 0x00523B40
bool InfantryTypeClass::Create_And_Place(Cell const & cell, HouseClass * house) const
{
	InfantryClass * i = new InfantryClass(this, house);
	if (i != NULL) {
		Coord cell_coord = cell;
		Coord coord = Map[cell_coord].Closest_Free_Spot(cell_coord);
		if (coord != COORD_NONE) {
			return(i->Unlimbo(coord, DIR_E));
		} else {
			delete i;
		}
	}
	return(false);
}

// FUNCTION: 0x00523C20
Cell const * InfantryTypeClass::Occupy_List(bool) const
{
	static Cell const _list[] = {Cell(0, 0), REFRESH_EOL};

	return(&_list[0]);
}

// FUNCTION: 0x00523C80
void InfantryTypeClass::Init(TheaterType)
{
}

// FUNCTION: 0x00523C90
InfantryType InfantryTypeClass::From_Name(char const * name)
{
	if (strcmpi(name, "<none>") != 0 && strcmpi(name, "none") != 0) {
		for (int classid = INFANTRY_FIRST; classid < InfantryTypes.Count(); classid++) {
			if (stricmp(InfantryTypes[classid]->Name(), name) == 0) {
				return((InfantryType)classid);
			}
		}
	}
	return(INFANTRY_NONE);
}

char const * SequenceName[DOINFO_COUNT] = {
	"Ready", "Guard", "Prone", "Walk", "FireUp", "Down", "Crawl", "Up", "FireProne",
	"Idle1", "Idle2", "Die1", "Die2", "Die3", "Die4", "Die5", "Tread", "Swim",
	"WetIdle1", "WetIdle2", "WetDie1", "WetDie2", "WetAttack", "Hover", "Fly", "Tumble",
	"FireFly", "Deploy", "Deployed", "DeployedFire", "DeployedIdle", "Undeploy", "Cheer",
	"Paradrop", "AirDeathStart", "AirDeathFalling", "AirDeathFinish", "Panic", "Shovel",
	"Carry", "SecondaryFire", "SecondaryProne"
};

// FUNCTION: 0x00523D00
void InfantryTypeClass::Read_Sequence_INI(void)
{
	char seqname[32];

	if (ArtINI.Get_String(Graphic_Name(), "Sequence", "", seqname, sizeof(seqname)) > 0) {
		DoInfoStruct * control = (DoInfoStruct *)DoControls;

		for (int i = 0; i < DOINFO_COUNT; i++) {
			char dostring[32];
			char facing[10] = {""};

			if (ArtINI.Get_String(seqname, SequenceName[i], "", dostring, sizeof(dostring)) > 0) {
				DoInfoStruct * doinfo = &control[i];

				sscanf(dostring, "%d,%d,%d,%s", &doinfo->Frame, &doinfo->Count, &doinfo->Jump, facing);
				if (strcmp("N", facing) == 0) {
					doinfo->Facing = FACING_N;
				} else if (strcmp("NE", facing) == 0) {
					doinfo->Facing = FACING_NE;
				} else if (strcmp("E", facing) == 0) {
					doinfo->Facing = FACING_E;
				} else if (strcmp("SE", facing) == 0) {
					doinfo->Facing = FACING_SE;
				} else if (strcmp("S", facing) == 0) {
					doinfo->Facing = FACING_S;
				} else if (strcmp("SW", facing) == 0) {
					doinfo->Facing = FACING_SW;
				} else if (strcmp("W", facing) == 0) {
					doinfo->Facing = FACING_W;
				} else if (strcmp("NW", facing) == 0) {
					doinfo->Facing = FACING_NW;
				}
			}

			char soundkey[200];
			sprintf(soundkey, "%sSounds", SequenceName[i]);
			if (ArtINI.Get_String(seqname, soundkey, "", dostring, sizeof(dostring)) > 0) {
				char * token = strtok(dostring, " ,\t");
				while (token != NULL) {
					int frame = atoi(token);
					token = strtok(NULL, " ,\t");
					if (token == NULL) break;
					int voc = VocClass::FindIndex(token);
					token = strtok(NULL, " ,\t");
					if (voc != -1 && control[i].SoundCount < 2) {
						(&control[i].Sound1StartFrame)[control[i].SoundCount * 2] = frame;
						(&control[i].Sound1Index)[control[i].SoundCount * 2] = voc;
						control[i].SoundCount++;
					}
				}
			}
		}
	}
}

// FUNCTION: 0x00524760
Point3D InfantryTypeClass::Lepton_Dimensions(void) const
{
	return(Point3D(CELL_LEPTON_W / 3, CELL_LEPTON_H / 3, LEVEL_LEPTON_H * 2));
}

// FUNCTION: 0x00524790
int InfantryTypeClass::Repair_Step(void) const
{
	return(Rule->IRepairStep);
}

// FUNCTION: 0x005247A0
int InfantryTypeClass::Repair_Cost(void) const
{
	return(0);
}

// FUNCTION: 0x005247B0
bool InfantryTypeClass::CanUseWaypoint(void) const
{
	return(BASECLASS::CanUseWaypoint());
}

// FUNCTION: 0x005247D0
Coord const InfantryTypeClass::Coord_Fixup(Coord const & coord) const
{
	Coord fix = coord;
	if (fix.Z < Map.Get_Height_GL(fix)) {
		fix.Z = Map.Get_Height_GL(fix);
	}
	return(fix);
}

// FUNCTION: 0x00524C70
HRESULT STDMETHODCALLTYPE InfantryTypeClass::GetClassID(CLSID * retval)
{
	if (retval == NULL) return(E_POINTER);
	*retval = CLSID_InfantryTypeClass;
	return(S_OK);
}
