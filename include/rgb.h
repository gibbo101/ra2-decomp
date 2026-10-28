// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).
#pragma once

#include "noinit.h"

class PaletteClass;
class HSVClass;
struct IStream;

#pragma pack(1)
struct RGBStruct
{
	bool operator==(const RGBStruct &that) const { return(Red == that.Red && Green == that.Green && Blue == that.Blue); }
	bool operator!=(const RGBStruct &that) const { return(Red != that.Red || Green != that.Green || Blue != that.Blue); }

	unsigned char Red;
	unsigned char Green;
	unsigned char Blue;
};
#pragma pack()

class RGBClass
{
	public:
		RGBClass(void) : Red(0), Green(0), Blue(0) {}
		RGBClass(NoInitClass const & x) {}
		RGBClass(unsigned char red, unsigned char green, unsigned char blue) : Red(red), Green(green), Blue(blue) {}
		operator HSVClass (void) const;

		bool operator==(const RGBClass &that) const { return(Red == that.Red && Green == that.Green && Blue == that.Blue); }
		bool operator!=(const RGBClass &that) const { return(Red != that.Red || Green != that.Green || Blue != that.Blue); }

		enum {
			MAX_VALUE=255
		};

		RGBClass & Lerp(RGBClass const & a, RGBClass const & b, float t);
		RGBClass & Set(RGBClass const & rgb, float value);
		void Adjust(int ratio, RGBClass const & rgb);
		int Difference(RGBClass const & rgb) const;
		int Get_Red(void) const {return(Red);}
		int Get_Green(void) const {return(Green);}
		int Get_Blue(void) const {return(Blue);}
		void Set_Red(unsigned char value) {Red = value;}
		void Set_Green(unsigned char value) {Green = value;}
		void Set_Blue(unsigned char value) {Blue = value;}
		void Save(IStream * stream) const;
		void Load(IStream * stream);

	private:

		friend class PaletteClass;

		unsigned char Red;
		unsigned char Green;
		unsigned char Blue;
};

extern RGBClass const BlackColor;
