// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"
#include "ccini.h"
#include "_rtti.h"
#include "_source.h"
#include "_theater.h"
#include "airctype.h"
#include "builtype.h"
#include "category.h"
#include "conquer.h"
#include "coord.h"
#include "globals.h"
#include "house.h"
#include "houstype.h"
#include "incdec.h"
#include "infatype.h"
#include "movie.h"
#include "scheme.h"
#include "shapipe.h"
#include "side.h"
#include "suprtype.h"
#include "target.h"
#include "theme.h"
#include "unittype.h"
#include "veteran.h"
#include "voc.h"
#include "vox.h"
#include "weapon.h"
#include "xpipe.h"
#include "xstraw.h"

namespace Powerups {
extern char const * Effects[19];
}
int __fastcall FUN_0050C170(char const * name);
int __fastcall FUN_00510FB0(char const * name);
bool FUN_00510F60(int index);
char const * __fastcall FUN_00510E10(int index, char * buffer, int size);
char const * __fastcall FUN_00751590(int index);
char const * Name_From_BuildCat(int value);
int BuildCat_From_Name(char const * name);
VoxType Vox_From_Name(char const * name);
char const * Name_From_Layer(LayerType layer);
LayerType Layer_From_Name(char const * name);

static inline int _Scale_To_256(int val)
{
	val = MIN(val, 100);
	val = MAX(val, 0);
	val = ((val * (MPH_LIGHT_SPEED + 1)) / 100);
	val = MIN(val, MPH_LIGHT_SPEED);
	return(val);
}

static char const * _vhpscan[3] = {"None", "Normal", "Strong"};

static struct
{
	char const * Name;
	PipEnum Type;
} _pips[11] = {
	{ "green", PIP_GREEN },
	{ "yellow", PIP_YELLOW },
	{ "white", PIP_WHITE },
	{ "red", PIP_RED },
	{ "blue", PIP_BLUE },
	{ "persongreen", PIP_PERSON_GREEN },
	{ "personyellow", PIP_PERSON_YELLOW },
	{ "personwhite", PIP_PERSON_WHITE },
	{ "personred", PIP_PERSON_RED },
	{ "personblue", PIP_PERSON_BLUE },
	{ "personpurple", PIP_PERSON_PURPLE }
};

static struct
{
	char const * Name;
	BSizeType Type;
} _foundations[] = {
	{ "1x1", BSIZE_11 },
	{ "2x1", BSIZE_21 },
	{ "1x2", BSIZE_12 },
	{ "2x2", BSIZE_22 },
	{ "2x3", BSIZE_23 },
	{ "3x2", BSIZE_32 },
	{ "3x3", BSIZE_33 },
	{ "3x5", BSIZE_35 },
	{ "4x2", BSIZE_42 },
	{ "3x3Refinery", BSIZE_33_REF },
	{ "1x3", BSIZE_13 },
	{ "3x1", BSIZE_31 },
	{ "4x3", BSIZE_43 },
	{ "1x4", BSIZE_14 },
	{ "1x5", BSIZE_15 },
	{ "2x6", BSIZE_26 },
	{ "2x5", BSIZE_25 },
	{ "5x3", BSIZE_53 },
	{ "4x4", BSIZE_44 },
	{ "3x4", BSIZE_34 },
	{ "6x4", BSIZE_64 },
	{ "0x0", BSIZE_00 }
};

// FUNCTION: 0x00476E20
void CCINIClass::Calculate_Message_Digest(void)
{
	SHAPipe sha;
	BASECLASS::Save(sha);
	sha.Result(Digest);
	IsDigestPresent = true;
}

// FUNCTION: 0x00476EA0
void CCINIClass::Invalidate_Message_Digest(void)
{
	IsDigestPresent = false;
}

// FUNCTION: 0x004741F0
int CCINIClass::ReadCCFile(FileClass & file, bool withdigest, bool loadcomments)
{
	FileStraw straw(file);
	return(Load(straw, withdigest, loadcomments));
}

// FUNCTION: 0x00474320
int CCINIClass::Load(Straw & file, bool withdigest, bool loadcomments)
{
	int ok = BASECLASS::Load(file, loadcomments);

	Invalidate_Message_Digest();
	if (ok && withdigest) {
		unsigned char digest[20];
		int len = Get_UUBlock("Digest", digest, sizeof(digest));
		if (len == 0) {
			return(2);
		}
		if (len > 0) {
			Clear("Digest");
			Calculate_Message_Digest();
			if (memcmp(digest, Digest, sizeof(digest)) != 0) {
				return(2);
			}
		}
	}
	return(ok);
}

// FUNCTION: 0x00474430
int CCINIClass::Save(FileClass & file, bool withdigest) const
{
	FilePipe pipe(file);
	return(Save(pipe, withdigest));
}

// FUNCTION: 0x00474540
int CCINIClass::Save(Pipe & pipe, bool withdigest) const
{
	if (!withdigest) {
		return(BASECLASS::Save(pipe));
	}

	((CCINIClass *)this)->Clear("Digest");
	((CCINIClass *)this)->Calculate_Message_Digest();
	((CCINIClass *)this)->Put_UUBlock("Digest", Digest, sizeof(Digest));
	int length = BASECLASS::Save(pipe);
	((CCINIClass *)this)->Clear("Digest");
	return(length);
}

// FUNCTION: 0x00474620
LEPTON CCINIClass::Get_Lepton(char const * section, char const * entry, LEPTON defvalue) const
{
	double result = Get_Float(section, entry, -1.0);
	if (result == -1.0) return(defvalue);
	return(result * CELL_LEPTON_W);
}

// FUNCTION: 0x00474660
bool CCINIClass::Put_Lepton(char const * section, char const * entry, LEPTON value)
{
	return(Put_Float(section, entry, value * (1.0 / CELL_LEPTON_W)));
}

