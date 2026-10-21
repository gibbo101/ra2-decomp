// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"
#include "pkpipe.h"
#include <cstring>

// FUNCTION: GAMEMD 0x00632CB0
PKPipe::PKPipe(CryptControl control, RandomStraw & rnd) :
	IsGettingKey(true),
	Rand(rnd),
	BF((control == ENCRYPT) ? BlowPipe::ENCRYPT : BlowPipe::DECRYPT),
	Control(control),
	CipherKey(NULL),
	Counter(0),
	BytesLeft(0)
{
}

// FUNCTION: GAMEMD 0x00632D10
void PKPipe::Put_To(Pipe * pipe)
{
	if (BF.ChainTo != pipe) {
		if (pipe != NULL && pipe->ChainFrom != NULL) {
			pipe->ChainFrom->Put_To(NULL);
			pipe->ChainFrom = NULL;
		}

		if (BF.ChainTo != NULL) {
			BF.ChainTo->ChainFrom = NULL;
		}
		BF.ChainTo = pipe;
		if (pipe != NULL) {
			pipe->ChainFrom = &BF;
		}
		BF.ChainFrom = this;
		ChainTo = &BF;
	}
}

// FUNCTION: GAMEMD 0x00632D60
void PKPipe::Key(PKey const * key)
{
	if (key == NULL) {
		Flush();
		IsGettingKey = false;
	}
	CipherKey = key;

	if (CipherKey != NULL) {
		IsGettingKey = true;
		if (Control == DECRYPT) {
			Counter = BytesLeft = Encrypted_Key_Length();
		}
	}
}

// FUNCTION: GAMEMD 0x00632DC0
int PKPipe::Put(void const * source, int length)
{
	if (source == NULL || length < 1 || CipherKey == NULL) {
		return(BASECLASS::Put(source, length));
	}

	int total = 0;

	if (IsGettingKey) {
		if (Control == ENCRYPT) {
			char buffer[MAX_KEY_BLOCK_SIZE];
			memset(buffer, '\0', sizeof(buffer));
			Rand.Get(buffer, BLOWFISH_KEY_SIZE);

			int didput = CipherKey->Encrypt(buffer, Plain_Key_Length(), Buffer);
			total += BASECLASS::Put(Buffer, didput);
			BF.Key(buffer, BLOWFISH_KEY_SIZE);

			IsGettingKey = false;

		} else {
			int toget = (BytesLeft < length) ? BytesLeft : length;
			memmove(&Buffer[Counter-BytesLeft], source, toget);
			length -= toget;
			BytesLeft -= toget;
			source = (char *)source + toget;

			if (BytesLeft == 0) {
				char buffer[MAX_KEY_BLOCK_SIZE];
				CipherKey->Decrypt(Buffer, Counter, buffer);
				BF.Key(buffer, BLOWFISH_KEY_SIZE);

				IsGettingKey = false;
			}
		}
	}

	total += BASECLASS::Put(source, length);
	return(total);
}

// FUNCTION: GAMEMD 0x00632F30
int PKPipe::Encrypted_Key_Length(void) const
{
	if (CipherKey == NULL) return(0);
	return(CipherKey->Block_Count(BLOWFISH_KEY_SIZE) * CipherKey->Crypt_Block_Size());
}

// FUNCTION: GAMEMD 0x00632F60
int PKPipe::Plain_Key_Length(void) const
{
	if (CipherKey == NULL) return(0);
	return(CipherKey->Block_Count(BLOWFISH_KEY_SIZE) * CipherKey->Plain_Block_Size());
}

// SYNTHETIC: GAMEMD 0x00632F90
// ??_GBlowPipe@@UAEPAXI@Z

// FUNCTION: GAMEMD 0x00632FE0
// PKPipe::Put_To

// SYNTHETIC: GAMEMD 0x00632FF0
// ??_GPKPipe@@UAEPAXI@Z
