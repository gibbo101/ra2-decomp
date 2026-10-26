// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#define DIRECTDRAW_VERSION 0x0300
#include "always.h"
#include <d3d.h>
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
namespace DirectDrawWrap { extern bool DoSmth; extern LPDIRECTDRAW lpDD; }
extern LPDIRECTDRAW2 DAT_008A0E00;
extern LPDIRECT3D2 DAT_008A0DF4;
extern LPDIRECT3DDEVICE2 DAT_008A0DF8;
extern LPDIRECT3DVIEWPORT2 DAT_008A0DFC;
extern const GUID DAT_007F7D50;
extern const GUID DAT_007F8070;
extern const GUID DAT_007F80B0;
extern const double DAT_007E3D90;
extern double __cdecl FUN_004CAC40(double);
extern const double DAT_007E1708;
extern const double DAT_007E1710;
extern const double DAT_007E8678;
extern const double DAT_007E8680;
extern const double DAT_007E1718;
extern const double DAT_007E1720;
extern const double DAT_007E1728;
extern const double DAT_007E1730;
extern const double DAT_007E1738;
extern const double DAT_007E1740;
extern double __cdecl FUN_004CAD50(double);
extern double __cdecl FUN_004CADE0(double);
extern double DAT_008A0E38;
extern double DAT_008A0EA8;

class Texture3D {
	public:
		Texture3D(int width, int height, int flag);
		int FUN_004A33A0(unsigned short * pixels);
		void FUN_004A3370(void);
	private:
		char Data[0x78];
};

class Texture3DCache {
	public:
		Texture3DCache(void);
		void FUN_004A3640(void);
	private:
		char Data[0x18004];
};

extern Texture3D * DAT_008A0E04;
extern Texture3D * DAT_008A0E08;
extern Texture3D * DAT_008A0E0C;
extern Texture3DCache * DAT_008A0E10;
unsigned Get_Video_Hardware_Capabilities(void);
void Process_DD_Result(HRESULT result, int display_ok_msg);
void __cdecl DebugString(const char * string, ...);

unsigned short DSurface::HalfbrightMask = 0;
unsigned short DSurface::QuarterbrightMask = 0;
unsigned short DSurface::EighthbrightMask = 0;

// SYNTHETIC: GAMEMD 0x004BA3B0 SYMBOL
// _$E2
// SYNTHETIC: GAMEMD 0x004BA3E0 SYMBOL
// _$E5
// SYNTHETIC: GAMEMD 0x004BA400 SYMBOL
// _$E8
// SYNTHETIC: GAMEMD 0x004BA420 SYMBOL
// _$E11
// SYNTHETIC: GAMEMD 0x004BA440 SYMBOL
// _$E14
// SYNTHETIC: GAMEMD 0x004BA460 SYMBOL
// _$E17
// SYNTHETIC: GAMEMD 0x004BA480 SYMBOL
// _$E20
// SYNTHETIC: GAMEMD 0x004BA4C0 SYMBOL
// _$E23
// SYNTHETIC: GAMEMD 0x004BA4F0 SYMBOL
// _$E26
// SYNTHETIC: GAMEMD 0x004BA520 SYMBOL
// _$E29
// SYNTHETIC: GAMEMD 0x004BA550 SYMBOL
// _$E32
// SYNTHETIC: GAMEMD 0x004BA560 SYMBOL
// _$E35
// SYNTHETIC: GAMEMD 0x004BA580 SYMBOL
// _$E38

// FUNCTION: GAMEMD 0x004BA5A0
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

// FUNCTION: GAMEMD 0x004BA6B0
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

// FUNCTION: GAMEMD 0x004BA720
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

// FUNCTION: GAMEMD 0x004BA770
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

// FUNCTION: GAMEMD 0x004BAC60
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

// FUNCTION: GAMEMD 0x004BACF0
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

// FUNCTION: GAMEMD 0x004BAD30
int DSurface::ReleaseDC(HDC hdc)
{
	HRESULT hr;

	hr = SurfacePtr->ReleaseDC(hdc);

	if ((hr == DD_OK) && (LockCount > 0)) {
		LockCount--;
	}

	return(1);
}


// FUNCTION: GAMEMD 0x004BAD60
int DSurface::Bytes_Per_Pixel(void) const
{
	return(BytesPerPixel);
}

// FUNCTION: GAMEMD 0x004BAD70
int DSurface::Stride(void) const
{
	return(Description->lPitch);
}

// FUNCTION: GAMEMD 0x004BAD80
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

// FUNCTION: GAMEMD 0x004BAEC0
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

// FUNCTION: GAMEMD 0x004BAF20
bool DSurface::Can_Blit(void) const
{
	return(SurfacePtr->GetBltStatus(DDGBS_CANBLT) == DD_OK);
}

// FUNCTION: GAMEMD 0x004BAF40
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

// FUNCTION: GAMEMD 0x004BAFE0
bool DSurface::Is_Lost(void) const
{
	return(SurfacePtr != NULL && SurfacePtr->IsLost() == DDERR_SURFACELOST);
}

// FUNCTION: GAMEMD 0x004BB000
bool DSurface::Restore_Check(void) const
{
	return(Restore_Surface(this));
}

// FUNCTION: GAMEMD 0x004BB080
bool DSurface::Blit_From(Rect const & destrect, Surface const & ssource, Rect const & sourcerect, bool trans, bool unknown)
{
	return(Blit_From(Get_Rect(), destrect, ssource, ssource.Get_Rect(), sourcerect, trans, unknown));
}

// FUNCTION: GAMEMD 0x004BB0D0
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

// FUNCTION: GAMEMD 0x004BB5F0
bool DSurface::Fill_Rect(Rect const & fillrect, int color)
{
	return(DSurface::Fill_Rect(Get_Rect(), fillrect, color));
}

// FUNCTION: GAMEMD 0x004BB620
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

// FUNCTION: GAMEMD 0x004BB830
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

// FUNCTION: GAMEMD 0x004BBB00
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

// FUNCTION: GAMEMD 0x004BBC30
int DSurface::Get_Red_Right(void)
{
	return(RedRight);
}

// FUNCTION: GAMEMD 0x004BBC40
int DSurface::Get_Red_Left(void)
{
	return(RedLeft);
}

// FUNCTION: GAMEMD 0x004BBC50
int DSurface::Get_Green_Right(void)
{
	return(GreenRight);
}

// FUNCTION: GAMEMD 0x004BBC60
int DSurface::Get_Green_Left(void)
{
	return(GreenLeft);
}

// FUNCTION: GAMEMD 0x004BBC70
int DSurface::Get_Blue_Right(void)
{
	return(BlueRight);
}

// FUNCTION: GAMEMD 0x004BBC80
int DSurface::Get_Blue_Left(void)
{
	return(BlueLeft);
}

// FUNCTION: GAMEMD 0x004BBC90
int DSurface::Get_Primary_Color_Mode(void)
{
	return(PrimaryColorMode);
}