// FUNCTION: 0x00474690
bool CCINIClass::Put_Angle(char const * section, char const * entry, int value)
{
	char buffer[32];

	sprintf(buffer, "%d", 360 * value / 256);
	return(Put_String(section, entry, buffer));
}

// FUNCTION: 0x004746E0
int CCINIClass::Get_Angle(char const * section, char const * entry, int defvalue) const
{
	char buffer[32];

	if (Get_String(section, entry, "", buffer, sizeof(buffer))) {
		return(atoi(buffer) * 256 / 360);
	}
	return(defvalue);
}

// FUNCTION: 0x00474740
Cell CCINIClass::Get_Cell(char const * section, char const * entry, Cell const & defvalue) const
{
	char buffer[64];

	if (Get_String(section, entry, "", buffer, sizeof(buffer))) {
		int x, y;
		sscanf(buffer, "%d,%d", &x, &y);
		return(Cell(x, y));
	}
	return(defvalue);
}

// FUNCTION: 0x004747C0
bool CCINIClass::Put_Cell(char const * section, char const * entry, Cell const & value)
{
	char buffer[64];
	sprintf(buffer, "%d,%d", value.X, value.Y);
	return(Put_String(section, entry, buffer));
}

// FUNCTION: 0x00474810
MPHType CCINIClass::Get_MPHType(char const * section, char const * entry, MPHType defvalue) const
{
	int val = Get_Int(section, entry, -1);
	if (val == -1) {
		return(defvalue);
	}
	return(MPHType(_Scale_To_256(val)));
}

// FUNCTION: 0x00474870
bool CCINIClass::Put_MPHType(char const * section, char const * entry, MPHType value)
{
	return(Put_Int(section, entry, ((int)value * 100) / (MPH_LIGHT_SPEED + 1)));
}

// FUNCTION: 0x004748A0
PipEnum CCINIClass::Get_PipEnum(char const * section, char const * entry, PipEnum defvalue) const
{
	char buffer[32];

	Get_String(section, entry, _pips[defvalue].Name, buffer, sizeof(buffer));
	for (int index = 0; index < ARRAY_SIZE(_pips); index++) {
		if (!strcmpi(buffer, _pips[index].Name)) {
			return(_pips[index].Type);
		}
	}
	return(PIP_GREEN);
}

// FUNCTION: 0x00474910
bool CCINIClass::Put_PipEnum(char const * section, char const * entry, PipEnum value)
{
	if (value != PIP_EMPTY) {
		return(Put_String(section, entry, _pips[value].Name));
	}
	return(false);
}

static struct
{
	char const * Name;
	PipScaleType Index;
} _pipscales[5] = {
	{ "Ammo", PIPSCALE_AMMO },
	{ "Tiberium", PIPSCALE_TIBERIUM },
	{ "Passengers", PIPSCALE_PASSENGERS },
	{ "Power", PIPSCALE_POWER },
	{ "Charge", PIPSCALE_CHARGE }
};

// FUNCTION: 0x00474940
PipScaleType CCINIClass::Get_PipScaleType(char const * section, char const * entry, PipScaleType defvalue) const
{
	char buffer[32];

	if (Get_String(section, entry, "", buffer, sizeof(buffer)) > 0) {
		for (int index = 0; index < ARRAY_SIZE(_pipscales); index++) {
			if (!strcmpi(buffer, _pipscales[index].Name)) {
				return(_pipscales[index].Index);
			}
		}
		return(PIPSCALE_NONE);
	}
	return(defvalue);
}

// FUNCTION: 0x004749B0
bool CCINIClass::Put_PipScaleType(char const * section, char const * entry, PipScaleType value)
{
	if (value != PIPSCALE_NONE) {
		return(Put_String(section, entry, _pipscales[value].Name));
	}
	return(false);
}

// FUNCTION: 0x004749E0
CategoryType CCINIClass::Get_CategoryType(char const * section, char const * entry, CategoryType defvalue) const
{
	char buffer[32];

	Get_String(section, entry, Name_From_Category(defvalue), buffer, sizeof(buffer));
	return(Category_From_Name(buffer));
}

// FUNCTION: 0x00474A20
bool CCINIClass::Put_CategoryType(char const * section, char const * entry, CategoryType value)
{
	if (value != CATEGORY_SOLDIER) {
		return(Put_String(section, entry, Name_From_Category(value)));
	}
	return(false);
}

// FUNCTION: 0x00474A50
TargetStruct CCINIClass::Get_xTarget(char const * section, char const * entry, TargetStruct defvalue) const
{
	TargetStruct t;
	t.Value = Get_Hex(section, entry, defvalue.Value);
	return(t);
}

// FUNCTION: 0x00474A70
bool CCINIClass::Put_xTarget(char const * section, char const * entry, TargetStruct value)
{
	return(Put_Hex(section, entry, value.Value));
}

// FUNCTION: 0x00474A90
int CCINIClass::Get_Scheme_Index(char const * section, char const * entry, int defvalue) const
{
	char buffer[32];

	Get_String(section, entry, ColorSchemes[defvalue]->Name, buffer, sizeof(buffer));
	for (int index = 0; index < ColorSchemes.Count(); index++) {
		if (!strcmpi(ColorSchemes[index]->Name, buffer) && ColorSchemes[index]->IntensityLevels != 1) {
			return(index);
		}
	}
	return(defvalue);
}

// FUNCTION: 0x00474B20
bool CCINIClass::Put_Scheme_Index(char const * section, char const * entry, int value)
{
	return(Put_String(section, entry, ColorSchemes[value]->Name));
}

