// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"
#include "_surface.h"
#include "_timer.h"
#include "assert.h"
#include "dbgprint.h"
#include "dsurface.h"
#include "misc.h"
#include "stimer.h"
#include "timer.h"
#include "vector.h"
#include <cstdio>

static PALETTEENTRY PaletteEntries[256];

static LPDIRECTDRAWPALETTE	PalettePtr;

static bool CanVblankSync = true;

int (*DirectDrawErrorHandler)(HRESULT error) = NULL;

void Check_Overlapped_Blit_Capability(void)
{
	DSurface test_buffer(64, 64, false);

	DebugString("Checking hardware region fill capability...");

	DSurface::AllowHWFill = true;

	test_buffer.Fill(0);
	if (test_buffer.Get_Pixel(Point2D(5,5)) != 0) {
		DSurface::AllowHWFill = false;
	} else {
		test_buffer.Fill(1);
		if (test_buffer.Get_Pixel(Point2D(5,5)) != 1) {
			DSurface::AllowHWFill = false;
		}
	}
	DebugStringNoPrefix("%s\n", DSurface::AllowHWFill ? "OK" : "Failed!");

	DebugString("Checking overlapped blit capability...");

	DSurface::OverlappedVideoBlits = true;

	test_buffer.Fill(0);

	test_buffer.Put_Pixel(Point2D(0, 0), 255);

	test_buffer.Blit_From(Rect(0, 1, 64, 64-1), test_buffer, Rect(0, 0, 64, 64-1), false, false);

	if (test_buffer.Get_Pixel(Point2D(0 ,5)) != 0) {
		DebugStringNoPrefix("Failed!\n");
		DSurface::OverlappedVideoBlits = false;
	} else {
		DebugStringNoPrefix("OK\n");
	}
}

