// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"
#include "preview.h"
#include "_map.h"
#include "_surface.h"
#include "_tactica.h"
#include "ccfile.h"
#include "ccini.h"
#include "cell.h"
#include "dbgprint.h"
#include "dsurface.h"
#include "language\language.h"
#include "lzopipe.h"
#include "lzostraw.h"
#include "overtype.h"
#include "ownrdraw.h"
#include "pcx.h"
#include "scenario.h"
#include "surface.h"
#include "tactical.h"
#include "terrain.h"
#include "geometry.h"
#include "windlg.h"
#include "xpipe.h"
#include "xstraw.h"
#include "dialog.h"
#include "scheme.h"
#include "draw.h"
#include "scenario.h"
#include "data.h"
#include "shapeset.h"
#include "color.hh"

#include <math.h>

// SYNTHETIC: GAMEMD 0x006404D0 SYMBOL
// _$E2
// SYNTHETIC: GAMEMD 0x006404F0 SYMBOL
// _$E84
// SYNTHETIC: GAMEMD 0x00640520 SYMBOL
// _$E87
// SYNTHETIC: GAMEMD 0x00640540 SYMBOL
// _$E90
// SYNTHETIC: GAMEMD 0x00640560 SYMBOL
// _$E93
// SYNTHETIC: GAMEMD 0x00640580 SYMBOL
// _$E96
// SYNTHETIC: GAMEMD 0x006405A0 SYMBOL
// _$E99
// SYNTHETIC: GAMEMD 0x006405C0 SYMBOL
// _$E102
// SYNTHETIC: GAMEMD 0x00640600 SYMBOL
// _$E105
// SYNTHETIC: GAMEMD 0x00640630 SYMBOL
// _$E108
// SYNTHETIC: GAMEMD 0x00640660 SYMBOL
// _$E111
// SYNTHETIC: GAMEMD 0x00640690 SYMBOL
// _$E114
// SYNTHETIC: GAMEMD 0x006406A0 SYMBOL
// _$E117
// SYNTHETIC: GAMEMD 0x006406C0 SYMBOL
// _$E120

// FUNCTION: GAMEMD 0x006406E0
MapPreviewClass::MapPreviewClass(void) :
	SurfacePtr(NULL)
{
}

// FUNCTION: GAMEMD 0x006406F0
MapPreviewClass::~MapPreviewClass(void)
{
	if (SurfacePtr != NULL) {
		delete SurfacePtr;
		SurfacePtr = NULL;
	}
}

static inline Point2D * Scratch(Point2D const & p) { return (Point2D *)&p; }
void * __fastcall FileSystem_LoadWholeFileEx(char const * name, bool & loaded);
ConvertClass * FUN_0072E280(void);

static inline int Min_Ref(int a, int b) { return (b < a) ? b : a; }

#include "house.h"
#include "lightcon.h"

struct Drawing {
	static int RenderWidth;
};

extern int DAT_007F5BE4;
extern int DAT_007F5BE8;

