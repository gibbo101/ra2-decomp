// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"
#include "unvqtblc.h"
#include "_vqa.h"
#include "assert.h"
#include "dsurface.h"

static bool Hicolor_Set_Color_Mode(int cmode);

bool Hicolor_Set_Color_Mode(int cmode)
{
	bool valid = false;

	int redshift, greenshift, blueshift;
	switch (cmode) {
	case COLORMODE_555:
		redshift = 10;
		greenshift = 5;
		blueshift = 0;
		valid = true;
		break;
	case COLORMODE_655:
		redshift = 11;
		greenshift = 5;
		blueshift = 0;
		valid = true;
		break;
	case COLORMODE_565:
		redshift = 11;
		greenshift = 6;
		blueshift = 0;
		valid = true;
		break;
	case COLORMODE_556:
		redshift = 11;
		greenshift = 6;
		blueshift = 1;
		valid = true;
		break;
	default:
		break;
	}

	if (valid) {
		for (int r = 0; r < 32; r++) {
			for (int g = 0; g < 32; g++) {
				for (int b = 0; b < 32; b++) {
					HicolorTable[(unsigned short)(32 * ((32 * r) | g)) | (unsigned short)b] = (r << redshift) | (g << greenshift) | (b << blueshift);
				}
			}
		}

		return(true);
	}

	return(false);
}

// FUNCTION: GAMEMD 0x00748220
bool Hicolor_Init_Table(int cmode)
{
	Hicolor_Clear_Table();

	HicolorTable = new unsigned short[65535 - 1];
	if (HicolorTable != NULL) {
		return(Hicolor_Set_Color_Mode(cmode));
	}

	return(false);
}

// FUNCTION: GAMEMD 0x00748360
void Hicolor_Clear_Table(void)
{
	if (HicolorTable != NULL) {
		delete[] HicolorTable;
		HicolorTable = NULL;
	}
}

// FUNCTION: GAMEMD 0x00748380
void Hicolor_Translate(void * buffer, int size)
{
	unsigned short * buf = (unsigned short *)buffer;
	while (size > 0) {
		*buf = HicolorTable[*buf];
		buf++;
		size--;
	}
}

static inline void memset32(void * D, unsigned int val, unsigned int n)
{
	unsigned int * dst = (unsigned int *)D;
	unsigned int i;

	for (i = 0; i < n; i++) dst[i] = val;
}