// FUNCTION: 0x004A3DD0
void Process_DD_Result(HRESULT result, int display_ok_msg)
{
#ifdef _DEBUG
	static struct {
		HRESULT Error;
		char const * Message;
	} _errors[] = {
		{DDERR_ALREADYINITIALIZED, "This object is already initialized"},
		{DDERR_BLTFASTCANTCLIP, "Return if a clipper object is attached to the source surface passed into a BltFast call."},
		{DDERR_CANNOTATTACHSURFACE, "This surface can not be attached to the requested surface."},
		{DDERR_CANNOTDETACHSURFACE, "This surface can not be detached from the requested surface."},
		{DDERR_CANTCREATEDC, "Windows can not create any more DCs"},
		{DDERR_CANTDUPLICATE, "Can't duplicate primary & 3D surfaces, or surfaces that are implicitly created."},
		{DDERR_CANTLOCKSURFACE, "Unable to lock surface because no driver exists which can supply a pointer to the surface."},
		{DDERR_CLIPPERISUSINGHWND, "An attempt was made to set a cliplist for a clipper object that is already monitoring an hwnd."},
		{DDERR_COLORKEYNOTSET, "No src color key specified for this operation."},
		{DDERR_CURRENTLYNOTAVAIL, "Support is currently not available."},
		{DDERR_DIRECTDRAWALREADYCREATED, "A DirectDraw object representing this driver has already been created for this process."},
		{DDERR_EXCEPTION, "An exception was encountered while performing the requested operation."},
		{DDERR_EXCLUSIVEMODEALREADYSET, "An attempt was made to set the cooperative level when it was already set to exclusive."},
		{DDERR_GENERIC, "Generic failure."},
		{DDERR_HEIGHTALIGN, "Height of rectangle provided is not a multiple of reqd alignment."},
		{DDERR_HWNDALREADYSET, "The CooperativeLevel HWND has already been set. It can not be reset while the process has surfaces or palettes created."},
		{DDERR_HWNDSUBCLASSED, "HWND used by DirectDraw CooperativeLevel has been subclassed, this prevents DirectDraw from restoring state."},
		{DDERR_IMPLICITLYCREATED, "This surface can not be restored because it is an implicitly created surface."},
		{DDERR_INCOMPATIBLEPRIMARY, "Unable to match primary surface creation request with existing primary surface."},
		{DDERR_INVALIDCAPS, "One or more of the caps bits passed to the callback are incorrect."},
		{DDERR_INVALIDCLIPLIST, "DirectDraw does not support the provided cliplist."},
		{DDERR_INVALIDDIRECTDRAWGUID, "The GUID passed to DirectDrawCreate is not a valid DirectDraw driver identifier."},
		{DDERR_INVALIDMODE, "DirectDraw does not support the requested mode."},
		{DDERR_INVALIDOBJECT, "DirectDraw received a pointer that was an invalid DIRECTDRAW object."},
		{DDERR_INVALIDPARAMS, "One or more of the parameters passed to the function are incorrect."},
		{DDERR_INVALIDPIXELFORMAT, "The pixel format was invalid as specified."},
		{DDERR_INVALIDPOSITION, "Returned when the position of the overlay on the destination is no longer legal for that destination."},
		{DDERR_INVALIDRECT, "Rectangle provided was invalid."},
		{DDERR_INVALIDSURFACETYPE, "The requested action could not be performed because the surface was of the wrong type."},
		{DDERR_LOCKEDSURFACES, "Operation could not be carried out because one or more surfaces are locked."},
		{DDERR_NO3D, "There is no 3D present."},
		{DDERR_NOALPHAHW, "Operation could not be carried out because there is no alpha accleration hardware present or available."},
		{DDERR_NOBLTHW, "No blter hardware present."},
		{DDERR_NOCLIPLIST, "No cliplist available."},
		{DDERR_NOCLIPPERATTACHED, "No clipper object attached to surface object."},
		{DDERR_NOCOLORCONVHW, "Operation could not be carried out because there is no color conversion hardware present or available."},
		{DDERR_NOCOLORKEY, "Surface doesn't currently have a color key"},
		{DDERR_NOCOLORKEYHW, "Operation could not be carried out because there is no hardware support of the destination color key."},
		{DDERR_NOCOOPERATIVELEVELSET, "Create function called without DirectDraw object method SetCooperativeLevel being called."},
		{DDERR_NODC, "No DC was ever created for this surface."},
		{DDERR_NODDROPSHW, "No DirectDraw ROP hardware."},
		{DDERR_NODIRECTDRAWHW, "A hardware-only DirectDraw object creation was attempted but the driver did not support any hardware."},
		{DDERR_NODIRECTDRAWSUPPORT, "No DirectDraw support possible with current display driver."},
		{DDERR_NOEMULATION, "Software emulation not available."},
		{DDERR_NOEXCLUSIVEMODE, "Operation requires the application to have exclusive mode but the application does not have exclusive mode."},
		{DDERR_NOFLIPHW, "Flipping visible surfaces is not supported."},
		{DDERR_NOGDI, "There is no GDI present."},
		{DDERR_NOHWND, "Clipper notification requires an HWND or no HWND has previously been set as the CooperativeLevel HWND."},
		{DDERR_NOMIRRORHW, "Operation could not be carried out because there is no hardware present or available."},
		{DDERR_NOOVERLAYDEST, "Returned when GetOverlayPosition is called on an overlay that UpdateOverlay has never been called on to establish a destination."},
		{DDERR_NOOVERLAYHW, "Operation could not be carried out because there is no overlay hardware present or available."},
		{DDERR_NOPALETTEATTACHED, "No palette object attached to this surface.	"},
		{DDERR_NOPALETTEHW, "No hardware support for 16 or 256 color palettes."},
		{DDERR_NORASTEROPHW, "Operation could not be carried out because there is no appropriate raster op hardware present or available."},
		{DDERR_NOROTATIONHW, "Operation could not be carried out because there is no rotation hardware present or available."},
		{DDERR_NOSTRETCHHW, "Operation could not be carried out because there is no hardware support for stretching."},
		{DDERR_NOT4BITCOLOR, "DirectDrawSurface is not in 4 bit color palette and the requested operation requires 4 bit color palette."},
		{DDERR_NOT4BITCOLORINDEX, "DirectDrawSurface is not in 4 bit color index palette and the requested operation requires 4 bit color index palette."},
		{DDERR_NOT8BITCOLOR, "DirectDrawSurface is not in 8 bit color mode and the requested operation requires 8 bit color."},
		{DDERR_NOTAOVERLAYSURFACE, "Returned when an overlay member is called for a non-overlay surface."},
		{DDERR_NOTEXTUREHW, "Operation could not be carried out because there is no texture mapping hardware present or available."},
		{DDERR_NOTFLIPPABLE, "An attempt has been made to flip a surface that is not flippable."},
		{DDERR_NOTFOUND, "Requested item was not found."},
		{DDERR_NOTLOCKED, "Surface was not locked.  An attempt to unlock a surface that was not locked at all, or by this process, has been attempted."},
		{DDERR_NOTPALETTIZED, "The surface being used is not a palette-based surface."},
		{DDERR_NOVSYNCHW, "Operation could not be carried out because there is no hardware support for vertical blank synchronized operations."},
		{DDERR_NOZBUFFERHW, "Operation could not be carried out because there is no hardware support for zbuffer blting."},
		{DDERR_NOZOVERLAYHW, "Overlay surfaces could not be z layered based on their BltOrder because the hardware does not support z layering of overlays."},
		{DDERR_OUTOFCAPS, "The hardware needed for the requested operation has already been allocated."},
		{DDERR_OUTOFMEMORY, "DirectDraw does not have enough memory to perform the operation."},
		{DDERR_OUTOFVIDEOMEMORY, "DirectDraw does not have enough memory to perform the operation."},
		{DDERR_OVERLAYCANTCLIP, "The hardware does not support clipped overlays."},
		{DDERR_OVERLAYCOLORKEYONLYONEACTIVE, "Can only have ony color key active at one time for overlays."},
		{DDERR_OVERLAYNOTVISIBLE, "Returned when GetOverlayPosition is called on a hidden overlay."},
		{DDERR_PALETTEBUSY, "Access to this palette is being refused because the palette is already locked by another thread."},
		{DDERR_PRIMARYSURFACEALREADYEXISTS, "This process already has created a primary surface."},
		{DDERR_REGIONTOOSMALL, "Region passed to Clipper::GetClipList is too small."},
		{DDERR_SURFACEALREADYATTACHED, "This surface is already attached to the surface it is being attached to."},
		{DDERR_SURFACEALREADYDEPENDENT, "This surface is already a dependency of the surface it is being made a dependency of."},
		{DDERR_SURFACEBUSY, "Access to this surface is being refused because the surface is already locked by another thread."},
		{DDERR_SURFACEISOBSCURED, "Access to surface refused because the surface is obscured."},
		{DDERR_SURFACELOST, "Access to this surface is being refused because the surface memory is gone. The DirectDrawSurface object representing this surface should have Restore called on it."},
		{DDERR_SURFACENOTATTACHED, "The requested surface is not attached."},
		{DDERR_TOOBIGHEIGHT, "Height requested by DirectDraw is too large."},
		{DDERR_TOOBIGSIZE, "Size requested by DirectDraw is too large --	the individual height and width are OK."},
		{DDERR_TOOBIGWIDTH, "Width requested by DirectDraw is too large."},
		{DDERR_UNSUPPORTED, "Action not supported."},
		{DDERR_UNSUPPORTEDFORMAT, "FOURCC format requested is unsupported by DirectDraw."},
		{DDERR_UNSUPPORTEDMASK, "Bitmask in the pixel format requested is unsupported by DirectDraw."},
		{DDERR_VERTICALBLANKINPROGRESS, "Vertical blank is in progress."},
		{DDERR_WASSTILLDRAWING, "Informs DirectDraw that the previous Blt which is transfering information to or from this Surface is incomplete."},
		{DDERR_WRONGMODE, "This surface can not be restored because it was created in a different mode."},
		{DDERR_XALIGN, "Rectangle provided was not horizontally aligned on required boundary."}
	};
#endif
	if (result == DD_OK) {
		if (display_ok_msg) {
			MessageBox(MainWindow, "Direct Draw operation processed without error", "Note", MB_OK);
		}
		return;
	}

	if (DirectDrawErrorHandler) {
		DirectDrawErrorHandler(result);
		return;
	}
#ifdef _DEBUG
	for (int index = 0; index < ARRAY_SIZE(_errors); index++) {
		if (_errors[index].Error == result) {
			MessageBox(MainWindow, _errors[index].Message, "Westwood Library Direct Draw Error", MB_ICONEXCLAMATION|MB_OK);
			return;
		}
	}
#endif

	char str[80];
	sprintf(str, "DDRAW.DLL Error code = %08X", result);
	MessageBox(MainWindow, str, "Direct X", MB_ICONEXCLAMATION|MB_OK);
}