// FUNCTION: GAMEMD 0x004BBCA0
bool DSurface::Draw_Depth_Glow_Line(Rect const & cliprect, Point2D const & startpoint, Point2D const & endpoint, int glow_strength, int start_depth, int end_depth, bool write_depth)
{
	Point2D start;
	Point2D end;

	Rect rect = Intersect(cliprect, Get_Rect());
	if (rect.Width != 0 || rect.Height != 0) {

	start = Bias_To(startpoint, rect);
	end = Bias_To(endpoint, rect);
	int z2 = end_depth;
	int z1 = start_depth;

	if (start.X > end.X) {
		swap(start, end);
		z1 = end_depth;
		z2 = start_depth;
	}

	int px = start.X;
	int py = start.Y;
	Point2D origend = end;

	if (Clip_Line_To_Rect(start, end, rect)) {

	int zend = z2;
	int zstart = z1;

	if (end.X != origend.X || end.Y != origend.Y) {
		unsigned int amount = abs((int)((double)Distance(Point2D(px, py), end) / (double)Distance(Point2D(px, py), origend) * (double)(z1 - z2)));
		if (z1 < z2) {
			zend = z1 + amount;
		} else {
			zend = z1 - amount;
		}
	}

	if (start.X != px || start.Y != py) {
		Point2D clipped_point(start.X - origend.X, start.Y - origend.Y);
		int clipped = clipped_point.Length();
		Point2D full_point(px - origend.X, py - origend.Y);
		unsigned int amount = abs((int)((double)clipped / (double)full_point.Length() * (double)(z1 - z2)));
		if (z2 > z1) {
			zstart = z2 - amount;
		} else {
			zstart = z2 + amount;
		}
	}

	Bytes_Per_Pixel();
	void * buffer = Lock(start);

	if (DepthBuffer != NULL) {

	Point2D point = start;
	point.Y -= DepthBuffer->Get_Bounds().Y;
	unsigned short * zbuffer = (unsigned short *)DepthBuffer->Get_Buffer_Offset(point);

	int start_y = start.Y;
	int zwrap = -1;
	ZBuffer * depth_buffer = DepthBuffer;
	unsigned short z = (unsigned short)(zstart + (short)(depth_buffer->Get_Bounds().Y + depth_buffer->Get_Scroll()) - start_y - rect.Y);
	int zwidth = depth_buffer->Get_Buffer_Width();

	if (buffer != NULL) {

	int dx = end.X - start.X;
	int xcount = dx;
	int dy = end.Y - start.Y;
	int dz = zend - zstart;

	int pitch = Stride();
	if (dy < 0) {
		pitch = -pitch;
		zwidth = -zwidth;
		zwrap = 1;
	}

	int zstep = 1;
	int abs_dy = abs(dy);
	if (dz < 0) {
		zstep = -1;
	}

	int dy2 = 2 * abs_dy;
	int abs_dz = abs(dz);
	int dx2 = 2 * dx;
	int dz2 = 2 * abs_dz;

	if (abs_dz > dx && abs_dz > abs_dy) {

		int xdelta = dy2 - abs_dz;
		int ydelta = dx2 - abs_dz;
		if (abs_dz > 0) {
			int offset = 0;
			for (int i = abs_dz; i != 0; i--) {

				if ((unsigned short)z < *zbuffer) {
					RGBClass rgb = Deconstruct_Hicolor_Pixel(*(unsigned short *)((char *)buffer + offset));
					int red = rgb.Get_Red();
					int green = rgb.Get_Green();
					red += (glow_strength * red) >> 8;
					green += (glow_strength * green) >> 8;
					int blue = rgb.Get_Blue();
					blue += (glow_strength * blue) >> 8;
					*(unsigned short *)((char *)buffer + offset) = (unsigned short)Build_Hicolor_Pixel((red > 255) ? 255 : red, (green > 255) ? 255 : green, (blue > 255) ? 255 : blue);
					if (write_depth) {
						*zbuffer = (unsigned short)z;
					}
				}

				if (xdelta > 0) {
					buffer = (unsigned char *)buffer + pitch;
					xdelta -= dz2;
					zbuffer += zwidth;
					if (zwidth > 0) {
						zbuffer = Blit_Wrap_Z_Buffer(zbuffer);
					} else {
						zbuffer = (unsigned short *)DepthBuffer->Wrap_Underflow((unsigned int)zbuffer);
					}
					z += zwrap;
				}

				if (ydelta > 0) {
					zbuffer++;
					offset += 2;
					zbuffer = Blit_Wrap_Z_Buffer(zbuffer);
					ydelta -= dz2;
				}

				z += zstep;
				ydelta += dx2;
				xdelta += dy2;
			}
		}

	} else if (dx > abs_dy) {

		int offset = 0;
		int ydelta = dy2 - dx;
		int zdelta = dz2 - dx;
		if (dx > 0) {
			do {

				if ((unsigned short)z < *zbuffer) {
					RGBClass rgb = Deconstruct_Hicolor_Pixel(*((unsigned short *)buffer + offset));
					int red = rgb.Get_Red();
					int green = rgb.Get_Green();
					red += (glow_strength * red) >> 8;
					green += (glow_strength * green) >> 8;
					int blue = rgb.Get_Blue();
					blue += (glow_strength * blue) >> 8;
					*((unsigned short *)buffer + offset) = (unsigned short)Build_Hicolor_Pixel((red > 255) ? 255 : red, (green > 255) ? 255 : green, (blue > 255) ? 255 : blue);
					if (write_depth) {
						*zbuffer = (unsigned short)z;
					}
				}

				if (ydelta > 0) {
					buffer = (unsigned char *)buffer + pitch;
					ydelta -= dx2;
					zbuffer += zwidth;
					if (zwidth > 0) {
						zbuffer = Blit_Wrap_Z_Buffer(zbuffer);
					} else {
						zbuffer = (unsigned short *)DepthBuffer->Wrap_Underflow((unsigned int)zbuffer);
					}
					z += zwrap;
				}

				if (zdelta > 0) {
					z += zstep;
					zdelta -= dx2;
				}

				zbuffer++;
				ydelta += dy2;
				zdelta += dz2;
				zbuffer = Blit_Wrap_Z_Buffer(zbuffer);

			} while (++offset < xcount);
		}
	} else {

		int xdelta = dx2 - abs_dy;
		int zdelta = dz2 - abs_dy;
		if (abs_dy > 0) {
			int offset = 0;
			for (int i = abs_dy; i != 0; i--) {

				if ((unsigned short)z < *zbuffer) {
					RGBClass rgb = Deconstruct_Hicolor_Pixel(*(unsigned short *)((char *)buffer + offset));
					int red = rgb.Get_Red();
					int green = rgb.Get_Green();
					red += (glow_strength * red) >> 8;
					green += (glow_strength * green) >> 8;
					int blue = rgb.Get_Blue();
					blue += (glow_strength * blue) >> 8;
					*(unsigned short *)((char *)buffer + offset) = (unsigned short)Build_Hicolor_Pixel((red > 255) ? 255 : red, (green > 255) ? 255 : green, (blue > 255) ? 255 : blue);
					if (write_depth) {
						*zbuffer = (unsigned short)z;
					}
				}

				if (xdelta > 0) {
					zbuffer++;
					offset += 2;
					zbuffer = Blit_Wrap_Z_Buffer(zbuffer);
					xdelta -= dy2;
				}

				if (zdelta > 0) {
					z += zstep;
					zdelta -= dy2;
				}

				xdelta += dx2;
				zdelta += dz2;
				buffer = (unsigned char *)buffer + pitch;
				zbuffer += zwidth;
				if (zwidth > 0) {
					zbuffer = Blit_Wrap_Z_Buffer(zbuffer);
				} else {
					zbuffer = (unsigned short *)DepthBuffer->Wrap_Underflow((unsigned int)zbuffer);
				}
				z += zwrap;
			}
		}

	}

	Unlock();
	return(true);

	}
	}
	}
	}

	return(false);
}

int __fastcall Brighten_Hicolor_Pixel(RGBClass const * rgb, int red_add, int green_add, int blue_add);

