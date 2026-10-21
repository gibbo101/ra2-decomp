// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#define INCLUDE_COM
#include "always.h"
#include "geometry.h"
#include "suprtype.h"
#include "_map.h"
#include "_mixfile.h"
#include "building.h"
#include "builtype.h"
#include "cell.h"
#include "findmake.h"
#include "globals.h"
#include "mixfile.h"
#include "mouse.h"
#include "noinit.h"
#include "sun.h"
#include "swizzle.h"
#include "weapon.h"
#include "voc.h"

struct FileSystem {
	static void const * LoadFile(char const * name, bool flag);
};

extern char DAT_00889F64[];

// SYNTHETIC: GAMEMD 0x006CE380 SYMBOL
// _$E2
// SYNTHETIC: GAMEMD 0x006CE3B0 SYMBOL
// _$E5
// SYNTHETIC: GAMEMD 0x006CE3D0 SYMBOL
// _$E8
// SYNTHETIC: GAMEMD 0x006CE3F0 SYMBOL
// _$E11
// SYNTHETIC: GAMEMD 0x006CE410 SYMBOL
// _$E14
// SYNTHETIC: GAMEMD 0x006CE430 SYMBOL
// _$E17
// SYNTHETIC: GAMEMD 0x006CE450 SYMBOL
// _$E20
// SYNTHETIC: GAMEMD 0x006CE490 SYMBOL
// _$E23
// SYNTHETIC: GAMEMD 0x006CE4C0 SYMBOL
// _$E26
// SYNTHETIC: GAMEMD 0x006CE4F0 SYMBOL
// _$E29
// SYNTHETIC: GAMEMD 0x006CE520 SYMBOL
// _$E32
// SYNTHETIC: GAMEMD 0x006CE530 SYMBOL
// _$E35
// SYNTHETIC: GAMEMD 0x006CE550 SYMBOL
// _$E38

// FUNCTION: GAMEMD 0x006CE570
SuperWeaponType Special_From_Name(char const * name)
{
	static const char * _names[SUPER_COUNT] = {
		"MultiMissile",
		"IronCurtain",
		"LightningStorm",
		"ChronoSphere",
		"ChronoWarp",
		"ParaDrop",
		"AmerParaDrop",
		"PsychicDominator",
		"SpyPlane",
		"GeneticConverter",
		"ForceShield",
		"PsychicReveal"
	};

	for (int i = SUPER_FIRST; i < SUPER_COUNT; i++) {
		if (strcmpi(_names[i], name) == 0) {
			return(SuperWeaponType)(i);
		}
	}
	return(SUPER_NONE);
}

// FUNCTION: GAMEMD 0x006CE5B0
SuperWeaponTypeClass::SuperWeaponTypeClass(char const * ininame) :
	BASECLASS(ininame),
	Weapon(NULL),
	VoxRecharge(VOX_NONE),
	VoxCharging(VOX_NONE),
	VoxImpatient(VOX_NONE),
	VoxSuspend(VOX_NONE),
	RechargeTime(4500),
	Type(SUPER_NONE),
	CameoData(NULL),
	Action(ACTION_NONE),
	SpecialSound(-1),
	StartSound(-1),
	AuxBuilding(NULL),
	SidebarImage(),
	UseChargeDrain(false),
	IsPowered(true),
	DisableableFromShell(false),
	FlashSidebarTabFrames(-1),
	AIDefendAgainst(false),
	PreClick(false),
	PostClick(false),
	PreDependent(-1),
	ShowTimer(false),
	IsManualControl(false),
	Range(0),
	LineMultiplier(0)
{
	Create_ID();
	HeapID = (SuperWeaponType)SuperWeaponTypes.Count();
	SuperWeaponTypes.Add(this);
	SidebarImage = IniName;
}

// FUNCTION: GAMEMD 0x006CE700
SuperWeaponTypeClass::SuperWeaponTypeClass(NoInitClass const & x) :
	BASECLASS(x),
	SidebarImage(x),
	CameoData(NULL)
{
}

