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

#define UPPER_MOST_BIT 0x80000000L
#define ARRAY_SIZE(a)	(sizeof(a)/sizeof(a[0]))
#define	_USERENTRY

void memrev(char * buffer, size_t length);

int XMP_Significance(const digit * number, int precision)
{
	number += precision;
	do {
		number--;
		if (*number) break;
		precision--;
	} while (precision);
	return(precision);
}

void XMP_Inc(digit * number, int precision)
{
	do {
		*number = (*number) + 1;
		if (*number != 0) break;
		number++;
		precision --;
	} while (precision);
}

void XMP_Dec(digit * number, int precision)
{
	do {
		*number -= 1;
		if ((*number) != ~(digit)0) break;
		number++;
		precision--;
	} while (precision);
}

void XMP_Not(digit * number, int precision)
{
	for (int index = 0; index < precision; index++) {
		*number = ~(*number);
		number++;
	}
}

void XMP_Neg(digit * number, int precision)
{
	XMP_Not(number, precision);
	XMP_Inc(number, precision);
}

bool XMP_Test_Eq_Int(digit const * r, int i, int p)
{
	return( (*r == (digit)i ) && XMP_Significance(r,p) <= 1 );
}

void XMP_Init(digit * number, digit value, int precision)
{
	memset(number, '\0', precision * sizeof(digit));
	*number = value;
}

unsigned XMP_Count_Bits(const digit * number, int precision)
{
	int sub_precision = XMP_Significance(number, precision);
	if (!sub_precision) return(0);

	int total_bit_count = XMP_Digits_To_Bits(sub_precision);
	number += sub_precision-1;
	digit high_bit_mask = UPPER_MOST_BIT;
	while (!((*number) & high_bit_mask)) {
		high_bit_mask >>= 1;
		total_bit_count--;
	}
	return(total_bit_count);
}

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

// FUNCTION: 0x005C6BC0
int XMP_Unsigned_Mult(digit * prod, const digit * multiplicand, const digit * multiplier, int precision)
{
	XMP_Init(prod, 0, precision);

	if (XMP_Test_Eq_Int(multiplicand, 0, precision) || XMP_Test_Eq_Int(multiplier, 0, precision)) {
		return(0);
	}

	int total_bit_count = XMP_Count_Bits(multiplier, precision);
	digit high_bit_mask = XMP_Bits_To_Mask(total_bit_count);
	int sub_precision = XMP_Bits_To_Digits(total_bit_count);
	if (!sub_precision) return(0);

	multiplier += sub_precision;
	while (total_bit_count--) {
		XMP_Shift_Left_Bits(prod, 1, precision);
		if ((*(multiplier-1)) & high_bit_mask) {
			XMP_Add(prod, prod, multiplicand, 0, precision);
		}
		high_bit_mask >>= 1;
		if (!high_bit_mask) {
			high_bit_mask = UPPER_MOST_BIT;
			multiplier--;
		}
	}
	return(0);
}

// FUNCTION: 0x005C6D90
int XMP_Unsigned_Mult_Int(digit * prod, const digit * multiplicand, short multiplier, int precision)
{
	const unsigned short * m2 = (const unsigned short *)multiplicand;
	unsigned short * pr = (unsigned short *)prod;
	unsigned int carry = 0;
	for (int i = 0; i < precision*2; ++i) {
		unsigned int p = (((unsigned int)multiplier) * *m2) + carry;;
		*pr = (unsigned short) p;
		carry = p >> 16;
		m2++;
		pr++;
	}
	return(0);
}

