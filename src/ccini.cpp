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

static inline int _Scale_To_256(int val)
{
	val = MIN(val, 100);
	val = MAX(val, 0);
	val = ((val * (MPH_LIGHT_SPEED + 1)) / 100);
	val = MIN(val, MPH_LIGHT_SPEED);
	return(val);
}

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

// FUNCTION: 0x004753F0
ArmorType CCINIClass::Get_ArmorType(char const * section, char const * entry, ArmorType defvalue) const
{
	char buffer[128];

	Get_String(section, entry, ArmorName[defvalue], buffer, sizeof(buffer));
	return(Armor_From_Name(buffer));
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

// FUNCTION: 0x004759F0
CrateType CCINIClass::Get_CrateType(char const * section, char const * entry, CrateType defvalue) const
{
	char buffer[128];

	if (Get_String(section, entry, "", buffer, sizeof(buffer))) {
		return(Crate_From_Name(buffer));
	}
	return(defvalue);
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

// FUNCTION: 0x00477EC0 ?$DynamicVectorClass@H::Add
template class DynamicVectorClass<int>;
