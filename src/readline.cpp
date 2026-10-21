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

// FUNCTION: 0x0065D510
int Read_Line(FileClass & file, char * buffer, int len, bool & eof)
{
	FileStraw fs(file);
	return(Read_Line(fs, buffer, len, eof));
}

// FUNCTION: 0x0065D5C0
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

// FUNCTION: 0x0065D660
ReferenceCounted::~ReferenceCounted(void)
{
}

// FUNCTION: 0x0065D670
void ReferenceCounted::FUN_0065D670(void)
{
	if (--Count == 0) delete this;
}

// FUNCTION: 0x0065D690
bool ReferenceCounted::FUN_0065D690(int)
{
	return false;
}

// FUNCTION: 0x0065D6A0 ReferenceCounted::~ReferenceCounted

// FUNCTION: 0x0065D6C0 $E2
double DAT_00B04C40 = FUN_004CAC40(pow(DAT_007E1710, DAT_007E1708) * 2);

// FUNCTION: 0x0065D6F0 $E5
double DAT_00B04C90 = DAT_00B04C40 / DAT_007F0960;

// FUNCTION: 0x0065D710 $E8
double DAT_00B04C78 = DAT_007E1718 / DAT_00B04C90;

// FUNCTION: 0x0065D730 $E11
double DAT_00B04C70 = DAT_007E1720 * DAT_007F0968;

// FUNCTION: 0x0065D750 $E14
double DAT_00B04C48 = DAT_007E1728 * DAT_007F0968;

// FUNCTION: 0x0065D770 $E17
double DAT_00B04C20 = DAT_007E1730 * DAT_007F0968;

// FUNCTION: 0x0065D790 $E20
int DAT_00B04C98 = (int)(FUN_004CAD50(DAT_00B04C20 - DAT_00B04C48) * DAT_007E1738 * DAT_00B04C40);

// FUNCTION: 0x0065D7D0 $E23
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

// FUNCTION: 0x0065D800 $E26
double DAT_00B04C68 = FUN_004CADE0((DAT_00B04C98 * 2) / DAT_00B04C40);

// FUNCTION: 0x0065D830 $E29
int DAT_00B04C8C = (int)(DAT_00B04C98 * 4 + DAT_007E1738);

// FUNCTION: 0x0065D860 $E32
ShortPair DAT_00B04C38;

// FUNCTION: 0x0065D870 $E35
IntTriple DAT_00B04C58;

// FUNCTION: 0x0065D890 $E38
PlayfieldDefaults DAT_00B04C80;

// FUNCTION: 0x0065D8B0 $E41
IntQuad DAT_00B04C28;