// FUNCTION: GAMEMD 0x004BDF00
bool DSurface::DrawRGBMultiplyingLine(Rect const & cliprect, Point2D const & startpoint, Point2D const & endpoint, RGBClass const & color, float intensity, int start_depth, int end_depth)
{
	int r = (int)(color.Get_Red() * intensity);
	int g = (int)(color.Get_Green() * intensity);
	int b = (int)(color.Get_Blue() * intensity);

	Point2D start;
	Point2D end;

	if (r >= 8 || g >= 8 || b >= 8) {

	Rect clipped_rect = Intersect(cliprect, Get_Rect());
	if (clipped_rect.Width != 0 || clipped_rect.Height != 0) {

	start = Bias_To(startpoint, clipped_rect);
	end = Bias_To(endpoint, clipped_rect);

	int z2 = end_depth;
	int z1 = start_depth;
	if (start.X > end.X) {
		swap(start, end);
		z1 = end_depth;
		z2 = start_depth;
	}

	Point2D unclipped = end;
	int px = start.X;
	int py = start.Y;

	if (Clip_Line_To_Rect(start, end, clipped_rect)) {

	int zend = z2;
	int zstart = z1;

	if (end.X != unclipped.X || end.Y != unclipped.Y) {
		Point2D clipped_point;
		clipped_point.X = px - end.X;
		double clipped_dx = (double)clipped_point.X;
		clipped_point.Y = py - end.Y;
		int clipped = (int)(int)fastmath::sqrt((double)clipped_point.Y * (double)clipped_point.Y + clipped_dx * clipped_dx);
		Point2D full_point;
		full_point.X = px - unclipped.X;
		double full_dx = (double)full_point.X;
		full_point.Y = py - unclipped.Y;
		int amount = abs((int)(int)((double)clipped
			/ (double)(int)(int)fastmath::sqrt((double)full_point.Y * (double)full_point.Y + full_dx * full_dx)
			* (double)(z1 - z2)));
		if (z1 < z2) {
			zend = z1 + amount;
		} else {
			zend = z1 - amount;
		}
	}

	if (start.X != px || start.Y != py) {
		Point2D clipped_point;
		clipped_point.X = start.X - unclipped.X;
		double clipped_dx = (double)clipped_point.X;
		clipped_point.Y = start.Y - unclipped.Y;
		int clipped = (int)(int)fastmath::sqrt((double)clipped_point.Y * (double)clipped_point.Y + clipped_dx * clipped_dx);
		Point2D full_point;
		full_point.X = px - unclipped.X;
		double full_dx = (double)full_point.X;
		full_point.Y = py - unclipped.Y;
		int amount = abs((int)(int)((double)clipped
			/ (double)(int)(int)fastmath::sqrt((double)full_point.Y * (double)full_point.Y + full_dx * full_dx)
			* (double)(z1 - z2)));
		if (z2 > z1) {
			zstart = z2 - amount;
		} else {
			zstart = z2 + amount;
		}
	}

	Bytes_Per_Pixel();
	void * buffer = Lock(start);

	if (DepthBuffer != NULL) {

	Point2D point = start;
	point.Y -= DepthBuffer->Get_Bounds().Y;
	unsigned short * zbuffer = (unsigned short *)DepthBuffer->Get_Buffer_Offset(point);
	point = start;
	point.Y -= AlphaBuffer->Get_Bounds().Y;
	unsigned short * abuffer = (unsigned short *)AlphaBuffer->Get_Buffer_Offset(point);

	int start_y = start.Y;
	int zwrap = -1;
	ZBuffer * depth_buffer = DepthBuffer;
	unsigned short z = (unsigned short)(zstart + (short)(depth_buffer->Get_Bounds().Y + depth_buffer->Get_Scroll()) - start_y - clipped_rect.Y);
	int zwidth = depth_buffer->Get_Buffer_Width();

	if (buffer != NULL) {

	int dx = end.X - start.X;
	int dy = end.Y - start.Y;
	int dz = zend - zstart;

	int pitch = Stride();
	if (dy < 0) {
		pitch = -pitch;
		zwidth = -zwidth;
		zwrap = 1;
	}

	int zstep = 1;
	int abs_dy = abs(dy);
	if (dz < 0) {
		zstep = -1;
	}

	RGBClass rgb;
	int abs_dz = abs(dz);
	int dx2 = 2 * dx;
	int dy2 = 2 * abs_dy;
	int dz2 = 2 * abs_dz;

	if (abs_dz > dx && abs_dz > abs_dy) {

		int xdelta = dy2 - abs_dz;
		int ydelta = dx2 - abs_dz;
		if (abs_dz > 0) {
			int offset = 0;
			for (int i = abs_dz; i != 0; i--) {

				int alpha = *abuffer;
				if (z < *zbuffer) {
					rgb = Deconstruct_Hicolor_Pixel(*(unsigned short *)((char *)buffer + offset));
					*(unsigned short *)((char *)buffer + offset) = (unsigned short)Brighten_Hicolor_Pixel(&rgb, (alpha * r) >> 7, (alpha * g) >> 7, (alpha * b) >> 7);
				}

				if (xdelta > 0) {
					buffer = (unsigned char *)buffer + pitch;
					xdelta -= dz2;
					zbuffer += zwidth;
					if (zwidth > 0) {
						zbuffer = (unsigned short *)DepthBuffer->Wrap_Overflow((unsigned int)zbuffer);
					} else {
						zbuffer = (unsigned short *)DepthBuffer->Wrap_Underflow((unsigned int)zbuffer);
					}
					abuffer += zwidth;
					if (zwidth > 0) {
						abuffer = (unsigned short *)AlphaBuffer->Wrap_Overflow((unsigned int)abuffer);
					} else {
						abuffer = (unsigned short *)AlphaBuffer->Wrap_Underflow((unsigned int)abuffer);
					}
					z += zwrap;
				}

				if (ydelta > 0) {
					zbuffer++;
					offset += 2;
					zbuffer = (unsigned short *)DepthBuffer->Wrap_Overflow((unsigned int)zbuffer);
					ydelta -= dz2;
				}

				z += zstep;
				ydelta += dx2;
				xdelta += dy2;
			}
		}
	} else if (dx > abs_dy) {

		int ydelta = dy2 - dx;
		int zdelta = dz2 - dx;
		int i = 0;
		if (dx > 0) {
			for (; i < dx; i++) {

				int alpha = *abuffer;
				if (z < *zbuffer) {
					rgb = Deconstruct_Hicolor_Pixel(*((unsigned short *)buffer + i));
					*((unsigned short *)buffer + i) = (unsigned short)Brighten_Hicolor_Pixel(&rgb, (alpha * r) >> 7, (alpha * g) >> 7, (alpha * b) >> 7);
				}

				if (ydelta > 0) {
					buffer = (unsigned char *)buffer + pitch;
					ydelta -= 2 * dx;
					zbuffer += zwidth;
					if (zwidth > 0) {
						zbuffer = (unsigned short *)DepthBuffer->Wrap_Overflow((unsigned int)zbuffer);
					} else {
						zbuffer = (unsigned short *)DepthBuffer->Wrap_Underflow((unsigned int)zbuffer);
					}
					abuffer += zwidth;
					if (zwidth > 0) {
						abuffer = (unsigned short *)AlphaBuffer->Wrap_Overflow((unsigned int)abuffer);
					} else {
						abuffer = (unsigned short *)AlphaBuffer->Wrap_Underflow((unsigned int)abuffer);
					}
					z += zwrap;
				}

				if (zdelta > 0) {
					z += zstep;
					zdelta -= 2 * dx;
				}

				zbuffer++;
				ydelta += dy2;
				zdelta += dz2;
				zbuffer = (unsigned short *)DepthBuffer->Wrap_Overflow((unsigned int)zbuffer);
			}
		}
	} else {

		int xdelta = dx2 - abs_dy;
		int zdelta = 2 * abs_dz - abs_dy;
		if (abs_dy > 0) {
			int offset = 0;
			for (int i = abs_dy; i != 0; i--) {

				int alpha = *abuffer;
				if (z < *zbuffer) {
					rgb = Deconstruct_Hicolor_Pixel(*(unsigned short *)((char *)buffer + offset));
					*(unsigned short *)((char *)buffer + offset) = (unsigned short)Brighten_Hicolor_Pixel(&rgb, (alpha * r) >> 7, (alpha * g) >> 7, (alpha * b) >> 7);
				}

				if (xdelta > 0) {
					zbuffer++;
					offset += 2;
					zbuffer = (unsigned short *)DepthBuffer->Wrap_Overflow((unsigned int)zbuffer);
					xdelta -= dy2;
				}

				if (zdelta > 0) {
					z += zstep;
					zdelta -= dy2;
				}

				xdelta += dx2;
				zdelta += dz2;
				buffer = (unsigned char *)buffer + pitch;

				zbuffer += zwidth;
				if (zwidth > 0) {
					zbuffer = (unsigned short *)DepthBuffer->Wrap_Overflow((unsigned int)zbuffer);
				} else {
					zbuffer = (unsigned short *)DepthBuffer->Wrap_Underflow((unsigned int)zbuffer);
				}
				abuffer += zwidth;
				if (zwidth > 0) {
					abuffer = (unsigned short *)AlphaBuffer->Wrap_Overflow((unsigned int)abuffer);
				} else {
					abuffer = (unsigned short *)AlphaBuffer->Wrap_Underflow((unsigned int)abuffer);
				}
				z += zwrap;

			}
		}
	}

	Unlock();
	return(true);

	}
	}
	}
	}
	}

	return(false);
}

// FUNCTION: GAMEMD 0x004BE9D0
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

// FUNCTION: GAMEMD 0x004BF650
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

