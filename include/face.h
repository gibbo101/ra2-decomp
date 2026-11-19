// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).
#pragma once

#include "comtypes.h"
#include "coord.h"
#include "fastmath.h"
#include "noinit.h"

#include <cstdlib>

class DirType;

inline DirType Direction(Coord const & coord1, Coord const & coord2);

bool operator == (DirType const & lvalue, DirType const & rvalue);
bool operator != (DirType const & lvalue, DirType const & rvalue);
bool operator < (DirType const & lvalue, DirType const & rvalue);
bool operator > (DirType const & lvalue, DirType const & rvalue);
bool operator <= (DirType const & lvalue, DirType const & rvalue);

enum Dir32 {};

enum Dir256 {
	DIR_MIN=0,
	DIR_N=0,
	DIR_NE=1<<5,
	DIR_E=2<<5,
	DIR_SE=3<<5,
	DIR_S=4<<5,
	DIR_SW=5<<5,
	DIR_W=6<<5,
	DIR_NW=7<<5,
	DIR_MAX=255,

	DIR_STEP_2=1<<7,
	DIR_STEP_4=1<<6,
	DIR_STEP_8=1<<5,
	DIR_STEP_16=1<<4,
	DIR_STEP_32=1<<3,
	DIR_STEP_64=1<<2,
	DIR_STEP_128=1<<1,
	DIR_STEP_256=1
};

#define DIR_SW_X1	Dir256((5<<5)-8)
#define DIR_SW_X2	Dir256((5<<5)-16)

class DirType
{
	public:
		DirType(NoInitClass const & x) {}

		DirType(void) {}
		DirType(int dir) { Facing = dir; }
		DirType(DirStruct dir) { Raw = dir.Raw; }
		DirType(FacingType dir) { From_Facing(dir); }
		DirType(Dir32 dir) { From_Dir32(dir); }
		DirType(Dir256 dir) { From_Dir256(dir); }
		DirType(double rad) { From_Radian(rad); }

		DirType & Direction(Coord const & coord1, Coord const & coord2)
		{
			if (coord1.TPoint2D<int>::operator==(coord2)) {
				Facing = 0;
			}

			*this = DirType(fastmath::atan2((double)coord1.Y - (double)coord2.Y, (double)coord2.X - (double)coord1.X));

			return *this;
		}

		DirType & Direction(Cell const & cell1, Cell const & cell2)
		{
			if (cell1 == cell2) {
				Facing = 0;
			}

			*this = DirType(fastmath::atan2((double)cell1.Y - (double)cell2.Y, (double)cell2.X - (double)cell1.X));

			return *this;
		}

		void From_Dir4(int dir)
		{
			Facing = 0;
			Facing |= (dir << 14);
		}

		void From_Facing(FacingType dir)
		{
			Facing = 0;
			Facing |= (dir << 13);
		}

		void From_Dir32(Dir32 dir)
		{
			Facing = 0;
			Facing |= (dir << 11);
		}

		void From_Dir256(Dir256 dir)
		{
			Facing = 0;
			Facing |= (dir << 8);
		}

		void From_Radian(double rad)
		{

			Facing = short((rad - M_PI / 2) / -DEG_TO_RAD(360.0 / 65534.0));
		}

		unsigned int	Round_To_4(void) const		{ return	((((unsigned int)Raw >> 13) + 1) >> 1); }
		unsigned int	Round_To_8(void) const		{ return	((((unsigned int)Raw >> 12) + 1) >> 1); }
		unsigned int	Round_To_32(void) const		{ return	((((unsigned int)Raw >> 10) + 1) >> 1); }
		unsigned int	Round_To_256(void) const	{ return	((((unsigned int)Raw >> 7) + 1) >> 1); }

		int			As_Dir4(void) const		{ return				(Round_To_4() % (4)); }
		FacingType	As_Dir8(void) const		{ return (FacingType)	(Round_To_8() % (8)); }
		Dir32		As_Dir32(void) const	{ return (Dir32)		(Round_To_32() % (32)); }
		Dir256		As_Dir256(void) const	{ return (Dir256)		(Round_To_256() % (256)); }

		int			As_Axis(void) const		{ return (int)			(Round_To_8() % (4)); }

		FacingType	As_Facing(void) const	{ return (FacingType)	((((((unsigned int)Facing >> 12) + 1) >> 1)) % (8)); }

		DirType const & Snap_To_4(void)		{ Facing &= 0xC000; return(*this); }
		DirType const & Snap_To_8(void)		{ Facing &= 0xE000; return(*this); }
		DirType const & Snap_To_32(void)	{ Facing &= 0xF800; return(*this); }
		DirType const & Snap_To_256(void)	{ Facing &= 0xFF00; return(*this); }

		double		As_Radian(void) const { return (double)		((Facing - 16383) * -DEG_TO_RAD(360.0 / 65534.0)); }
		double		As_Radian32(void) const { return (double)		((As_Dir32() - 8) * -DEG_TO_RAD(360.0 / 32.0)); }

		int			As_Int(void) const { return(Facing); }

		DirType Left_90(void) const { return(DirType(Facing - 0x3FFF)); }
		DirType Left_180(void) const { return(DirType(Facing - 0x7FFF)); }
		DirType Left_270(void) const { return(DirType(Facing - 0xBFFF)); }
		DirType Right_90(void) const { return(DirType(Facing + 0x3FFF)); }
		DirType Right_180(void) const { return(DirType(Facing + 0x7FFF)); }
		DirType Right_270(void) const { return(DirType(Facing + 0xBFFF)); }

		DirType operator + (DirType const & rvalue) const { return DirType(Facing + rvalue.Facing); }
		DirType operator - (DirType const & rvalue) const { return DirType(Facing - rvalue.Facing); }
		DirType operator * (int rvalue) const { return DirType(Facing * rvalue); }
		DirType operator / (int rvalue) const { return DirType(Facing / rvalue); }

		const DirType & operator += (DirType const & rvalue) { Facing += rvalue.Facing; return *this; }
		const DirType & operator -= (DirType const & rvalue) { Facing -= rvalue.Facing; return *this; }
		const DirType & operator *= (int rvalue) { Facing *= rvalue; return *this; }
		const DirType & operator /= (int rvalue) { Facing /= rvalue; return *this; }

		bool operator >= (DirType const & rvalue) const {return abs(Facing) >= abs(rvalue.Facing); }

		bool Is_Complete_Turn(DirType const & towards, DirType const & by)
		{
			DirType diff(*this - towards);
			if (by >= diff) {
				return(true);
			}
			return(false);
		}

		bool Turn(DirType const & towards, DirType const & by)
		{
			if (Is_Complete_Turn(towards, by)) {
				Facing = towards.Facing;
				return(true);
			}

			if ((towards - *this).Facing < 0) {
				*this -= by;
			} else {
				*this += by;
			}
			return(false);
		}

	public:
		union {

			short Facing;

			int Raw;
		};
};

inline bool operator == (DirType const & lvalue, DirType const & rvalue) { return lvalue.Facing == rvalue.Facing; }
inline bool operator != (DirType const & lvalue, DirType const & rvalue) { return lvalue.Facing != rvalue.Facing; }

inline bool operator < (DirType const & lvalue, DirType const & rvalue) {return abs(lvalue.Facing) < abs(rvalue.Facing); }
inline bool operator > (DirType const & lvalue, DirType const & rvalue) {return abs(lvalue.Facing) > abs(rvalue.Facing); }
inline bool operator <= (DirType const & lvalue, DirType const & rvalue) {return abs(lvalue.Facing) <= abs(rvalue.Facing); }

