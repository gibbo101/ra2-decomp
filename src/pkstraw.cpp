// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"
#include "pkstraw.h"
#include "blwstraw.h"
#include "rndstraw.h"
#include <cmath>
#include <cstring>


// FUNCTION: GAMEMD 0x00633050
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

// FUNCTION: GAMEMD 0x006330C0
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

// FUNCTION: GAMEMD 0x00633110
void PKStraw::Key(PKey const * key)
{
	CipherKey = key;
	if (key != NULL) {
		IsGettingKey = true;
	}
	Counter = 0;
	BytesLeft = 0;
}

// FUNCTION: GAMEMD 0x00633130
int PKStraw::Get(void * source, int length)
{
	if (source == NULL || length < 1 || CipherKey == NULL) {
		return(BASECLASS::Get(source, length));
	}

	int total = 0;

	if (IsGettingKey) {
		if (Control == DECRYPT) {
			char cbuffer[MAX_KEY_BLOCK_SIZE];
			int got = BASECLASS::Get(cbuffer, Encrypted_Key_Length());

			if (got != Encrypted_Key_Length()) return(0);

			CipherKey->Decrypt(cbuffer, got, Buffer);
			BF.Key(Buffer, BLOWFISH_KEY_SIZE);

		} else {
			char buffer[MAX_KEY_BLOCK_SIZE];
			memset(buffer, '\0', sizeof(buffer));
			Rand.Get(buffer, BLOWFISH_KEY_SIZE);

			Counter = BytesLeft = CipherKey->Encrypt(buffer, Plain_Key_Length(), Buffer);
			BF.Key(buffer, BLOWFISH_KEY_SIZE);
		}

		IsGettingKey = false;
	}

	if (BytesLeft > 0) {
		int tocopy = (length < BytesLeft) ? length : BytesLeft;
		memmove(source, &Buffer[Counter-BytesLeft], tocopy);
		source = (char *)source + tocopy;
		BytesLeft -= tocopy;
		length -= tocopy;
		total += tocopy;
	}

	total += BASECLASS::Get(source, length);

	return(total);
}

// FUNCTION: GAMEMD 0x00633300
int PKStraw::Encrypted_Key_Length(void) const
{
	if (CipherKey == NULL) return(0);
	return(CipherKey->Block_Count(BLOWFISH_KEY_SIZE) * CipherKey->Crypt_Block_Size());
}

// FUNCTION: GAMEMD 0x00633330
int PKStraw::Plain_Key_Length(void) const
{
	if (CipherKey == NULL) return(0);
	return(CipherKey->Block_Count(BLOWFISH_KEY_SIZE) * CipherKey->Plain_Block_Size());
}

// FUNCTION: GAMEMD 0x00633360
// PKStraw::Get_From

// SYNTHETIC: GAMEMD 0x00633370
// ??_GPKStraw@@UAEPAXI@Z
