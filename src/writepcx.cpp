// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"

#include "dsurface.h"
#include "geometry.h"
#include "pcx.h"
#include <math.h>

#include "wsproto.h"

static void Write_Pcx_ScanLine(FileClass & file, int scansize, char * ptr);
static int Write_Pcx_ScanLine_Buf(int scansize, char * ptr, char * file_ptr);

static const unsigned char rle_code = 0xC0;
static const unsigned char rle_max_run = 0x2F;
static const unsigned char rle_full_run = (rle_max_run|rle_code);

// FUNCTION: GAMEMD 0x007B05C0
bool Write_PCX_File(FileClass & file, Surface & pic, PaletteClass * palette)
{
	unsigned char palcopy[256 * sizeof(RGB)];
	char	* ptr;
	//RGB	* pal;
	PCX_HEADER header = {
		10,
		5,
		1,
		8,
		0,
		0,
		(unsigned short)(pic.Get_Width()-1),
		(unsigned short)(pic.Get_Height()-1),
		(short)(pic.Get_Width()),
		(short)(pic.Get_Height()),
		{{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0}},
		0,
		(unsigned char)(pic.Bytes_Per_Pixel() == 2 ? 3 : 1),
		(unsigned short)pic.Get_Width(),
		1,
		(short)(pic.Get_Width()),
		(short)(pic.Get_Height()),
		{0}
	};

	bool open = false;
	if (!file.Is_Open()) {
		file.Open(FileClass::WRITE);
		open = true;
	}
	file.Write(&header, sizeof(header));

	if (pic.Bytes_Per_Pixel() == 2) {

		unsigned char *line1 = new unsigned char[pic.Get_Width()];
		unsigned char *line2 = new unsigned char[pic.Get_Width()];
		unsigned char *line3 = new unsigned char[pic.Get_Width()];
		unsigned char *file_ptr = new unsigned char[pic.Get_Width() * ((4 + 1) * 2)];

		for (int line = 0; line < header.height + 1; line++) {
			unsigned short * buffer = (unsigned short *)pic.Lock(Point2D(0, line));
			if (buffer == NULL) {
				return(1);
			}
			for (int j = 0; j < pic.Get_Width(); j++) {
				RGBClass color = DSurface::Deconstruct_Hicolor_Pixel(*buffer++);
				line1[j] = color.Get_Red();
				line2[j] = color.Get_Green();
				line3[j] = color.Get_Blue();
			}
			pic.Unlock();
			int size = 0;
			size += Write_Pcx_ScanLine_Buf(pic.Get_Width(),(char *)line1, (char *)file_ptr + size);
			size += Write_Pcx_ScanLine_Buf(pic.Get_Width(),(char *)line2, (char *)file_ptr + size);
			size += Write_Pcx_ScanLine_Buf(pic.Get_Width(),(char *)line3, (char *)file_ptr + size);

			file.Write(file_ptr, size);
		}

		delete [] line1;
		delete [] line2;
		delete [] line3;
		delete [] file_ptr;

	} else {

		for (int line = 0; line < header.height + 1; line++) {
			ptr = (char *)pic.Lock(Point2D(0, line));
			Write_Pcx_ScanLine(file, header.byte_per_line, ptr);
			pic.Unlock();
		}
	}

	unsigned char ender = 0x0C;
	file.Write(&ender, sizeof(ender));

	if (pic.Bytes_Per_Pixel() == 1) {
		memmove(palcopy, palette, sizeof(PaletteClass));
		#if 0
		pal = (RGB *)palcopy ;
		for (int palindex = 0; palindex < 256; palindex++) {
			pal->red = (unsigned char)((pal->red<<2)); // | (pal->red>>6));
			pal->green = (unsigned char)((pal->green<<2)); // | (pal->green>>6));
			pal->blue = (unsigned char)((pal->blue<<2)); // | (pal->blue>>6));
			pal++;
		}
		#endif

		file.Write(palcopy, sizeof(palcopy));
	}

	if (open) {
		file.Close();
	}
	return(false);
}

