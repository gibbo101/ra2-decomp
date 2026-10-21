// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"
#include "pk.h"
#include <cstring>

// FUNCTION: GAMEMD 0x00631F60
PKey::PKey(void const * exponent, void const * modulus)
{
	Modulus.DERDecode((unsigned char *)modulus);
	Exponent.DERDecode((unsigned char *)exponent);
	BitPrecision = Modulus.BitCount()-1;
}

// FUNCTION: GAMEMD 0x00631FC0
int PKey::Encode_Modulus(void * buffer) const
{
	if (buffer == NULL) {
		return(0);
	}
	return(Modulus.DEREncode((unsigned char *)buffer));
}

// FUNCTION: GAMEMD 0x00631FE0
int PKey::Encode_Exponent(void * buffer) const
{
	if (buffer == NULL) {
		return(0);
	}
	return(Exponent.DEREncode((unsigned char *)buffer));
}

// FUNCTION: GAMEMD 0x00632000
void PKey::Decode_Modulus(void * buffer)
{
	Modulus.DERDecode((unsigned char *)buffer);
	BitPrecision = Modulus.BitCount()-1;
}

// FUNCTION: GAMEMD 0x00632030
void PKey::Decode_Exponent(void * buffer)
{
	Exponent.DERDecode((unsigned char *)buffer);
}

// FUNCTION: GAMEMD 0x00632050
void PKey::Generate(Straw & random, int bits, PKey & fastkey, PKey & slowkey)
{
	for (;;) {
		BigInt p = Generate_Prime(random, bits, &p);
		BigInt q = Generate_Prime(random, bits, &q);

		BigInt e = Fast_Exponent();
		BigInt n = p * q;
		BigInt pqmin = (p-(unsigned short)1)*(q-(unsigned short)1);
		BigInt d = e.Inverse(pqmin);

		fastkey.Exponent = e;
		fastkey.Modulus = n;
		fastkey.BitPrecision = n.BitCount()-1;

		slowkey.Exponent = d;
		slowkey.Modulus = n;
		slowkey.BitPrecision = fastkey.BitPrecision;

		char before[256];
		char after[256];

		for (int index = 0; index < fastkey.Plain_Block_Size(); index++) {
			before[index] = (char)rand();
		}
		fastkey.Encrypt(before, fastkey.Plain_Block_Size(), after);
		slowkey.Decrypt(after, slowkey.Crypt_Block_Size(), after);

		if (memcmp(before, after, fastkey.Plain_Block_Size()) == 0) break;
	}
}

// FUNCTION: GAMEMD 0x00632740
int PKey::Encrypt(void const * source, int slen, void * dest) const
{
	int total = 0;

	while (slen >= Plain_Block_Size()) {
		BigInt temp = 0;
		memmove(&temp, source, Plain_Block_Size());
		temp = temp.exp_b_mod_c(Exponent, Modulus);

		memmove(dest, &temp, Crypt_Block_Size());
		slen -= Plain_Block_Size();
		source = (char *)source + Plain_Block_Size();
		dest = (char *)dest + Crypt_Block_Size();
		total += Crypt_Block_Size();
	}

	return(total);
}

// FUNCTION: GAMEMD 0x00632870
int PKey::Decrypt(void const * source, int slen, void * dest) const
{
	int total = 0;
	BigInt temp;

	while (slen >= Crypt_Block_Size()) {
		temp = 0;
		memmove(&temp, source, Crypt_Block_Size());
		temp = temp.exp_b_mod_c(Exponent, Modulus);

		memmove(dest, &temp, Plain_Block_Size());
		slen -= Crypt_Block_Size();
		source = (char *)source + Crypt_Block_Size();
		dest = (char *)dest + Plain_Block_Size();
		total += Plain_Block_Size();
	}

	return(total);
}
