// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"
#include "rect.h"

// SYNTHETIC: GAMEMD 0x006302F0 SYMBOL
// _$E2

#include "pcx.h"
#include "dsurface.h"

#define	POOL_SIZE 2048
#define	READ_CHAR()  *file_ptr++ ; \
							 if ( file_ptr	>= & pool [ POOL_SIZE ]	) { \
								 file_handle.Read (pool, POOL_SIZE ); \
								 file_ptr = pool ; \
							 }
#define	READ_CHARx()  *file_ptr++ ; \
							 if ( file_ptr	>= & pool [ POOL_SIZE ]	) { \
								 file_handle.Read (pool, POOL_SIZE ); \
							 }


// FUNCTION: GAMEMD 0x00630310
Surface * Read_PCX_File(FileClass & file_handle, PaletteClass * palette, void * Buff, int Size)
{
	unsigned					i, j;
	unsigned char			rle;
	unsigned char			color;
	unsigned					scan_pos;
	char						*file_ptr;
	unsigned					width;
	unsigned					height;
	char						*buffer;
	PCX_HEADER				header;
	char						pool [POOL_SIZE];
	BSurface * pic;

	if (!file_handle.Is_Available()) return(NULL);

	file_handle.Open(FileClass::READ);

	file_handle.Read (&header, sizeof (PCX_HEADER));

	if (header.id != 10 &&  header.version != 5 && header.pixelsize != 8 ) return(NULL);

	width = header.width - header.x + 1;
	height = header.height - header.y + 1;
	unsigned in_line = header.color_planes * header.byte_per_line;
	unsigned bytes_per_pixel = (header.color_planes == 1) ? 1 : 2;

	if (Buff != NULL) {
		i = Size / width;
		height = MIN ((int)(i - 1), (int)height);
		Buffer b(Buff, Size);
		pic = new BSurface(width, height, bytes_per_pixel, &b);
	} else {
		pic = new BSurface(width, height, bytes_per_pixel);
	}
	if (pic == NULL) return(NULL);

	buffer = (char *)pic->Lock();
	if (buffer != NULL) {
		file_ptr = pool ;
		file_handle.Read (pool, POOL_SIZE);

		unsigned short * hibuffer = (unsigned short *)buffer;

		if ( header.color_planes == 3 ) {

			// Decode each scanline into separate RGB planes, then interleave them into hicolor pixels.
			unsigned char * line = new unsigned char [in_line];
			if (line == NULL) {
				pic->Unlock();
				delete pic;
				return(NULL);
			}

			for ( j = 0 ; j < height ; j ++ ) {
				i = 0;
				do {
					rle = READ_CHAR ();
					if ( (rle & 192) == 192 ) {
						rle &= 63 ;
						color = READ_CHAR ();
						do {
							line[i++] = color;
						} while (--rle);
					} else {
						line[i++] = rle;
					}
				} while ( i < in_line );

				unsigned i = 0;
				unsigned green = header.byte_per_line;
				unsigned blue = header.byte_per_line * 2;
				while ( i < width ) {
					*hibuffer++ = (unsigned short)DSurface::Build_Hicolor_Pixel(line[i++], line[green++], line[blue++]);
				}
			}

			delete [] line;

		} else if ( header.byte_per_line != width ) {

			i = 0;
			rle = 0;
			for ( scan_pos = j = 0 ; j < height ; j ++, scan_pos += width ) {
				for ( i = 0 ; i < header.byte_per_line ; ) {
					rle = READ_CHAR ();
					if ( (rle & 192) == 192 ) {
						rle &= 63 ;
						color =	READ_CHAR (); ;
						for ( unsigned k = 0 ; k < rle ; k ++ ) {
							if ( k + i < width ) {
								*(buffer + scan_pos + k + i) = color;
							}
						}
						i += rle;
					} else {
						if ( i < width ) {
							*(buffer+scan_pos + i++ ) = (char)rle;
						}
					}
				}
			}

			if ( i == width ) {
				rle = READ_CHAR ();
			}
			if ( (rle & 192) == 192 ) {
				READ_CHARx();
			}

		} else {

			for ( i = 0 ; i < width * height ; ) {
				rle = READ_CHAR ();
				if ( (rle & 192) == 192 ) {
					rle &= 63 ;
					color = READ_CHAR ();
					memset ( buffer + i, color, rle );
					i += rle ;
				} else {
					*(buffer + i++) = (char)rle;
				}
			}
		}
		pic->Unlock();
	}

	if ( palette && header.color_planes == 1 ) {
		file_handle.Seek (- (256 * (int)sizeof(RGB)), SEEK_END );
		file_handle.Read (palette, 256L * sizeof ( RGB ));
	}

	file_handle.Close();
	return(pic);
}