// FUNCTION: GAMEMD 0x006CE740
SuperWeaponTypeClass::~SuperWeaponTypeClass(void)
{
	SuperWeaponTypes.Delete(this);
}

// FUNCTION: GAMEMD 0x006CE7C0
HRESULT STDMETHODCALLTYPE SuperWeaponTypeClass::GetClassID(CLSID * retval)
{
	if (retval == NULL) return(E_POINTER);
	*retval = CLSID_SuperWeaponTypeClass;
	return(S_OK);
}

// FUNCTION: GAMEMD 0x006CE800
HRESULT STDMETHODCALLTYPE SuperWeaponTypeClass::Load(IStream *stream)
{
	HRESULT result = BASECLASS::Load(stream);
	if (SUCCEEDED(result)) {
		new (this) SuperWeaponTypeClass(NoInitClass());

		Swizzle_Pointer(&Weapon);
		Swizzle_Pointer(&AuxBuilding);

		char fullname[_MAX_FNAME+_MAX_EXT];
		_makepath(fullname, NULL, NULL, SidebarImage, ".SHP");
		CameoData = (ShapeSet const *)FileSystem::LoadFile(fullname, false);
		if (CameoData == NULL) {
			CameoData = (ShapeSet const *)FileSystem::LoadFile("XXICON.SHP", false);
		}

		result = S_OK;
	}
	return(result);
}

// FUNCTION: GAMEMD 0x006CE8D0
HRESULT STDMETHODCALLTYPE SuperWeaponTypeClass::Save(IStream *stream, int cleardirty)
{
	HRESULT result = BASECLASS::Save(stream, cleardirty);
	if (SUCCEEDED(result)) {
		result = S_OK;
	}
	return(result);
}

// FUNCTION: GAMEMD 0x006CE8F0
RTTIType SuperWeaponTypeClass::Fetch_RTTI(void) const
{
	return(RTTI_SUPERWEAPONTYPE);
}

// FUNCTION: GAMEMD 0x006CE900
int SuperWeaponTypeClass::Fetch_Object_Size(void) const
{
	return(sizeof(*this));
}

// FUNCTION: GAMEMD 0x006CE910
void SuperWeaponTypeClass::Compute_CRC(CRCEngine & crc) const
{
	BASECLASS::Compute_CRC(crc);
	crc(HeapID);
	crc(Action);
	crc(VoxRecharge);
	crc(VoxCharging);
	crc(VoxImpatient);
	crc(VoxSuspend);
	crc(IsPowered);
	crc(DisableableFromShell);
	crc(PreClick);
	crc(PostClick);
	crc(PreDependent);
	crc(ShowTimer);
	crc(RechargeTime);
	crc(Type);
	crc(UseChargeDrain);
	crc(IsManualControl);
}

// FUNCTION: GAMEMD 0x006CEA10
int SuperWeaponTypeClass::Fetch_Heap_ID(void) const
{
	return(HeapID);
}

static inline int Get_Sound(CCINIClass const & ini, char const * section, char const * entry, int defvalue)
{
	char buffer[128];
	if (ini.Get_String(section, entry, "", buffer, sizeof(buffer)) == 0) {
		return(defvalue);
	}
	int index = VocClass::FindIndex(buffer);
	if (index == -1) {
		return(defvalue);
	}
	return(index);
}