// FUNCTION: 0x00474B50
RGBClass CCINIClass::Get_RGBClass(char const * section, char const * entry, RGBClass const & defvalue) const
{
	char defstr[64];
	char buffer[64];

	sprintf(defstr, "%d,%d,%d", defvalue.Get_Red(), defvalue.Get_Green(), defvalue.Get_Blue());
	if (Get_String(section, entry, defstr, buffer, sizeof(buffer))) {
		int r, g, b;
		sscanf(buffer, "%d,%d,%d", &r, &g, &b);
		RGBClass rgb(r, g, b);
		return(rgb);
	}
	return(defvalue);
}

// FUNCTION: 0x00474C20
bool CCINIClass::Put_RGBClass(char const * section, char const * entry, RGBClass const & value)
{
	char buffer[64];

	sprintf(buffer, "%d,%d,%d", value.Get_Red(), value.Get_Green(), value.Get_Blue());
	return(Put_String(section, entry, buffer));
}

// FUNCTION: 0x00474C70
HSVClass CCINIClass::Get_HSVClass(char const * section, char const * entry, HSVClass const & defvalue) const
{
	char defstr[64];
	char buffer[64];

	sprintf(defstr, "%d,%d,%d", defvalue.Get_Hue(), defvalue.Get_Saturation(), defvalue.Get_Value());
	if (Get_String(section, entry, defstr, buffer, sizeof(buffer))) {
		int h = 0, s = 0, v = 0;
		sscanf(buffer, "%d,%d,%d", &h, &s, &v);
		HSVClass hsv(h, s, v);
		return(hsv);
	}
	return(defvalue);
}

// FUNCTION: 0x00474D50
bool CCINIClass::Put_HSVClass(char const * section, char const * entry, HSVClass const & value)
{
	char buffer[64];

	sprintf(buffer, "%d,%d,%d", value.Get_Hue(), value.Get_Saturation(), value.Get_Value());
	return(Put_String(section, entry, buffer));
}

// FUNCTION: 0x00474DA0
BSizeType CCINIClass::Get_BSizeType(char const * section, char const * entry, BSizeType defvalue) const
{
	char buffer[32];

	Get_String(section, entry, _foundations[defvalue].Name, buffer, sizeof(buffer));
	for (int index = 0; index < ARRAY_SIZE(_foundations); index++) {
		if (!strcmpi(buffer, _foundations[index].Name)) {
			return(_foundations[index].Type);
		}
	}
	return(BSIZE_FIRST);
}

// FUNCTION: 0x00474E10
bool CCINIClass::Put_BSizeType(char const * section, char const * entry, BSizeType value)
{
	if (value != BSIZE_NONE) {
		return(Put_String(section, entry, _foundations[value].Name));
	}
	return(false);
}

// FUNCTION: 0x00474E40
MZoneType CCINIClass::Get_MZoneType(char const * section, char const * entry, MZoneType defvalue) const
{
	char buffer[32];

	Get_String(section, entry, _mzones[defvalue], buffer, sizeof(buffer));
	for (int index = 0; index < ARRAY_SIZE(_mzones); index++) {
		if (!strcmpi(buffer, _mzones[index])) {
			return(MZoneType)(index);
		}
	}
	return(MZONE_NONE);
}

// FUNCTION: 0x00474EB0
bool CCINIClass::Put_MZoneType(char const * section, char const * entry, MZoneType value)
{
	if (value != MZONE_NONE) {
		return(Put_String(section, entry, _mzones[value]));
	}
	return(false);
}

// FUNCTION: 0x00474EE0
ActionType CCINIClass::Get_ActionType(char const * section, char const * entry, ActionType defvalue) const
{
	char buffer[32];

	Get_String(section, entry, ActionName[defvalue], buffer, sizeof(buffer));
	for (int index = 0; index < ARRAY_SIZE(ActionName); index++) {
		if (!strcmpi(buffer, ActionName[index])) {
			return(ActionType)(index);
		}
	}
	return(ACTION_NONE);
}

// FUNCTION: 0x00474F50
SuperWeaponType CCINIClass::Get_SuperWeaponType(char const * section, char const * entry, SuperWeaponType defvalue) const
{
	char buffer[32];
	char emptystr[28];

	char * defname;
	if (defvalue != SUPER_NONE) {
		defname = SuperWeaponTypes[defvalue]->IniName;
	}
	else {
		emptystr[0] = '\0';
		defname = emptystr;
	}
	Get_String(section, entry, defname, buffer, sizeof(buffer));
	return(SuperWeaponTypeClass::From_Name(buffer));
}

// FUNCTION: 0x00474FA0
VoxType CCINIClass::Get_VoxType(char const * section, char const * entry, VoxType defvalue) const
{
	char buffer[100];

	Get_String(section, entry, "", buffer, sizeof(buffer));
	if (strlen(buffer) != 0) {
		return(Vox_From_Name(buffer));
	}
	return(defvalue);
}

// FUNCTION: 0x00474FF0
RTTIType CCINIClass::Get_RTTIType(char const * section, char const * entry, RTTIType defvalue) const
{
	char buffer[32];

	Get_String(section, entry, Name_From_RTTI(defvalue), buffer, sizeof(buffer));
	return(RTTI_From_Name(buffer));
}

// FUNCTION: 0x00475030
bool CCINIClass::Put_RTTIType(char const * section, char const * entry, RTTIType value)
{
	return(Put_String(section, entry, Name_From_RTTI(value)));
}

// FUNCTION: 0x00475060
int CCINIClass::Get_BuildCat(char const * section, char const * entry, int defvalue) const
{
	char buffer[32];

	Get_String(section, entry, Name_From_BuildCat(defvalue), buffer, sizeof(buffer));
	return(BuildCat_From_Name(buffer));
}

// FUNCTION: 0x004750A0
bool CCINIClass::Put_BuildCat(char const * section, char const * entry, int value)
{
	return(Put_String(section, entry, Name_From_BuildCat(value)));
}

