// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"
#include "readline.h"
#include "wwfile.h"
#include "xstraw.h"
#include <cstring>
#include <math.h>
#include "refcount.h"

extern double __cdecl FUN_004CAC40(double);
extern double __cdecl FUN_004CAD50(double);
extern double __cdecl FUN_004CADE0(double);
extern const double DAT_007E1708;
extern const double DAT_007E1710;
extern const double DAT_007E1718;
extern const double DAT_007E1720;
extern const double DAT_007E1728;
extern const double DAT_007E1730;
extern const double DAT_007E1738;
extern const double DAT_007E1740;
extern const double DAT_007F0960;
extern const double DAT_007F0968;

// FUNCTION: GAMEMD 0x0065D510
int Read_Line(FileClass & file, char * buffer, int len, bool & eof)
{
	FileStraw fs(file);
	return(Read_Line(fs, buffer, len, eof));
}

// FUNCTION: GAMEMD 0x0065D5C0
int Read_Line(Straw & file, char * buffer, int len, bool & eof)
{
	if (len == 0 || buffer == NULL) return(0);

	int count = 0;
	for (;;) {
		char c;
		if (file.Get(&c, sizeof(c)) != sizeof(c)) {
			eof = true;
			buffer[count] = '\0';
			break;
		}

		if (c == '\x0A') break;
		if (c != '\x0D' && count+1 < len) {
			buffer[count++] = c;
		}
	}
	buffer[count] = '\0';

	return(strlen(buffer));
}

// FUNCTION: GAMEMD 0x0065D660
ReferenceCounted::~ReferenceCounted(void)
{
}

// FUNCTION: GAMEMD 0x0065D670
void ReferenceCounted::FUN_0065D670(void)
{
	if (--Count == 0) delete this;
}

// FUNCTION: GAMEMD 0x0065D690
bool ReferenceCounted::FUN_0065D690(int)
{
	return false;
}

// SYNTHETIC: GAMEMD 0x0065D6A0
// ??_GReferenceCounted@@UAEPAXI@Z

// SYNTHETIC: GAMEMD 0x0065D6C0 SYMBOL
// _$E2
double DAT_00B04C40 = FUN_004CAC40(pow(DAT_007E1710, DAT_007E1708) * 2);

// SYNTHETIC: GAMEMD 0x0065D6F0 SYMBOL
// _$E5
double DAT_00B04C90 = DAT_00B04C40 / DAT_007F0960;

// SYNTHETIC: GAMEMD 0x0065D710 SYMBOL
// _$E8
double DAT_00B04C78 = DAT_007E1718 / DAT_00B04C90;

// SYNTHETIC: GAMEMD 0x0065D730 SYMBOL
// _$E11
double DAT_00B04C70 = DAT_007E1720 * DAT_007F0968;

// SYNTHETIC: GAMEMD 0x0065D750 SYMBOL
// _$E14
double DAT_00B04C48 = DAT_007E1728 * DAT_007F0968;

// SYNTHETIC: GAMEMD 0x0065D770 SYMBOL
// _$E17
double DAT_00B04C20 = DAT_007E1730 * DAT_007F0968;

// SYNTHETIC: GAMEMD 0x0065D790 SYMBOL
// _$E20
int DAT_00B04C98 = (int)(FUN_004CAD50(DAT_00B04C20 - DAT_00B04C48) * DAT_007E1738 * DAT_00B04C40);

// SYNTHETIC: GAMEMD 0x0065D7D0 SYMBOL
// _$E23
double DAT_00B04C50 = FUN_004CADE0(DAT_00B04C98 * DAT_007E1740);

struct ShortPair {
	unsigned short A, B;
	ShortPair() { A = 0; B = 0; }
};

struct IntTriple {
	int A, B, C;
	IntTriple() { A = 0; B = 0; C = 0; }
};

struct IntQuad {
	int A, B, C, D;
	IntQuad() { A = 0; B = 0; C = 0; D = 0; }
};

struct PlayfieldDefaults {
	int A, B, C;
	PlayfieldDefaults() { C = 0; A = 128; B = 128; }
};

// SYNTHETIC: GAMEMD 0x0065D800 SYMBOL
// _$E26
double DAT_00B04C68 = FUN_004CADE0((DAT_00B04C98 * 2) / DAT_00B04C40);

// SYNTHETIC: GAMEMD 0x0065D830 SYMBOL
// _$E29
int DAT_00B04C8C = (int)(DAT_00B04C98 * 4 + DAT_007E1738);

// SYNTHETIC: GAMEMD 0x0065D860 SYMBOL
// _$E32
ShortPair DAT_00B04C38;

// SYNTHETIC: GAMEMD 0x0065D870 SYMBOL
// _$E35
IntTriple DAT_00B04C58;

// SYNTHETIC: GAMEMD 0x0065D890 SYMBOL
// _$E38
PlayfieldDefaults DAT_00B04C80;

// SYNTHETIC: GAMEMD 0x0065D8B0 SYMBOL
// _$E41
IntQuad DAT_00B04C28;