static void Write_Pcx_ScanLine(FileClass & file, int scansize, char * ptr)
{
	unsigned char last = *ptr;
	unsigned char rle=1;
	unsigned char c;
	for (int i = 1; i < scansize; i++) {
		unsigned char color = (unsigned char)(0xff & * ++ptr);
		if (color == last) {
			rle++;
			if (rle == rle_max_run) {
				file.Write(&rle_full_run, sizeof(rle_full_run));
				file.Write(&color, sizeof(color));
				rle = 0 ;
			}
		} else {
			if (rle) {
				if (rle == 1 && (rle_code != (rle_code & last))) {
					file.Write(&last, sizeof(last));
				} else {
					c = (unsigned char)(rle | rle_code);
					file.Write(&c, sizeof(c));
					file.Write(&last, sizeof(last));
				}
			}
			last = color ;
			rle = 1 ;
		}
	}
	if (rle) {
		if (rle == 1 && ( rle_code != (rle_code & last))) {
			file.Write(&last, sizeof(last));
		} else {
			c = (unsigned char)(rle | rle_code);
			file.Write(&c, sizeof(c));
			file.Write(&last, sizeof(last));
		}
	}
}


/// <summary>
/// Compresses a PCX scanline into a memory buffer.
/// This routine is the buffered counterpart to Write_Pcx_ScanLine. It is used when
/// writing a hicolor picture, where the three color planes of a line are packed into
/// one buffer and then written to the file as a single block.
/// </summary>
/// <param name="scansize">The number of bytes to compress.</param>
/// <param name="ptr">Pointer to the data to compress.</param>
/// <param name="file_ptr">Buffer to place the compressed scanline into.</param>
/// <returns>Returns with the number of bytes placed into the destination buffer.</returns>

#define WRITE_CHAR(x) {	file_ptr [size++] = x ; }
// FUNCTION: GAMEMD 0x007B0B00
static int Write_Pcx_ScanLine_Buf( int scansize, char * ptr, char * file_ptr )
{
	unsigned char last = *ptr;
	unsigned char rle=1;
	unsigned char c;
	int size = 0;

	for (int i = 1; i < scansize; i++) {
		unsigned char color = (unsigned char)(0xff & * ++ptr);
		if (color == last) {
			rle++;
			if (rle == rle_max_run) {
				WRITE_CHAR((*(unsigned char *)&rle_full_run));
				WRITE_CHAR(color);
				rle = 0 ;
			}
		} else {
			if (rle) {
				if (rle == 1 && (rle_code != (rle_code & last))) {
					WRITE_CHAR(last);
				} else {
					c = (unsigned char)(rle | rle_code);
					WRITE_CHAR(c);
					WRITE_CHAR(last);
				}
			}
			last = color ;
			rle = 1 ;
		}
	}
	if (rle) {
		if (rle == 1 && ( rle_code != (rle_code & last))) {
			WRITE_CHAR(last);
		} else {
			c = (unsigned char)(rle | rle_code);
			WRITE_CHAR(c);
			WRITE_CHAR(last);
		}
	}

	return(size);
}

// SYNTHETIC: GAMEMD 0x007B0BA0 SYMBOL
// _$E23
// SYNTHETIC: GAMEMD 0x007B0BD0 SYMBOL
// _$E26
// SYNTHETIC: GAMEMD 0x007B0BF0 SYMBOL
// _$E29
// SYNTHETIC: GAMEMD 0x007B0C10 SYMBOL
// _$E32
// SYNTHETIC: GAMEMD 0x007B0C30 SYMBOL
// _$E35
// SYNTHETIC: GAMEMD 0x007B0C50 SYMBOL
// _$E38
// SYNTHETIC: GAMEMD 0x007B0C70 SYMBOL
// _$E41
// SYNTHETIC: GAMEMD 0x007B0CB0 SYMBOL
// _$E44
// SYNTHETIC: GAMEMD 0x007B0CE0 SYMBOL
// _$E47
// SYNTHETIC: GAMEMD 0x007B0D10 SYMBOL
// _$E50
// SYNTHETIC: GAMEMD 0x007B0D40 SYMBOL
// _$E53
// SYNTHETIC: GAMEMD 0x007B0D50 SYMBOL
// _$E56
// SYNTHETIC: GAMEMD 0x007B0D70 SYMBOL
// _$E59
