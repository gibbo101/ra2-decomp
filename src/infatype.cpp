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

// FUNCTION: GAMEMD 0x005236A0
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

// FUNCTION: GAMEMD 0x00523980
InfantryTypeClass::InfantryTypeClass(NoInitClass const & x) :
	BASECLASS(x),
	DeadBodies(x),
	DeathAnims(x),
	VoiceComment(x)
{
}

// FUNCTION: GAMEMD 0x005239D0
InfantryTypeClass::~InfantryTypeClass(void)
{
	Detach_This_From_All(this);
	InfantryTypes.Delete(this);
	delete (DoInfoStruct *)DoControls;
	DoControls = NULL;
}

// FUNCTION: GAMEMD 0x00523B10
ObjectClass * InfantryTypeClass::Create_One_Of(HouseClass * house) const
{
	return(new InfantryClass(this, house));
}

// FUNCTION: GAMEMD 0x00523B40
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

// FUNCTION: GAMEMD 0x00523C20
Cell const * InfantryTypeClass::Occupy_List(bool) const
{
	static Cell const _list[] = {Cell(0, 0), REFRESH_EOL};

	return(&_list[0]);
}

// FUNCTION: GAMEMD 0x00523C80
void InfantryTypeClass::Init(TheaterType)
{
}

// FUNCTION: GAMEMD 0x00523C90
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

struct Z4 { int a, b, c, d; Z4(void) : a(0), b(0), c(0), d(0) {} };
static Z4 Unused_Zero;

char const * SequenceName[DOINFO_COUNT] = {
	"Ready", "Guard", "Prone", "Walk", "FireUp", "Down", "Crawl", "Up", "FireProne",
	"Idle1", "Idle2", "Die1", "Die2", "Die3", "Die4", "Die5", "Tread", "Swim",
	"WetIdle1", "WetIdle2", "WetDie1", "WetDie2", "WetAttack", "Hover", "Fly", "Tumble",
	"FireFly", "Deploy", "Deployed", "DeployedFire", "DeployedIdle", "Undeploy", "Cheer",
	"Paradrop", "AirDeathStart", "AirDeathFalling", "AirDeathFinish", "Panic", "Shovel",
	"Carry", "SecondaryFire", "SecondaryProne"
};

// FUNCTION: GAMEMD 0x00523D00
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

// FUNCTION: GAMEMD 0x00524760
Point3D InfantryTypeClass::Lepton_Dimensions(void) const
{
	return(Point3D(CELL_LEPTON_W / 3, CELL_LEPTON_H / 3, LEVEL_LEPTON_H * 2));
}

// FUNCTION: GAMEMD 0x00524790
int InfantryTypeClass::Repair_Step(void) const
{
	return(Rule->IRepairStep);
}

// FUNCTION: GAMEMD 0x005247A0
int InfantryTypeClass::Repair_Cost(void) const
{
	return(0);
}

// FUNCTION: GAMEMD 0x005247B0
bool InfantryTypeClass::CanUseWaypoint(void) const
{
	return(BASECLASS::CanUseWaypoint());
}

// FUNCTION: GAMEMD 0x005247C0
bool InfantryTypeClass::CanAttackMove(void) const
{
	return(BASECLASS::CanAttackMove());
}

// FUNCTION: GAMEMD 0x005247D0
Coord const InfantryTypeClass::Coord_Fixup(Coord const & coord) const
{
	Coord fix = coord;
	if (fix.Z < Map.Get_Height_GL(fix)) {
		fix.Z = Map.Get_Height_GL(fix);
	}
	return(fix);
}

// FUNCTION: GAMEMD 0x00524840
void InfantryTypeClass::Compute_CRC(CRCEngine & crc) const
{
	BASECLASS::Compute_CRC(crc);
	crc(Pip);
	crc(DoControls, sizeof(*DoControls) * DOINFO_COUNT);
	crc(FireLaunch);
	crc(ProneLaunch);
	crc(VoiceComment.Count());
	crc(DeadBodies.Count());
	crc(DeathAnims.Count());
	crc(IsFearless);
	crc(IsCrawling);
	crc(IsCapture);
	crc(IsFraidyCat);
	crc(IsTiberiumProof);
	crc(IsCivilian);
	crc(IsBomber);
	crc(IsEngineer);
	crc(IsAgent);
	crc(IsThief);
	crc(IsVehicleThief);
}

// FUNCTION: GAMEMD 0x00524B60
HRESULT STDMETHODCALLTYPE InfantryTypeClass::Save(IStream * stream, BOOL cleardirty)
{
	HRESULT result = BASECLASS::Save(stream, cleardirty);
	if (SUCCEEDED(result)) {
		int index;
		int count = VoiceComment.Count();
		stream->Write(&count, sizeof(count), NULL);
		for (index = 0; index < count; index++) {
			stream->Write(&VoiceComment[index], sizeof(int), NULL);
		}

		count = DeadBodies.Count();
		stream->Write(&count, sizeof(count), NULL);
		for (index = 0; index < count; index++) {
			stream->Write(&DeadBodies[index], sizeof(AnimTypeClass *), NULL);
		}

		count = DeathAnims.Count();
		stream->Write(&count, sizeof(count), NULL);
		for (index = 0; index < count; index++) {
			stream->Write(&DeathAnims[index], sizeof(AnimTypeClass *), NULL);
		}

		stream->Write((void *)DoControls, sizeof(DoInfoStruct) * DOINFO_COUNT, NULL);
	}
	return(result);
}