// FUNCTION: GAMEMD 0x007483B0
void __cdecl UnVQ2_4x4_Table(unsigned char * codebook, unsigned char * pointers, unsigned char * buffer, unsigned long blocksperrow, unsigned long numrows, unsigned long bufwidth)
{
	bufwidth = 2 * bufwidth;
	unsigned int last_pos = 8 * blocksperrow;
	unsigned char * data_end = &buffer[4 * numrows * bufwidth];
	unsigned char * row_ptr = buffer;
	unsigned char * last_buf_pos = &buffer[8 * blocksperrow];
	unsigned int blocks_per_rowa = 4 * bufwidth;
	unsigned short * ptrs = (unsigned short *)pointers;
	unsigned char * dst = buffer;

	while (dst < data_end) {
		unsigned short valw = *ptrs++;
		unsigned int val = valw;
		unsigned int count = val & 0xFFFF0FFF;
		int code = val & 0xF000;
		if (code <= 0x3000) {
			if (code != 0x3000) {
				if (code) {
					if (code != 0x1000) {
						if (code == 0x2000) {
							unsigned int len = 4;
							unsigned short * cbptr = (unsigned short *)&codebook[32 * *ptrs++];
							while (len > 0) {
								unsigned int w0 = cbptr[0];
								unsigned int w1 = cbptr[1];
								int pixels = w0 | (w1 << 16);
								cbptr += 2;
								unsigned char * dptr = dst;
								unsigned int rows = count;
								while (rows > 0) {
									*(unsigned int *)dptr = pixels;
									dptr += 8;
									--rows;
								}
								w0 = cbptr[0];
								w1 = cbptr[1];
								int pixels2 = w0 | (w1 << 16);
								cbptr += 2;
								unsigned char * dptr2 = dst + 4;
								unsigned int c2 = count;
								while (c2 > 0) {
									*(unsigned int *)dptr2 = pixels2;
									dptr2 += 8;
									--c2;
								}
								dst += bufwidth;
								--len;
							}
							dst += 8 * count - blocks_per_rowa;
						}
					} else {
						dst += 8 * count;
					}
				} else {
					int len = 2 * count;
					unsigned short pixel = HicolorTable[*ptrs++];
					int value = pixel | (pixel << 16);
					memset32(dst, value, len);
					memset32(&dst[bufwidth], value, len);
					memset32(&dst[2 * bufwidth], value, len);
					memset32(&dst[2 * bufwidth + bufwidth], value, len);
					dst += 8 * count;
				}
			} else {
				unsigned int len = 4;
				unsigned short * cbsrc = (unsigned short *)&codebook[32 * *ptrs++];
				while (len > 0) {
					unsigned short * dstw = (unsigned short *)dst;
					unsigned int c2 = 4;
					while (c2 > 0) {
						unsigned short cbpixel = *cbsrc;
						if ((cbpixel & 0x8000) == 0) {
							unsigned short * dstwalk = dstw;
							unsigned int c3 = count;
							while (c3 > 0) {
								*dstwalk = cbpixel;
								dstwalk += 4;
								--c3;
							}
						}
						++dstw;
						++cbsrc;
						--c2;
					}
					dst += bufwidth;
					--len;
				}

				dst += 8 * count - blocks_per_rowa;
			}
		} else {
			switch (code) {
			case 0x5000:
				if (count > 0) {
					int fillwidth = 4 * (bufwidth >> 2);
					while (count > 0) {
						unsigned int c2 = 4;
						unsigned short fpixel = HicolorTable[*ptrs];
						int fillpix = fpixel | (fpixel << 16);
						++ptrs;
						unsigned char * fillptr = dst;
						while (c2 > 0) {
							*(unsigned int *)fillptr = fillpix;
							*((unsigned int *)fillptr + 1) = fillpix;
							fillptr += fillwidth;
							--c2;
						}
						dst += 8;
						--count;
					}
				}
				break;
			case 0x6000:
				if (count > 0) {
					int rowwidth = 4 * (bufwidth >> 2);
					while (count > 0) {
						unsigned short * cbrow = (unsigned short *)&codebook[32 * *ptrs++];
						unsigned char * rowptr = dst;
						unsigned int c2 = 4;
						while (c2 > 0) {
							unsigned int pix0 = cbrow[0] | (cbrow[1] << 16);
							cbrow += 2;
							*(unsigned int *)rowptr = pix0;
							unsigned int pix1 = cbrow[0] | (cbrow[1] << 16);
							cbrow += 2;
							*((unsigned int *)rowptr + 1) = pix1;
							rowptr += rowwidth;
							--c2;
						}
						dst += 8;
						--count;
					}
				}
				break;
			case 0x7000:
				if (count > 0) {
					while (count > 0) {
						int c2 = 4;
						unsigned short * cbline = (unsigned short *)&codebook[32 * *ptrs++];
						unsigned char * lineptr = dst;
						while (c2) {
							unsigned short * linew = (unsigned short *)lineptr;
							unsigned int c3 = 4;
							while (c3 > 0) {
								if ((*cbline & 0x8000) == 0) {
									*linew = *cbline;
								}
								++linew;
								++cbline;
								--c3;
							}
							--c2;
							lineptr += 2 * (bufwidth >> 1);
						}
						dst += 8;
						--count;
					}
				}
				break;
			}
		}
		if (dst == last_buf_pos) {
			row_ptr += blocks_per_rowa;
			dst = row_ptr;
			last_buf_pos = &dst[last_pos];
		}
	}
}

