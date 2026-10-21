// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"
#include "vector.h"
#include <cstring>
#include <new.h>

// FUNCTION: GAMEMD 0x0074BBF0
BooleanVectorClass::BooleanVectorClass(BooleanVectorClass const & vector)
{
	LastIndex = -1;
	*this = vector;
}

// FUNCTION: GAMEMD 0x0074BCA0
BooleanVectorClass & BooleanVectorClass::operator =(BooleanVectorClass const & vector)
{
	Fixup();
	Copy = vector.Copy;
	LastIndex = vector.LastIndex;
	BitArray = vector.BitArray;
	BitCount = vector.BitCount;
	return(*this);
}

// FUNCTION: GAMEMD 0x0074BD50
bool BooleanVectorClass::operator == (const BooleanVectorClass & vector) const
{
	Fixup(LastIndex);
	return(BitCount == vector.BitCount && BitArray == vector.BitArray);
}

// FUNCTION: GAMEMD 0x0074BDC0
int BooleanVectorClass::Resize(unsigned size)
{
	Fixup();

	if (size > 0) {
		unsigned oldsize = BitCount;

		int success = BitArray.Resize(((size + (8-1)) / 8));

		BitCount = size;
		if (success && oldsize < size) {
			for (unsigned index = oldsize; index < size; index++) {
				(*this)[index] = 0;
			}
		}

		return(success);
	}

	Clear();
	return(true);
}

// FUNCTION: GAMEMD 0x0074BEA0
void BooleanVectorClass::Clear(void)
{
	Fixup();
	BitCount = 0;
	BitArray.Clear();
}

// FUNCTION: GAMEMD 0x0074BEE0
void BooleanVectorClass::Reset(void)
{
	LastIndex = -1;
	if (BitArray.Length() > 0) {
		memset(&BitArray[0], '\0', BitArray.Length());
	}
}

// FUNCTION: GAMEMD 0x0074BF10
void BooleanVectorClass::Set(void)
{
	LastIndex = -1;
	if (BitArray.Length() > 0) {
		memset(&BitArray[0], '\xFF', BitArray.Length());
	}
}

// FUNCTION: GAMEMD 0x0074BF40
void BooleanVectorClass::Fixup(int index) const
{
	if ((unsigned)index >= (unsigned)BitCount) {
		index = -1;
	}

	if (index != LastIndex) {
		if (LastIndex != -1) {
			Set_Bit((void*)&BitArray[0], LastIndex, Copy);
		}

		if (index != -1) {
			((unsigned char &)Copy) = (unsigned char)Get_Bit((void*)&BitArray[0], index);
		}

		((BooleanVectorClass *)this)->LastIndex = index;
	}
}

template class VectorClass<unsigned char>;

// TEMPLATE: GAMEMD 0x0074BF90
// ?GetItem@?$VectorClass@E@@UBEEH@Z

// SYNTHETIC: GAMEMD 0x0074BFA0
// ??_G?$VectorClass@E@@UAEPAXI@Z

// TEMPLATE: GAMEMD 0x0074BFF0
// ??8?$VectorClass@E@@UBE_NABV0@@Z

// TEMPLATE: GAMEMD 0x0074C030
// ?Resize@?$VectorClass@E@@UAE_NHPBE@Z

// TEMPLATE: GAMEMD 0x0074C0E0
// ?Clear@?$VectorClass@E@@UAEXXZ

// TEMPLATE: GAMEMD 0x0074C110
// ?ID@?$VectorClass@E@@UAEHABE@Z

// TEMPLATE: GAMEMD 0x0074C150
// ?ID@?$VectorClass@E@@UAEHPBE@Z

struct IntQuad {
	int A, B, C, D;
	IntQuad() { A = 0; B = 0; C = 0; D = 0; }
};

// SYNTHETIC: GAMEMD 0x0074C170 SYMBOL
// _$E2
IntQuad DAT_00B1D260;