// FUNCTION: 0x004A3FD0
void Prep_Direct_Draw(void)
{
	DebugString("Prep direct draw.\n");

	if ( DirectDrawObject == NULL ) {
		HRESULT result = DirectDrawCreate(NULL, &DirectDrawObject, NULL);
		Process_DD_Result(result, false);
		if (result == DD_OK) {
			if (WindowedMode) {
				result = DirectDrawObject->SetCooperativeLevel(MainWindow, DDSCL_NORMAL);
			} else {
				result = DirectDrawObject->SetCooperativeLevel(MainWindow, DDSCL_EXCLUSIVE|DDSCL_FULLSCREEN);
			}
			Process_DD_Result(result, false);
		}else{
			DebugString("DirectDrawCreate failed!\n");
		}
	}
}

// FUNCTION: 0x004A40C0
void Destroy_Direct_Draw(void)
{
	if (DirectDrawObject != NULL) {
		HRESULT result = DirectDrawObject->Release();
		Process_DD_Result(result, FALSE);
		DirectDrawObject = NULL;
	}
}

// FUNCTION: 0x004A42F0
bool Set_Video_Mode(HWND , int w, int h, int bits_per_pixel)
{
	HRESULT result;

	Prep_Direct_Draw();

	DebugString("SetDisplayMode: %dx%dx%d\n", w, h, bits_per_pixel);
	result = DirectDrawObject->SetDisplayMode (w, h, bits_per_pixel);
	if (result != DD_OK) {
		DebugString("SetDisplayMode failed\n");
		return(false);
	}

	VideoModeWidth = w;
	VideoModeHeight = h;
	VideoModeBitsPerPixel = bits_per_pixel;
	if (bits_per_pixel == 8) {
		DebugString("CreatePalette\n");
		result = DirectDrawObject->CreatePalette( DDPCAPS_8BIT | DDPCAPS_ALLOW256, &PaletteEntries[0] , &PalettePtr ,NULL);
		Process_DD_Result(result, false);
		if (result != DD_OK) {
			DebugString("CreatePalette failed\n");
			return(false);
		}
	}

	Check_Overlapped_Blit_Capability();
	DebugString("Display mode set\n");

#if (0)
	result = DirectDrawObject->QueryInterface (IID_IDirectDraw2, (LPVOID*)&DirectDraw2Interface);
	SystemToVideoBlits = false;
	VideoToSystemBlits = false;
	SystemToSystemBlits= false;
	if (result != DD_OK) {
		DirectDraw2Interface = NULL;
	} else {
		DDCAPS capabilities;
		DDCAPS emulated_capabilities;

		memset ((char*)&capabilities, 0, sizeof(capabilities));
		memset ((char*)&emulated_capabilities, 0, sizeof(emulated_capabilities));
		capabilities.dwSize = sizeof (capabilities);
		emulated_capabilities.dwSize = sizeof (emulated_capabilities);

		DirectDrawObject->GetCaps (&capabilities, &emulated_capabilities);

		if (capabilities.dwCaps & DDCAPS_CANBLTSYSMEM) {
			SystemToVideoBlits = (capabilities.dwSVBCaps & DDCAPS_BLT) ? true : false;
			VideoToSystemBlits = (capabilities.dwVSBCaps & DDCAPS_BLT) ? true : false;
			SystemToSystemBlits = (capabilities.dwSSBCaps & DDCAPS_BLT) ? true : false;
		}
	}
#endif

	return(true);
}