// FUNCTION: GAMEMD 0x00748750
void __cdecl UnVQ2_4x2_Table(unsigned char * codebook, unsigned char * pointers, unsigned char * buffer, unsigned long blocksperrow, unsigned long numrows, unsigned long bufwidth)
{
	bufwidth *= 2u;
	unsigned int block_row_stride = bufwidth * 4u;
	unsigned char * data_end = buffer + (numrows * bufwidth * 4u);
	unsigned char * row_base = buffer;
	unsigned char * row_end = row_base + (blocksperrow * 8u);
	unsigned short * src = (unsigned short *)pointers;
	unsigned char * dst = row_base;
	while (dst < data_end) {
		unsigned short command = *src;
		int tag = command & 0xF000;
		unsigned int count = command;
		count &= 0xFFFF0FFFu;
		src += 1;
		switch (tag) {
		case 0x0000u: {
			unsigned short w = HicolorTable[*src];
			unsigned int v = (w << 16) | w;
			src += 1;
			memset32(dst, v, 2u * count);
			memset32(dst + 2u * bufwidth, v, 2u * count);
			dst += 8u * count;
		} break;
		case 0x1000u: {
			dst += 8u * count;
		} break;
		case 0x2000u: {
			const unsigned char * cb = codebook + ((unsigned int)*src << 5);
			src += 1;
			for (unsigned int r = 0; r < 2; r++) {
				unsigned int lo = ((unsigned int)*(const unsigned short *)(cb + 2) << 16) | *(const unsigned short *)(cb + 0);
				cb += 4;
				unsigned int i;
				unsigned char * p1 = dst;
				for (i = 0; i < count; i++) {
					*(unsigned int *)p1 = lo;
					p1 += 8;
				}
				unsigned int hi = ((unsigned int)*(const unsigned short *)(cb + 2) << 16) | *(const unsigned short *)(cb + 0);
				cb += 12;
				unsigned char * p2 = dst + 4;
				for (i = 0; i < count; i++) {
					*(unsigned int *)p2 = hi;
					p2 += 8;
				}
				dst += 2u * bufwidth;
			}
			dst += (count * 8u) - block_row_stride;
		} break;
		case 0x3000u: {
			unsigned short cb_index = *src;
			const unsigned char * cb = codebook + 32u * cb_index;
			unsigned int r;
			src += 1;
			for (r = 0; r < 2; ++r) {
				unsigned char * rowp = dst;
				unsigned int c;
				for (c = 0; c < 4; ++c) {
					unsigned short px = *(const unsigned short *)cb;
					if ((px & 0x8000u) == 0) {
						unsigned char * p = rowp;
						unsigned int i;
						for (i = 0; i < count; ++i) {
							*(unsigned short *)p = px;
							p += 8;
						}
					}
					rowp += 2;
					cb += 2;
				}
				cb += 8;
				dst += 2u * bufwidth;
			}
			dst += 8u * count - block_row_stride;
		} break;
		case 0x5000u: {
			unsigned int i;
			for (i = 0; i < count; i++) {
				unsigned int step = 4u * (bufwidth >> 1);
				unsigned int j;
				unsigned int v;
				unsigned int r = 2;
				unsigned char * p;
				unsigned short w = HicolorTable[*src];
				v = (w << 16) | w;
				src += 1;
				p = dst;
				for (j = 0; j < r; j++) {
					*(unsigned int *)(p + 0) = v;
					*(unsigned int *)(p + 4) = v;
					p += step;
				}
				dst += 8;
			}
		} break;
		case 0x6000u: {
			unsigned int i;
			for (i = 0; i < count; i++) {
				unsigned int step = 4u * (bufwidth >> 1);
				const unsigned char * cb = codebook + ((unsigned int)*src << 5);
				src += 1;
				unsigned char * p = dst;
				for (unsigned int r = 0; r < 2; ++r) {
					unsigned int lo;
					unsigned int hi;
					lo = ((unsigned int)*(const unsigned short *)(cb + 2) << 16) | *(const unsigned short *)(cb + 0);
					cb += 4;
					*(unsigned int *)(p + 0) = lo;
					hi = ((unsigned int)*(const unsigned short *)(cb + 2) << 16) | *(const unsigned short *)(cb + 0);
					cb += 12;
					*(unsigned int *)(p + 4) = hi;
					p += step;
				}
				dst += 8;
			}
		} break;
		case 0x7000u: {
			unsigned int i;
			for (i = 0; i < count; ++i) {
				unsigned short cb_index = *src;
				const unsigned char * cb = codebook + 32u * cb_index;
				src += 1;
				unsigned char * p = dst;
				for (unsigned int r = 0; r < 2; ++r) {
					unsigned char * q = p;
					for (unsigned int c = 0; c < 4; ++c) {
						unsigned short px = *(const unsigned short *)cb;
						if ((px & 0x8000u) == 0) {
							*(unsigned short *)q = px;
						}
						q += 2;
						cb += 2;
					}
					p += 2u * bufwidth;
				}
				dst += 8;
			}
		} break;
		}
		if (dst == row_end) {
			row_base += block_row_stride;
			dst = row_base;
			row_end = row_base + (8u * blocksperrow);
		}
	}
}