// FUNCTION: GAMEMD 0x004BF750
bool DSurface::Draw_Ping_Pong_Gradient_Line(Rect const & cliprect, Point2D const & startpoint, Point2D const & endpoint, RGBClass const & start_color, RGBClass const & end_color, float & gradient_step, float & gradient_position) const
{
	Rect clipped_rect = Intersect(cliprect, Get_Rect());

	Point2D start = Bias_To(startpoint, clipped_rect);
	Point2D end = Bias_To(endpoint, clipped_rect);

	if (!Clip_Line_To_Rect(start, end, clipped_rect)) return(false);

	if (start.X > end.X) {
		swap(start, end);
	}

	if (Bytes_Per_Pixel() == 2) {

		void * buffer = Lock(start);
		if (buffer != NULL) {

			if (start.Y == end.Y) {
				for (int i = 0; i <= end.X - start.X; i++) {
					RGBClass gradient_color = RGBClass(NoInitClass()).Lerp(start_color, end_color, gradient_position);

					*((unsigned short *)buffer + i) = (unsigned short)DSurface::Build_Hicolor_Pixel(gradient_color);

					gradient_position += gradient_step;
					if (gradient_position < 0.0f && gradient_step < 0.0f) {
						gradient_position = 0.0f;
						gradient_step = -gradient_step;
					} else if (gradient_position > 1.0f && gradient_step > 0.0f) {
						gradient_position = 1.0f;
						gradient_step = -gradient_step;
					}
				}

			} else if (start.X == end.X) {
				int pitch = start.Y > end.Y ? -Stride() : Stride();

				int dy = abs(end.Y - start.Y);
				for (int i = 0; i <= dy; i++) {
					RGBClass gradient_color = RGBClass(NoInitClass()).Lerp(start_color, end_color, gradient_position);

					*(unsigned short *)buffer = (unsigned short)DSurface::Build_Hicolor_Pixel(gradient_color);

					gradient_position += gradient_step;
					if (gradient_position < 0.0f && gradient_step < 0.0f) {
						gradient_position = 0.0f;
						gradient_step = -gradient_step;
					} else if (gradient_position > 1.0f && gradient_step > 0.0f) {
						gradient_position = 1.0f;
						gradient_step = -gradient_step;
					}

					buffer = (unsigned char *)buffer + pitch;
				}
			} else {
				int dx = end.X - start.X;
				int dy = end.Y - start.Y;
				int pitch = Stride();
				if (dy < 0) {
					pitch = -pitch;
				}

				dy = abs(dy);
				int dx2 = 2 * dx;
				int dy2 = 2 * dy;

				if (dx > dy) {
					int delta = dy2 - dx;

					for (int i = 0; i <= dx; i++) {
						RGBClass gradient_color = RGBClass(NoInitClass()).Lerp(start_color, end_color, gradient_position);

						*((unsigned short *)buffer + i) = (unsigned short)DSurface::Build_Hicolor_Pixel(gradient_color);

						gradient_position += gradient_step;
						if (gradient_position < 0.0f && gradient_step < 0.0f) {
							gradient_position = 0.0f;
							gradient_step = -gradient_step;
						} else if (gradient_position > 1.0f && gradient_step > 0.0f) {
							gradient_position = 1.0f;
							gradient_step = -gradient_step;
						}

						if (delta > 0) {
							buffer = (unsigned char *)buffer + pitch;
							delta -= dx2;
						}

						delta += dy2;

					}
				} else {
					int delta = dx2 - dy;
					int k = 0;

					for (int i = 0; i <= dy; i++) {
						RGBClass gradient_color = RGBClass(NoInitClass()).Lerp(start_color, end_color, gradient_position);

						*((unsigned short *)buffer + k) = (unsigned short)DSurface::Build_Hicolor_Pixel(gradient_color);

						gradient_position += gradient_step;
						if (gradient_position < 0.0f && gradient_step < 0.0f) {
							gradient_position = 0.0f;
							gradient_step = -gradient_step;
						} else if (gradient_position > 1.0f && gradient_step > 0.0f) {
							gradient_position = 1.0f;
							gradient_step = -gradient_step;
						}

						if (delta > 0) {
							k++;
							delta -= dy2;
						}

						delta += dx2;
						buffer = (unsigned char *)buffer + pitch;
					}
				}
			}

			Unlock();
			return(true);
		}
	}
	return(false);
}