// FUNCTION: 0x004A44F0
void Reset_Video_Mode(void)
{
	HRESULT result;

	if ( DirectDrawObject ) {
		result = DirectDrawObject->RestoreDisplayMode();
		Process_DD_Result(result, false);
		VideoModeWidth = 0;
		VideoModeHeight = 0;
		VideoModeBitsPerPixel = 0;
		result = DirectDrawObject->Release();
		Process_DD_Result(result, false);

		DirectDrawObject = NULL;
	}
}

// FUNCTION: 0x004A45C0
unsigned int Get_Free_Video_Memory(void)
{
	DDCAPS	video_capabilities;

	if (DirectDrawObject) {
		video_capabilities.dwSize = sizeof (video_capabilities);

		if (DD_OK == DirectDrawObject->GetCaps (&video_capabilities, NULL)) {
			char string [256];
			sprintf (string, "In Get_Free_Video_Memory. About to return %d bytes",video_capabilities.dwVidMemFree);
			return(video_capabilities.dwVidMemFree);
		}
	}

	return(0);
}

// FUNCTION: 0x004A4620
unsigned Get_Video_Hardware_Capabilities(void)
{
	DDCAPS	video_capabilities;
	unsigned	video;

	if (!DirectDrawObject) return(0);

	memset(&video_capabilities, 0, sizeof(video_capabilities));
	video_capabilities.dwSize = sizeof(video_capabilities);
	HRESULT result = DirectDrawObject->GetCaps (&video_capabilities, NULL);
	if (result != DD_OK) {
		Process_DD_Result(result, false);
		return(0);
	}

	video = 0;

	if (video_capabilities.dwCaps & DDCAPS_BLT) 				video |= VIDEO_BLITTER;

	if (video_capabilities.dwCaps & DDCAPS_BLTQUEUE) 		video |= VIDEO_BLITTER_ASYNC;

	if (video_capabilities.dwCaps & DDCAPS_PALETTEVSYNC) 	video |= VIDEO_SYNC_PALETTE;

	if (video_capabilities.dwCaps & DDCAPS_BANKSWITCHED) 	video |= VIDEO_BANK_SWITCHED;

	if (video_capabilities.dwCaps & DDCAPS_BLTCOLORFILL)	video |= VIDEO_COLOR_FILL;

	if (video_capabilities.dwCaps & DDCAPS_NOHARDWARE) 	video |= VIDEO_NO_HARDWARE_ASSIST;

	if (video_capabilities.dwCaps & DDCAPS_BLTSTRETCH) 	video |= VIDEO_BLIT_STRETCH;

	return(video);
}

