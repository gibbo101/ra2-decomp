// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).
#pragma once

enum CommProtocolType {
	COMM_PROTOCOL_SINGLE_NO_COMP = 0,
	COMM_PROTOCOL_SINGLE_E_COMP,
	COMM_PROTOCOL_MULTI_E_COMP,
	COMM_PROTOCOL_COUNT,
	DEFAULT_COMM_PROTOCOL = COMM_PROTOCOL_MULTI_E_COMP
};

struct VersionProtocolType {
	unsigned int Version;
	CommProtocolType Protocol;
};

class VersionClass {
	public:

		VersionClass(void);
		virtual ~VersionClass(void) {};

		unsigned int Version_Number(void);
		unsigned short Major_Version(void);
		unsigned short Minor_Version(void);

		char *Version_Name(void);

		char *Version_Text(void) {return(VersionText);}

		CommProtocolType Version_Protocol(unsigned int version);

		void Init_Clipping(void);
		unsigned int Clip_Version(unsigned int minver, unsigned int maxver);
		unsigned int Get_Clipped_Version(void) {return(MaxClipVer);}

		unsigned int Min_Version(void);
		unsigned int Max_Version(void);

	private:

		void Read_Text_String(void);

		enum VersionEnum {
			MAJOR_VERSION = 0x0001,
			MINOR_VERSION = 0x0001
		};

		enum VersionRangeEnum {

			MIN_VERSION = 0x20000,
			MAX_VERSION = 0x20000
		};

		unsigned int Version;
		unsigned short MajorVer;
		unsigned short MinorVer;

		char VersionName[30];

		char VersionText[16];

		unsigned int MinClipVer;
		unsigned int MaxClipVer;

		unsigned VersionInit :		1;
		unsigned MajorInit :		1;
		unsigned MinorInit :		1;
		unsigned TextInit :			1;
};

char const * Version_Name(void);