// FUNCTION: GAMEMD 0x00748AF0
void __cdecl UnVQ1_4x4_Table(unsigned char *codebook, unsigned char *pointers, unsigned char *buffer,
                             unsigned long blocks_per_row, unsigned long rows, unsigned long width)
{
	width *= 2;
	unsigned int block_row_stride = width * 4;
	unsigned char *end = buffer + rows * width * 4;
	unsigned char *dst = buffer;
	unsigned char *row_base = buffer;
	unsigned char *row_end = row_base + blocks_per_row * 8;
	unsigned short *src = (unsigned short *)pointers;
	while (dst < end) {
		unsigned int command = (*src & 0xE000);
		unsigned short cb_index = (*src & 0x1FFF);
		src += 1;
		switch (command) {
		case 0x0000: {
			unsigned char const *cb = codebook + (cb_index << 5);
			for (unsigned int r = 0; r < 2; r++) {
				unsigned int lo;
				unsigned int hi;
				lo = ((unsigned int)*(unsigned short const *)(cb + 2) << 16) | *(unsigned short const *)(cb + 0);
				cb += 4;
				*(unsigned int *)(dst + 0) = lo;
				hi = ((unsigned int)*(unsigned short const *)(cb + 2) << 16) | *(unsigned short const *)(cb + 0);
				cb += 12;
				*(unsigned int *)(dst + 4) = hi;
				dst += width * 2;
			}
			dst += 8 - block_row_stride;
		} break;
		case 0x2000: {
			unsigned short const *cb = (unsigned short const *)(codebook + (cb_index << 5));
			for (unsigned int r = 0; r < 2; r++) {
				unsigned short *p = (unsigned short *)dst;
				for (unsigned int c = 0; c < 4; c++) {
					if (!(*cb & 0x8000))
						*p = *cb;
					p++;
					cb++;
				}
				cb += 4;
				dst += width * 2;
			}
			dst += 8 - block_row_stride;
		} break;
		case 0x4000: {
			dst += 8;
		} break;
		default:
			break;
		}
		if (dst == row_end) {
			row_base += block_row_stride;
			dst = row_base;
			row_end = row_base + blocks_per_row * 8;
		}
	}
}

