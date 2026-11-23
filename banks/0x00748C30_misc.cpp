// best 91.8% raw / 91.2% normalised (923/923 B), no overlay; append to src/unvqtblc.cpp. NOT landable as it stands: `pcode` is a dead pointer whose only job is to make `code` address-taken, which stops VC6 hoisting the constant 2 into EBP and fixes the whole 0xC000 case. Left: ECX/EDX swapped between count and the codebook pointer through the 0x2000 case. With the old head (count = ((cb_index >> 7) & 0x3E) + 2; cb_byte = (unsigned char)cb_index;) the swap goes and only four head instructions differ (89.3% raw, 98.8% normalised, 917 B).
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
	unsigned int count;
	unsigned int cb_byte;
	unsigned int code;
	unsigned int * pcode = &code;
	while (dst < end) {
		unsigned int command = (*src & 0xE000);
		unsigned short cb_index = (*src & 0x1FFF);
		src += 1;
		switch (command) {
		case 0: {
					dst += (cb_index & 0xFF) * 8;
		} break;
		case 0x2000: {
						code = cb_index;
						code &= 0xFFFF;
						count = ((code >> 7) & 0x3E) + 2;
						cb_byte = code & 0xFF;
						if (count == 0) {
					extended:
							count = *(unsigned char *)src;
							cb_byte = cb_index;
							src = (unsigned short *)((unsigned char *)src + 1);
						}
						unsigned char const *cb = codebook + (cb_byte << 5);
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
								unsigned int idx = *bytes;
								bytes++;
								unsigned char const *cb2 = codebook + (idx << 5);
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
		case 0xA000: goto extended;
		case 0xC000: {
				code = cb_index;
				count = *(unsigned char *)src;
				src = (unsigned short *)((unsigned char *)src + 1);
				unsigned short const *cb = (unsigned short const *)(codebook + (code << 5));
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
		}
		if (dst == row_end) {
			row_base += block_row_stride;
			dst = row_base;
			row_end = row_base + blocks_per_row * 8;
		}
	}
}