// FUNCTION: GAMEMD 0x00640710
void MapPreviewClass::DrawStartPoints(HWND window)
{
	ValidateRect(window, NULL);

	if (SurfacePtr != NULL) {
		RECT winrect;
		Get_Display_Rect(GetDlgItem(window, IDC_PREVIEW_FRAME), &winrect);
		Rect framerect(winrect.left, winrect.top, winrect.right - winrect.left, winrect.bottom - winrect.top);
		Rect destrect;
		Rect srcrect = SurfacePtr->Get_Rect();
		int scale = Min_Ref(1000 * framerect.Width / srcrect.Width, 1000 * framerect.Height / srcrect.Height);

		destrect.X = framerect.X + framerect.Width / 2 - (scale * srcrect.Width) / 2000;
		destrect.Y = framerect.Y + framerect.Height / 2 - (scale * srcrect.Height) / 2000;
		destrect.Width = (scale * srcrect.Width) / 1000;
		destrect.Height = (scale * srcrect.Height) / 1000;
		AlternateSurface->Fill_Rect(framerect, 0);
		AlternateSurface->Blit_From(destrect, *SurfacePtr, SurfacePtr->Get_Rect(), false, false);

		bool allocated = false;
		static ShapeSet * button = (ShapeSet *)FileSystem_LoadWholeFileEx("STARTBUT.SHP", allocated);
		ConvertClass * convert = FUN_0072E280();
		ColorScheme * scheme = Fetch_Scheme_By_Name("Yellow", 1);

		if (Scen->NumberStartingPoints > 0 && Scen->NumberStartingPoints <= 8) {
			for (int i = 0; i < Scen->NumberStartingPoints; i++) {
				int x = ((Scen->StartingPoints[i].X - Scen->StartX) * 1000 / Scen->Width) * destrect.Width / 1000 + destrect.X;
				int y = ((Scen->StartingPoints[i].Y - Scen->StartY) * 1000 / Scen->Height) * destrect.Height / 1000 + destrect.Y;
				if (button != NULL) {
					Draw_Shape(*AlternateSurface, *convert, button, 0, Point2D(x - 9, y - 6), AlternateSurface->Get_Rect(), (ShapeFlags_Type)0x400);
				}
				Fancy_Text_Print_Wide(Scratch(Point2D()), L"%d", AlternateSurface, &AlternateSurface->Get_Rect(), &(Point2D const &)Point2D(x - 2, y - 6), scheme, 8, TextPrintType(0x19), i + 1);
			}
		}
	}
}

// FUNCTION: GAMEMD 0x00640A40
void MapPreviewClass::DrawMap(DSurface & surface)
{
	if (SurfacePtr != NULL) {
		CellClass * cptr;

		Map.Reset_Iterator();

		int minx = 10000;
		int originx = 10000;
		int miny = 10000;
		int maxx = 0;
		int maxy = 0;

		cptr = Map.Iterate();
		while (cptr != NULL) {
			if (Map.In_Local_Radar(cptr->CellID, false)) {
				Point2D pixel = Cell_To_Preview_Pixel(cptr->CellID);
				if (pixel.X < minx) {
					originx = pixel.X;
					minx = pixel.X;
				}
				if (pixel.X > maxx) {
					maxx = pixel.X;
				}
				if (pixel.Y < miny) {
					miny = pixel.Y;
				}
				if (pixel.Y > maxy) {
					maxy = pixel.Y;
				}
			}
			cptr = Map.Iterate();
		}

		int count = 0;
		int i;
		for (i = 0; i < 8; i++) {
			if (Scen->Is_Valid_Waypoint(i)) {
				count++;
			}
		}

		for (int j = 0; j < count; j++) {
			if (j == 0 || j <= i) {
				if (Scen->Is_Valid_Waypoint(j)) {
					Point2D pixel = Cell_To_Preview_Pixel(Scen->Get_Waypoint_Cell(j));
					pixel = pixel - Point2D(minx, miny);
					Rect rect(2 * pixel.X - 1, pixel.Y - 1, 4, 4);
					SurfacePtr->Fill_Rect(SurfacePtr->Get_Rect(), rect, DSurface::Build_Hicolor_Pixel(0, 0, 0));
				}
			}
		}

		Rect framerect;
		if (Drawing::RenderWidth == DAT_007F5BE4) {
			framerect = Rect(499, 379, 216, 166);
		} else if (Drawing::RenderWidth == DAT_007F5BE8) {
			framerect = Rect(570, 424, 300, 260);
		} else {
			framerect = Rect(385, 270, 200, 200);
		}

		Rect srcrect = SurfacePtr->Get_Rect();
		int scale = Min_Ref(1000 * framerect.Width / srcrect.Width, 1000 * framerect.Height / srcrect.Height);

		int dx = (framerect.Width - (scale * srcrect.Width) / 1000) / 2;
		int dy = (framerect.Height - (scale * srcrect.Height) / 1000) / 2;
		Rect destrect;
		destrect.X = dx;
		destrect.Width = framerect.Width - 2 * dx;
		destrect.Height = framerect.Height - 2 * dy;
		destrect.Y = dy;

		DSurface * surf = new DSurface(framerect.Width, framerect.Height, true);
		surf->Blit_From(destrect, *SurfacePtr, SurfacePtr->Get_Rect(), false, false);

		bool allocated = false;
		ShapeSet * shape = (ShapeSet *)FileSystem_LoadWholeFileEx("mmpb.shp", allocated);

		for (int k = 0; k < count; k++) {
			if (k == 0 || k <= i) {
				if (Scen->Is_Valid_Waypoint(k)) {
					Point2D pixel = Cell_To_Preview_Pixel(Scen->Get_Waypoint_Cell(k));
					pixel = pixel - Point2D(originx, miny);
					if (Scen->HouseIndices[k] != -1) {
						ConvertClass * convert = ColorSchemes[Houses[Scen->HouseIndices[k]]->Scheme]->Converter;
						if (shape != NULL && convert != NULL) {
							int x = (pixel.X * 1000000 / Scen->Width) * destrect.Width / 1000000 + destrect.X - 3;
							int y = (pixel.Y * 1000000 / Scen->Height) * destrect.Height / 1000000 + destrect.Y - 2;
							Draw_Shape(*surf, *convert, shape, 0, Point2D(x, y), surf->Get_Rect(), (ShapeFlags_Type)0x400);
						}
					}
				}
			}
		}

		surface.Fill_Rect(framerect, 0);
		surface.Blit_From(framerect, *surf, surf->Get_Rect(), false, false);
		delete surf;
	}
}

