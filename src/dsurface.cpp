// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"
#include "geometry.h"
#include "dsurface.h"
#include "blit.h"
#include "dbgprint.h"
#include "fastmath.h"
#include "misc.h"
#include "swap.h"
#include <math.h>

extern LPDIRECTDRAW DirectDrawObject;
extern LPDIRECTDRAWSURFACE PaletteSurface;
extern bool WindowedMode;
extern bool GameInFocus;
namespace DirectDrawWrap { extern bool DoSmth; }
unsigned Get_Video_Hardware_Capabilities(void);
void Process_DD_Result(HRESULT result, int display_ok_msg);
void __cdecl DebugString(const char * string, ...);

unsigned short DSurface::HalfbrightMask = 0;
unsigned short DSurface::QuarterbrightMask = 0;
unsigned short DSurface::EighthbrightMask = 0;

// FUNCTION: 0x004BA3B0 $E2
// FUNCTION: 0x004BA3E0 $E5
// FUNCTION: 0x004BA400 $E8
// FUNCTION: 0x004BA420 $E11
// FUNCTION: 0x004BA440 $E14
// FUNCTION: 0x004BA460 $E17
// FUNCTION: 0x004BA480 $E20
// FUNCTION: 0x004BA4C0 $E23
// FUNCTION: 0x004BA4F0 $E26
// FUNCTION: 0x004BA520 $E29
// FUNCTION: 0x004BA550 $E32
// FUNCTION: 0x004BA560 $E35
// FUNCTION: 0x004BA580 $E38

// FUNCTION: 0x004BA5A0
DSurface::DSurface(int width, int height, bool system_memory, bool device_3d) :
	BASECLASS(width, height),
	BytesPerPixel(0),
	LockPtr(NULL),
	IsPrimary(false),
	IsVideoRam(false),
	SurfacePtr(NULL),
	Description(NULL)
{
	Description = new DDSURFACEDESC;
	if (Description != NULL) {
		memset(Description, '\0', sizeof(DDSURFACEDESC));
		Description->dwSize = sizeof(DDSURFACEDESC);
		Description->dwFlags = DDSD_CAPS | DDSD_HEIGHT | DDSD_WIDTH;
		Description->dwWidth = width;
		Description->dwHeight = height;

		if (system_memory == true) {
			Description->ddsCaps.dwCaps = DDSCAPS_SYSTEMMEMORY;
		}
		if (DirectDrawWrap::DoSmth && device_3d) {
			Description->ddsCaps.dwCaps = DDSCAPS_3DDEVICE | DDSCAPS_OFFSCREENPLAIN;
		}

		DirectDrawObject->CreateSurface(Description, &SurfacePtr, NULL);

		if (SurfacePtr != NULL) {
			memset(Description, '\0', sizeof(DDSURFACEDESC));
			Description->dwSize = sizeof(DDSURFACEDESC);
			SurfacePtr->GetSurfaceDesc(Description);
			BytesPerPixel = (Description->ddpfPixelFormat.dwRGBBitCount+7)/8;
			IsVideoRam = ((Description->ddsCaps.dwCaps & DDSCAPS_VIDEOMEMORY) != 0);
		}
	}
}

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

