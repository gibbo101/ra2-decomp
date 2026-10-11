// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"
#include "mpmath.h"
#include "assert.h"
#include "bsearch.h"
#include <cctype>
#include <climits>
#include <cstdlib>
#include <cstring>

extern unsigned short primeTable[3511];

#define ARRAY_SIZE(a)	(sizeof(a)/sizeof(a[0]))
#define	_USERENTRY

void memrev(char * buffer, size_t length);

// FUNCTION: 0x005C69A0
int XMP_Count_Bytes(const digit * number, int precision)
{
	unsigned char * ptr = (unsigned char *)number;
	int count = 0;
	for (unsigned index = 0; index < precision*sizeof(digit); index++) {
		if (!*ptr) break;
		count++;
		ptr++;
	}
	return(count);
}

// FUNCTION: 0x005C69C0
void XMP_Move(digit * dest, digit const * source, int precision)
{
	memcpy(dest, source, precision * sizeof(digit));
}

// FUNCTION: 0x005C69E0
int XMP_Compare(const digit * left_number, const digit * right_number, int precision)
{
	left_number += precision-1;
	right_number += precision-1;
	do {
		if (*left_number < *right_number) return(-1);
		if (*left_number > *right_number) return(1);
		left_number--;
		right_number--;
		precision--;
	} while (precision);
	return(0);
}

// FUNCTION: 0x005C6A30
bool XMP_Add(digit * result, const digit * left_number, const digit * right_number, bool carry, int precision)
{
	while (precision--) {
		digit term = *left_number + *right_number;
		digit final = term + carry;
		carry = (term < *left_number || (carry && final == 0));

		right_number++;
		left_number++;
		*result++ = final;
	}
	return(carry);
}

// FUNCTION: 0x005C6AA0
bool XMP_Add_Int(digit * result, const digit * left_number, digit right_number, bool carry, int precision)
{
	while (precision--) {
		digit term = *left_number + right_number;
		digit final = term + carry;
		carry = (term < *left_number || (carry && final == 0));

		right_number = 0;
		left_number++;
		*result++ = final;
	}
	return(carry);
}

// FUNCTION: 0x005C6B00
bool XMP_Sub(digit * result, const digit * left_number, const digit * right_number, bool borrow, int precision)
{
	const unsigned short * left_number_ptr = (const unsigned short *)left_number;
	const unsigned short * right_number_ptr = (const unsigned short *)right_number;
	unsigned short * result_ptr = (unsigned short *)result;

	precision *= 2;
	while (precision--) {
		digit x = (digit) *left_number_ptr - (digit) *right_number_ptr - (digit) borrow;
		right_number_ptr++;
		left_number_ptr++;
		*result_ptr++ = (unsigned short)x;
		borrow = (((1L << 16) & x) != 0L);
	}
	return(borrow);
}

// FUNCTION: 0x005C6B60
bool XMP_Sub_Int(digit * result, const digit * left_number, unsigned short right_number, bool borrow, int precision)
{
	const unsigned short * left_number_ptr = (const unsigned short *)left_number;
	unsigned short * result_ptr = (unsigned short *)result;

	precision *= 2;
	while (precision--) {
		digit x = (digit) *left_number_ptr - right_number - borrow;
		left_number_ptr++;
		*result_ptr++ = (unsigned short)x;
		borrow = (((1L << 16) & x) != 0L);

		right_number = 0;
	}
	return(borrow);
}

// FUNCTION: 0x005C7DB0
void XMP_Hybrid_Mul(unsigned short * prod, unsigned short * multiplicand, unsigned short multiplier, int precision)
{
	unsigned int carry = 0;
	for (int i = 0; i < precision; ++i) {
		unsigned int p = (unsigned int)multiplier * *multiplicand++;
		p += *prod + carry;
		*prod++ = (unsigned short) p;
		carry = p >> 16;
	}

	*prod += (unsigned short) carry;
}

// FUNCTION: 0x005C8870
void memrev(char * buffer, size_t length)
{
	char * r2 = &(buffer[length - 1]);
	while (buffer < r2) {
		char b = *buffer;
		*buffer++ = *r2;
		*r2-- = b;
	}
}

// FUNCTION: 0x005C8890
int _USERENTRY pfunc(const void * pkey, const void * base)
{
	if (*(unsigned short *)pkey < *(unsigned short *)base) return(-1);
	if (*(unsigned short *)pkey > *(unsigned short *)base) return(1);
	return(0);
}

// FUNCTION: 0x005C8930
bool XMP_Small_Divisors_Test(const digit * candidate, int precision)
{
	digit quotient[MAX_UNIT_PRECISION];

	for (unsigned i = 0; i < ARRAY_SIZE(primeTable); i++) {
		if (XMP_Unsigned_Div_Int(quotient, candidate, primeTable[i], precision) == 0) return(false);
	}
	return(true);
}