// FUNCTION: GAMEMD 0x004BFD30
bool DSurface::Draw_Depth_Shaded_Line(Rect const & cliprect, Point2D const & startpoint, Point2D const & endpoint, unsigned color, int start_depth, int end_depth, bool write_depth)
{
	Rect clipped_rect = Intersect(cliprect, Get_Rect());
	if (clipped_rect.Width == 0 && clipped_rect.Height == 0) {
		return(false);
	}

	Point2D start = Bias_To(startpoint, clipped_rect);
	Point2D end = Bias_To(endpoint, clipped_rect);

	int zfirst = start_depth;
	int zlast = end_depth;
	if (start.X > end.X) {
		swap(start, end);
		zfirst = end_depth;
		zlast = start_depth;
	}

	Point2D unclipped = end;
	int px = start.X;
	int py = start.Y;

	if (!Clip_Line_To_Rect(start, end, clipped_rect)) {
		return(false);
	}

	int zend = zlast;
	int zstart = zfirst;

	if (end.X != unclipped.X || end.Y != unclipped.Y) {
		Point2D clipped_point;
		clipped_point.X = px - end.X;
		double clipped_dx = (double)clipped_point.X;
		clipped_point.Y = py - end.Y;
		int clipped = (int)(int)fastmath::sqrt((double)clipped_point.Y * (double)clipped_point.Y + clipped_dx * clipped_dx);
		Point2D full_point;
		full_point.X = px - unclipped.X;
		double full_dx = (double)full_point.X;
		full_point.Y = py - unclipped.Y;
		int amount = abs((int)(int)((double)clipped
			/ (double)(int)(int)fastmath::sqrt((double)full_point.Y * (double)full_point.Y + full_dx * full_dx)
			* (double)(zfirst - zlast)));
		if (zfirst < zlast) {
			zend = zfirst + amount;
		} else {
			zend = zfirst - amount;
		}
	}

	if (start.X != px || start.Y != py) {
		Point2D clipped_point;
		clipped_point.X = start.X - unclipped.X;
		double clipped_dx = (double)clipped_point.X;
		clipped_point.Y = start.Y - unclipped.Y;
		int clipped = (int)(int)fastmath::sqrt((double)clipped_point.Y * (double)clipped_point.Y + clipped_dx * clipped_dx);
		Point2D full_point;
		full_point.X = px - unclipped.X;
		double full_dx = (double)full_point.X;
		full_point.Y = py - unclipped.Y;
		int amount = abs((int)(int)((double)clipped
			/ (double)(int)(int)fastmath::sqrt((double)full_point.Y * (double)full_point.Y + full_dx * full_dx)
			* (double)(zfirst - zlast)));
		if (zlast > zfirst) {
			zstart = zlast - amount;
		} else {
			zstart = zlast + amount;
		}
	}

	Bytes_Per_Pixel();
	void * buffer = Lock(start);

	if (DepthBuffer == NULL) {
		return(false);
	}

	Point2D point = start;
	point.Y -= DepthBuffer->Get_Bounds().Y;
	unsigned short * zbuffer = (unsigned short *)DepthBuffer->Get_Buffer_Offset(point);
	point = start;
	point.Y -= AlphaBuffer->Get_Bounds().Y;
	unsigned short * abuffer = (unsigned short *)AlphaBuffer->Get_Buffer_Offset(point);

	int start_y = start.Y;
	int zwrap = -1;
	unsigned short z = (unsigned short)(zstart + (short)DepthBuffer->Get_Scroll_Delta(start_y + clipped_rect.Y - DepthBuffer->Get_Bounds().Y));
	int zwidth = DepthBuffer->Get_Buffer_Width();

	int red;
	int green;
	int blue;

	if (buffer != NULL) {
		/// Decompose the source color into its gun components so each plotted pixel
		/// can be darkened by the alpha-buffer brightness value.
		RGBClass rgb(
			(unsigned char)(((unsigned short)color >> RedRight) << RedLeft),
			(unsigned char)(((unsigned short)color >> GreenRight) << GreenLeft),
			(unsigned char)(((unsigned short)color >> BlueRight) << BlueLeft));
		red = rgb.Get_Red();
		green = rgb.Get_Green();
		blue = rgb.Get_Blue();
	} else {
		return(false);
	}

	int dy = end.Y - start.Y;
	int dx = end.X - start.X;
	int dz = zend - zstart;

	int pitch = Stride();
	if (dy < 0) {
		pitch = -pitch;
		zwidth = -zwidth;
		zwrap = 1;
	}

	int zstep = 1;
	int abs_dy = abs(dy);
	if (dz < 0) {
		zstep = -1;
	}

	int abs_dz = abs(dz);
	int dx2 = 2 * dx;
	int dy2 = 2 * abs_dy;
	int dz2 = 2 * abs_dz;

	if (abs_dz > dx && abs_dz > abs_dy) {

		int xdelta = dy2 - abs_dz;
		int ydelta = dx2 - abs_dz;
		if (abs_dz > 0) {
			int offset = 0;
			for (int i = abs_dz; i != 0; i--) {

				int v = *abuffer;
				if (z < *zbuffer && v != 0) {
					if (v != 127) {
						*(unsigned short *)((char *)buffer + offset) = (unsigned short)((((red * v) >> 7 >> RedLeft) << RedRight) | (((green * v) >> 7 >> GreenLeft) << GreenRight) | (((blue * v) >> 7 >> BlueLeft) << BlueRight));
					} else {
						*(unsigned short *)((char *)buffer + offset) = (unsigned short)color;
					}
					if (write_depth) {
						*zbuffer = z;
					}
				}

				if (xdelta > 0) {
					buffer = (unsigned char *)buffer + pitch;
					xdelta -= dz2;
					zbuffer += zwidth;
					if (zwidth > 0) {
						zbuffer = (unsigned short *)DepthBuffer->Wrap_Overflow((unsigned int)zbuffer);
					} else {
						zbuffer = (unsigned short *)DepthBuffer->Wrap_Underflow((unsigned int)zbuffer);
					}
					abuffer += zwidth;
					if (zwidth > 0) {
						abuffer = (unsigned short *)AlphaBuffer->Wrap_Overflow((unsigned int)abuffer);
					} else {
						abuffer = (unsigned short *)AlphaBuffer->Wrap_Underflow((unsigned int)abuffer);
					}
					z += zwrap;
				}

				if (ydelta > 0) {
					zbuffer++;
					offset += 2;
					zbuffer = (unsigned short *)DepthBuffer->Wrap_Overflow((unsigned int)zbuffer);
					abuffer++;
					abuffer = (unsigned short *)AlphaBuffer->Wrap_Overflow((unsigned int)abuffer);
					ydelta -= dz2;
				}

				z += zstep;
				ydelta += dx2;
				xdelta += dy2;
			}
		}
	} else if (dx > abs_dy) {

		int ydelta = dy2 - dx;
		int i = 0;
		int zdelta = dz2 - dx;
		if (dx > 0) {
			for (; i < dx; i++) {

				int v = *abuffer;
				if (z < *zbuffer && v != 0) {
					if (v != 127) {
						*(unsigned short *)((char *)buffer + 2 * i) = (unsigned short)((((red * v) >> 7 >> RedLeft) << RedRight) | (((green * v) >> 7 >> GreenLeft) << GreenRight) | (((blue * v) >> 7 >> BlueLeft) << BlueRight));
					} else {
						*(unsigned short *)((char *)buffer + 2 * i) = (unsigned short)color;
					}
					if (write_depth) {
						*zbuffer = z;
					}
				}

				if (ydelta > 0) {
					buffer = (unsigned char *)buffer + pitch;
					ydelta -= 2 * dx;
					zbuffer += zwidth;
					if (zwidth > 0) {
						zbuffer = (unsigned short *)DepthBuffer->Wrap_Overflow((unsigned int)zbuffer);
					} else {
						zbuffer = (unsigned short *)DepthBuffer->Wrap_Underflow((unsigned int)zbuffer);
					}
					z += zwrap;
					abuffer += zwidth;
					if (zwidth > 0) {
						abuffer = (unsigned short *)AlphaBuffer->Wrap_Overflow((unsigned int)abuffer);
					} else {
						abuffer = (unsigned short *)AlphaBuffer->Wrap_Underflow((unsigned int)abuffer);
					}
				}

				if (zdelta > 0) {
					z += zstep;
					zdelta -= 2 * dx;
				}

				zbuffer++;
				ydelta += dy2;
				zdelta += dz2;
				zbuffer = (unsigned short *)DepthBuffer->Wrap_Overflow((unsigned int)zbuffer);
				abuffer++;
				abuffer = (unsigned short *)AlphaBuffer->Wrap_Overflow((unsigned int)abuffer);
			}
		}
	} else {

		int xdelta = dx2 - abs_dy;
		int zdelta = 2 * abs_dz - abs_dy;
		if (abs_dy > 0) {
			int i = abs_dy;
			int offset = 0;
			do {

				int v = *abuffer;
				if (z < *zbuffer && v != 0) {
					if (v != 127) {
						*(unsigned short *)((char *)buffer + offset) = (unsigned short)((((red * v) >> 7 >> RedLeft) << RedRight) | (((green * v) >> 7 >> GreenLeft) << GreenRight) | (((blue * v) >> 7 >> BlueLeft) << BlueRight));
					} else {
						*(unsigned short *)((char *)buffer + offset) = (unsigned short)color;
					}
					if (write_depth) {
						*zbuffer = z;
					}
				}

				if (xdelta > 0) {
					zbuffer++;
					offset += 2;
					zbuffer = (unsigned short *)DepthBuffer->Wrap_Overflow((unsigned int)zbuffer);
					abuffer++;
					abuffer = (unsigned short *)AlphaBuffer->Wrap_Overflow((unsigned int)abuffer);
					xdelta -= dy2;
				}

				if (zdelta > 0) {
					z += zstep;
					zdelta -= dy2;
				}

				xdelta += dx2;
				zdelta += dz2;
				buffer = (unsigned char *)buffer + pitch;

				zbuffer += zwidth;
				if (zwidth > 0) {
					zbuffer = (unsigned short *)DepthBuffer->Wrap_Overflow((unsigned int)zbuffer);
				} else {
					zbuffer = (unsigned short *)DepthBuffer->Wrap_Underflow((unsigned int)zbuffer);
				}
				z += zwrap;
				abuffer += zwidth;
				if (zwidth > 0) {
					abuffer = (unsigned short *)AlphaBuffer->Wrap_Overflow((unsigned int)abuffer);
				} else {
					abuffer = (unsigned short *)AlphaBuffer->Wrap_Underflow((unsigned int)abuffer);
				}

				i--;
			} while (i != 0);
		}
	}

	Unlock();
	return(true);
}