// FUNCTION: 0x005C6DD0
int XMP_Signed_Mult_Int(digit * prod, const digit * multiplicand, signed short multiplier, int precision)
{
	if (XMP_Is_Negative(multiplicand, precision)) {
		digit abs_multiplicand[MAX_UNIT_PRECISION];
		XMP_Move(abs_multiplicand, multiplicand, precision);
		XMP_Neg(abs_multiplicand, precision);

		if (multiplier < 0) {
			multiplier = (signed short)-multiplier;
			XMP_Unsigned_Mult_Int(prod, abs_multiplicand, multiplier, precision);
		} else {
			XMP_Unsigned_Mult_Int(prod, abs_multiplicand, multiplier, precision);
			XMP_Neg(prod, precision);
		}
	} else {
		if (multiplier < 0) {
			multiplier = (signed short)-multiplier;
			XMP_Unsigned_Mult_Int(prod, multiplicand, multiplier, precision);
			XMP_Neg(prod, precision);
		} else {
			XMP_Unsigned_Mult_Int(prod, multiplicand, multiplier, precision);
		}
	}
	return(0);
}

// FUNCTION: 0x005C6FE0
int XMP_Signed_Mult(digit * prod, const digit * multiplicand, const digit * multiplier, int precision)
{
	if (XMP_Is_Negative(multiplicand, precision)) {
		digit abs_multiplicand[MAX_UNIT_PRECISION];
		XMP_Move(abs_multiplicand, multiplicand, precision);
		XMP_Neg(abs_multiplicand, precision);

		if (XMP_Is_Negative(multiplier, precision)) {
			digit abs_multiplier[MAX_UNIT_PRECISION];
			XMP_Move(abs_multiplier, multiplier, precision);
			XMP_Neg(abs_multiplier, precision);
			XMP_Unsigned_Mult(prod, abs_multiplicand, abs_multiplier, precision);
		} else {
			XMP_Unsigned_Mult(prod, abs_multiplicand, multiplier, precision);
			XMP_Neg(prod, precision);
		}
	} else {
		if (XMP_Is_Negative(multiplier, precision)) {
			digit abs_multiplier[MAX_UNIT_PRECISION];
			XMP_Move(abs_multiplier, multiplier, precision);
			XMP_Neg(abs_multiplier, precision);
			XMP_Unsigned_Mult(prod, multiplicand, abs_multiplier, precision);
			XMP_Neg(prod, precision);
		} else {
			XMP_Unsigned_Mult(prod, multiplicand, multiplier, precision);
		}
	}
	XMP_Is_Negative(prod, precision);
	return(0);
}

// FUNCTION: 0x005C71E0
unsigned short XMP_Unsigned_Div_Int(digit * quotient, digit const * dividend, unsigned short divisor, int precision)
{
	if (!divisor) return(0);

	unsigned short remainder = 0;
	XMP_Init(quotient, 0, precision);

	int total_bit_count = XMP_Count_Bits(dividend, precision);
	int digit_precision = XMP_Bits_To_Digits(total_bit_count);
	digit const * dividend_ptr = dividend + (digit_precision-1);
	if (!digit_precision) return(0);

	digit high_bit_mask = XMP_Bits_To_Mask(total_bit_count);
	digit * quotient_ptr = quotient + (digit_precision-1);

	while (total_bit_count--) {
		remainder <<= 1;
		if ((*dividend_ptr) & high_bit_mask) remainder++;
		if (remainder >= divisor) {
			remainder -= divisor;
			*quotient_ptr |= high_bit_mask;
		}
		high_bit_mask >>= 1;
		if (!high_bit_mask) {
			high_bit_mask = UPPER_MOST_BIT;
			--dividend_ptr;
			--quotient_ptr;
		}
	}
	return(remainder);
}