// FUNCTION: GAMEMD 0x006410C0
Point2D MapPreviewClass::Cell_To_Preview_Pixel(Cell cell) const
{
	int x;
	int y;
	TacticalMap->Lepton_To_Map_Pixel(Coord(cell).X, Coord(cell).Y, x, y);
	return(Point2D(x / 60, y / 30));
}

// FUNCTION: GAMEMD 0x006418B0
bool MapPreviewClass::Write_INI(CCINIClass & ini)
{
	static char const * const PREVIEW = "Preview";
	static char const * const PREVIEWPACK = "PreviewPack";

	bool created = false;
	if (SurfacePtr == NULL) {
		Create_Preview();
		created = true;
	}

	ini.Clear(PREVIEW);

	ini.Put_Rect(PREVIEW, "Size", SurfacePtr->Get_Rect());

	BufferPipe bpipe(AlternateSurface->Lock(), AlternateSurface->Get_Width() * AlternateSurface->Get_Height() * AlternateSurface->Bytes_Per_Pixel());
	LZOPipe comp(LZOPipe::COMPRESS);
	comp.Put_To(&bpipe);

	int total = 0;
	RGBClass rgb(0,0,0);

	SurfacePtr->Lock();
	for (int y = 0; y < SurfacePtr->Get_Height(); y++) {
		for (int x = 0; x < SurfacePtr->Get_Width(); x++) {
			int pixel = SurfacePtr->Get_Pixel(Point2D(x,y));
			rgb = DSurface::Deconstruct_Hicolor_Pixel(pixel);

			total += comp.Put(&rgb, sizeof(rgb));
		}
	}
	SurfacePtr->Unlock();

	total += comp.End();
	ini.Clear(PREVIEWPACK);
	if (total > 0) {
		ini.Put_UUBlock(PREVIEWPACK, AlternateSurface->Lock(), total);
		AlternateSurface->Unlock();
	}

	AlternateSurface->Unlock();

	if (created) {
		delete SurfacePtr;
		SurfacePtr = NULL;
	}
	return(total > 0);
}

