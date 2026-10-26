// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).
#pragma once

#include "coord.h"
#include "hsv.h"
#include "ini.h"
#include "pk.h"
#include "point.h"
#include "rgb.h"
#include "target.h"
#include "typelist.h"
#include "voc.h"

#include "action.hh"
#include "anim.hh"
#include "armor.hh"
#include "bsize.hh"
#include "bullet.hh"
#include "category.hh"
#include "crate.hh"
#include "house.hh"
#include "infantry.hh"
#include "land.hh"
#include "layer.hh"
#include "mph.hh"
#include "mzone.hh"
#include "overlay.hh"
#include "pip.hh"
#include "rtti.hh"
#include "side.hh"
#include "source.hh"
#include "speed.hh"
#include "struct.hh"
#include "super.hh"
#include "terrain.hh"
#include "theater.hh"
#include "theme.hh"
#include "unit.hh"
#include "voc.hh"
#include "vox.hh"
#include "vq.hh"
#include "warhead.hh"
#include "weapon.hh"

class TriggerTypeClass;
class HouseTypeClass;
class TechnoTypeClass;

enum INIScopeType {
	SCOPE_LOCAL,
	SCOPE_GLOBAL,
};

struct TargetStruct {
	/*
	 * This is the target value, stored in the database as a hexadecimal number. The
	 * structure wraps it only so that the target accessors overload apart from the plain
	 * integer ones.
	 */
	int Value;
};

/*
**	The advanced version of the INI database manager. It handles the C&C expansion types and
**	identifiers. In addition, it automatically stores a message digest with the INI data
**	so that verification can occur.
*/
class CCINIClass : public INIClass
{
		typedef INIClass BASECLASS;

	public:
		CCINIClass(void) : IsDigestPresent(false) {}

		int ReadCCFile(FileClass & file, bool withdigest, bool loadcomments);
		int Load(FileClass & file, bool withdigest, bool loadcomments = false);
		int Load(Straw & file, bool withdigest, bool loadcomments = false);
		int Save(FileClass & file, bool withdigest) const;
		int Save(Pipe & pipe, bool withdigest) const;

		int Get_Buildings(char const * section, char const * entry, int defvalue) const;
		ArmorType Get_ArmorType(char const * section, char const * entry, ArmorType defvalue) const;
		HousesType Get_HousesType(char const * section, char const * entry, HousesType defvalue) const;
		LEPTON Get_Lepton(char const * section, char const * entry, LEPTON defvalue) const;
		MPHType Get_MPHType(char const * section, char const * entry, MPHType defvalue) const;
		SourceType Get_SourceType(char const * section, char const * entry, SourceType defvalue) const;
		TheaterType Get_TheaterType(char const * section, char const * entry, TheaterType defvalue) const;
		ThemeType Get_ThemeType(char const * section, char const * entry, ThemeType defvalue) const;
		VQType Get_VQType(char const * section, char const * entry, VQType defvalue) const;
		VocType Get_VocType(char const * section, char const * entry, VocType defvalue) const;
		int Get_Owners(char const * section, char const * entry, int defvalue) const;
		int Get_Houses(char const * section, char const * entry, int defvalue) const;
		CrateType Get_CrateType(char const * section, char const * entry, CrateType defvalue) const;