// FUNCTION: 0x005C7310
int XMP_Unsigned_Div(digit * remainder, digit * quotient, digit const * dividend, digit const * divisor, int precision)
{
	if (XMP_Test_Eq_Int(divisor, 0, precision)) return(-1);

	XMP_Init(remainder, 0, precision);
	XMP_Init(quotient, 0, precision);

	int total_bit_count = XMP_Count_Bits(dividend, precision);
	int digit_precision = XMP_Bits_To_Digits(total_bit_count);
	if (!digit_precision) return(0);

	digit const * dividend_ptr = dividend + (digit_precision-1);
	digit * quotient_ptr = quotient + (digit_precision-1);
	digit high_bit_mask = XMP_Bits_To_Mask(total_bit_count);

	while (total_bit_count--) {
		XMP_Shift_Left_Bits(remainder, 1, precision);
		if (((*dividend_ptr) & high_bit_mask) != 0) {
			XMP_Inc(remainder, precision);
		}
		if (XMP_Compare(remainder, divisor, precision) >= 0) {
			XMP_Sub(remainder, remainder, divisor, 0, precision);
			*quotient_ptr |= high_bit_mask;
		}
		high_bit_mask >>= 1;
		if (!high_bit_mask) {
			high_bit_mask = UPPER_MOST_BIT;
			dividend_ptr--;
			quotient_ptr--;
		}
	}
	return(0);
}

// FUNCTION: 0x005C7520
void XMP_Signed_Div(digit * remainder, digit * quotient, digit const * dividend, digit const * divisor, int precision)
{
	bool negative = false;

	digit scratch_dividend[MAX_UNIT_PRECISION];
	XMP_Move(scratch_dividend, dividend, precision);
	digit scratch_divisor[MAX_UNIT_PRECISION];
	XMP_Move(scratch_divisor, divisor, precision);

	if (XMP_Is_Negative(scratch_dividend, precision)) {
		XMP_Neg(scratch_dividend, precision);
		negative = !negative;
	}
	if (XMP_Is_Negative(scratch_divisor, precision)) {
		XMP_Neg(scratch_divisor, precision);
		negative = !negative;
	}

	XMP_Unsigned_Div(remainder, quotient, scratch_dividend, scratch_divisor, precision);

	if (negative) {
		XMP_Neg(quotient, precision);
		if (!XMP_Test_Eq_Int(remainder, 0, precision)) {
			XMP_Dec(quotient, precision);
			XMP_Neg(remainder, precision);
			XMP_Add(remainder, remainder, scratch_divisor, 0, precision);
		}
	}
}

// FUNCTION: 0x005C7740
void XMP_Inverse_A_Mod_B(digit * result, digit const * number, digit const * modulus, int precision)
{
	digit g[3][MAX_UNIT_PRECISION];
	XMP_Move(g[0], modulus, precision);
	XMP_Move(g[1], number, precision);

	digit v[3][MAX_UNIT_PRECISION];
	XMP_Init(v[0], 0, precision);
	XMP_Init(v[1], 1, precision);

	digit y[MAX_UNIT_PRECISION];

	int i;
	for (i = 1; !XMP_Test_Eq_Int(g[i%3], 0, precision); i++) {
		XMP_Unsigned_Div(g[(i+1)%3], y, g[(i-1)%3], g[i%3], precision);
		XMP_Unsigned_Mult(result, v[i%3], y, precision);
		XMP_Sub(v[(i+1)%3], v[(i-1)%3], result, 0, precision);
	}

	if (XMP_Is_Negative(v[(i-1)%3], precision)) {
		XMP_Add(v[(i-1)%3], v[(i-1)%3], modulus, 0, precision);
	}
	XMP_Move(result, v[(i-1)%3], precision);
}

// FUNCTION: 0x005C7990
int XMP_Reciprocal(digit * quotient, const digit * divisor, int precision)
{
	digit remainder[MAX_UNIT_PRECISION];

	if (XMP_Test_Eq_Int(divisor, 0, precision)) return(-1);

	XMP_Init(remainder, 0, precision);
	XMP_Init(quotient, 0, precision);

	unsigned total_bit_count = XMP_Count_Bits(divisor, precision);
	digit high_bit_mask = XMP_Bits_To_Mask(total_bit_count + 1);
	int sub_precision = XMP_Bits_To_Digits(total_bit_count + 1);
	XMP_Set_Bit(remainder, total_bit_count - 1);

	quotient += sub_precision-1;

	while (total_bit_count--) {
		XMP_Shift_Left_Bits(remainder, 1, precision);
		if (XMP_Compare(remainder, divisor, precision) >= 0) {
			XMP_Sub(remainder, remainder, divisor, 0, precision);
			*quotient |= high_bit_mask;
		}
		high_bit_mask >>= 1;
		if (!high_bit_mask) {
			high_bit_mask = UPPER_MOST_BIT;
			quotient--;
		}
	}
	XMP_Init(remainder, 0, precision);
	return(0);
}