// FUNCTION: GAMEMD 0x00641B00
bool MapPreviewClass::Read_INI(CCINIClass const & ini)
{
	static char const * const PREVIEW = "Preview";
	static char const * const PREVIEWPACK = "PreviewPack";

	const_cast<CCINIClass &>(ini).Reset();

	if (SurfacePtr != NULL) {
		delete SurfacePtr;
		SurfacePtr = NULL;
	}

	Rect rect = ini.Get_Rect(PREVIEW, "Size", Map.LocalRect);

	SurfacePtr = new DSurface(rect.Width, rect.Height, true);
	SurfacePtr->Fill(0);

	int size = AlternateSurface->Get_Width() * AlternateSurface->Get_Height() * AlternateSurface->Bytes_Per_Pixel();
	void * block = AlternateSurface->Lock();
	if (block != NULL) {
		int length = ini.Get_UUBlock(PREVIEWPACK, block, size);

		if (length > 0) {
			unsigned char *buffer = (unsigned char *)AlternateSurface->Lock();
			BufferStraw bstraw(buffer, length);
			LZOStraw uncomp(LZOStraw::DECOMPRESS);
			uncomp.Get_From(&bstraw);
			unsigned char rgb[3] = {0,0,0};
			SurfacePtr->Lock();

			for (int y = 0; y < SurfacePtr->Get_Height(); y++) {
				for (int x = 0; x < SurfacePtr->Get_Width(); x++) {
					if (uncomp.Get(rgb, sizeof(rgb)) != sizeof(rgb)) {
						SurfacePtr->Unlock();
						AlternateSurface->Unlock();
						AlternateSurface->Unlock();
						return(false);
					}
					SurfacePtr->Put_Pixel(Point2D(x,y), DSurface::Build_Hicolor_Pixel(rgb[0], rgb[1], rgb[2]));
				}
			}

			SurfacePtr->Unlock();
			AlternateSurface->Unlock();
		}
		AlternateSurface->Unlock();
	}
	return(true);
}

// FUNCTION: GAMEMD 0x00641DB0
bool MapPreviewClass::Read_PCX_Preview(char const * filename)
{
	CCFileClass file(filename);

	if (file.Is_Available()) {
		if (SurfacePtr != NULL) {
			delete SurfacePtr;
			SurfacePtr = NULL;
		}
		Surface *surf = Read_PCX_File(file);

		if (surf != NULL && surf->Get_Width() && surf->Get_Height()) {
			SurfacePtr = new DSurface(surf->Get_Width(), surf->Get_Height(), true);
			SurfacePtr->Fill(0);
			SurfacePtr->Blit_From(*surf);
			delete surf;
			return(true);
		}
	}

	return(false);
}

// FUNCTION: GAMEMD 0x00641EE0
bool MapPreviewClass::Read_INI_Preview(char const * filename)
{
	CCFileClass file(filename);
	if (file.Is_Available() && file.Open(FileClass::READ)) {
		CCINIClass fullini;
		fullini.ReadCCFile(file, true, false);
		fullini.Reset();
		Scen->ReadStartPoints(fullini);

		int percent_load = 0;
		char * buffer = NULL;

		while (percent_load < 100) {
			percent_load += 25;

			int length = percent_load * file.Size() / 100;
			if (buffer != NULL) {
				delete [] buffer;
			}
			buffer = new char[length + 1];
			if (buffer == NULL) {
				return(false);
			}

			file.Seek(0, 0);
			file.Read(buffer, length);
			buffer[length] = '\0';

			char * section = strstr(buffer, "[Map]");
			if (section != NULL) {
				section[0] = '\0';
				CCINIClass ini;
				BufferStraw straw(buffer, section - buffer - 1);
				ini.Load(straw, false);
				bool result = Read_INI(ini);
				delete [] buffer;
				return(result);
			}
		}
		delete [] buffer;
	}
	return(false);
}