// FUNCTION: 0x004750D0
int CCINIClass::Get_Owners(char const * section, char const * entry, int defvalue) const
{
	char buffer[128];
	int ownable = defvalue;

	if (Get_String(section, entry, "", buffer, sizeof(buffer))) {
		ownable = 0;
		char * name = strtok(buffer, ",");

		while (name) {
			ownable |= Owner_From_Name(name);
			name = strtok(NULL, ",");
		}
	}
	return(ownable);
}

// FUNCTION: 0x00475150
bool CCINIClass::Put_Owners(char const * section, char const * entry, int value)
{
	char buffer[128];

	buffer[0] = '\0';

	if (value == 0) {
		return(true);
	}

	for (HousesType house = HOUSE_FIRST; house < HouseTypes.Count(); house++) {
		HouseTypeClass * type = HouseTypes[house];
		if ((value & (1 << type->House)) != 0) {
			if (buffer[0] != '\0') {
				strcat(buffer, ",");
			}
			strcat(buffer, type->Name());
		}
	}

	if (buffer[0] != '\0') {
		return(Put_String(section, entry, buffer));
	}
	return(true);
}

// FUNCTION: 0x00475260
int CCINIClass::Get_Houses(char const * section, char const * entry, int defvalue) const
{
	char buffer[128];
	int ownable = defvalue;

	if (Get_String(section, entry, "", buffer, sizeof(buffer))) {
		ownable = 0;
		char * name = strtok(buffer, ",");

		while (name) {
			ownable |= (1 << FUN_0050C170(name));
			name = strtok(NULL, ",");
		}
	}
	return(ownable);
}

// FUNCTION: 0x004752E0
bool CCINIClass::Put_Houses(char const * section, char const * entry, int value)
{
	char buffer[128];

	buffer[0] = '\0';

	if (value == 0) {
		return(true);
	}

	for (int house = 0; house < Houses.Count(); house++) {
		HouseClass * ptr = Houses[house];
		if ((value & (1 << ptr->HeapID)) != 0) {
			if (buffer[0] != '\0') {
				strcat(buffer, ",");
			}
			strcat(buffer, ptr->IniName);
		}
	}

	if (buffer[0] != '\0') {
		return(Put_String(section, entry, buffer));
	}
	return(true);
}

// FUNCTION: 0x004753F0
ArmorType CCINIClass::Get_ArmorType(char const * section, char const * entry, ArmorType defvalue) const
{
	char buffer[128];

	Get_String(section, entry, ArmorName[defvalue], buffer, sizeof(buffer));
	return(Armor_From_Name(buffer));
}

// FUNCTION: 0x00475440
bool CCINIClass::Put_ArmorType(char const * section, char const * entry, ArmorType value)
{
	return(Put_String(section, entry, ArmorName[value]));
}

// FUNCTION: 0x00475460
bool CCINIClass::Put_VocType(char const * section, char const * entry, VocType value)
{
	if ((char)value == VOC_NONE) {
		return(Put_String(section, entry, "<none>"));
	}
	return(Put_String(section, entry, FUN_00751590(value)));
}

// FUNCTION: 0x004754B0
LandType CCINIClass::Get_LandType(char const * section, char const * entry, LandType defvalue) const
{
	char buffer[128];

	if (Get_String(section, entry, Name_From_Land(defvalue), buffer, sizeof(buffer))) {
		return(Land_From_Name(buffer));
	}
	return(defvalue);
}

// FUNCTION: 0x00475510
bool CCINIClass::Put_LandType(char const * section, char const * entry, LandType value)
{
	return(Put_String(section, entry, Name_From_Land(value)));
}

// FUNCTION: 0x00475540
HousesType CCINIClass::Get_HousesType(char const * section, char const * entry, HousesType defvalue) const
{
	char buffer[128];

	if (Get_String(section, entry, "", buffer, sizeof(buffer))) {
		HousesType house = (HousesType)FUN_00510FB0(buffer);
		if (FUN_00510F60(house)) {
			return(house);
		}
		house = HouseTypeClass::From_Name(buffer);
		if (house == HOUSE_NONE) {
			HouseTypeClass * type = new HouseTypeClass(buffer);
			return((HousesType)HouseTypes.ID(type));
		}
		return(house);
	}
	return(defvalue);
}

// FUNCTION: 0x00475610
bool CCINIClass::Put_HousesType(char const * section, char const * entry, HousesType value)
{
	char buffer[128];

	strcpy(buffer, "<none>");
	if (FUN_00510F60(value)) {
		FUN_00510E10(value, buffer, sizeof(buffer));
	} else if (value == -2) {
		strcpy(buffer, "<random>");
	} else if (value == -3) {
		strcpy(buffer, "<random>");
	} else if (value != HOUSE_NONE && value >= 0 && value < HouseTypes.Count()) {
		strcpy(buffer, HouseTypes[value]->Name());
	}
	return(Put_String(section, entry, buffer));
}

// FUNCTION: 0x004756F0
SideType CCINIClass::Get_Side(char const * section, char const * entry, SideType defvalue) const
{
	char buffer[128];

	if (Get_String(section, entry, "", buffer, sizeof(buffer))) {
		SideType side = SideClass::From_Name(buffer);
		if (side == SIDE_NONE) {
			SideClass * type = new SideClass(buffer);
			return((SideType)Sides.ID(type));
		}
		return(side);
	}
	return(defvalue);
}

// FUNCTION: 0x004757A0
bool CCINIClass::Put_Side(char const * section, char const * entry, SideType value)
{
	char const * name = "<none>";
	if (value != SIDE_NONE) {
		name = Sides[value]->Name();
	}
	return(Put_String(section, entry, name));
}

