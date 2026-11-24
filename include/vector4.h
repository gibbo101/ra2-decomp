// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).
#pragma once

class Vector4
{
	public:

		void Set(float x, float y, float z, float w) { X = x; Y = y; Z = z; W = w; }

		float & operator [](int i) { return (&X)[i]; }
		const float operator [](int i) const { return (&X)[i]; }

		float X;
		float Y;
		float Z;
		float W;
};