// FUNCTION: GAMEMD 0x00642130
unsigned * MapPreviewClass::Create_Paletted_Preview(int colorcount, int & size)
{
	int i;
	int k;
	int x;
	int y;

	if (SurfacePtr == NULL) {
		return(NULL);
	}

	DSurface surf(SurfacePtr->Get_Width() / 2, SurfacePtr->Get_Height() / 2);
	surf.Blit_From(surf.Get_Rect(), *SurfacePtr, SurfacePtr->Get_Rect(), false, false);

	int *counts = new int[4096];
	memset(counts, 0, 4096 * sizeof(int));
	int colorsused = 0;

	surf.Lock();
	for (y = 0; y < surf.Get_Height(); y++) {
		for (x = 0; x < surf.Get_Width(); x++) {
			int pixel = surf.Get_Pixel(Point2D(x,y));
			RGBClass color = DSurface::Deconstruct_Hicolor_Pixel(pixel);
			int red = color.Get_Red() & 0xF0;
			int green = color.Get_Green() & 0xF0;
			int blue = color.Get_Blue() & 0xF0;
			surf.Put_Pixel(Point2D(x,y), DSurface::Build_Hicolor_Pixel(red, green, blue));
			int colorindex = (red << 4) | green | (blue >> 4);
			int count = counts[colorindex];
			if (count == 0) {
				colorsused++;
			}
			counts[colorindex] = count + 1;
		}
	}
	surf.Unlock();

	unsigned short *palette = new unsigned short[colorcount];
	for (i = 0; i < colorcount; i++) {
		int bestcolor = -1;
		int bestcount = 0;
		for (k = 0; k < 4096; k++) {
			if (counts[k] > bestcount) {
				bestcount = counts[k];
				bestcolor = k;
			}
		}
		if (bestcolor == -1) {
			break;
		}
		counts[bestcolor] = 0;
		palette[i] = bestcolor;
	}

	unsigned char *indexbuf = new unsigned char[surf.Get_Width() * surf.Get_Height()];
	unsigned char *indexptr = indexbuf;

	surf.Lock();
	for (y = 0; y < surf.Get_Height(); y++) {
		for (x = 0; x < surf.Get_Width(); x++) {
			int pixel = surf.Get_Pixel(Point2D(x,y));
			RGBClass color = DSurface::Deconstruct_Hicolor_Pixel(pixel);

			int bestindex = -1;
			int bestdist = 0xFFFF;
			for (k = 0; k < colorcount; k++) {
				unsigned short entry = palette[k];
				int dist = abs(((entry >> 4) & 0xF0) - color.Get_Red()) + abs((16 * (entry & 0x000F)) - color.Get_Blue()) + abs((entry & 0x00F0) - color.Get_Green());
				if (dist == 0) {
					bestindex = k;
					break;
				}
				if (dist < bestdist) {
					bestdist = dist;
					bestindex = k;
				}
			}
			if (bestindex == -1 || bestindex >= 256) {
				bestindex = 0;
			}
			*indexptr++ = (unsigned char)bestindex;
		}
	}
	surf.Unlock();

	int offset = (colorcount * sizeof(unsigned short)) + sizeof(Header) + sizeof(int);
	int bytes = offset + surf.Get_Width() * surf.Get_Height();
	Header *block = (Header *)new unsigned char[bytes];
	block->Width = surf.Get_Width();
	block->Height = surf.Get_Height();
	*(int *)(block + 1) = colorcount;
	memcpy((unsigned char *)block + sizeof(Header) + sizeof(int), palette, colorcount << 1);
	memcpy(((unsigned char *)block) + offset, indexbuf, surf.Get_Width() * surf.Get_Height());
	delete [] palette;
	delete [] indexbuf;
	delete [] counts;
	size = bytes;
	DebugString("Created paletted preview of size %d bytes\n", bytes);

	return((unsigned *)block);
}

// FUNCTION: GAMEMD 0x006425F0
void MapPreviewClass::Create_Preview_Surface(char * buffer)
{
	int * header = (int *)buffer;

	int width = *header++;
	int height = *header++;
	int colorcount = *header;
	unsigned short *palette = (unsigned short *)header;

	if (SurfacePtr != NULL) {
		delete SurfacePtr;
	}
	SurfacePtr = new DSurface(width, height, true);
	SurfacePtr->Fill(TBLACK);

	int offset = (colorcount * sizeof(unsigned short)) + sizeof(Header) + sizeof(int);
	unsigned char * indexptr = (unsigned char *)buffer + offset;
	for (int y = 0; y < height; ++y) {
		for (int x = 0; x < width; ++x) {
			unsigned short entry = palette[*indexptr++ + 2];
			int color = DSurface::Build_Hicolor_Pixel((entry >> 4) & 0x00F0, entry & 0x00F0, 16 * (entry & 0x000F));

			SurfacePtr->Put_Pixel_Clip(Point2D(x, y), color, SurfacePtr->Get_Rect());
		}
	}
}