// FUNCTION: GAMEMD 0x004C0750
int DSurface::Draw_Masked_Dashed_Line(Point2D const & startpoint, Point2D const & endpoint, unsigned color, bool pattern[], int offset, bool draw_on_zero_alpha)
{
	Point2D start = startpoint;
	Point2D end = endpoint;
	int pattern_step = 1;

	if (Bytes_Per_Pixel() != 2) return(0);

	if (start.X > end.X) {
		swap(start, end);
		int dx = abs(start.X - end.X);
		int dy = abs(start.Y - end.Y) + 1;
		if (dx < dy) {
			dx = dy;
		}
		offset = (offset + dx) % 16;
		pattern_step = -1;
	}

	Point2D point = start;
	point.Y -= AlphaBuffer->Get_Bounds().Y;
	unsigned short * abuffer = (unsigned short *)AlphaBuffer->Get_Buffer_Offset(point);
	int astride = AlphaBuffer->Get_Buffer_Width();
	if (start.Y > end.Y) astride = -astride;

	void * buffer = (unsigned short *)Lock(start);
	if (buffer != NULL) {
		if (draw_on_zero_alpha) {

			if (start.Y == end.Y) {

				for (int i = 0; i <= end.X - start.X; i++, offset += pattern_step) {
					if (offset < 0) {
						offset += 16;
					}
					if (offset >= 16) {
						offset -= 16;
					}

					if (pattern[offset] && *abuffer == 0) {
						*((unsigned short *)buffer + i) = color;
					}

					if (astride > 0) {
						abuffer++;
						abuffer = (unsigned short *)AlphaBuffer->Wrap_Overflow((unsigned int)abuffer);
					} else {
						abuffer++;
						abuffer = (unsigned short *)AlphaBuffer->Wrap_Underflow((unsigned int)abuffer);
					}
				}
			} else if (start.X == end.X) {
				int pitch = start.Y > end.Y ? -Stride() : Stride();

				int dy = abs(end.Y - start.Y);
				for (int i = 0; i <= dy; i++, offset += pattern_step) {
					if (offset < 0) {
						offset += 16;
					}
					if (offset >= 16) {
						offset -= 16;
					}

					if (pattern[offset] && *abuffer == 0) {
						*(unsigned short *)buffer = color;
					}

					buffer = (unsigned char *)buffer + pitch;

					if (astride > 0) {
						abuffer += astride;
						abuffer = (unsigned short *)AlphaBuffer->Wrap_Overflow((unsigned int)abuffer);
					} else {
						abuffer += astride;
						abuffer = (unsigned short *)AlphaBuffer->Wrap_Underflow((unsigned int)abuffer);
					}
				}
			} else {
				int dx = end.X - start.X;
				int dy = end.Y - start.Y;
				int pitch = Stride();
				if (dy < 0) {
					pitch = -pitch;
				}

				dy = abs(dy);
				int dx2 = 2 * dx;
				int dy2 = 2 * dy;

				if (dx > dy) {

					int delta = dy2 - dx;

					for (int i = 0; i < dx; i++, offset += pattern_step) {
						if (offset < 0) {
							offset += 16;
						}
						if (offset >= 16) {
							offset -= 16;
						}

						if (pattern[offset] && *abuffer == 0) {
							*((unsigned short *)buffer + i) = color;
						}

						if (delta > 0) {
							buffer = (unsigned char *)buffer + pitch;
							abuffer += astride;
							abuffer = (unsigned short *)AlphaBuffer->Wrap_Overflow((unsigned int)abuffer);
							delta -= dx2;
						}

						delta += dy2;

						if (astride > 0) {
							abuffer++;
							abuffer = (unsigned short *)AlphaBuffer->Wrap_Overflow((unsigned int)abuffer);
						} else {
							abuffer++;
							abuffer = (unsigned short *)AlphaBuffer->Wrap_Underflow((unsigned int)abuffer);
						}
					}
				} else {
					int delta = dx2 - dy;
					int k = 0;

					for (int i = 0; i < dy; i++, offset += pattern_step) {
						if (offset < 0) {
							offset += 16;
						}
						if (offset >= 16) {
							offset -= 16;
						}

						if (pattern[offset] && *abuffer == 0) {
							*((unsigned short *)buffer + k) = color;
						}

						if (delta > 0) {
							k++;
							abuffer++;
							abuffer = (unsigned short *)AlphaBuffer->Wrap_Overflow((unsigned int)abuffer);
							delta -= dy2;
						}

						delta += dx2;
						buffer = (unsigned char *)buffer + pitch;

						if (astride > 0) {
							abuffer += astride;
							abuffer = (unsigned short *)AlphaBuffer->Wrap_Overflow((unsigned int)abuffer);
						} else {
							abuffer += astride;
							abuffer = (unsigned short *)AlphaBuffer->Wrap_Underflow((unsigned int)abuffer);
						}
					}
				}
			}
		} else {

			if (start.Y == end.Y) {

				for (int i = 0; i <= end.X - start.X; i++, offset += pattern_step) {
					if (offset < 0) {
						offset += 16;
					}
					if (offset >= 16) {
						offset -= 16;
					}

					if (pattern[offset] && *abuffer != 0) {
						*((unsigned short *)buffer + i) = color;
					}

					if (astride > 0) {
						abuffer++;
						abuffer = (unsigned short *)AlphaBuffer->Wrap_Overflow((unsigned int)abuffer);
					} else {
						abuffer++;
						abuffer = (unsigned short *)AlphaBuffer->Wrap_Underflow((unsigned int)abuffer);
					}
				}
			} else if (start.X == end.X) {
				int pitch = start.Y > end.Y ? -Stride() : Stride();

				int dy = abs(end.Y - start.Y);
				for (int i = 0; i <= dy; i++, offset += pattern_step) {
					if (offset < 0) {
						offset += 16;
					}
					if (offset >= 16) {
						offset -= 16;
					}

					if (pattern[offset] && *abuffer != 0) {
						*(unsigned short *)buffer = color;
					}

					buffer = (unsigned char *)buffer + pitch;

					if (astride > 0) {
						abuffer += astride;
						abuffer = (unsigned short *)AlphaBuffer->Wrap_Overflow((unsigned int)abuffer);
					} else {
						abuffer += astride;
						abuffer = (unsigned short *)AlphaBuffer->Wrap_Underflow((unsigned int)abuffer);
					}
				}
			} else {
				int dx = end.X - start.X;
				int dy = end.Y - start.Y;
				int pitch = Stride();
				if (dy < 0) {
					pitch = -pitch;
				}

				dy = abs(dy);
				int dx2 = 2 * dx;
				int dy2 = 2 * dy;

				if (dx > dy) {

					int delta = dy2 - dx;

					for (int i = 0; i < dx; i++, offset += pattern_step) {
						if (offset < 0) {
							offset += 16;
						}
						if (offset >= 16) {
							offset -= 16;
						}

						if (pattern[offset] && *abuffer != 0) {
							*((unsigned short *)buffer + i) = color;
						}

						if (delta > 0) {
							buffer = (unsigned char *)buffer + pitch;
							abuffer += astride;
							abuffer = (unsigned short *)AlphaBuffer->Wrap_Overflow((unsigned int)abuffer);
							delta -= dx2;
						}

						delta += dy2;

						if (astride > 0) {
							abuffer++;
							abuffer = (unsigned short *)AlphaBuffer->Wrap_Overflow((unsigned int)abuffer);
						} else {
							abuffer++;
							abuffer = (unsigned short *)AlphaBuffer->Wrap_Underflow((unsigned int)abuffer);
						}
					}
				} else {
					int delta = dx2 - dy;
					int k = 0;

					for (int i = 0; i < dy; i++, offset += pattern_step) {
						if (offset < 0) {
							offset += 16;
						}
						if (offset >= 16) {
							offset -= 16;
						}

						if (pattern[offset] && *abuffer != 0) {
							*((unsigned short *)buffer + k) = color;
						}

						if (delta > 0) {
							k++;
							abuffer++;
							abuffer = (unsigned short *)AlphaBuffer->Wrap_Overflow((unsigned int)abuffer);
							delta -= dy2;
						}

						delta += dx2;
						buffer = (unsigned char *)buffer + pitch;

						if (astride > 0) {
							abuffer += astride;
							abuffer = (unsigned short *)AlphaBuffer->Wrap_Overflow((unsigned int)abuffer);
						} else {
							abuffer += astride;
							abuffer = (unsigned short *)AlphaBuffer->Wrap_Underflow((unsigned int)abuffer);
						}
					}
				}
			}
		}

		Unlock();
		return(offset);
	}

	return(offset);
}