// FUNCTION: GAMEMD 0x006CEA20
bool SuperWeaponTypeClass::Read_INI(CCINIClass & ini)
{
	ini.Reset();
	if (BASECLASS::Read_INI(ini)) {
		Weapon = TGet_Class(ini, IniName, "WeaponType", Weapon);
		Action = ini.Get_ActionType(IniName, "Action", Action);
		IsPowered = ini.Get_Bool(IniName, "IsPowered", IsPowered);
		DisableableFromShell = ini.Get_Bool(IniName, "DisableableFromShell", DisableableFromShell);
		FlashSidebarTabFrames = ini.Get_Int(IniName, "FlashSidebarTabFrames", FlashSidebarTabFrames);
		AIDefendAgainst = ini.Get_Bool(IniName, "AIDefendAgainst", AIDefendAgainst);
		PreClick = ini.Get_Bool(IniName, "PreClick", PreClick);
		PostClick = ini.Get_Bool(IniName, "PostClick", PostClick);
		ShowTimer = ini.Get_Bool(IniName, "ShowTimer", ShowTimer);

		SpecialSound = Get_Sound(ini, IniName, "SpecialSound", SpecialSound);
		StartSound = Get_Sound(ini, IniName, "StartSound", StartSound);

		Range = ini.Get_Float(IniName, "Range", Range);
		LineMultiplier = ini.Get_Int(IniName, "LineMultiplier", LineMultiplier);

		char type_name[40];
		char dependent_name[40];
		ini.Get_String(IniName, "Type", "", type_name, sizeof(type_name));
		if (strlen(type_name) != 0) {
			SuperWeaponType type = Special_From_Name(type_name);
			if (type != SUPER_NONE) Type = type;
		}

		ini.Get_String(IniName, "PreDependent", "", dependent_name, sizeof(dependent_name));
		if (strlen(dependent_name) != 0) {
			SuperWeaponType type = Special_From_Name(dependent_name);
			if (type != SUPER_NONE) PreDependent = type;
		}

		AuxBuilding = TGet_Class(ini, IniName, "AuxBuilding", AuxBuilding);
		UseChargeDrain = ini.Get_Bool(IniName, "UseChargeDrain", UseChargeDrain);
		IsManualControl = ini.Get_Bool(IniName, "ManualControl", IsManualControl);

		double recharge = ini.Get_Float(IniName, "RechargeTime");
		if (recharge != 0.0) {
			RechargeTime = (float)recharge * 900.0f;
		}

		char sidebar_image[256];
		ini.Get_String(IniName, "SidebarImage", (char *)SidebarImage, sidebar_image, sizeof(sidebar_image));
		SidebarImage = sidebar_image;

		char fullname[_MAX_FNAME+_MAX_EXT];
		_makepath(fullname, NULL, NULL, SidebarImage, ".SHP");
		CameoData = (ShapeSet const *)FileSystem::LoadFile(fullname, false);
		if (CameoData == NULL) {
			CameoData = (ShapeSet const *)FileSystem::LoadFile("XXICON.SHP", false);
		}

		return(true);
	}
	return(false);
}

// FUNCTION: GAMEMD 0x006CEE60
SuperWeaponType SuperWeaponTypeClass::From_Name(char const * name)
{
	if (name != NULL) {
		for (int classid = SUPER_FIRST; classid < SuperWeaponTypes.Count(); classid++) {
			if (stricmp(SuperWeaponTypes[classid]->IniName, name) == 0) {
				return(SuperWeaponType)classid;
			}
		}
	}
	return(SUPER_NONE);
}

// FUNCTION: GAMEMD 0x006CEEB0
SuperWeaponTypeClass * SuperWeaponTypeClass::From_Action(ActionType action)
{
	for (int classid = SUPER_FIRST; classid < SuperWeaponTypes.Count(); classid++) {
		if (SuperWeaponTypes[classid]->Action == action) {
			return(SuperWeaponTypes[classid]);
		}
	}
	return(NULL);
}

// FUNCTION: GAMEMD 0x006CEEF0
SuperWeaponTypeClass * SuperWeaponTypeClass::Find_Or_Make(const char * name)
{
	return(TFind_Or_Make<SuperWeaponTypeClass>(name, SuperWeaponTypes));
}

// FUNCTION: GAMEMD 0x006CEF80
ActionType SuperWeaponTypeClass::What_Action(Cell const & cell, ObjectClass * object)
{
	if (Type == 10) {
		if (Is_Target_Building(object)) {
			BuildingClass * building = (object->RTTI == RTTI_BUILDING ? (BuildingClass *)object : NULL);
			if (building->House->Is_Ally(PlayerPtr)) {
				return(Action);
			}
		}
		return((ActionType)0x46);
	}
	return(Action);
}


// SYNTHETIC: GAMEMD 0x006CEFE0
// ??_GSuperWeaponTypeClass@@UAEPAXI@Z
