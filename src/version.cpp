// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"
#include "version.h"
#include "globals.h"
#include "rawfile.h"
#include "winstub.h"

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