// FUNCTION: GAMEMD 0x00524C70
HRESULT STDMETHODCALLTYPE InfantryTypeClass::GetClassID(CLSID * retval)
{
	if (retval == NULL) return(E_POINTER);
	*retval = CLSID_InfantryTypeClass;
	return(S_OK);
}

// FUNCTION: GAMEMD 0x00524CB0
InfantryTypeClass * InfantryTypeClass::Find_Or_Make(char const * name)
{
	return(TFind_Or_Make<InfantryTypeClass>(name, InfantryTypes));
}

// FUNCTION: GAMEMD 0x00524D40
RTTIType InfantryTypeClass::Fetch_RTTI(void) const
{
	return(RTTI_INFANTRYTYPE);
}

// FUNCTION: GAMEMD 0x00524D50
int InfantryTypeClass::Fetch_Object_Size(void) const
{
	return(sizeof(*this));
}

// FUNCTION: GAMEMD 0x00524D60
int InfantryTypeClass::Fetch_Heap_ID(void) const
{
	return(HeapID);
}

// SYNTHETIC: GAMEMD 0x00524D70
// ??_GInfantryTypeClass@@UAEPAXI@Z

// FUNCTION: GAMEMD 0x00524EC0
char const * __fastcall FUN_00524EC0(char const * string)
{
	return(string);
}

// TEMPLATE: GAMEMD 0x00524ED0
// ?Clear@?$DynamicVectorClass@PAVAnimTypeClass@@@@UAEXXZ
template class DynamicVectorClass<AnimTypeClass *>;
template class VectorClass<AnimTypeClass *>;
template class TypeList<AnimTypeClass *>;

// TEMPLATE: GAMEMD 0x00524F00
// ?ID@?$DynamicVectorClass@PAVAnimTypeClass@@@@UAEHPBQAVAnimTypeClass@@@Z

// TEMPLATE: GAMEMD 0x00524F20
// ??4?$DynamicVectorClass@PAVAnimTypeClass@@@@QAEAAV0@ABV0@@Z

// TEMPLATE: GAMEMD 0x00524FB0
// ?GetItem@?$VectorClass@PAVAnimTypeClass@@@@UBEPAVAnimTypeClass@@H@Z

// SYNTHETIC: GAMEMD 0x00524FC0
// ??_G?$DynamicVectorClass@PAVAnimTypeClass@@@@UAEPAXI@Z

// SYNTHETIC: GAMEMD 0x00525010
// ??_G?$VectorClass@PAVAnimTypeClass@@@@UAEPAXI@Z

// TEMPLATE: GAMEMD 0x00525060
// ??4?$VectorClass@PAVAnimTypeClass@@@@QAEAAV0@ABV0@@Z

// TEMPLATE: GAMEMD 0x005250D0
// ??8?$VectorClass@PAVAnimTypeClass@@@@UBE_NABV0@@Z

// TEMPLATE: GAMEMD 0x00525110
// ?Resize@?$VectorClass@PAVAnimTypeClass@@@@UAE_NHPBQAVAnimTypeClass@@@Z

// TEMPLATE: GAMEMD 0x005251C0
// ?Clear@?$VectorClass@PAVAnimTypeClass@@@@UAEXXZ

// TEMPLATE: GAMEMD 0x005251F0
// ?ID@?$VectorClass@PAVAnimTypeClass@@@@UAEHABQAVAnimTypeClass@@@Z

// TEMPLATE: GAMEMD 0x00525230
// ?ID@?$VectorClass@PAVAnimTypeClass@@@@UAEHPBQAVAnimTypeClass@@@Z

// TEMPLATE: GAMEMD 0x00525250
// ??0?$DynamicVectorClass@PAVAnimTypeClass@@@@QAE@IPBQAVAnimTypeClass@@@Z

// SYNTHETIC: GAMEMD 0x005252B0
// ??_G?$TypeList@PAVAnimTypeClass@@@@UAEPAXI@Z

// TEMPLATE: GAMEMD 0x00525300
// ?Resize@?$DynamicVectorClass@PBVAnimTypeClass@@@@UAE_NHPBQBVAnimTypeClass@@@Z
// TEMPLATE: GAMEMD 0x005253B0
// ?Add@?$DynamicVectorClass@PBVAnimTypeClass@@@@QAE_NABQBVAnimTypeClass@@@Z
// TEMPLATE: GAMEMD 0x00525400
// ?ID@?$DynamicVectorClass@PBVAnimTypeClass@@@@UAEHABQBVAnimTypeClass@@@Z
template class DynamicVectorClass<AnimTypeClass const *>;

// TEMPLATE: GAMEMD 0x005255F0
// ??0?$TypeList@PBVAnimTypeClass@@@@QAE@ABV0@@Z
template class TypeList<AnimTypeClass const *>;

// TEMPLATE: GAMEMD 0x00525680
// ??0?$VectorClass@PBVAnimTypeClass@@@@QAE@HPBQBVAnimTypeClass@@@Z
template class VectorClass<AnimTypeClass const *>;

// SYNTHETIC: GAMEMD 0x005256D0 SYMBOL
// _$E74