// FUNCTION: 0x004BA770
DSurface * DSurface::Create_Primary(DSurface ** backsurface1)
{
	DebugString("DSurface::Create_Primary\n");
	unsigned video_capabilities = Get_Video_Hardware_Capabilities();
	AllowStretchBlits = false;
	if ((video_capabilities & VIDEO_BLIT_STRETCH) != 0) {
		AllowStretchBlits = true;
	}
	DebugString("DSurface::AllowStretchBlits = %s\n", AllowStretchBlits ? "true" : "false");
	if ((video_capabilities & VIDEO_COLOR_FILL) == 0) {
		AllowHWFill = false;
	}
	DebugString("DSurface::AllowHWFill = %s\n", AllowHWFill == true ? "true" : "false");
	DSurface * surface = new DSurface();
	int backcount = (backsurface1 != NULL) ? 1 : 0;
	surface->Description->dwFlags = DDSD_CAPS;
	surface->Description->ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE;
	if (backcount > 0) {
		surface->Description->ddsCaps.dwCaps |= DDSCAPS_FLIP | DDSCAPS_COMPLEX;
		surface->Description->dwFlags |= DDSD_BACKBUFFERCOUNT;
		surface->Description->dwBackBufferCount = backcount;
	}
	DebugString("DSurface::Create_Primary - Creating surface\n");
	HRESULT result = DirectDrawObject->CreateSurface(surface->Description, &surface->SurfacePtr, NULL);
	if (result == DD_OK) {
		DebugString("CreateSurface OK\n");
		if (backcount > 0) {
			LPDIRECTDRAWSURFACE back;
			DDSCAPS caps;
			caps.dwCaps = DDSCAPS_BACKBUFFER;
			result = surface->SurfacePtr->GetAttachedSurface(&caps, &back);
			if (result == DD_OK) {
				*backsurface1 = new DSurface(back);
			}
		}
		memset(surface->Description, '\0', sizeof(DDSURFACEDESC));
		surface->Description->dwSize = sizeof(DDSURFACEDESC);
		result = surface->SurfacePtr->GetSurfaceDesc(surface->Description);
		if (result != DD_OK) {
			DebugString("Failed to get description of primary surface\n");
		}
		surface->BytesPerPixel = (surface->Description->ddpfPixelFormat.dwRGBBitCount+7)/8;
		surface->IsVideoRam = (surface->Description->ddsCaps.dwCaps & DDSCAPS_VIDEOMEMORY) != 0;
		surface->IsPrimary = true;
		surface->Width = surface->Description->dwWidth;
		surface->Height = surface->Description->dwHeight;
		PaletteSurface = surface->SurfacePtr;
		if (DirectDrawObject->CreateClipper(0, &Clipper, NULL) == DD_OK) {
			if (Clipper->SetHWnd(0, GetActiveWindow()) == DD_OK) {
				surface->SurfacePtr->SetClipper(Clipper);
			}
		}
		memcpy(&PixelFormat, &surface->Description->ddpfPixelFormat, sizeof(DDPIXELFORMAT));
		if (surface->Bytes_Per_Pixel() == 2) {
			int index;
			int shift = PixelFormat.dwRBitMask;
			RedRight = 0;
			RedLeft = 0;
			for (index = 0; index < 16; index++) {
				if (shift & 0x01) break;
				shift >>= 1;
				RedRight++;
			}
			for (index = 0; index < 8; index++) {
				if (shift & 0x80) break;
				shift <<= 1;
				RedLeft++;
			}
			shift = PixelFormat.dwGBitMask;
			GreenRight = 0;
			GreenLeft = 0;
			for (index = 0; index < 16; index++) {
				if (shift & 0x01) break;
				GreenRight++;
				shift >>= 1;
			}
			for (index = 0; index < 8; index++) {
				if (shift & 0x80) break;
				GreenLeft++;
				shift <<= 1;
			}
			shift = PixelFormat.dwBBitMask;
			BlueRight = 0;
			BlueLeft = 0;
			for (index = 0; index < 16; index++) {
				if (shift & 0x01) break;
				BlueRight++;
				shift >>= 1;
			}
			for (index = 0; index < 8; index++) {
				if (shift & 0x80) break;
				BlueLeft++;
				shift <<= 1;
			}
			PrimaryColorMode = COLORMODE_INVALID;
			HalfbrightMask = (unsigned short)Build_Hicolor_Pixel(127, 127, 127);
			QuarterbrightMask = (unsigned short)Build_Hicolor_Pixel(63, 63, 63);
			EighthbrightMask = (unsigned short)Build_Hicolor_Pixel(31, 31, 31);
			if (BlueRight == 0 && BlueLeft == 3 && GreenRight == 5 && GreenLeft == 3 && RedRight == 10 && RedLeft == 3) {
				PrimaryColorMode = COLORMODE_555;
			} else
			if (BlueRight == 0 && BlueLeft == 2 && GreenRight == 6 && GreenLeft == 3 && RedRight == 11 && RedLeft == 3) {
				PrimaryColorMode = COLORMODE_556;
			} else
			if (BlueRight == 0 && BlueLeft == 3 && GreenRight == 5 && GreenLeft == 2 && RedRight == 11 && RedLeft == 3) {
				PrimaryColorMode = COLORMODE_565;
			} else
			if (BlueRight == 0 && BlueLeft == 3 && GreenRight == 5 && GreenLeft == 3 && RedRight == 11 && RedLeft == 2) {
				PrimaryColorMode = COLORMODE_556;
			}
		}
		DebugString("DSurface::Create_Primary done\n");
	} else {
		Process_DD_Result(result, TRUE);
		DebugString("CreateSurface failed with error code %08x\n", result);
		delete surface;
		surface = NULL;
	}
	return(surface);
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

inline bool Restore_Surface(DSurface const * surface)
{
	if (!WindowedMode && !GameInFocus) {
		return(false);
	}

	if (surface->SurfacePtr != NULL && surface->SurfacePtr->IsLost() == DDERR_SURFACELOST) {
		if (surface->SurfacePtr->Restore() != DD_OK || surface->SurfacePtr->IsLost() != DD_OK) return(false);
		int oldlockcount = surface->LockCount;
		if (surface->LockCount > 0) {
			surface->LockCount = 0;
			surface->Lock();
			surface->LockCount++;
			surface->Unlock();
			surface->LockCount = oldlockcount;
		}
	}

	return(true);
}

// FUNCTION: 0x004BACF0
HDC DSurface::GetDC(void)
{
	HDC hdc = NULL;
	HRESULT hr;

	if (!Is_Locked()) {
		hr = SurfacePtr->GetDC(&hdc);
		if (hr == DD_OK) {
			LockCount++;
		} else {
			hdc = NULL;
		}
	}

	return(hdc);
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


// FUNCTION: 0x004BAD60
int DSurface::Bytes_Per_Pixel(void) const
{
	return(BytesPerPixel);
}

// FUNCTION: 0x004BAD70
int DSurface::Stride(void) const
{
	return(Description->lPitch);
}

// FUNCTION: 0x004BAD80
void * DSurface::Lock(Point2D point) const
{
	if (!Restore_Surface(this)) return(NULL);
	if (point.X < 0 || point.Y < 0) return(NULL);

	if (LockCount == 0) {
		DDSURFACEDESC desc;
		memset(&desc, '\0', sizeof(desc));
		desc.dwSize = sizeof(desc);
		HRESULT result = SurfacePtr->Lock(NULL, &desc, DDLOCK_SURFACEMEMORYPTR|DDLOCK_WAIT, NULL);
		if (result != DD_OK) return(NULL);
		memcpy(Description, &desc, sizeof(DDSURFACEDESC));
		BytesPerPixel = (Description->ddpfPixelFormat.dwRGBBitCount+7)/8;
		IsVideoRam = (Description->ddsCaps.dwCaps & DDSCAPS_VIDEOMEMORY) != 0;
		LockPtr = Description->lpSurface;
	}
	BASECLASS::Lock();
	return(((char*)LockPtr) + point.Y * Stride() + point.X * Bytes_Per_Pixel());
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

// FUNCTION: 0x004BAF40
bool DSurface::Unlock(void) const
{
	Restore_Surface(this);
	if (LockCount > 0) {
		BASECLASS::Unlock();
		if (LockCount == 0) {
			SurfacePtr->Unlock(LockPtr);
			LockPtr = NULL;
		}
		return(true);
	}
	return(false);
}

// FUNCTION: 0x004BAFE0
bool DSurface::Is_Lost(void) const
{
	return(SurfacePtr != NULL && SurfacePtr->IsLost() == DDERR_SURFACELOST);
}

// FUNCTION: 0x004BB000
bool DSurface::Restore_Check(void) const
{
	return(Restore_Surface(this));
}

// FUNCTION: 0x004BB080
bool DSurface::Blit_From(Rect const & destrect, Surface const & ssource, Rect const & sourcerect, bool trans, bool unknown)
{
	return(Blit_From(Get_Rect(), destrect, ssource, ssource.Get_Rect(), sourcerect, trans, unknown));
}

// FUNCTION: 0x004BB0D0
bool DSurface::Blit_From(Rect const & dcliprect, Rect const & destrect, Surface const & ssource, Rect const & scliprect, Rect const & sourcerect, bool trans, bool unknown)
{
	if (!dcliprect.Is_Valid() || !scliprect.Is_Valid() || !destrect.Is_Valid() || !sourcerect.Is_Valid()) return(false);
	bool use_xsurface = false;
	DSurface & dsource = (DSurface &)ssource;
	if (!ssource.Is_Direct_Draw() == true || dsource.Is_Locked() == true || trans == true || Bytes_Per_Pixel() != ssource.Bytes_Per_Pixel()) {
		use_xsurface = true;
	}
	if (IsPrimary == true && WindowedMode == true) {
		unknown = false;
	}
	if ((use_xsurface == false && unknown == true && (IsVideoRam == false || ((DSurface &)ssource).Is_Direct_Draw() == true && ((DSurface &)ssource).IsVideoRam == false) && sourcerect.Width == destrect.Width) &&
		sourcerect.Height == destrect.Height) {
		use_xsurface = true;
	}
	if (Restore_Surface(this) == false) {
		return(false);
	}
	if (use_xsurface == true) {
		return(BASECLASS::Blit_From(destrect, ssource, sourcerect, trans, true));
	}
	DSurface const & source = (DSurface const &)ssource;
	Rect drect = destrect;
	Rect srect = sourcerect;
	Rect swindow(Intersect(scliprect, ssource.Get_Rect()));
	Rect dwindow = Intersect(dcliprect, Get_Rect());
	if (Blit_Clip(drect, dwindow, srect, swindow)) {
		RECT xdestrect;
		xdestrect.left = drect.X+dwindow.X;
		xdestrect.top = drect.Y+dwindow.Y;
		xdestrect.right = drect.X+dwindow.X+drect.Width;
		xdestrect.bottom = drect.Y+dwindow.Y+drect.Height;
		RECT xsrcrect;
		xsrcrect.left = srect.X+swindow.X;
		xsrcrect.top = srect.Y+swindow.Y;
		xsrcrect.right = srect.X+swindow.X+srect.Width;
		xsrcrect.bottom = srect.Y+swindow.Y+srect.Height;
		HRESULT result = SurfacePtr->Blt(&xdestrect, source.SurfacePtr, &xsrcrect, DDBLT_WAIT, NULL);
		return(result == DD_OK);
	}
	return(false);
}

// FUNCTION: 0x004BB5F0
bool DSurface::Fill_Rect(Rect const & fillrect, int color)
{
	return(DSurface::Fill_Rect(Get_Rect(), fillrect, color));
}

// FUNCTION: 0x004BB620
bool DSurface::Fill_Rect(Rect const & cliprect, Rect const & fillrect, int color)
{
	if (SurfacePtr == NULL || !fillrect.Is_Valid()) return(false);

	if (!AllowHWFill || Is_Locked() || !DSurface::Can_Blit()) {
		return(BASECLASS::Fill_Rect(cliprect, fillrect, color));
	}

	if (!Restore_Check()) return(false);

	Rect crect = Intersect(cliprect, Get_Rect());
	Rect frect = fillrect.Bias_To(cliprect);

	frect = Intersect(frect, crect);
	if (!frect.Is_Valid()) return(false);

	RECT rect;
	rect.left = frect.X;
	rect.top = frect.Y;
	rect.right = rect.left + frect.Width;
	rect.bottom = rect.top + frect.Height;

	DDBLTFX fx;
	memset(&fx, '\0', sizeof(fx));
	fx.dwSize = sizeof(fx);
	fx.dwFillColor = color;
	HRESULT result = SurfacePtr->Blt(&rect, NULL, NULL, DDBLT_WAIT|DDBLT_COLORFILL, &fx);
	return(result == DD_OK);
}

// FUNCTION: 0x004BB830
bool DSurface::Fill_Rect_Trans(Rect const & xcliprect, const RGBClass & color, unsigned int opacity)
{

	if (Bytes_Per_Pixel() < 2) {
		return(false);
	}

	if (!xcliprect.Is_Valid()) {
		return(false);
	}

	
	Rect cliprect = Intersect(Get_Rect(), xcliprect);

	if (!cliprect.Is_Valid()) {
		return(false);
	}

	unsigned short r_mask;
	unsigned short g_mask;
	unsigned short b_mask;

	r_mask = 255;
	g_mask = 255;
	b_mask = 255;

	r_mask >>= (unsigned short)RedLeft;
	g_mask >>= (unsigned short)GreenLeft;
	b_mask >>= (unsigned short)BlueLeft;

	r_mask <<= (unsigned short)RedRight;
	g_mask <<= (unsigned short)GreenRight;
	b_mask <<= (unsigned short)BlueRight;

	unsigned short *ptr = (unsigned short *)Lock(cliprect.Top_Left());
	if (ptr == NULL) {
		return(false);
	}

	if (opacity > 100) {
		opacity = 100;
	}

	opacity = ((opacity * 255) / 100);
	unsigned short s2 = (255 - opacity);

	unsigned short c1 = Build_Hicolor_Pixel(color.Get_Red(), color.Get_Green(), color.Get_Blue());

	for (int y = 0; y < cliprect.Height; y++) {
		int pos = y * (Stride() / 2);

		for (int x = 0; x < cliprect.Width; x++) {
			unsigned short *p = &ptr[pos];
			unsigned short c2 = *p;

			unsigned int r1 = (c1 & r_mask) * opacity;
			unsigned int g1 = (c1 & g_mask) * opacity;
			unsigned int b1 = (c1 & b_mask) * opacity;

			unsigned int r2 = (c2 & r_mask) * s2;
			unsigned int g2 = (c2 & g_mask) * s2;
			unsigned int b2 = (c2 & b_mask) * s2;

			unsigned int r = ((r1 + r2) / 256);
			unsigned int g = ((g1 + g2) / 256);
			unsigned int b = ((b1 + b2) / 256);

			*p = ((r & r_mask) | (g & g_mask) | b);
			pos++;
		}
	}

	Unlock();
	return(true);
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

// FUNCTION: 0x004BBC30
int DSurface::Get_Red_Right(void)
{
	return(RedRight);
}

// FUNCTION: 0x004BBC40
int DSurface::Get_Red_Left(void)
{
	return(RedLeft);
}

// FUNCTION: 0x004BBC50
int DSurface::Get_Green_Right(void)
{
	return(GreenRight);
}

// FUNCTION: 0x004BBC60
int DSurface::Get_Green_Left(void)
{
	return(GreenLeft);
}

// FUNCTION: 0x004BBC70
int DSurface::Get_Blue_Right(void)
{
	return(BlueRight);
}

// FUNCTION: 0x004BBC80
int DSurface::Get_Blue_Left(void)
{
	return(BlueLeft);
}

// FUNCTION: 0x004BBC90
int DSurface::Get_Primary_Color_Mode(void)
{
	return(PrimaryColorMode);
}

// FUNCTION: 0x004BE9D0
int __fastcall Brighten_Hicolor_Pixel(RGBClass const * rgb, int red_add, int green_add, int blue_add)
{
	int red = rgb->Get_Red() + red_add;
	int green = rgb->Get_Green() + green_add;
	int blue = rgb->Get_Blue() + blue_add;
	int excess = 0;
	int full = 0;

	if (red >= 255) {
		full = 1;
		excess = red - 255;
	}
	if (green >= 255) {
		full++;
		excess += green - 255;
	}
	if (blue >= 255) {
		full++;
		excess += blue - 255;
	}
	if (full != 0 && full < 3) {
		if (full == 1) {
			excess >>= 1;
		}
		if (red < 255) red += excess;
		if (green < 255) green += excess;
		if (blue < 255) blue += excess;
	}
	if (blue > 255) blue = 255;
	if (green > 255) green = 255;
	if (red > 255) red = 255;
	return(DSurface::Build_Hicolor_Pixel(red, green, blue));
}

// FUNCTION: 0x004BF650
unsigned short DSurface::Blend_Pixel(unsigned short src_color, unsigned short dst_color, int level)
{
	switch (level) {
		case 100:
			goto done;
		case 75:
			return(((src_color / 4) & DSurface::QuarterbrightMask) * 4);
		case 50:
			return(((src_color / 2) & HalfbrightMask) + ((dst_color / 2) & HalfbrightMask));
		case 25:
			return(((src_color / 4) & DSurface::QuarterbrightMask) * 4);
		case 0:
			return(dst_color);
		default:
			break;
	}
done:
	return(src_color);
}