// FUNCTION: 0x004757D0
VQType CCINIClass::Get_VQType(char const * section, char const * entry, VQType defvalue) const
{
	char buffer[128];

	if (Get_String(section, entry, "", buffer, sizeof(buffer))) {
		VQType vq = VQ_From_Name(buffer);
		if (vq != VQ_NONE) {
			return(vq);
		}
	}
	return(defvalue);
}

// FUNCTION: 0x00475820
bool CCINIClass::Put_VQType(char const * section, char const * entry, VQType value)
{
	if ((unsigned)value >= (unsigned)Movies.Count()) {
		return(Put_String(section, entry, "<none>"));
	}
	return(Put_String(section, entry, Movies[value]));
}

// FUNCTION: 0x00475870
TheaterType CCINIClass::Get_TheaterType(char const * section, char const * entry, TheaterType defvalue) const
{
	char buffer[128];

	if (Get_String(section, entry, "", buffer, sizeof(buffer))) {
		return(Theater_From_Name(buffer));
	}
	return(defvalue);
}

// FUNCTION: 0x004758C0
bool CCINIClass::Put_TheaterType(char const * section, char const * entry, TheaterType value)
{
	return(Put_String(section, entry, Theaters[value].Name));
}

// FUNCTION: 0x004758F0
ThemeType CCINIClass::Get_ThemeType(char const * section, char const * entry, ThemeType defvalue) const
{
	char buffer[128];

	if (Get_String(section, entry, "", buffer, sizeof(buffer))) {
		return(Theme.From_Name(buffer));
	}
	return(defvalue);
}

// FUNCTION: 0x00475950
bool CCINIClass::Put_ThemeType(char const * section, char const * entry, ThemeType value)
{
	return(Put_String(section, entry, Theme.Base_Name(value)));
}

// FUNCTION: 0x00475980
SourceType CCINIClass::Get_SourceType(char const * section, char const * entry, SourceType defvalue) const
{
	char buffer[128];

	if (Get_String(section, entry, "", buffer, sizeof(buffer))) {
		return(Source_From_Name(buffer));
	}
	return(defvalue);
}

// FUNCTION: 0x004759D0
bool CCINIClass::Put_SourceType(char const * section, char const * entry, SourceType value)
{
	return(Put_String(section, entry, SourceName[value]));
}

// FUNCTION: 0x004759F0
CrateType CCINIClass::Get_CrateType(char const * section, char const * entry, CrateType defvalue) const
{
	char buffer[128];

	if (Get_String(section, entry, "", buffer, sizeof(buffer))) {
		return(Crate_From_Name(buffer));
	}
	return(defvalue);
}

// FUNCTION: 0x00475A40
bool CCINIClass::Put_CrateType(char const * section, char const * entry, CrateType value)
{
	return(Put_String(section, entry, ::Powerups::Effects[value]));
}

// FUNCTION: 0x00475A60
int CCINIClass::Get_Buildings(char const * section, char const * entry, int defvalue) const
{
	char buffer[128];
	int pre;

	if (Get_String(section, entry, "", buffer, sizeof(buffer))) {
		pre = 0;
		char * token = strtok(buffer, ",");
		while (token != NULL && *token != '\0') {
			if (!strcmpi(token, "POWER")) {
				pre |= 1L << (32 - 1);
			}
			if (!strcmpi(token, "FACTORY")) {
				pre |= 1L << (32 - 2);
			}
			if (!strcmpi(token, "BARRACKS")) {
				pre |= 1L << (32 - 3);
			}
			if (!strcmpi(token, "RADAR")) {
				pre |= 1L << (32 - 4);
			}
			StructType building = BuildingTypeClass::From_Name(token);
			if (building != STRUCT_NONE) {
				pre |= (1L << building);
			}
			token = strtok(NULL, ",");
		}
	} else {
		pre = defvalue;
	}

	return(pre);
}

// FUNCTION: 0x00475B70
bool CCINIClass::Put_Buildings(char const * section, char const * entry, int value)
{
	char buffer[128] = "";
	int maxi = (32 < BuildingTypes.Count()) ? 32 : BuildingTypes.Count();

	for (StructType index = STRUCT_FIRST; index < maxi; index++) {
		if ((value & (1L << index)) != 0) {

			if (buffer[0] != '\0') {
				strcat(buffer, ",");
			}
			strcat(buffer, BuildingTypes[index]->IniName);
		}
	}

	return(Put_String(section, entry, buffer));
}

// FUNCTION: 0x00475C70
bool CCINIClass::Put_VocType_List(char const * section, char const * entry, TypeList<int> value)
{
	char buffer[128] = "";
	for (int index = 0; index < value.Count(); index++) {
		if (buffer[0]) {
			strcat(buffer, ",");
		}
		strcat(buffer, FUN_00751590(value[index]));
	}
	return(Put_String(section, entry, buffer));
}

// FUNCTION: 0x00475D70
TypeList<int> CCINIClass::Get_IntList(const char * section, const char * entry, TypeList<int> defvalue) const
{
	char buffer[MAX_LINE_LENGTH];

	if (Get_String(section, entry, "", buffer, sizeof(buffer))) {
		TypeList<int> list;
		char * token = strtok(buffer, ",");
		while (token != NULL && *token != '\0') {
			list.Add(atoi(token));
			token = strtok(NULL, ",");
		}
		return(list);
	}
	return(defvalue);
}

// FUNCTION: 0x00475F30
bool CCINIClass::Put_IntList(char const * section, char const * entry, TypeList<int> const & value)
{
	char buffer[MAX_LINE_LENGTH] = "";
	char number[12];

	for (int index = 0; index < value.Count(); index++) {
		if (buffer[0]) {
			strcat(buffer, ",");
		}
		sprintf(number, "%d", value[index]);
		strcat(buffer, number);
	}
	return(Put_String(section, entry, buffer));
}