		SideType Get_Side(char const * section, char const * entry, SideType defvalue) const;
		int Get_Angle(char const * section, char const * entry, int defvalue) const;
		Cell Get_Cell(char const * section, char const * entry, Cell const & defvalue) const;
		PipEnum Get_PipEnum(char const * section, char const * entry, PipEnum defvalue) const;
		PipScaleType Get_PipScaleType(char const * section, char const * entry, PipScaleType defvalue) const;
		CategoryType Get_CategoryType(char const * section, char const * entry, CategoryType defvalue) const;
		TargetStruct Get_xTarget(char const * section, char const * entry, TargetStruct defvalue) const;
		int Get_Scheme_Index(char const * section, char const * entry, int defvalue) const;
		RGBClass Get_RGBClass(char const * section, char const * entry, RGBClass const & defvalue) const;
		HSVClass Get_HSVClass(char const * section, char const * entry, HSVClass const & defvalue) const;
		BSizeType Get_BSizeType(char const * section, char const * entry, BSizeType defvalue) const;
		MZoneType Get_MZoneType(char const * section, char const * entry, MZoneType defvalue) const;
		ActionType Get_ActionType(char const * section, char const * entry, ActionType defvalue) const;
		int Get_BuildCat(char const * section, char const * entry, int defvalue) const;
		SuperWeaponType Get_SuperWeaponType(char const * section, char const * entry, SuperWeaponType defvalue) const;
		VoxType Get_VoxType(char const * section, char const * entry, VoxType defvalue) const;
		RTTIType Get_RTTIType(char const * section, char const * entry, RTTIType defvalue) const;
		LandType Get_LandType(char const * section, char const * entry, LandType defvalue) const;
		TypeList<int> Get_IntList(const char * section, const char * entry, TypeList<int> defvalue) const;
		TypeList<int> Get_Target_List(const char * section, const char * entry, TypeList<int> defvalue) const; /// The values are TargetClass encoded.
		TPoint3D<float> Get_Vector(char const * section, char const * entry, TPoint3D<float> const & defvalue) const;
		TPoint3D<int> Get_Offset(char const * section, char const * entry, TPoint3D<int> const & defvalue) const;
		TypeList<TechnoTypeClass *> Get_TechnoType_List(const char * section, const char * entry, TypeList<TechnoTypeClass *> defvalue) const;
		TypeList<int> Get_House_List(const char * section, const char * entry, TypeList<int> defvalue) const;
		TargetClass Get_Target(char const * section, char const * entry, TargetClass const & defvalue) const;
		SpeedType Get_SpeedType(char const * section, char const * entry, SpeedType defvalue) const;
		static TypeList<int> Get_VocType_List(CCINIClass const & ini, const char * section, const char * entry, TypeList<int> defvalue);

		static TypeList<int> Get_BuildingType_List(CCINIClass const & ini, char const * section, char const * entry, TypeList<int> defvalue);
		TechnoTypeClass * Get_TechnoType(char const * section, char const * entry) const;
		LayerType Get_LayerType(char const * section, char const * entry, LayerType defvalue) const;
		void Get_Int3(char const * section, char const * entry, int * first, int * second, int * third) const;
		int Get_VHPScan(char const * section, char const * entry, int defvalue) const;
		AbilityFlagsType Get_Abilities(char const * section, char const * entry, AbilityFlagsType const & defvalue) const;
		TypeList<RGBClass> Get_RGBClass_List(const char * section, const char * entry, TypeList<RGBClass> defvalue) const;

		bool Put_Buildings(char const * section, char const * entry, int value);
		bool Put_ArmorType(char const * section, char const * entry, ArmorType value);
		bool Put_HousesType(char const * section, char const * entry, HousesType value);
		bool Put_Lepton(char const * section, char const * entry, LEPTON value);
		bool Put_MPHType(char const * section, char const * entry, MPHType value);
		bool Put_VQType(char const * section, char const * entry, VQType value);
		bool Put_Owners(char const * section, char const * entry, int value);
		bool Put_Houses(char const * section, char const * entry, int value);
		bool Put_BuildCat(char const * section, char const * entry, int value);
		bool Put_SourceType(char const * section, char const * entry, SourceType value);
		bool Put_TheaterType(char const * section, char const * entry, TheaterType value);
		bool Put_ThemeType(char const * section, char const * entry, ThemeType value);
		bool Put_VocType(char const * section, char const * entry, VocType value);
		bool Put_CrateType(char const * section, char const * entry, CrateType value);

