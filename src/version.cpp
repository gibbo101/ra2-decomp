// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"
#include "version.h"
#include "globals.h"
#include "rawfile.h"
#include "winstub.h"
#include "veteran.h"
#include <stdio.h>
#include <string.h>

static VersionProtocolType VersionProtocol[] = {
	{0x00001000,COMM_PROTOCOL_SINGLE_NO_COMP},
	{0x00002000,COMM_PROTOCOL_SINGLE_E_COMP},
	{0x00010000,COMM_PROTOCOL_MULTI_E_COMP},
};

// FUNCTION: 0x0074F730
VersionClass::VersionClass(void) :
	Version(0),
	MajorVer(0),
	MinorVer(0),
	MinClipVer(0),
	MaxClipVer(0),
	VersionInit(false),
	MajorInit(false),
	MinorInit(false),
	TextInit(false)
{
	VersionText[0] = '\0';
	VersionName[0] = '\0';
}

// FUNCTION: 0x0074F760
unsigned int VersionClass::Version_Number(void)
{
	if (!TextInit) {
		Read_Text_String();
		TextInit = 1;
	}

	if (VersionInit) {
		return(Version);
	}

	Version = ((Major_Version() << 16) | Minor_Version());
	VersionInit = 1;

	return(Version);
}

// FUNCTION: 0x0074F960
unsigned short VersionClass::Major_Version(void)
{
	if (!TextInit) {
		Read_Text_String();
		TextInit = 1;
	}

	if (MajorInit) {
		return(MajorVer);
	}

	MajorVer = MAJOR_VERSION;
	MajorInit = 1;

	return(MajorVer);
}

// FUNCTION: 0x0074FA20
unsigned short VersionClass::Minor_Version(void)
{
	if (!TextInit) {
		Read_Text_String();
		TextInit = 1;
	}

	if (MinorInit) {
		return(MinorVer);
	}

	MinorVer = MINOR_VERSION;
	MinorInit = 1;

	return(MinorVer);
}

// FUNCTION: 0x0074FAE0
char * VersionClass::Version_Name(void)
{
	unsigned short adjusted_minor;

	adjusted_minor = Minor_Version();
	sprintf(VersionName, "%d.%3.3d", VerNum.Major_Version(), adjusted_minor);

	return(VersionName);
}

// FUNCTION: 0x0074FC80
void VersionClass::Read_Text_String(void)
{
	RawFileClass file("VERSION.TXT");

	if (file.Is_Available()) {
		file.Read(VersionText, sizeof(VersionText));
		VersionText[sizeof(VersionText)-1] = '\0';
		while (VersionText[strlen(VersionText)-1] == '\r') {
			VersionText[strlen(VersionText)-1] = '\0';
		}
	} else {
		VersionText[0] = '\0';
	}
}

// FUNCTION: 0x0074FD20
CommProtocolType VersionClass::Version_Protocol(unsigned int version)
{
	int i,j;

	j = sizeof (VersionProtocol) / sizeof(VersionProtocolType);

	for (i = j - 1; i >= 0; i--) {
		if (version >= VersionProtocol[i].Version) {
			return(VersionProtocol[i].Protocol);
		}
	}

	return(VersionProtocol[j-1].Protocol);
}

// FUNCTION: 0x0074FD50
void VersionClass::Init_Clipping(void)
{
	MinClipVer = Min_Version();
	MaxClipVer = Max_Version();
}

// FUNCTION: 0x0074FD60
unsigned int VersionClass::Clip_Version(unsigned int minver,
	unsigned int maxver)
{
	if (minver > MaxClipVer)
		return(0xffffffff);

	if (maxver < MinClipVer)
		return(0);

	if (minver > MinClipVer)
		MinClipVer = minver;

	if (maxver < MaxClipVer)
		MaxClipVer = maxver;

	return(MaxClipVer);
}

// FUNCTION: 0x0074FDA0
unsigned int VersionClass::Min_Version(void)
{
	return(MIN_VERSION);
}

// FUNCTION: 0x0074FDB0
unsigned int VersionClass::Max_Version(void)
{
	return(MAX_VERSION);
}

extern "C" {
DWORD __stdcall FUN_007C89EC(LPSTR filename, LPDWORD handle);
BOOL __stdcall FUN_007C89E6(LPSTR filename, DWORD handle, DWORD size, LPVOID block);
BOOL __stdcall FUN_007C89E0(LPVOID block, LPSTR sub, LPVOID * buffer, UINT * len);
}

// FUNCTION: 0x0074FDC0
char const * Version_Name(void)
{
	static char buffer[512] = {"zzz"};
	static bool empty = true;

	int size;
	void *block;
	unsigned int translate_len;
	DWORD handle;

	struct LANGANDCODEPAGE {
		WORD wLanguage;
		WORD wCodePage;
	} *translate;

	char query[128];
	char filename[MAX_PATH];

	if (empty) {

		empty = false;

		if (GetModuleFileName(ProgramInstance, filename, sizeof(filename)) > 0) {
			handle = 1;
			size = FUN_007C89EC(filename, &handle);
			if (size > 0) {
				block = new BYTE[size];
				if (FUN_007C89E6(filename, handle, size, block)) {
					FUN_007C89E0(block, "\\VarFileInfo\\Translation", (LPVOID *)&translate, &translate_len);
					if (translate_len > 0) {
						sprintf(query, "\\StringFileInfo\\%04d%04d\\ProductVersion", translate->wLanguage, translate->wCodePage);
						FUN_007C89E0(block, query, (LPVOID *)&translate, &translate_len);
						if (translate_len > 0) {
							strcpy(buffer, (const char *)translate);
						}
					}
				}

				delete [] block;
			}
		}
	}
	return(buffer);
}

static const char *AbilityName[ABILITY_COUNT] = {
	"FASTER",
	"STRONGER",
	"FIREPOWER",
	"SCATTER",
	"ROF",
	"SIGHT",
	"CLOAK",
	"TIBERIUM_PROOF",
	"VEIN_PROOF",
	"SELF_HEAL",
	"EXPLODES",
	"RADAR_INVISIBLE",
	"SENSORS",
	"FEARLESS",
	"C4",
	"TIBERIUM_HEAL",
	"GUARD_AREA",
	"CRUSHER"
};

// FUNCTION: 0x0074FEF0
AbilityType Ability_From_Name(const char *name)
{
	for (int ability = ABILITY_FIRST; ability < ABILITY_COUNT; ability++) {
		if (stricmp(AbilityName[ability], name) == 0) {
			return((AbilityType)ability);
		}
	}
	return(ABILITY_NONE);
}

// FUNCTION: 0x0074FF30
VeterancyClass::VeterancyClass(void) :
	Experience(0)
{
}

// FUNCTION: 0x0074FF40
VeterancyClass::~VeterancyClass(void)
{
}