// FUNCTION: GAMEMD 0x004C0E30
bool DSurface::Draw_Masked_Line(Point2D const & startpoint, Point2D const & endpoint, unsigned color, bool draw_on_zero_alpha)
{
	if (Bytes_Per_Pixel() == 2) {
		Rect cliprect = Get_Rect();

		Point2D start = Bias_To(startpoint, cliprect);
		Point2D end = Bias_To(endpoint, cliprect);

		if (!Clip_Line_To_Rect(start, end, cliprect)) return(false);

		if (start.X > end.X) {
			swap(start, end);
		}

		Point2D point = start;
		point.Y -= AlphaBuffer->Get_Bounds().Y;
		unsigned short * abuffer = (unsigned short *)AlphaBuffer->Get_Buffer_Offset(point);
		int astride = AlphaBuffer->Get_Buffer_Width();
		if (start.Y > end.Y) astride = -astride;

		void * buffer = (unsigned short *)Lock(start);
		if (buffer != NULL) {
			if (draw_on_zero_alpha) {

				if (start.Y == end.Y) {

					for (int i = 0; i <= end.X - start.X; i++) {
						if (*abuffer == 0) {
							*((unsigned short *)buffer + i) = color;
						}

						if (astride > 0) {
							abuffer++;
							abuffer = (unsigned short *)AlphaBuffer->Wrap_Overflow((unsigned int)abuffer);
						} else {
							abuffer++;
							abuffer = (unsigned short *)AlphaBuffer->Wrap_Underflow((unsigned int)abuffer);
						}
					}
				} else if (start.X == end.X) {
					int pitch = start.Y > end.Y ? -Stride() : Stride();

					int dy = abs(end.Y - start.Y);
					for (int i = 0; i <= dy; i++) {
						if (*abuffer == 0) {
							*(unsigned short *)buffer = color;
						}

						if (astride > 0) {
							abuffer += astride;
							abuffer = (unsigned short *)AlphaBuffer->Wrap_Overflow((unsigned int)abuffer);
						} else {
							abuffer += astride;
							abuffer = (unsigned short *)AlphaBuffer->Wrap_Underflow((unsigned int)abuffer);
						}

						buffer = (unsigned char *)buffer + pitch;
					}
				} else {
					int dx = end.X - start.X;
					int dy = end.Y - start.Y;
					int pitch = Stride();
					if (dy < 0) {
						pitch = -pitch;
					}

					dy = abs(dy);
					int dx2 = 2 * dx;
					int dy2 = 2 * dy;

					if (dx > dy) {

						int delta = dy2 - dx;

						for (int i = 0; i < dx; i++) {
							if (*abuffer == 0) {
								*((unsigned short *)buffer + i) = color;
							}

							if (delta > 0) {
								buffer = (unsigned char *)buffer + pitch;
								abuffer += astride;
								abuffer = (unsigned short *)AlphaBuffer->Wrap_Overflow((unsigned int)abuffer);
								delta -= dx2;
							}

							if (astride > 0) {
								abuffer++;
								abuffer = (unsigned short *)AlphaBuffer->Wrap_Overflow((unsigned int)abuffer);
							} else {
								abuffer++;
								abuffer = (unsigned short *)AlphaBuffer->Wrap_Underflow((unsigned int)abuffer);
							}

							delta += dy2;
						}
					} else {
						int delta = dx2 - dy;
						int k = 0;

						for (int i = 0; i < dy; i++) {
							if (*abuffer == 0) {
								*((unsigned short *)buffer + k) = color;
							}

							if (delta > 0) {
								k++;
								delta -= dy2;
								abuffer++;
								abuffer = (unsigned short *)AlphaBuffer->Wrap_Overflow((unsigned int)abuffer);
							}

							delta += dx2;
							buffer = (unsigned char *)buffer + pitch;

							if (astride > 0) {
								abuffer += astride;
								abuffer = (unsigned short *)AlphaBuffer->Wrap_Overflow((unsigned int)abuffer);
							} else {
								abuffer += astride;
								abuffer = (unsigned short *)AlphaBuffer->Wrap_Underflow((unsigned int)abuffer);
							}
						}
					}
				}
			} else {

				if (start.Y == end.Y) {

					for (int i = 0; i <= end.X - start.X; i++) {
						if (*abuffer != 0) {
							*((unsigned short *)buffer + i) = color;
						}

						if (astride > 0) {
							abuffer++;
							abuffer = (unsigned short *)AlphaBuffer->Wrap_Overflow((unsigned int)abuffer);
						} else {
							abuffer++;
							abuffer = (unsigned short *)AlphaBuffer->Wrap_Underflow((unsigned int)abuffer);
						}
					}
				} else if (start.X == end.X) {
					int pitch = start.Y > end.Y ? -Stride() : Stride();

					int dy = abs(end.Y - start.Y);
					for (int i = 0; i <= dy; i++) {
						if (*abuffer != 0) {
							*(unsigned short *)buffer = color;
						}

						if (astride > 0) {
							abuffer += astride;
							abuffer = (unsigned short *)AlphaBuffer->Wrap_Overflow((unsigned int)abuffer);
						} else {
							abuffer += astride;
							abuffer = (unsigned short *)AlphaBuffer->Wrap_Underflow((unsigned int)abuffer);
						}

						buffer = (unsigned char *)buffer + pitch;
					}
				} else {
					int dx = end.X - start.X;
					int dy = end.Y - start.Y;
					int pitch = Stride();
					if (dy < 0) {
						pitch = -pitch;
					}

					dy = abs(dy);
					int dx2 = 2 * dx;
					int dy2 = 2 * dy;

					if (dx > dy) {

						int delta = dy2 - dx;

						for (int i = 0; i < dx; i++) {
							if (*abuffer != 0) {
								*((unsigned short *)buffer + i) = color;
							}

							if (delta > 0) {
								buffer = (unsigned char *)buffer + pitch;
								abuffer += astride;
								abuffer = (unsigned short *)AlphaBuffer->Wrap_Overflow((unsigned int)abuffer);
								delta -= dx2;
							}

							if (astride > 0) {
								abuffer++;
								abuffer = (unsigned short *)AlphaBuffer->Wrap_Overflow((unsigned int)abuffer);
							} else {
								abuffer++;
								abuffer = (unsigned short *)AlphaBuffer->Wrap_Underflow((unsigned int)abuffer);
							}

							delta += dy2;
						}
					} else {
						int delta = dx2 - dy;
						int k = 0;

						for (int i = 0; i < dy; i++) {
							if (*abuffer != 0) {
								*((unsigned short *)buffer + k) = color;
							}

							if (delta > 0) {
								k++;
								delta -= dy2;
								abuffer++;
								abuffer = (unsigned short *)AlphaBuffer->Wrap_Overflow((unsigned int)abuffer);
							}

							delta += dx2;
							buffer = (unsigned char *)buffer + pitch;

							if (astride > 0) {
								abuffer += astride;
								abuffer = (unsigned short *)AlphaBuffer->Wrap_Overflow((unsigned int)abuffer);
							} else {
								abuffer += astride;
								abuffer = (unsigned short *)AlphaBuffer->Wrap_Underflow((unsigned int)abuffer);
							}
						}
					}
				}
			}

			Unlock();
			return(true);
		}
	}

	return(false);
}

// FUNCTION: GAMEMD 0x004C1440
int FUN_004C1440(void)
{
	DDCAPS caps;
	memset(&caps, 0, sizeof(caps));
	caps.dwSize = sizeof(caps);

	HRESULT result = DAT_008A0E00->GetCaps(&caps, NULL);
	if (result != DD_OK) return(result);

	if (!(caps.dwCaps & DDCAPS_3D)) return(-1);
	if (!(caps.dwZBufferBitDepths & DDBD_16)) return(-1);
	if (!(caps.ddsCaps.dwCaps & DDSCAPS_3DDEVICE)) return(-1);
	if (!(caps.ddsCaps.dwCaps & DDSCAPS_TEXTURE)) return(-1);
	if (caps.ddsCaps.dwCaps & DDSCAPS_ZBUFFER) return(0);
	return(-1);
}

struct DeviceDesc : D3DDEVICEDESC {
	DWORD dx6_fields[12];
};

static inline void Clear_Desc(DeviceDesc * desc)
{
	memset(desc, 0, sizeof(DeviceDesc));
	desc->dwSize = sizeof(DeviceDesc);
}

// FUNCTION: GAMEMD 0x004C14D0
int FUN_004C14D0(void)
{
	DeviceDesc hardware;
	DeviceDesc software;
	Clear_Desc(&hardware);
	Clear_Desc(&software);

	HRESULT result = DAT_008A0DF8->GetCaps(&hardware, &software);
	if (result != D3D_OK) return(result);

	if ((hardware.dwFlags & 0x1C3) != 0x1C3) return(-1);
	if (!(hardware.dcmColorModel & D3DCOLOR_RGB)) return(-1);
	if (!(hardware.dwDevCaps & 0x400)) return(-1);
	if (!(hardware.dwDevCaps & 0x200)) return(-1);
	if (!(hardware.dwDeviceRenderBitDepth & DDBD_16)) return(-1);
	if (!(hardware.dwDeviceZBufferBitDepth & DDBD_16)) return(-1);
	if (!(hardware.dpcTriCaps.dwZCmpCaps & 0xA)) return(-1);
	if (!(hardware.dpcTriCaps.dwSrcBlendCaps & 2)) return(-1);
	if (!(hardware.dpcTriCaps.dwDestBlendCaps & 2)) return(-1);
	if (hardware.dwMaxTextureWidth < 256) return(-1);
	if (hardware.dwMaxTextureHeight < 256) return(-1);
	return(0);
}