// FUNCTION: GAMEMD 0x00748C30
void __cdecl UnVQ1_4x2_Table(unsigned char *codebook, unsigned char *pointers, unsigned char *buffer,
                             unsigned long blocks_per_row, unsigned long rows, unsigned long width)
{
	width *= 2;
	unsigned int block_row_stride = width * 4;
	unsigned char *end = buffer + rows * width * 4;
	unsigned char *dst = buffer;
	unsigned char *row_base = buffer;
	unsigned char *row_end = row_base + blocks_per_row * 8;
	unsigned short *src = (unsigned short *)pointers;
	while (dst < end) {
		unsigned int count = 0;
		unsigned int index;
		unsigned int command = (*src & 0xE000);
		unsigned short cb_index = (*src & 0x1FFF);
		src += 1;
		switch (command) {
		case 0x0000: {
			dst += (cb_index & 0xFF) * 8;
		} break;
		case 0x2000: {
			unsigned int code = cb_index;
			count = ((code >> 7) & 0x3E) + 2;
			index = code & 0xFF;
		}
		// A run with no count in its command word reads the count and index from the stream.
		case 0xA000: {
			if (count == 0) {
				count = *(unsigned char *)src;
				index = cb_index;
				src = (unsigned short *)((unsigned char *)src + 1);
			}
			unsigned char const *cb = codebook + (index << 5);
			for (unsigned int r = 0; r < 2; r++) {
				unsigned int lo = ((unsigned int)*(unsigned short const *)(cb + 2) << 16) | *(unsigned short const *)(cb + 0);
				cb += 4;
				unsigned char *p = dst;
				for (unsigned int n = 0; n < count; n++) {
					*(unsigned int *)p = lo;
					p += 8;
				}
				unsigned int hi = ((unsigned int)*(unsigned short const *)(cb + 2) << 16) | *(unsigned short const *)(cb + 0);
				cb += 12;
				p = dst + 4;
				for (unsigned int n2 = 0; n2 < count; n2++) {
					*(unsigned int *)p = hi;
					p += 8;
				}
				dst += width * 2;
			}
			dst += 8 * count - block_row_stride;
		} break;
		case 0x4000: {
			count = ((cb_index >> 7) & 0x3E) + 2;
			unsigned char const *cb = codebook + (*((unsigned char *)src - 2) << 5);
			unsigned int stride = width >> 2;
			unsigned char *p = dst;
			for (unsigned int r = 0; r < 2; r++) {
				unsigned int lo = ((unsigned int)*(unsigned short const *)(cb + 2) << 16) | *(unsigned short const *)(cb + 0);
				cb += 4;
				*(unsigned int *)(p + 0) = lo;
				unsigned int hi = ((unsigned int)*(unsigned short const *)(cb + 2) << 16) | *(unsigned short const *)(cb + 0);
				cb += 12;
				*(unsigned int *)(p + 4) = hi;
				p += stride * 4;
			}
			dst += 8;
			unsigned char *bytes = (unsigned char *)src;
			for (unsigned int n = 0; n < count; n++) {
				unsigned int next = *bytes;
				bytes++;
				unsigned char const *cb2 = codebook + (next << 5);
				unsigned char *q = dst;
				for (unsigned int r2 = 0; r2 < 2; r2++) {
					unsigned int lo = ((unsigned int)*(unsigned short const *)(cb2 + 2) << 16) | *(unsigned short const *)(cb2 + 0);
					cb2 += 4;
					*(unsigned int *)(q + 0) = lo;
					unsigned int hi = ((unsigned int)*(unsigned short const *)(cb2 + 2) << 16) | *(unsigned short const *)(cb2 + 0);
					cb2 += 12;
					*(unsigned int *)(q + 4) = hi;
					q += stride * 8;
				}
				dst += 8;
			}
			src = (unsigned short *)bytes;
		} break;
		case 0x6000: {
			unsigned char const *cb = codebook + (cb_index << 5);
			for (unsigned int r = 0; r < 4; r++) {
				unsigned int lo = ((unsigned int)*(unsigned short const *)(cb + 2) << 16) | *(unsigned short const *)(cb + 0);
				cb += 4;
				*(unsigned int *)(dst + 0) = lo;
				unsigned int hi = ((unsigned int)*(unsigned short const *)(cb + 2) << 16) | *(unsigned short const *)(cb + 0);
				cb += 12;
				*(unsigned int *)(dst + 4) = hi;
				dst += width * 2;
			}
			dst += 8 - block_row_stride;
		} break;
		case 0x8000: {
			unsigned short const *cb = (unsigned short const *)(codebook + (cb_index << 5));
			for (unsigned int r = 0; r < 4; r++) {
				unsigned short *p = (unsigned short *)dst;
				for (unsigned int c = 0; c < 4; c++) {
					if (!(*cb & 0x8000))
						*p = *cb;
					p++;
					cb++;
				}
				cb += 4;
				dst += width * 2;
			}
			dst += 8 - block_row_stride;
		} break;
		case 0xC000: {
			count = *(unsigned char *)src;
			index = cb_index;
			src = (unsigned short *)((unsigned char *)src + 1);
			unsigned short const *cb = (unsigned short const *)(codebook + (index << 5));
			for (unsigned int r = 0; r < 2; r++) {
				unsigned short *p = (unsigned short *)dst;
				for (unsigned int c = 0; c < 4; c++) {
					unsigned short pixel = *cb;
					if (!(*cb & 0x8000)) {
						unsigned short *q = p;
						for (unsigned int n = 0; n < count; n++) {
							*q = pixel;
							q += 4;
						}
					}
					p++;
					cb++;
				}
				cb += 4;
				dst += width * 2;
			}
			dst += 8 * count - block_row_stride;
		} break;
		default:
			break;
		}
		if (dst == row_end) {
			row_base += block_row_stride;
			dst = row_base;
			row_end = row_base + blocks_per_row * 8;
		}
	}
}