		bool Put_Side(char const * section, char const * entry, SideType value);
		bool Put_Angle(char const * section, char const * entry, int value);
		bool Put_Cell(char const * section, char const * entry, Cell const & value);
		bool Put_PipEnum(char const * section, char const * entry, PipEnum value);
		bool Put_PipScaleType(char const * section, char const * entry, PipScaleType value);
		bool Put_CategoryType(char const * section, char const * entry, CategoryType value);
		bool Put_xTarget(char const * section, char const * entry, TargetStruct value);
		bool Put_Scheme_Index(char const * section, char const * entry, int value);
		bool Put_RGBClass(char const * section, char const * entry, RGBClass const & value);
		bool Put_HSVClass(char const * section, char const * entry, HSVClass const & value);
		bool Put_BSizeType(char const * section, char const * entry, BSizeType value);
		bool Put_MZoneType(char const * section, char const * entry, MZoneType value);
		bool Put_ActionType(char const * section, char const * entry, ActionType value);
		bool Put_SuperWeaponType(char const * section, char const * entry, SuperWeaponType value);
		bool Put_VoxType(char const * section, char const * entry, VoxType value);
		bool Put_RTTIType(char const * section, char const * entry, RTTIType value);
		bool Put_LandType(char const * section, char const * entry, LandType value);
		bool Put_IntList(char const * section, char const * entry, TypeList<int> const & value);
		bool Put_Target_List(const char * section, const char * entry, TypeList<int> const & value);
		bool Put_Vector(char const * section, char const * entry, TPoint3D<float> const & value);
		bool Put_Offset(char const * section, char const * entry, TPoint3D<int> const & value);
		bool Put_TechnoType_List(char const * section, char const * entry, TypeList<TechnoTypeClass *> const & value);
		bool Put_House_List(char const * section, char const * entry, TypeList<HousesType> value);
		bool Put_Target(char const * section, char const * entry, TargetClass const & value);
		bool Put_SpeedType(char const * section, char const * entry, SpeedType value);
		bool Put_LayerType(char const * section, char const * entry, LayerType value);
		bool Put_Int3(char const * section, char const * entry, int first, int second, int third);
		bool Put_VocType_List(char const * section, char const * entry, TypeList<int> value);

		int Get_Unique_ID(void) const;

	private:
		void Calculate_Message_Digest(void);
		void Invalidate_Message_Digest(void);

		bool IsDigestPresent;

		/*
		**	This is the message digest (SHA) of the INI database that was embedded as part of
		**	the INI file.
		*/
		unsigned char Digest[20];
};


/***********************************************************************************************
 * CCINIClass::Get_VocType -- Fetch a voc (sound effect) from the INI database.                *
 *                                                                                             *
 *    This routine will fetch a voc number from the database. The voc number will either       *
 *    be a valid sound effect or VOC_NONE if no match could be found.                          *
 *                                                                                             *
 * INPUT:   section  -- Identifier for the section to search for the entry under.              *
 *                                                                                             *
 *          entry    -- The entry to search for.                                               *
 *                                                                                             *
 *          defvalue -- The default value to return if the entry could not be located.         *
 *                                                                                             *
 * OUTPUT:  Returns with the sound effect (VocType) from the INI database. If the entry could  *
 *          not be located, then the default value is returned.                                *
 *                                                                                             *
 * WARNINGS:   none                                                                            *
 *                                                                                             *
 * HISTORY:                                                                                    *
 *   07/03/1996 JLB : Created.                                                                 *
 *=============================================================================================*/
inline VocType CCINIClass::Get_VocType(char const * section, char const * entry, VocType defvalue) const
{
	char buffer[128];

	if (Get_String(section, entry, "", buffer, sizeof(buffer)) == 0) {
		return(defvalue);
	}
	VocType voc = VocClass::From_Name(buffer);
	if (voc == VOC_NONE) {
		return(defvalue);
	}
	return(voc);
}


inline TypeList<int> CCINIClass::Get_VocType_List(CCINIClass const & ini, const char * section, const char * entry, TypeList<int> defvalue)
{
	char buffer[128];

	if (ini.Get_String(section, entry, "", buffer, sizeof(buffer))) {
		TypeList<int> list;
		char * token = strtok(buffer, ",");
		while (token != NULL && *token != '\0') {
			VocEntry * voc = FUN_00751520(token);
			if (voc != NULL) {
				list.Add(FUN_007515C0(voc));
			}
			token = strtok(NULL, ",");
		}
		return(list);
	}
	return(defvalue);
}

extern const char * _mzones[MZONE_COUNT];