// FUNCTION: 0x00476240
bool CCINIClass::Put_Target_List(const char * section, const char * entry, TypeList<int> const & value)
{
	char buffer[MAX_LINE_LENGTH] = "";

	for (int index = 0; index < value.Count(); index++) {
		if (buffer[0]) {
			strcat(buffer, ",");
		}
		TargetClass target;
		target.Decode(value[index]);
		TechnoTypeClass *ttype = target.As_TechnoType();
		strcat(buffer, ttype->Name());
	}
	return(Put_String(section, entry, buffer));
}

// FUNCTION: 0x00476340
TPoint3D<float> CCINIClass::Get_Vector(char const * section, char const * entry, TPoint3D<float> const & defvalue) const
{
	char buffer[MAX_LINE_LENGTH];
	TPoint3D<float> point = defvalue;

	if (Get_String(section, entry, "", buffer, sizeof(buffer))) {
		char * token = strtok(buffer, ",");
		point.X = atof(token);
		token = strtok(NULL, ",");
		point.Y = atof(token);
		token = strtok(NULL, ",");
		point.Z = atof(token);
		return(point);
	}
	return(defvalue);
}

// FUNCTION: 0x00476420
TPoint3D<int> CCINIClass::Get_Offset(char const * section, char const * entry, TPoint3D<int> const & defvalue) const
{
	char buffer[MAX_LINE_LENGTH];
	TPoint3D<int> point = defvalue;

	if (Get_String(section, entry, "", buffer, sizeof(buffer))) {
		char * token = strtok(buffer, ",");
		point.X = atoi(token);
		token = strtok(NULL, ",");
		point.Y = atoi(token);
		token = strtok(NULL, ",");
		point.Z = atoi(token);
		return(point);
	}
	return(defvalue);
}

// FUNCTION: 0x004764F0
TypeList<TechnoTypeClass *> CCINIClass::Get_TechnoType_List(const char * section, const char * entry, TypeList<TechnoTypeClass *> defvalue) const
{
	char buffer[MAX_LINE_LENGTH];

	if (Get_String(section, entry, "", buffer, sizeof(buffer))) {
		TypeList<TechnoTypeClass *> list;
		char * token = strtok(buffer, ",");
		while (token != NULL && *token != '\0') {
			for (int index = 0; index < TechnoTypes.Count(); index++) {
				if (!strcmpi(token, TechnoTypes[index]->Name())) {
					list.Add(TechnoTypes[index]);
					break;
				}
			}
			token = strtok(NULL, ",");
		}
		return(list);
	}
	return(defvalue);
}

// FUNCTION: 0x004766F0
bool CCINIClass::Put_TechnoType_List(char const * section, char const * entry, TypeList<TechnoTypeClass *> const & value)
{
	char buffer[MAX_LINE_LENGTH] = "";

	for (int index = 0; index < value.Count(); index++) {
		if (buffer[0]) {
			strcat(buffer, ",");
		}
		strcat(buffer, value[index]->Name());
	}
	return(Put_String(section, entry, buffer));
}

// FUNCTION: 0x004767C0
TypeList<int> CCINIClass::Get_House_List(const char * section, const char * entry, TypeList<int> defvalue) const
{
	char buffer[128];

	if (Get_String(section, entry, "", buffer, sizeof(buffer))) {
		TypeList<int> list;
		char * token = strtok(buffer, ",");
		while (token != NULL && *token != '\0') {
			int house = (int)HouseTypeClass::From_Name(token);
			if (house != HOUSE_NONE) {
				list.Add(house);
			}
			else {
				SideType side = SideClass::From_Name(token);
				if (side != SIDE_NONE) {
					SideClass * otherside = Sides[side];
					for (int index = 0; index < otherside->Houses.Count(); index++) {
						list.Add(otherside->Houses[index]);
					}
				}
			}
			token = strtok(NULL, ",");
		}
		return(list);
	}
	return(defvalue);
}

// FUNCTION: 0x00476A20
bool CCINIClass::Put_House_List(char const * section, char const * entry, TypeList<HousesType> value)
{
	char buffer[128] = "";

	for (int index = 0; index < value.Count(); index++) {
		if (buffer[0]) {
			strcat(buffer, ",");
		}
		strcat(buffer, HouseTypes[value[index]]->Name());
	}
	return(Put_String(section, entry, buffer));
}

// FUNCTION: 0x00476B20
TypeList<RGBClass> CCINIClass::Get_RGBClass_List(const char * section, const char * entry, TypeList<RGBClass> defvalue) const
{
	char buffer[MAX_LINE_LENGTH];

	if (Get_String(section, entry, "", buffer, sizeof(buffer))) {
		TypeList<RGBClass> list;
		char * token = strtok(buffer, ",");
		while (token != NULL && *token != '\0') {
			RGBClass c(0,0,0);
			bool valid = true;

			if (token && *token) {
				c.Set_Red(atoi(token + 1));
			} else {
				valid = false;
			}

			token = strtok(NULL, ",");
			if (token && *token) {
				c.Set_Green(atoi(token));
			} else {
				valid = false;
			}

			token = strtok(NULL, ",");
			if (token && *token) {
				token[strlen(token) - 1] = '\0';
				c.Set_Blue(atoi(token));
			} else {
				valid = false;
			}

			if (valid) {
				list.Add(c);
			}

			token = strtok(NULL, ",");
		}
		return(list);
	}
	return(defvalue);
}

// FUNCTION: 0x00476D80
int CCINIClass::Get_Unique_ID(void) const
{
	if (!IsDigestPresent) {
		((CCINIClass *)this)->Calculate_Message_Digest();
	}

	return(CRCEngine()(&Digest[0], sizeof(Digest)));
}

