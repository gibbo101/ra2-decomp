// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"
#include "abstype.h"
#include "ccini.h"
#include "crc.h"
#include "globals.h"
#include "stringtable.h"
#include "vector.h"
#include <cstdio>
#include <math.h>
#include "geometry.h"
#include "rect.h"

// SYNTHETIC: GAMEMD 0x00410610 SYMBOL
// _$E10
// SYNTHETIC: GAMEMD 0x00410640 SYMBOL
// _$E13
// SYNTHETIC: GAMEMD 0x00410660 SYMBOL
// _$E16
// SYNTHETIC: GAMEMD 0x00410680 SYMBOL
// _$E19
// SYNTHETIC: GAMEMD 0x004106A0 SYMBOL
// _$E22
// SYNTHETIC: GAMEMD 0x004106C0 SYMBOL
// _$E25
// SYNTHETIC: GAMEMD 0x004106E0 SYMBOL
// _$E28
// SYNTHETIC: GAMEMD 0x00410720 SYMBOL
// _$E31
// SYNTHETIC: GAMEMD 0x00410750 SYMBOL
// _$E34
// SYNTHETIC: GAMEMD 0x00410780 SYMBOL
// _$E37
// SYNTHETIC: GAMEMD 0x004107B0 SYMBOL
// _$E40
// SYNTHETIC: GAMEMD 0x004107C0 SYMBOL
// _$E43
// SYNTHETIC: GAMEMD 0x004107E0 SYMBOL
// _$E46

// The display name of a type whose INI section gives no UIName label: empty.
static wchar_t NoUIName_00887734[2];

// FUNCTION: GAMEMD 0x00410800
AbstractTypeClass::AbstractTypeClass(char const * ininame) :
	BASECLASS(),
	IniName(),
	UINameLabel(),
	GivenName()
{
	if (ininame == NULL) {
		char pstr[16];
		sprintf(pstr, "%08X", (unsigned int)this);
		IniName = TStringID<24>(pstr);
	} else {
		IniName = TStringID<24>(ininame);
	}
	GivenName = TStringID<48>(IniName);
	UIName = NoUIName_00887734;
	AbstractTypes.Add(this);
}

// FUNCTION: GAMEMD 0x00410960
AbstractTypeClass::AbstractTypeClass(NoInitClass const & x) :
	BASECLASS(x),
	IniName(x),
	UINameLabel(x),
	GivenName(x)
{
	if (*UINameLabel) {
#line 127 "D:\\ra2mdpost\\AbsType.cpp"
		UIName = StringTable::LoadString(UINameLabel, NULL, __FILE__, __LINE__);
	} else {
		UIName = NoUIName_00887734;
	}
}

// FUNCTION: GAMEMD 0x004109C0
AbstractTypeClass::~AbstractTypeClass(void)
{
	AbstractTypes.Delete(this);
}

// FUNCTION: GAMEMD 0x00410A40
bool AbstractTypeClass::operator == (char const * string) const
{
	return(stricmp(string, IniName) == 0);
}

// FUNCTION: GAMEMD 0x00410A60
bool AbstractTypeClass::Read_INI(CCINIClass & ini)
{
	char buffer[1024];

	ini.Reset();
	if (ini.Section_Present(IniName)) {
		ini.Get_String(IniName, "Name", GivenName, buffer, GivenName.Size());
		GivenName = TStringID<48>(buffer);
		ini.Get_String(IniName, "UIName", UINameLabel, buffer, UINameLabel.Size());
		UINameLabel = TStringID<31>(buffer);
		if (UINameLabel[0]) {
#line 215 "D:\\ra2mdpost\\AbsType.cpp"
			UIName = StringTable::LoadString(UINameLabel, NULL, __FILE__, __LINE__);
		} else {
			UIName = NoUIName_00887734;
		}
		return(true);
	}
	return(false);
}

// FUNCTION: GAMEMD 0x00410B90
bool AbstractTypeClass::Write_INI(CCINIClass & ini) const
{
	ini.Clear(IniName);
	ini.Put_String(IniName, "Name", GivenName);
	if (UINameLabel[0]) {
		ini.Put_String(IniName, "UIName", UINameLabel);
	}
	return(true);
}

// FUNCTION: GAMEMD 0x00410BE0
void AbstractTypeClass::Compute_CRC(CRCEngine & crc) const
{
	BASECLASS::Compute_CRC(crc);
	crc((const char *)IniName);
	crc((const char *)GivenName);
	crc((const char *)UINameLabel);
}

// FUNCTION: GAMEMD 0x00410C20
void AbstractTypeClass::Init_Theater(TheaterType theater)
{
}

// SYNTHETIC: GAMEMD 0x00410C30
// ??_GAbstractTypeClass@@UAEPAXI@Z
