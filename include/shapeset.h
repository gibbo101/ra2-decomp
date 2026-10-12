// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).
#pragma once

#include "_rect.h"
#include "rect.h"
#include "rgb.h"

#pragma pack(push, 4)
class ShapeSet
{
	public:

		void * Get_Data(int shape) const;

		Rect Get_Rect(int shape) const;

		RGBClass Get_Color(int shape) const;

		bool Is_Transparent(int shape) const;

		bool Is_RLE_Compressed(int shape) const;

		int Get_Width(void) const {return(Width);}

		int Get_Height(void) const {return(Height);}

		int Get_Count(void) const{return(Count);}

		int Get_Size(int shape) const;

	protected:

		short Flags;

		short Width;
		short Height;

		short Count;

		class ShapeRecord
		{
			public:

				short X;
				short Y;

				short Width;
				short Height;

				short Flags;
				enum {
					SFLAG_TRANSPARENT=0x01,
					SFLAG_RLE=0x02
				};

				short Size;

				unsigned char Color[3];

				unsigned char Unused[5];

				int Data;

				bool Is_Transparent(void) const {return((Flags & SFLAG_TRANSPARENT) != 0);}
				bool Is_RLE_Compressed(void) const {return((Flags & SFLAG_RLE) != 0);}
				short Get_Size(void) const {return(Size);}

				void Flag_Transparent(void) {Flags |= SFLAG_TRANSPARENT;}
				void Flag_RLE_Compressed(void) {Flags |= SFLAG_RLE;}
				void Set_Size(short size) {Size = size;}
		};

		bool Is_Shape_Index_Valid(int index) const {return(unsigned(index) < unsigned(Count));}

		ShapeRecord const * Fetch_Record_Pointer(int shape) const
		{
			if (Is_Shape_Index_Valid(shape)) {
				return((ShapeRecord const *)(((char *)this) + sizeof(ShapeSet) + sizeof(ShapeRecord) * shape));
			}
			return(NULL);
		}

		ShapeSet(void) {};
		ShapeSet(ShapeSet const & rvalue);
		ShapeSet const & operator = (ShapeSet const & rvalue);
};
#pragma pack(pop)

inline void * ShapeSet::Get_Data(int shape) const
{
	ShapeRecord const * record = Fetch_Record_Pointer(shape);
	if (record != NULL && record->Data != 0) return(((char *)this) + record->Data);
	return(NULL);
}

inline Rect ShapeSet::Get_Rect(int shape) const
{
	ShapeRecord const * record = Fetch_Record_Pointer(shape);
	if (record != NULL) {
		return(Rect(record->X, record->Y, record->Width, record->Height));
	}
	return(RECT_NONE);
}

inline bool ShapeSet::Is_Transparent(int shape) const
{
	ShapeRecord const * record = Fetch_Record_Pointer(shape);
	if (record != NULL) {
		return(record->Is_Transparent());
	}
	return(false);
}

inline bool ShapeSet::Is_RLE_Compressed(int shape) const
{
	ShapeRecord const * record = Fetch_Record_Pointer(shape);
	if (record != NULL) {
		return(record->Is_RLE_Compressed());
	}
	return(false);
}
