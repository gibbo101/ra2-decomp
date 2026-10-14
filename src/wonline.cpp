// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#define NDEBUG

#include "always.h"
#include "arraylist.h"
#include "dict.h"
#include "wstring.h"

#include <windows.h>
#include <ole2.h>

unsigned int __fastcall Wstring_Hash(Wstring & string);
#define HASH_ARG (Wstring_Hash)

class OLEInitializer
{
public:
	OLEInitializer(void) { OleInitialize(NULL); }
	~OLEInitializer(void) { OleUninitialize(); }
};

// FUNCTION: 0x0077B8A0
// FUNCTION: 0x0077B8C0
OLEInitializer g_OLEInitializer;

// FUNCTION: 0x0077B8D0
// FUNCTION: 0x0077B8F0
Wstring DAT_00B73AE0;

// FUNCTION: 0x0077B900
// FUNCTION: 0x0077B920
ArrayList<int> DAT_00B778D8;

// FUNCTION: 0x0077B940
// FUNCTION: 0x0077B960
ArrayList<int> DAT_00B73E10;

// FUNCTION: 0x0077B980
// FUNCTION: 0x0077B9A0
ArrayList<int> DAT_00B76AE0;

// FUNCTION: 0x0077B9C0
// FUNCTION: 0x0077B9E0
ArrayList<int> DAT_00B77900;

// FUNCTION: 0x0077BA00
// FUNCTION: 0x0077BA20
ArrayList<int> DAT_00B76520;

struct LadderA {
	char Data[0x6C];
};

struct LadderB {
	char Data[0x44];
};

// FUNCTION: 0x0077BA40
// FUNCTION: 0x0077BAE0
Dictionary<Wstring, LadderA> DAT_00B768C0 HASH_ARG;

// FUNCTION: 0x0077BC10
// FUNCTION: 0x0077BCB0
Dictionary<Wstring, LadderA> DAT_00B73AF8 HASH_ARG;

// FUNCTION: 0x0077BDE0
// FUNCTION: 0x0077BE80
Dictionary<Wstring, LadderA> DAT_00B77D80 HASH_ARG;

// FUNCTION: 0x0077BFB0
// FUNCTION: 0x0077C050
Dictionary<Wstring, LadderB> DAT_00B76640 HASH_ARG;

// FUNCTION: 0x0077C180
// FUNCTION: 0x0077C220
Dictionary<Wstring, int> DAT_00B76680 HASH_ARG;

// FUNCTION: 0x0077C350
// FUNCTION: 0x0077C370
ArrayList<Wstring> DAT_00B777A0;

// FUNCTION: 0x0077C3B0
// FUNCTION: 0x0077C450
Dictionary<Wstring, int> DAT_00B76530 HASH_ARG;

// FUNCTION: 0x0077C580
// FUNCTION: 0x0077C5A0
ArrayList<Wstring> DAT_00B765F8;

// FUNCTION: 0x0077C5E0
// FUNCTION: 0x0077C600
ArrayList<Wstring> DAT_00B73BD8;

// FUNCTION: 0x0077C640
// FUNCTION: 0x0077C660
ArrayList<Wstring> DAT_00B736F8;

// FUNCTION: 0x0077C6A0
// FUNCTION: 0x0077C6C0
ArrayList<Wstring> DAT_00B76B00;

// FUNCTION: 0x0077C700
// FUNCTION: 0x0077C720
ArrayList<Wstring> DAT_00B76510;

// FUNCTION: 0x0077C760
// FUNCTION: 0x0077C780
ArrayList<Wstring> DAT_00B73AE8;

// FUNCTION: 0x0077C7C0
// FUNCTION: 0x0077C7E0
ArrayList<Wstring> DAT_00B77820;

// FUNCTION: 0x0077C820
// FUNCTION: 0x0077C840
ArrayList<Wstring> DAT_00B73708;

// FUNCTION: 0x0077C880
// FUNCTION: 0x0077C8A0
ArrayList<Wstring> DAT_00B76188;

// FUNCTION: 0x0077C8E0
// FUNCTION: 0x0077C900
ArrayList<Wstring> DAT_00B73AA8;
