// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"
#include "blowfish.h"
#include "assert.h"

// FUNCTION: 0x00437FC0
BlowfishEngine::~BlowfishEngine(void)
{
#ifdef NO_BLOWFISH_DLL
	if (IsKeyed) {
		Submit_Key(NULL, 0);
	}
#endif
}

// FUNCTION: 0x00437FD0
void BlowfishEngine::Submit_Key(void const * key, int length)
{
#ifndef NO_BLOWFISH_DLL
	BlockCypher->Set_Key(length, key);
	return;
#else
	assert(length <= MAX_KEY_LENGTH);

	memcpy(P_Encrypt, P_Init, sizeof(P_Init));
	memcpy(P_Decrypt, P_Init, sizeof(P_Init));
	memcpy(bf_S, S_Init, sizeof(S_Init));

	if (key == 0 || length == 0) {
		IsKeyed = false;
		return;
	}

	int j = 0;
	unsigned char const * key_ptr = (unsigned char const *)key;
	unsigned int * p_ptr = &P_Encrypt[0];
	for (int index = 0; index < ROUNDS+2; index++) {
		unsigned int data = 0;

		data = (data << CHAR_BIT) | key_ptr[j++ % length];
		data = (data << CHAR_BIT) | key_ptr[j++ % length];
		data = (data << CHAR_BIT) | key_ptr[j++ % length];
		data = (data << CHAR_BIT) | key_ptr[j++ % length];

		*p_ptr++ ^= data;
	}

	unsigned int left = 0x00000000L;
	unsigned int right = 0x00000000L;
	unsigned int * p_en = &P_Encrypt[0];
	unsigned int * p_de = &P_Decrypt[ROUNDS+1];
	for (int p_index = 0; p_index < ROUNDS+2; p_index += 2) {
		Sub_Key_Encrypt(left, right);

		*p_en++ = left;
		*p_en++ = right;

		*p_de-- = left;
		*p_de-- = right;
	}

	for (int sbox_index = 0; sbox_index < 4; sbox_index++) {
		for (int ss_index = 0; ss_index < UCHAR_MAX+1; ss_index += 2) {
			Sub_Key_Encrypt(left, right);
			bf_S[sbox_index][ss_index] = left;
			bf_S[sbox_index][ss_index + 1] = right;
		}
	}

	IsKeyed = true;
#endif
}

// FUNCTION: 0x00438000
int BlowfishEngine::Encrypt(void const * plaintext, int length, void * cyphertext)
{
#ifndef NO_BLOWFISH_DLL
	BlockCypher->Encrypt(length, plaintext, cyphertext);
	return(length);
#else
	if (plaintext == 0 || length == 0) {
		return(0);
	}
	if (cyphertext == 0) cyphertext = (void *)plaintext;

	if (IsKeyed) {
		int blocks = length / BYTES_PER_BLOCK;

		for (int index = 0; index < blocks; index++) {
			Process_Block(plaintext, cyphertext, P_Encrypt);
			plaintext = ((char *)plaintext) + BYTES_PER_BLOCK;
			cyphertext = ((char *)cyphertext) + BYTES_PER_BLOCK;
		}
		int encrypted = blocks * BYTES_PER_BLOCK;

		if (encrypted < length) {
			memmove(cyphertext, plaintext, length - encrypted);
		}

		return(encrypted);
	}

	if (plaintext != cyphertext) {
		memmove(cyphertext, plaintext, length);
	}
	return(length);
#endif
}

// FUNCTION: 0x00438030
int BlowfishEngine::Decrypt(void const * cyphertext, int length, void * plaintext)
{
#ifndef NO_BLOWFISH_DLL
	BlockCypher->Decrypt(length, cyphertext, plaintext);
	return(length);
#else
	if (cyphertext == 0 || length == 0) {
		return(0);
	}
	if (plaintext == 0) plaintext = (void *)cyphertext;

	if (IsKeyed) {
		int blocks = length / BYTES_PER_BLOCK;

		for (int index = 0; index < blocks; index++) {
			Process_Block(cyphertext, plaintext, P_Decrypt);
			cyphertext = ((char *)cyphertext) + BYTES_PER_BLOCK;
			plaintext = ((char *)plaintext) + BYTES_PER_BLOCK;
		}
		int encrypted = blocks * BYTES_PER_BLOCK;

		if (encrypted < length) {
			memmove(plaintext, cyphertext, length - encrypted);
		}

		return(encrypted);
	}

	if (plaintext != cyphertext) {
		memmove(plaintext, cyphertext, length);
	}
	return(length);
#endif
}
