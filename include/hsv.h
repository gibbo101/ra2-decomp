// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).
#pragma once

class RGBClass;
class HSVClass;

class HSVClass
{
	private:
		static HSVClass const BlackColor;

	public:
		HSVClass(void) : Hue(0), Saturation(0), Value(0) {};
		HSVClass(unsigned char hue, unsigned char saturation, unsigned char value) :
				Hue(hue),
				Saturation(saturation),
				Value(value)
			{};
		operator RGBClass (void) const;

		bool operator==(HSVClass &that);
		bool operator!=(const HSVClass &that);

		enum {
			MAX_VALUE=255
		};

		void Adjust(int ratio, HSVClass const & hsv);
		int Difference(HSVClass const & hsv) const;
		int Get_Hue(void) const {return(Hue);};
		int Get_Saturation(void) const {return(Saturation);};
		int Get_Value(void) const {return(Value);};
		void Set_Hue(unsigned char value) {Hue = value;}
		void Set_Saturation(unsigned char value) {Saturation = value;}
		void Set_Value(unsigned char value) {Value = value;}

	private:
		unsigned char Hue;
		unsigned char Saturation;
		unsigned char Value;
};

bool operator==(HSVClass const & a, HSVClass const & b);
bool operator!=(HSVClass const & a, HSVClass const & b);
