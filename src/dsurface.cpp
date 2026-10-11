// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"
#include "dsurface.h"
#include "blit.h"
#include "dbgprint.h"
#include "fastmath.h"
#include "misc.h"
#include "swap.h"
#include <math.h>

// FUNCTION: 0x004BA6B0
DSurface::~DSurface(void)
{
	if (IsPrimary && SurfacePtr != NULL && Clipper != NULL) {
		SurfacePtr->SetClipper(NULL);
		Clipper->Release();
		Clipper = NULL;
	}

	delete Description;
	Description = NULL;

	if (SurfacePtr != NULL)  {
		SurfacePtr->Release();
	}
	SurfacePtr = NULL;
}

// FUNCTION: 0x004BA720
DSurface::DSurface(void) :
	BytesPerPixel(0),
	LockPtr(NULL),
	IsPrimary(false),
	IsVideoRam(false),
	SurfacePtr(NULL)
{
	Description = new DDSURFACEDESC;
	memset(Description, '\0', sizeof(DDSURFACEDESC));
	Description->dwSize = sizeof(DDSURFACEDESC);
}

// FUNCTION: 0x004BAC60
DSurface::DSurface(LPDIRECTDRAWSURFACE surfaceptr) :
	BytesPerPixel(0),
	LockPtr(NULL),
	IsPrimary(false),
	IsVideoRam(false),
	SurfacePtr(surfaceptr),
	Description(NULL)
{
	if (SurfacePtr != NULL) {
		Description = new DDSURFACEDESC;
		memset(Description, '\0', sizeof(DDSURFACEDESC));
		Description->dwSize = sizeof(DDSURFACEDESC);
		HRESULT result = SurfacePtr->GetSurfaceDesc(Description);
		if (result == DD_OK) {
			BytesPerPixel = (Description->ddpfPixelFormat.dwRGBBitCount+7)/8;
			IsVideoRam = (Description->ddsCaps.dwCaps & DDSCAPS_VIDEOMEMORY) != 0;
			Width = Description->dwWidth;
			Height = Description->dwHeight;
		}
	}
}

// FUNCTION: 0x004BAD30
int DSurface::ReleaseDC(HDC hdc)
{
	HRESULT hr;

	hr = SurfacePtr->ReleaseDC(hdc);

	if ((hr == DD_OK) && (LockCount > 0)) {
		LockCount--;
	}

	return(1);
}

// FUNCTION: 0x004BAEC0
bool DSurface::Can_Lock(int x, int y) const
{
	if (LockCount == 0) {
		DDSURFACEDESC desc;
		memset(&desc, 0, sizeof(desc));
		desc.dwSize = sizeof(desc);
		HRESULT result = SurfacePtr->Lock(NULL, &desc, DDLOCK_SURFACEMEMORYPTR, NULL) != DD_OK;
		if (result != DD_OK) return(NULL);
		SurfacePtr->Unlock(desc.lpSurface);
	}
	return(true);
}

// FUNCTION: 0x004BAF20
bool DSurface::Can_Blit(void) const
{
	return(SurfacePtr->GetBltStatus(DDGBS_CANBLT) == DD_OK);
}

// FUNCTION: 0x004BBB00
void DSurface::Build_Remap_Table(unsigned short * table, int count, PaletteClass const & palette)
{
	if (count >= 1) {
		int i = count - 1;

		if (i >= 0) {
			int dividend = 0;

			i++;
			do {
				int intensity = count > 1 ? dividend / (count - 1) : 65536;

				for (int index = 0; index < 256; index++) {
					const RGBClass *rgb = &palette[index];
					int r = (rgb->Get_Red() * intensity) >> 16;
					int g = (rgb->Get_Green() * intensity) >> 16;
					int b = (rgb->Get_Blue() * intensity) >> 16;

					r = MIN(r, 255);
					g = MIN(g, 255);
					b = MIN(b, 255);

					*table = Build_Hicolor_Pixel(r, g, b);
					table++;
				}

				dividend += 131072;
				i--;
			} while (i);
		}
	}
}
