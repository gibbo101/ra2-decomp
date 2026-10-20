// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).
#pragma once

#include "point.h"

#include <windows.h>

class Surface;
class XSurface;
class CCINIClass;
class Cell;

class MapPreviewClass
{
public:
	MapPreviewClass(void);
	~MapPreviewClass(void);

	bool Write_INI(CCINIClass & ini);
	bool Read_INI(CCINIClass const & ini);

	bool Read_PCX_Preview(char const * filename);
	bool Read_INI_Preview(char const * filename);

	void Create_Preview(void);
	void Blit_Preview(HWND window);
	void DrawStartPoints(HWND window);

	unsigned * Create_Paletted_Preview(int, int & size);
	void Create_Preview_Surface(char * buffer);
	XSurface * Get_Preview_Surface(void) { return(SurfacePtr); }

private:
	Point2D Cell_To_Preview_Pixel(Cell cell) const;

public:
	struct Header {
		int Width;
		int Height;
	};

private:
	XSurface * SurfacePtr;
};