// FUNCTION: 0x00476EB0
TechnoTypeClass * CCINIClass::Get_TechnoType(char const * section, char const * entry) const
{
	char buffer[128];

	int type = -1;
	TechnoTypeClass * target = NULL;

	Get_String(section, entry, "<none>", buffer, sizeof(buffer));

	if (type == -1) {
		type = (int)InfantryTypeClass::From_Name(buffer);
		if (type != -1) {
			target = InfantryTypes[type];
		}
	}
	if (type == -1) {
		type = (int)UnitTypeClass::From_Name(buffer);
		if (type != -1) {
			target = UnitTypes[type];
		}
	}
	if (type == -1) {
		type = (int)AircraftTypeClass::From_Name(buffer);
		if (type != -1) {
			target = AircraftTypes[type];
		}
	}
	if (type == -1) {
		type = (int)BuildingTypeClass::From_Name(buffer);
		if (type != -1) {
			target = BuildingTypes[type];
		}
	}
	return(target);
}

// FUNCTION: 0x00476F70
bool CCINIClass::Put_Target(char const * section, char const * entry, TargetClass const & value)
{
	TechnoTypeClass *ttype = value.As_TechnoType();
	if (ttype != NULL) {
		return(Put_String(section, entry, ttype->Name()));
	}
	return(Put_String(section, entry, "<none>"));
}

// FUNCTION: 0x00476FC0
SpeedType CCINIClass::Get_SpeedType(char const * section, char const * entry, SpeedType defvalue) const
{
	char buffer[128];

	if (Get_String(section, entry, Name_From_Speed(defvalue), buffer, sizeof(buffer))) {
		return(Speed_From_Name(buffer));
	}
	return(defvalue);
}

// FUNCTION: 0x00477020
bool CCINIClass::Put_SpeedType(char const * section, char const * entry, SpeedType value)
{
	return(Put_String(section, entry, Name_From_Speed(value)));
}

// FUNCTION: 0x00477050
LayerType CCINIClass::Get_LayerType(char const * section, char const * entry, LayerType defvalue) const
{
	char buffer[128];

	if (Get_String(section, entry, Name_From_Layer(defvalue), buffer, sizeof(buffer))) {
		return(Layer_From_Name(buffer));
	}
	return(defvalue);
}

// FUNCTION: 0x004770B0
bool CCINIClass::Put_LayerType(char const * section, char const * entry, LayerType value)
{
	return(Put_String(section, entry, Name_From_Layer(value)));
}

// FUNCTION: 0x004770E0
TypeList<int> CCINIClass::Get_BuildingType_List(CCINIClass const & ini, char const * section, char const * entry, TypeList<int> defvalue)
{
	char buffer[128];

	if (ini.Get_String(section, entry, "", buffer, sizeof(buffer))) {
		TypeList<int> list;
		char * token = strtok(buffer, ",");
		while (token != NULL && *token != '\0') {

			bool isgroup = false;
			if (!strcmpi(token, "POWER")) {
				list.Add(STRUCT_G_POWER);
				isgroup = true;
			}
			if (!strcmpi(token, "FACTORY")) {
				list.Add(STRUCT_G_FACTORY);
				isgroup = true;
			}
			if (!strcmpi(token, "BARRACKS")) {
				list.Add(STRUCT_G_BARRACKS);
				isgroup = true;
			}
			if (!strcmpi(token, "RADAR")) {
				list.Add(STRUCT_G_RADAR);
				isgroup = true;
			}
			if (!strcmpi(token, "TECH")) {
				list.Add(STRUCT_G_TECH);
				isgroup = true;
			}
			if (!strcmpi(token, "PROC")) {
				list.Add(STRUCT_G_GDIFACTORY);
				isgroup = true;
			}
			if (!isgroup) {
				int building = BuildingTypeClass::From_Name(token);
				if (building != STRUCT_NONE) {
					list.Add(building);
				}
			}

			token = strtok(NULL, ",");
		}
		return(list);
	}

	return(defvalue);
}

// FUNCTION: 0x00477440
void CCINIClass::Get_Int3(char const * section, char const * entry, int * first, int * second, int * third) const
{
	char buffer[512] = "";

	if (Get_String(section, entry, "", buffer, sizeof(buffer))) {
		char * token = strtok(buffer, ",");
		if (token) *first = atoi(token);
		token = strtok(NULL, ",");
		if (token) *second = atoi(token);
		token = strtok(NULL, ",");
		if (token) *third = atoi(token);
	}
}

// FUNCTION: 0x00477510
bool CCINIClass::Put_Int3(char const * section, char const * entry, int first, int second, int third)
{
	char buffer[512] = "";

	sprintf(buffer, "%d,%d,%d", first, second, third);
	return(Put_String(section, entry, buffer));
}

// FUNCTION: 0x00477590
int CCINIClass::Get_VHPScan(char const * section, char const * entry, int defvalue) const
{
	char buffer[128];

	Get_String(section, entry, _vhpscan[defvalue], buffer, sizeof(buffer));
	char * token = strtok(buffer, ",");
	if (!strcmpi(token, "None")) {
		return(0);
	}
	if (!strcmpi(token, "Normal")) {
		return(1);
	}
	if (!strcmpi(token, "Strong")) {
		return(2);
	}
	return(defvalue);
}

// FUNCTION: 0x00477640
AbilityFlagsType CCINIClass::Get_Abilities(char const * section, char const * entry, AbilityFlagsType const & defvalue) const
{
	char buffer[128];

	if (Get_String(section, entry, "", buffer, sizeof(buffer))) {
		AbilityFlagsType abilities;
		char * token = strtok(buffer, ",");
		while (token != NULL && *token != '\0') {
			AbilityType ability = Ability_From_Name(token);
			if (ability != ABILITY_NONE) {
				abilities[ability] = true;
			}
			token = strtok(NULL, ",");
		}
		return(abilities);
	}
	return(defvalue);
}

