// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"
#include "pkstraw.h"
#include "blwstraw.h"
#include "rndstraw.h"
#include <cstring>

// FUNCTION: 0x00633050
PKStraw::PKStraw(CryptControl control, RandomStraw & rnd) :
	IsGettingKey(true),
	Rand(rnd),
	BF((control == ENCRYPT) ? BlowStraw::ENCRYPT : BlowStraw::DECRYPT),
	Control(control),
	CipherKey(NULL),
	Counter(0),
	BytesLeft(0)
{
	BASECLASS::Get_From(BF);
}

// FUNCTION: 0x006330C0
void PKStraw::Get_From(Straw * straw)
{
	if (BF.ChainTo != straw) {
		if (straw != NULL && straw->ChainFrom != NULL) {
			straw->ChainFrom->Get_From(NULL);
			straw->ChainFrom = NULL;
		}

		if (BF.ChainTo != NULL) {
			BF.ChainTo->ChainFrom = NULL;
		}

		BF.ChainTo = straw;
		BF.ChainFrom = this;
		ChainTo = &BF;
		if (BF.ChainTo != NULL) {
			BF.ChainTo->ChainFrom = this;
		}
	}
}

// FUNCTION: 0x00633110
void PKStraw::Key(PKey const * key)
{
	CipherKey = key;
	if (key != NULL) {
		IsGettingKey = true;
	}
	Counter = 0;
	BytesLeft = 0;
}