// FUNCTION: 0x004A4700
void Wait_Vert_Blank(void)
{
	if (CanVblankSync) {
		HRESULT result = DirectDrawObject->WaitForVerticalBlank(DDWAITVB_BLOCKBEGIN, 0);
		if (result == E_NOTIMPL) {
			CanVblankSync = false;
			return;
		}
		Process_DD_Result(result, false);
	}
}

// FUNCTION: 0x004A4830
void Wait_Blit (void)
{
	HRESULT	return_code;

	do {
		return_code=PaletteSurface->GetBltStatus (DDGBS_ISBLTDONE);
	} while (return_code != DD_OK && return_code != DDERR_SURFACELOST);
}

// FUNCTION: 0x004A4850
HRESULT CALLBACK EnumDisplayModesCallback(LPDDSURFACEDESC pddsd, LPVOID lpContext)
{
	DynamicVectorClass<DDSURFACEDESC *>* descs = (DynamicVectorClass<DDSURFACEDESC *>*)lpContext;
	DDSURFACEDESC *desc = new DDSURFACEDESC;
	*desc = *pddsd;

	descs->Add(desc);

	return(DDENUMRET_OK);
}

// FUNCTION: 0x004A48C0
int __cdecl CompareModes(const void *p1, const void *p2)
{
	int w1 = *((int *)p1);
	int h1 = *((int *)p1 + 1);
	int w2 = *((int *)p2);
	int h2 = *((int *)p2 + 1);

	if (w1 < w2) return(-1);
	if (w2 < w1) return(1);

	if (h1 < h2) return(-1);
	if (h2 < h1) return(1);

	return(0);
}

// FUNCTION: 0x004A4900
int * EnumDisplayModes(DWORD minw, DWORD minh, DWORD maxw, DWORD maxh, DWORD bitdepth)
{
	if (DirectDrawObject != NULL) {
		DynamicVectorClass<DDSURFACEDESC *> descs;
		HRESULT result = DirectDrawObject->EnumDisplayModes(0, NULL, &descs, EnumDisplayModesCallback);

		if (result == S_OK && descs.Count() != 0) {
			char *list = new char[2 * sizeof(int) * descs.Count() + sizeof(int)];
			int *l = (int *)list;
			memset(l, 0, 2 * sizeof(int) * descs.Count() + sizeof(int));
			int count = 0;

			for (int i = 0; i < descs.Count(); i++) {
				DDSURFACEDESC *dsc = descs[i];

				DWORD w = dsc->dwWidth;
				if (w <= maxw) {
					DWORD h = dsc->dwHeight;
					if (h <= maxh && w >= minw && h >= minh && dsc->ddpfPixelFormat.dwRGBBitCount == bitdepth) {
						*l++ = dsc->dwWidth;
						*l++ = dsc->dwHeight;
						count++;
					}
				}
				delete descs[i];
			}

			if ((char *)l == list) {
				delete list;
				return(NULL);
			}
			qsort(list, count, sizeof(int) + sizeof(int), CompareModes);

			return((int *)(list));
		}
	}
	return(NULL);
}