// FUNCTION: GAMEMD 0x004C1600
int FUN_004C1600(void)
{
	HRESULT result = DirectDrawWrap::lpDD->QueryInterface(DAT_007F7D50, (void **)&DAT_008A0E00);
	if (result == DD_OK) {
		result = DAT_008A0E00->QueryInterface(DAT_007F8070, (void **)&DAT_008A0DF4);
		if (result == DD_OK) {
			if (FUN_004C1440() != 0) return(-1);
			result = 0;
		}
	}
	return(result);
}

struct SurfaceDesc : DDSURFACEDESC {
	SurfaceDesc(void) { memset(this, 0, sizeof(DDSURFACEDESC)); dwSize = sizeof(DDSURFACEDESC); }
};

// FUNCTION: GAMEMD 0x004C16A0
int FUN_004C16A0(void)
{
	LPDIRECTDRAWSURFACE surface = DSurface::Composite->Get_DD_Surface();
	HRESULT result;
	D3DVIEWPORT2 viewport;
	unsigned short * pixels;
	unsigned short * out;
	int i;
	int j;
	int x;
	int y;
	int value;

	result = DAT_008A0DF4->CreateDevice(DAT_007F80B0, surface, &DAT_008A0DF8);
	if (result != D3D_OK) goto done;

	if (FUN_004C14D0() != 0) return(-1);

	result = DAT_008A0DF4->CreateViewport(&DAT_008A0DFC, NULL);
	if (result != D3D_OK) goto done;

	result = DAT_008A0DF8->AddViewport(DAT_008A0DFC);
	if (result != D3D_OK) goto done;

	{
		SurfaceDesc desc;
		result = surface->GetSurfaceDesc(&desc);
		if (result != DD_OK) goto done;

		memset(&viewport, 0, sizeof(viewport));
		viewport.dwSize = sizeof(viewport);
		viewport.dwWidth = desc.dwWidth;
		viewport.dwHeight = desc.dwHeight;
		viewport.dvClipWidth = (float)desc.dwWidth;
		viewport.dvClipHeight = (float)desc.dwHeight;
		viewport.dvMaxZ = 1.0f;
	}
	result = DAT_008A0DFC->SetViewport2(&viewport);
	if (result != D3D_OK) goto done;

	result = DAT_008A0DF8->SetCurrentViewport(DAT_008A0DFC);
	if (result != D3D_OK) goto done;

	DAT_008A0E04 = new Texture3D(256, 256, 0);
	if (DAT_008A0E04 != NULL) {
		pixels = (unsigned short *)malloc(0x20000);
		if (pixels != NULL) {
			out = pixels;
			for (j = 0; j < 256; j++) {
				y = j - 128;
				for (i = 0; i < 256; i++) {
					x = i - 128;
					value = 400 - (int)(FUN_004CAC40((double)(x * x + y * y)) * DAT_007E3D90);
					if (value < 0) {
						value = 0;
					} else if (value > 255) {
						value = 255;
					}
					*out++ = (unsigned short)DSurface::Build_Hicolor_Pixel(value, value, value);
				}
			}

			if (DAT_008A0E04->FUN_004A33A0(pixels) != 0) return(3);

			SurfaceDesc desc;
			result = surface->GetSurfaceDesc(&desc);
			if (result == DD_OK) {
				DAT_008A0E08 = new Texture3D(desc.dwWidth, desc.dwHeight, 1);
				DAT_008A0E10 = new Texture3DCache;

				if (DAT_008A0E08 == NULL || DAT_008A0E10 == NULL) return(-1);
				return(0);
			}
			goto done;
		}
		return(2);
	}
	return(1);
done:
	return(result);
}

// FUNCTION: GAMEMD 0x004C19B0
void FUN_004C19B0(void)
{
	Texture3DCache * cache = DAT_008A0E10;
	if (cache != NULL) {
		cache->FUN_004A3640();
		operator delete(cache);
		DAT_008A0E10 = NULL;
	}
	Texture3D * texture = DAT_008A0E0C;
	if (texture != NULL) {
		texture->FUN_004A3370();
		operator delete(texture);
		DAT_008A0E0C = NULL;
	}
	texture = DAT_008A0E08;
	if (texture != NULL) {
		texture->FUN_004A3370();
		operator delete(texture);
		DAT_008A0E08 = NULL;
	}
	texture = DAT_008A0E04;
	if (texture != NULL) {
		texture->FUN_004A3370();
		operator delete(texture);
		DAT_008A0E04 = NULL;
	}
	if (DAT_008A0DFC != NULL) {
		DAT_008A0DFC->Release();
		DAT_008A0DFC = NULL;
	}
	if (DAT_008A0DF8 != NULL) {
		DAT_008A0DF8->Release();
		DAT_008A0DF8 = NULL;
	}
	if (DAT_008A0DF4 != NULL) {
		DAT_008A0DF4->Release();
		DAT_008A0DF4 = NULL;
	}
	if (DAT_008A0E00 != NULL) {
		DAT_008A0E00->Release();
		DAT_008A0E00 = NULL;
	}
}

// FUNCTION: GAMEMD 0x004C1A90
// DSurface::Blit_From

// FUNCTION: GAMEMD 0x004C1AB0
// DSurface::Is_Direct_Draw

// SYNTHETIC: GAMEMD 0x004C1AC0
// ??_GDSurface@@UAEPAXI@Z

// TEMPLATE: GAMEMD 0x004C1B50
// ?Length@?$TPoint2D@H@@QBEHXZ
template class TPoint2D<int>;

// SYNTHETIC: GAMEMD 0x004C1B80 SYMBOL
// _$E63
double DAT_008A0E38 = FUN_004CAC40(pow(DAT_007E1710, DAT_007E1708) * 2);

// SYNTHETIC: GAMEMD 0x004C1BB0 SYMBOL
// _$E66
double DAT_008A0EA8 = DAT_008A0E38 / DAT_007E8678;

// SYNTHETIC: GAMEMD 0x004C1BD0 SYMBOL
// _$E69
double DAT_008A0E70 = DAT_007E1718 / DAT_008A0EA8;

// SYNTHETIC: GAMEMD 0x004C1BF0 SYMBOL
// _$E72
double DAT_008A0E68 = DAT_007E1720 * DAT_007E8680;

// SYNTHETIC: GAMEMD 0x004C1C10 SYMBOL
// _$E75
double DAT_008A0E40 = DAT_007E1728 * DAT_007E8680;

// SYNTHETIC: GAMEMD 0x004C1C30 SYMBOL
// _$E78
double DAT_008A0E18 = DAT_007E1730 * DAT_007E8680;

// SYNTHETIC: GAMEMD 0x004C1C50 SYMBOL
// _$E81
int DAT_008A0EB0 = (int)(DAT_007E1738 * (FUN_004CAD50(DAT_008A0E18 - DAT_008A0E40) * DAT_008A0E38));

// SYNTHETIC: GAMEMD 0x004C1C90 SYMBOL
// _$E84
double DAT_008A0E48 = FUN_004CADE0(DAT_008A0EB0 * DAT_007E1740);

// SYNTHETIC: GAMEMD 0x004C1CC0 SYMBOL
// _$E87
double DAT_008A0E60 = FUN_004CADE0((DAT_008A0EB0 * 2) / DAT_008A0E38);

// SYNTHETIC: GAMEMD 0x004C1CF0 SYMBOL
// _$E90
int DAT_008A0EA0 = (int)(DAT_008A0EB0 * 4 + DAT_007E1738);

struct ShortPair {
	unsigned short A, B;
	ShortPair() { A = 0; B = 0; }
};

// SYNTHETIC: GAMEMD 0x004C1D20 SYMBOL
// _$E93
ShortPair DAT_008A0E30;

struct IntTriple {
	int A, B, C;
	IntTriple() { A = 0; B = 0; C = 0; }
};

// SYNTHETIC: GAMEMD 0x004C1D30 SYMBOL
// _$E96
IntTriple DAT_008A0E50;

struct IntSeed {
	int X, Y, Z;
	IntSeed() { Z = 0; X = 0x80; Y = 0x80; }
};

// SYNTHETIC: GAMEMD 0x004C1D50 SYMBOL
// _$E99
IntSeed DAT_008A0E78;

struct IntQuad {
	int A, B, C, D;
	IntQuad() { A = 0; B = 0; C = 0; D = 0; }
};

// SYNTHETIC: GAMEMD 0x004C1D70 SYMBOL
// _$E102
IntQuad DAT_008A0E20;
