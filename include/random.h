// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).
#pragma once

class RandomClass {
	public:
		RandomClass(unsigned seed=0);

		operator int(void) {return(operator()());};
		int operator() (void);
		int operator() (int minval, int maxval);

		enum {
			SIGNIFICANT_BITS=15
		};

	protected:
		unsigned int Seed;

		enum {
			MULT_CONSTANT=0x41C64E6D,
			ADD_CONSTANT=0x00003039,
			THROW_AWAY_BITS=10
		};
};

class Random2Class {
	public:
		Random2Class(unsigned seed=0);

		operator int(void) {return(operator()());};
		int operator() (void);
		int operator() (int minval, int maxval);

		enum {
			TABLE_SIZE = 250,
			SIGNIFICANT_BITS=32
		};

	protected:
		bool unknown_00;
		int Index1;
		int Index2;
		int Table[TABLE_SIZE];
};

class Random3Class {
	public:
		Random3Class(unsigned seed1=0, unsigned seed2=0);

		operator int(void) {return(operator()());};
		int operator() (void);
		int operator() (int minval, int maxval);

		enum {
			TABLE_SIZE = 20,
			SIGNIFICANT_BITS=32
		};

	protected:
		static int Mix1[TABLE_SIZE];
		static int Mix2[TABLE_SIZE];
		int Seed;
		int Index;
};

template<class T>
int Pick_Random_Number(T & generator, int minval, int maxval)
{

	if (minval == maxval) return(minval);

	if (minval > maxval) {
		int temp = minval;
		minval = maxval;
		maxval = temp;
	}

	int magnitude = maxval - minval;
	int highbit = T::SIGNIFICANT_BITS-1;
	while ((magnitude & (1 << highbit)) == 0 && highbit > 0) {
		highbit--;
	}

	int mask = ~( (~0L) << (highbit+1));

	int pick = magnitude+1;
	while (pick > magnitude) {
		pick = generator() & mask;
	}

	return(pick + minval);
}