// FUNCTION: 0x005C7BA0
void XMP_Decode_ASCII(char const * str, digit * mpn, int precision)
{
	XMP_Init(mpn, 0, precision);

	if (!str) return;
	int i = strlen(str);
	if (i == 0) return;

	unsigned short radix;
	switch (toupper(str[i-1])) {
		case '.':
			radix = 10;
			break;
		case 'H':
			radix = 16;
			break;
		case 'O':
			radix = 8;
			break;
		case 'B':
			radix = 2;
			break;
		default:
			radix = 10;
			break;
	}

	bool minus = (*str == '-');
	if (minus) str++;

	digit c;
	while ((c = (unsigned char)*str++) != 0) {
		if (c == ',') continue;

		if (!isxdigit((char)c)) break;

		if (isdigit((char)c)) {
			c -= '0';
		} else {
			c = (unsigned char)(toupper((char)c) - 'A') + 10;
		}

		if (c >= radix) break;

		XMP_Unsigned_Mult_Int(mpn, mpn, radix, precision);
		XMP_Add_Int(mpn, mpn, c, 0, precision);
	}

	if (minus) {
		XMP_Neg(mpn, precision);
	}
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

// FUNCTION: 0x005C7E00
void XMP_Double_Mul(digit * prod, const digit * multiplicand, const digit * multiplier, int precision)
{
	XMP_Init(prod, 0, precision*2);

	const unsigned short * multiplier_ptr = (const unsigned short *) multiplier;
	unsigned short * product_ptr = (unsigned short *) prod;

	for (int i = 0; i < precision*2; ++i) {
		XMP_Hybrid_Mul(product_ptr++, (unsigned short *)multiplicand, *multiplier_ptr++, precision*2);
	}
}

static int _modulus_shift;
static unsigned short _reciprical_high_digit;
static unsigned short _reciprical_low_digit;
static int _modulus_sub_precision;
static int _modulus_bit_count;
static digit _scratch_modulus[MAX_UNIT_PRECISION];
static digit _mod_quotient[4];
static digit _mod_divisor[4];

// FUNCTION: 0x005C7EC0
int XMP_Prepare_Modulus(const digit * n_modulus, int precision)
{
	XMP_Move(_scratch_modulus, n_modulus, precision);
	_modulus_bit_count = XMP_Count_Bits(_scratch_modulus, precision);
	_modulus_sub_precision = (_modulus_bit_count + 16 - 1) / 16;

	int sub_precision = XMP_Significance(_scratch_modulus, precision);
	XMP_Move(_mod_divisor, &_scratch_modulus[sub_precision-2], 2);
	_modulus_shift = XMP_Count_Bits(_mod_divisor, 2) - 2 * 16;
	XMP_Shift_Right_Bits(_mod_divisor, _modulus_shift, 2);

	XMP_Reciprocal(_mod_quotient, _mod_divisor, 2);
	XMP_Shift_Right_Bits(_mod_quotient, 1, 2);

	_modulus_shift = ((_modulus_shift + (16 - 1)) % 16) + 1;

	XMP_Inc(_mod_quotient, 2);
	if (XMP_Count_Bits(_mod_quotient, 2) > 2 * 16) {
		XMP_Shift_Right_Bits(_mod_quotient, 1, 2);
		_modulus_shift--;
	}

	unsigned short * mpm = (unsigned short *) _mod_quotient;
	_reciprical_low_digit = *mpm++;
	_reciprical_high_digit = *mpm;
	return(0);
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