// FUNCTION: 0x00477740 Pipe::End

// FUNCTION: 0x00477750 SHAPipe::~SHAPipe

// FUNCTION: 0x00477770 FileStraw::~FileStraw
// FUNCTION: 0x00477790 FilePipe::~FilePipe

// FUNCTION: 0x004777B0 ?$TypeList@H::?$TypeList@H
template class TypeList<int>;

// FUNCTION: 0x00477840 ?$DynamicVectorClass@H::Clear
// FUNCTION: 0x00477870 ?$DynamicVectorClass@H::ID
// FUNCTION: 0x00477890 ?$VectorClass@H::GetItem

// FUNCTION: 0x004778A0 ?$DynamicVectorClass@PAVTechnoTypeClass@@::Clear
template class DynamicVectorClass<TechnoTypeClass *>;

// FUNCTION: 0x004778D0 ?$DynamicVectorClass@PAVTechnoTypeClass@@::ID
// FUNCTION: 0x004778F0 ?$VectorClass@PAVTechnoTypeClass@@::GetItem

// FUNCTION: 0x00477900 ?$DynamicVectorClass@VRGBClass@@::Clear
template class DynamicVectorClass<RGBClass>;

// FUNCTION: 0x00477930 ?$DynamicVectorClass@VRGBClass@@::ID
// FUNCTION: 0x00477960 ?$VectorClass@VRGBClass@@::GetItem

// FUNCTION: 0x00477990 ?$TypeList@H::~?$TypeList@H
// FUNCTION: 0x004779E0 ?$DynamicVectorClass@H::?$DynamicVectorClass@H

// FUNCTION: 0x00477A70 ?$TypeList@PAVTechnoTypeClass@@::~?$TypeList@PAVTechnoTypeClass@@
template class TypeList<TechnoTypeClass *>;

// FUNCTION: 0x00477AC0 ?$TypeList@VRGBClass@@::~?$TypeList@VRGBClass@@
template class TypeList<RGBClass>;

// FUNCTION: 0x00477B10 ?$DynamicVectorClass@H::~?$DynamicVectorClass@H
// FUNCTION: 0x00477B60 ?$VectorClass@H::?$VectorClass@H
// FUNCTION: 0x00477BE0 ?$VectorClass@H::?$VectorClass@H
// FUNCTION: 0x00477C30 ?$VectorClass@H::operator==
// FUNCTION: 0x00477C70 ?$VectorClass@H::Resize
// FUNCTION: 0x00477D20 ?$VectorClass@H::Clear
// FUNCTION: 0x00477D50 ?$VectorClass@H::ID
// FUNCTION: 0x00477D90 ?$VectorClass@H::ID
template class VectorClass<int>;

// FUNCTION: 0x00477DB0 ?$DynamicVectorClass@H::?$DynamicVectorClass@H
// FUNCTION: 0x00477E10 ?$DynamicVectorClass@H::Resize
// FUNCTION: 0x00477EC0 ?$DynamicVectorClass@H::Add
// FUNCTION: 0x00477F10 ?$DynamicVectorClass@H::ID
template class DynamicVectorClass<int>;

// FUNCTION: 0x00477F40 ?$VectorClass@PAVTechnoTypeClass@@::?$VectorClass@PAVTechnoTypeClass@@
// FUNCTION: 0x00477FC0 ?$VectorClass@PAVTechnoTypeClass@@::operator==
// FUNCTION: 0x00478000 ?$VectorClass@PAVTechnoTypeClass@@::Resize
// FUNCTION: 0x004780B0 ?$VectorClass@PAVTechnoTypeClass@@::Clear
// FUNCTION: 0x004780E0 ?$VectorClass@PAVTechnoTypeClass@@::ID
// FUNCTION: 0x00478120 ?$VectorClass@PAVTechnoTypeClass@@::ID
template class VectorClass<TechnoTypeClass *>;

// FUNCTION: 0x00478140 ?$DynamicVectorClass@PAVTechnoTypeClass@@::Resize
// FUNCTION: 0x004781F0 ?$DynamicVectorClass@PAVTechnoTypeClass@@::ID
template class DynamicVectorClass<TechnoTypeClass *>;

// FUNCTION: 0x00478220 ?$VectorClass@VRGBClass@@::?$VectorClass@VRGBClass@@
// FUNCTION: 0x004782E0 ?$VectorClass@VRGBClass@@::operator==
// FUNCTION: 0x00478340 ?$VectorClass@VRGBClass@@::Resize
// FUNCTION: 0x00478440 ?$VectorClass@VRGBClass@@::Clear
// FUNCTION: 0x00478470 ?$VectorClass@VRGBClass@@::ID
// FUNCTION: 0x004784C0 ?$VectorClass@VRGBClass@@::ID
template class VectorClass<RGBClass>;

// FUNCTION: 0x004784F0 ?$DynamicVectorClass@VRGBClass@@::Resize
// FUNCTION: 0x004785F0 ?$DynamicVectorClass@VRGBClass@@::ID
template class DynamicVectorClass<RGBClass>;

// FUNCTION: 0x00478630 ?$VectorClass@H::~?$VectorClass@H
// FUNCTION: 0x00478680 ?$VectorClass@PAVTechnoTypeClass@@::~?$VectorClass@PAVTechnoTypeClass@@
// FUNCTION: 0x004786D0 ?$VectorClass@VRGBClass@@::~?$VectorClass@VRGBClass@@
// FUNCTION: 0x00478720 ?$VectorClass@H::operator=
// FUNCTION: 0x00478790 ?$VectorClass@PAVTechnoTypeClass@@::?$VectorClass@PAVTechnoTypeClass@@
