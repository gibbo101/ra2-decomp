// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include <string.h>

typedef signed char int8_t;
typedef signed short int16_t;
typedef signed int int32_t;

typedef unsigned char uint8_t;
typedef unsigned short uint16_t;
typedef unsigned int uint32_t;

// FUNCTION: GAMEMD 0x007C3D60
void __cdecl UnVQ1_C4_4x4(unsigned char * codebook, unsigned char * pointers, unsigned char * buffer, unsigned long blocksperrow, unsigned long numrows, unsigned long bufwidth)
{
	bufwidth *= 2u;
	uint32_t block_row_stride = bufwidth * 4u;

	uint8_t * end = (uint8_t *)buffer + ((uint32_t)numrows * bufwidth * 4u);
	uint8_t * dst = (uint8_t *)buffer;

	uint8_t * row_base = (uint8_t *)buffer;
	uint8_t * row_end = row_base + ((uint32_t)blocksperrow * 8u);

	uint16_t * src = (uint16_t *)pointers;

	while (dst < end) {
		uint32_t command = (*src & 0xE000);
		uint16_t cb_index = (*src & 0x1FFF);

		src += 1;

		switch (command) {
		case 0x0000u: {
			const uint8_t * cb = (const uint8_t *)codebook + (cb_index << 5);

			for (uint32_t r = 0; r < 4; r++) {
				uint32_t lo;
				uint32_t hi;

				lo = ((uint32_t)*(const uint16_t *)(cb + 2) << 16) | *(const uint16_t *)(cb + 0);
				cb += 4;
				*(uint32_t *)(dst + 0) = lo;

				hi = ((uint32_t)*(const uint16_t *)(cb + 2) << 16) | *(const uint16_t *)(cb + 0);
				cb += 4;
				*(uint32_t *)(dst + 4) = hi;

				dst += bufwidth;
			}

			dst += 8u - block_row_stride;
		} break;

		case 0x2000u: {
			const uint8_t * cb = (const uint8_t *)codebook + (cb_index << 5);

			for (uint32_t r = 0; r < 4; r++) {
				uint8_t * p = dst;

				for (uint32_t c = 0; c < 4; c++) {
					uint16_t v = *(const uint16_t *)cb;

					if (v != 0x8000u) {
						*(uint16_t *)p = v;
					}

					p += 2;
					cb += 2;
				}

				dst += bufwidth;
			}

			dst += 8u - block_row_stride;
		} break;

		case 0x4000u: {
			dst += 8;
		} break;

		default:
			break;
		}

		if (dst == row_end) {
			row_base += block_row_stride;
			dst = row_base;
			row_end = row_base + ((uint32_t)blocksperrow * 8u);
		}
	}
}

// FUNCTION: GAMEMD 0x007C4260
void __cdecl UnVQ1_C4_4x2(unsigned char * codebook, unsigned char * pointers, unsigned char * buffer, unsigned long blocksperrow, unsigned long numrows, unsigned long bufwidth)
{
	bufwidth *= 2u;
	uint32_t block_row_stride = bufwidth * 2u;

	uint8_t * end = (uint8_t *)buffer + ((uint32_t)numrows * bufwidth * 2u);
	uint8_t * dst = (uint8_t *)buffer;

	uint8_t * row_base = (uint8_t *)buffer;
	uint8_t * row_end = row_base + ((uint32_t)blocksperrow * 8u);

	uint16_t * src = (uint16_t *)pointers;

	while (dst < end) {
		uint32_t command = (*src & 0xE000);
		uint16_t cb_index = (*src & 0x1FFF);

		src += 1;

		switch (command) {
		case 0x0000u: {
			const uint8_t * cb = (const uint8_t *)codebook + (cb_index << 4);

			for (uint32_t r = 0; r < 2; r++) {
				uint32_t lo;
				uint32_t hi;

				lo = ((uint32_t)*(const uint16_t *)(cb + 2) << 16) | *(const uint16_t *)(cb + 0);
				cb += 4;
				*(uint32_t *)(dst + 0) = lo;

				hi = ((uint32_t)*(const uint16_t *)(cb + 2) << 16) | *(const uint16_t *)(cb + 0);
				cb += 4;
				*(uint32_t *)(dst + 4) = hi;

				dst += bufwidth;
			}

			dst += 8u - block_row_stride;
		} break;

		case 0x2000u: {
			const uint8_t * cb = (const uint8_t *)codebook + (cb_index << 4);

			for (uint32_t r = 0; r < 2; r++) {
				uint8_t * p = dst;

				for (uint32_t c = 0; c < 4; c++) {
					uint16_t v = *(const uint16_t *)cb;

					if (v != 0x8000u) {
						*(uint16_t *)p = v;
					}

					p += 2;
					cb += 2;
				}

				dst += bufwidth;
			}

			dst += 8u - block_row_stride;
		} break;

		case 0x4000u: {
			dst += 8;
		} break;

		default:
			break;
		}

		if (dst == row_end) {
			row_base += block_row_stride;
			dst = row_base;
			row_end = row_base + ((uint32_t)blocksperrow * 8u);
		}
	}
}

// FUNCTION: GAMEMD 0x007C43A0
void __cdecl UnVQ2_C4_4x2(unsigned char * codebook, unsigned char * pointers, unsigned char * buffer, unsigned long blocksperrow, unsigned long numrows, unsigned long bufwidth)
{
	bufwidth *= 2u;
	uint32_t block_row_stride = bufwidth * 2u;

	uint8_t * end = (uint8_t *)buffer + ((uint32_t)numrows * bufwidth * 2u);
	uint8_t * dst = (uint8_t *)buffer;

	uint8_t * row_base = (uint8_t *)buffer;
	uint8_t * row_end = row_base + ((uint32_t)blocksperrow * 8u);

	uint8_t * src = pointers;

	uint32_t height;
	uint32_t code;
	uint32_t idx;

	while (dst < end) {

		uint32_t command = (*(uint16_t *)src & 0xE000);
		uint16_t cb_index = (*(uint16_t *)src & 0x1FFF);
		src += 2;

		switch (command) {

		case 0x4000u: {
			uint32_t repeat = ((cb_index >> 7) & 0x3Eu) + 2u;
			const uint8_t * cb = codebook + (*(src - 2) << 4);
			uint8_t * rowp = dst;
			uint32_t step = 4u * (bufwidth >> 2);

			for (uint32_t r = 0; r < 2; ++r) {
				uint32_t lo = ((uint32_t)*(uint16_t *)(cb + 2) << 16) | *(uint16_t *)(cb + 0);
				cb += 4;
				*(uint32_t *)(rowp + 0) = lo;

				uint32_t hi = ((uint32_t)*(uint16_t *)(cb + 2) << 16) | *(uint16_t *)(cb + 0);
				cb += 4;
				*(uint32_t *)(rowp + 4) = hi;

				rowp += step;
			}

			uint8_t * cursor = src;
			dst += 8;

			for (int32_t i = 0; i < repeat; i++) {
				uint32_t idx = *cursor;
				cursor += 1;
				uint8_t * p = dst;
				const uint8_t * cb2 = codebook + (idx << 4);

				for (uint32_t r = 0; r < 2; ++r) {
					uint32_t lo = ((uint32_t)*(uint16_t *)(cb2 + 2) << 16) | *(uint16_t *)(cb2 + 0);
					cb2 += 4;
					*(uint32_t *)(p + 0) = lo;

					uint32_t hi = ((uint32_t)*(uint16_t *)(cb2 + 2) << 16) | *(uint16_t *)(cb2 + 0);
					cb2 += 4;
					*(uint32_t *)(p + 4) = hi;

					p += step;
				}

				dst += 8;
			}

			src = cursor;
		} break;

		case 0x2000u: {
			code = cb_index;
			code &= 0xFFFFu;
			height = ((code >> 7) & 0x3Eu) + 2u;
		label_check:
			if (height == 0u) {
				idx = cb_index;
				height = *src;
				src += 1;
				goto label_vertical_fill;
			} else {
				goto label_masked_index;
			}

		}

		case 0x0000u: {
			dst += 8u * (uint8_t)cb_index;
		} break;

		case 0x6000u: {
			const uint8_t * cb = codebook + ((cb_index & 0xFFFFu) << 4);

			for (uint32_t r = 0; r < 2; ++r) {
				uint32_t lo = ((uint32_t)*(uint16_t *)(cb + 2) << 16) | *(uint16_t *)(cb + 0);
				cb += 4;
				*(uint32_t *)(dst + 0) = lo;

				uint32_t hi = ((uint32_t)*(uint16_t *)(cb + 2) << 16) | *(uint16_t *)(cb + 0);
				cb += 4;
				*(uint32_t *)(dst + 4) = hi;

				dst += bufwidth;
			}

			dst += 8u - block_row_stride;
		} break;

		case 0xC000u: {
			height = *src;
			src += 1;

			const uint8_t * cb = codebook + (16u * (uint16_t)cb_index);

			for (uint32_t r = 0; r < 2; ++r) {
				uint8_t * p = dst;

				for (uint32_t c = 0; c < 4; ++c) {
					uint16_t v = *(uint16_t *)cb;

					if (v != 0x8000u) {
						uint8_t * q = p;
						for (uint32_t i = 0; i < height; i++) {
							*(uint16_t *)q = v;
							q += 8;
						}
					}

					p += 2;
					cb += 2;
				}

				dst += bufwidth;
			}

			dst += 8u * height - block_row_stride;
		} break;

		case 0xA000u:
			height = 0;
			goto label_check;

		label_masked_index:
			idx = code & 0xFFu;

		label_vertical_fill:
			{
				const uint8_t * cb = codebook + (16u * idx);

				for (uint32_t r = 0; r < 2; ++r) {

					uint32_t lo = ((uint32_t)*(uint16_t *)(cb + 2) << 16) | *(uint16_t *)(cb + 0);
					cb += 4;

					{
						uint8_t * p = dst;
						uint32_t i;
						for (i = 0; i < height; i++) {
							*(uint32_t *)p = lo;
							p += 8;
						}
					}

					uint32_t hi = ((uint32_t)*(uint16_t *)(cb + 2) << 16) | *(uint16_t *)(cb + 0);
					cb += 4;

					{
						uint8_t * p = dst + 4;
						uint32_t i;
						for (i = 0; i < height; i++) {
							*(uint32_t *)p = hi;
							p += 8;
						}
					}

					dst += bufwidth;
				}

				dst += 8u * height - block_row_stride;
			}
			break;

		case 0x8000u: {
			const uint8_t * cb = codebook + (16u * (uint16_t)cb_index);

			for (uint32_t r = 0; r < 2; ++r) {
				uint8_t * p = dst;

				for (uint32_t c = 0; c < 4; ++c) {
					if (*(uint16_t *)cb != 0x8000u) {
						*(uint16_t *)p = *(uint16_t *)cb;
					}
					p += 2;
					cb += 2;
				}

				dst += bufwidth;
			}

			dst += 8u - block_row_stride;
		} break;
		}

		if (dst == row_end) {
			row_base += block_row_stride;
			dst = row_base;
			row_end = row_base + ((uint32_t)blocksperrow * 8u);
		}
	}
}

// FUNCTION: GAMEMD 0x007C4B80
void __cdecl UnVQ2_C0_4x4_KEY(unsigned char * codebook, unsigned char * pointers, unsigned char * buffer, unsigned long blocksperrow, unsigned long numrows, unsigned long bufwidth)
{
	uint16_t * src = (uint16_t *)pointers;
	uint8_t * dst = (uint8_t *)buffer;
	uint8_t * row_base = (uint8_t *)buffer;
	uint32_t blocks = 0;
	uint32_t total = numrows * blocksperrow;
	int r;
	uint32_t count;

	if (total > 0u) {

		while (1) {

			uint16_t word = *src;

			if ((word & 0x8000u) != 0) {

				int32_t op = word & 0xF000u;

				if (op <= 0xC000) {

					if (op != 0xC000) {

						switch (op) {

						case 0xB000: {
							count = ((uint32_t)word >> 8) & 0xFu;

							{
								uint32_t n = 4u * count;

								for (r = 0; r < 4; ++r) {
									for (uint32_t k = 0; k < n; ++k) {
										*dst++ = (uint8_t)word;
									}
									dst -= n;
									dst += bufwidth;
								}

								dst -= 4u * bufwidth;
							}

							dst += 4u * count;
							blocks += count;
							src += 1;
						} break;

						case 0xA000: {
							count = word & 0xFFFu;
							src += 1;
							uint16_t word2 = *src;

							if ((word2 & 0x8000u) != 0) {
								int sub = word2 & 0xC000;

								switch (sub) {

								case 0xC000: {
									const uint8_t * cb = codebook + ((uint32_t)(word2 & 0xFFFu) << 4);

									for (r = 0; r < 4; ++r) {
										uint32_t nn;
										for (nn = 0; nn < count; nn++) {
											for (int c = 0; c < 4; ++c) {
												*dst = *cb;
												++dst;
												++cb;
											}
											cb -= 4;
										}
										cb += 4;
										dst -= 4u * count;
										dst += bufwidth;
									}

									dst += 4u * count;
									dst -= 4u * bufwidth;
									src += 1;
									blocks += count;

								} break;

								case 0x8000: {
									{
										uint32_t n = 4u * count;

										for (r = 0; r < 4; ++r) {
											for (uint32_t k = 0; k < n; ++k) {
												*dst++ = (uint8_t)word2;
											}
											dst -= n;
											dst += bufwidth;
										}

										dst -= 4u * bufwidth;
									}

									dst += 4u * count;
									blocks += count;
									src += 1;

								} break;
								}

							} else {
								const uint8_t * cb = codebook + ((uint32_t)word2 << 4);

								for (r = 0; r < 4; ++r) {
									uint32_t nn;
									for (nn = 0; nn < count; nn++) {
										for (int c = 0; c < 4; ++c) {
											*dst++ = *cb++;
										}
										cb -= 4;
									}
									cb += 4;
									dst -= 4u * count;
									dst += bufwidth;
								}

								dst += 4u * count;
								dst -= 4u * bufwidth;
								src += 1;
								blocks += count;
							}

						} break;
						}

					} else {
						count = word & 0xFFFu;
						src += 1;
						for (uint32_t pairs = count >> 1; pairs != 0u; pairs--) {
							uint16_t word2 = *src;
							uint8_t hi = (uint8_t)(word2 >> 8);

							for (r = 0; r < 4; ++r) {
								for (int c = 0; c < 4; ++c) {
									*dst++ = hi;
								}
								dst -= 4u;
								dst += bufwidth;
							}
							dst -= 4u * bufwidth;
							dst += 4;

							uint8_t lo = (uint8_t)word2 & 0xFF;

							for (r = 0; r < 4; ++r) {
								for (int c = 0; c < 4; ++c) {
									*dst++ = lo;
								}
								dst -= 4u;
								dst += bufwidth;
							}
							dst -= 4u * bufwidth;
							dst += 4;

							src += 1;
						}

						blocks += count;
					}

				} else {

					switch (op) {

					case 0xF000: {
						for (r = 0; r < 4; ++r) {
							for (int c = 0; c < 4; ++c) {
								*dst++ = (uint8_t)word;
							}
							dst -= 4u;
							dst += bufwidth;
						}
						dst -= 4u * bufwidth;

						dst += 4;
						src += 1;
						blocks += 1;
					} break;

					case 0xE000: {
						const uint8_t * cb = codebook + ((uint32_t)(word & 0xFFFu) << 4);

						for (r = 0; r < 4; ++r) {
							for (int c = 0; c < 4; ++c) {
								*dst = *cb;
								++dst;
								++cb;
							}
							dst -= 4u;
							dst += bufwidth;
						}

						dst -= 4u * bufwidth;

						dst += 4u;
						src += 1;
						blocks += 1;

					} break;
					}
				}

			} else {
				const uint8_t * cb = codebook + ((uint32_t)*src << 4);

				for (r = 0; r < 4; ++r) {
					for (int c = 0; c < 4; ++c) {
						*dst++ = *cb++;
					}
					dst -= 4u;
					dst += bufwidth;
				}

				dst -= 4u * bufwidth;

				dst += 4u;
				src += 1;
				blocks += 1;
			}

			if (blocks % blocksperrow == 0u) {
				dst = row_base + 4u * bufwidth;
				row_base = dst;
			}

			if (blocks >= total) {
				return;
			}
		}
	}
}

// FUNCTION: GAMEMD 0x007C4F90
void __cdecl UnVQ2_C0_4x4_TRANS_HALF(unsigned char * codebook, unsigned char * pointers, unsigned char * buffer, unsigned long blocksperrow, unsigned long numrows, unsigned long bufwidth)
{
	uint16_t * src = (uint16_t *)pointers;
	uint8_t * dst = (uint8_t *)buffer;
	uint32_t blocks = 0;
	uint8_t * row_base = (uint8_t *)buffer;
	uint32_t total = numrows * blocksperrow;
	int r;
	uint32_t count;

	if (total > 0u) {

		while (1) {

			uint16_t word = *src;

			if ((word & 0x8000u) != 0) {

				int op = (word & 0xFFFF) & 0xF000;

				if (op <= 0xC000) {

					if (op != 0xC000) {

						switch (op) {

						case 0xB000: {
							count = ((uint32_t)word >> 8) & 0xFu;

							if ((uint8_t)word) {
								uint32_t n = 2u * count;

								for (r = 0; r < 2; ++r) {
									for (uint32_t k = 0; k < n; ++k) {
										*dst++ = (uint8_t)word;
									}
									dst -= n;
									dst += bufwidth;
								}

								dst -= 2u * bufwidth;
							}

							dst += 2u * count;
							blocks += count;
							src += 1;
						} break;

						case 0xA000: {
							src += 1;
							uint16_t word2 = *src;
							count = word & 0xFFFu;

							if ((word2 & 0x8000u) != 0) {
								int sub = word2 & 0xC000;

								switch (sub) {

								case 0xC000: {
									const uint8_t * cb = codebook + ((uint32_t)(word2 & 0xFFFu) << 4);

									for (r = 0; r < 2; ++r) {
										uint32_t nn;
										for (nn = 0; nn < count; nn++) {
											for (int c = 0; c < 2; ++c) {
												if (*cb) *dst = *cb;
												++dst;
												cb += 2;
											}
											cb -= 4;
										}
										cb += 4;
										dst -= 2u * count;
										dst += bufwidth;
									}

									dst += 2u * count;
									dst -= 2u * bufwidth;
									src += 1;
									blocks += count;
								} break;

								case 0x8000: {
									if ((uint8_t)word2) {
										uint32_t n = 2u * count;

										for (r = 0; r < 2; ++r) {
											for (uint32_t k = 0; k < n; ++k) {
												*dst++ = (uint8_t)word2;
											}
											dst -= n;
											dst += bufwidth;
										}

										dst -= 2u * bufwidth;
									}

									dst += 2u * count;
									blocks += count;
									src += 1;

								} break;
								}

							} else {
								const uint8_t * cb = codebook + ((uint32_t)word2 << 4);

								for (r = 0; r < 2; ++r) {
									uint32_t nn;
									for (nn = 0; nn < count; nn++) {
										for (int c = 0; c < 2; ++c) {
											*dst++ = *cb;
											cb += 2;
										}
										cb -= 4;
									}
									cb += 4;
									dst -= 2u * count;
									dst += bufwidth;
								}

								dst += 2u * count;
								dst -= 2u * bufwidth;
								src += 1;
								blocks += count;
							}

						} break;
						}

					} else {
						count = word & 0xFFFu;
						src += 1;
						for (uint32_t pairs = count >> 1; pairs != 0u; pairs--) {
							uint16_t word2 = *src;
							uint8_t hi = (uint8_t)(word2 >> 8);

							if (hi != 0u) {
								for (r = 0; r < 2; ++r) {
									for (int c = 0; c < 2; ++c) {
										*dst++ = hi;
									}
									dst -= 2u;
									dst += bufwidth;
								}
								dst -= 2u * bufwidth;
							}
							dst += 2;

							if ((uint8_t)word2 != 0u) {
								for (r = 0; r < 2; ++r) {
									for (int c = 0; c < 2; ++c) {
										*dst++ = (uint8_t)word2;
									}
									dst -= 2u;
									dst += bufwidth;
								}
								dst -= 2u * bufwidth;
							}
							dst += 2;

							src += 1;
						}

						blocks += count;
					}

				} else {

					switch (op) {

					case 0xF000: {
						if ((uint8_t)word) {
							for (r = 0; r < 2; ++r) {
								for (int c = 0; c < 2; ++c) {
									*dst++ = (uint8_t)word;
								}
								dst -= 2u;
								dst += bufwidth;
							}
							dst -= 2u * bufwidth;
						}

						dst += 2;
						src += 1;
						blocks += 1;
					} break;

					case 0xE000: {
						const uint8_t * cb = codebook + ((uint32_t)(word & 0xFFFu) << 4);

						for (r = 0; r < 2; ++r) {
							for (int c = 0; c < 2; ++c) {
								if (*cb) *dst = *cb;
								++dst;
								cb += 2;
							}
							dst -= 2u;
							dst += bufwidth;
						}

						dst -= 2u * bufwidth;

						dst += 2u;
						src += 1;
						blocks += 1;

					} break;
					}
				}

			} else {
				const uint8_t * cb = codebook + ((uint32_t)*src << 4);

				for (r = 0; r < 2; ++r) {
					for (int c = 0; c < 2; ++c) {
						*dst++ = *cb;
						cb += 2;
					}
					dst -= 2u;
					dst += bufwidth;
				}

				dst -= 2u * bufwidth;

				dst += 2u;
				src += 1;
				blocks += 1;
			}

			if (blocks % blocksperrow == 0u) {
				dst = row_base + 2u * bufwidth;
				row_base = dst;
			}

			if (blocks >= total) {
				return;
			}
		}
	}
}

// FUNCTION: GAMEMD 0x007C53A0
void __cdecl UnVQ2_C0_4x2_TRANS(unsigned char * codebook, unsigned char * pointers, unsigned char * buffer, unsigned long blocksperrow, unsigned long numrows, unsigned long bufwidth)
{
	uint16_t * src = (uint16_t *)pointers;
	uint8_t * dst = (uint8_t *)buffer;
	uint32_t blocks = 0;
	uint8_t * row_base = (uint8_t *)buffer;
	uint32_t total = numrows * blocksperrow;
	int r;
	uint32_t count;
	uint32_t pairs;

	if (total > 0u) {

		while (1) {

			uint16_t word = *src;

			if ((word & 0x8000u) != 0) {

				int32_t op = word & 0xF000u;

				if (op <= 0xC000) {

					if (op != 0xC000) {

						switch (op) {

						case 0xB000: {
							count = ((uint32_t)word >> 8) & 0xFu;

							if ((uint8_t)word) {
								uint32_t n = 4u * count;

								for (r = 0; r < 2; ++r) {
									for (uint32_t k = 0; k < n; ++k) {
										*dst++ = (uint8_t)word;
									}
									dst -= n;
									dst += bufwidth;
								}

								dst -= 2u * bufwidth;
							}

							dst += 4u * count;
							blocks += count;
							src += 1;
						} break;

						case 0xA000: {
							src += 1;
							uint16_t word2 = *src;
							count = word & 0xFFFu;

							if ((word2 & 0x8000u) != 0) {
								int sub = word2 & 0xC000;

								switch (sub) {

								case 0xC000: {
									const uint8_t * cb = codebook + ((uint32_t)(word2 & 0xFFFu) << 3);

									for (r = 0; r < 2; ++r) {
										uint32_t nn;
										for (nn = 0; nn < count; nn++) {
											for (int c = 0; c < 4; ++c) {
												if (*cb) *dst = *cb;
												++dst;
												++cb;
											}
											cb -= 4;
										}
										cb += 4;
										dst -= 4u * count;
										dst += bufwidth;
									}

									dst += 4u * count;
									dst -= 2u * bufwidth;
									src += 1;
									blocks += count;

								} break;

								case 0x8000: {
									if ((uint8_t)word2) {
										uint32_t n = 4u * count;

										for (r = 0; r < 2; ++r) {
											for (uint32_t k = 0; k < n; ++k) {
												*dst++ = (uint8_t)word2;
											}
											dst -= n;
											dst += bufwidth;
										}

										dst -= 2u * bufwidth;
									}

									dst += 4u * count;
									blocks += count;
									src += 1;

								} break;
								}

							} else {
								const uint8_t * cb = codebook + ((uint32_t)word2 << 3);

								for (r = 0; r < 2; ++r) {
									uint32_t nn;
									for (nn = 0; nn < count; nn++) {
										for (int c = 0; c < 4; ++c) {
											*dst++ = *cb++;
										}
										cb -= 4;
									}
									cb += 4;
									dst -= 4u * count;
									dst += bufwidth;
								}

								dst += 4u * count;
								dst -= 2u * bufwidth;
								src += 1;
								blocks += count;
							}

						} break;
						}

					} else {
						count = word & 0xFFFu;
						src += 1;
						for (pairs = count >> 1; pairs != 0u; pairs--) {
							uint16_t word2 = *src;
							uint8_t hi = (uint8_t)(word2 >> 8);

							if (hi != 0u) {
								for (r = 0; r < 2; ++r) {
									for (int c = 0; c < 4; ++c) {
										*dst++ = hi;
									}
									dst -= 4u;
									dst += bufwidth;
								}
								dst -= 2u * bufwidth;
							}
							dst += 4;

							uint8_t lo = (uint8_t)word2 & 0xFF;

							if (lo != 0u) {
								for (r = 0; r < 2; ++r) {
									for (int c = 0; c < 4; ++c) {
										*dst++ = lo;
									}
									dst -= 4u;
									dst += bufwidth;
								}
								dst -= 2u * bufwidth;
							}
							dst += 4;

							src += 1;
						}

						blocks += count;
					}

				} else {

					switch (op) {

					case 0xF000: {
						if ((uint8_t)word) {
							for (r = 0; r < 2; ++r) {
								for (int c = 0; c < 4; ++c) {
									*dst++ = (uint8_t)word;
								}
								dst -= 4u;
								dst += bufwidth;
							}
							dst -= 2u * bufwidth;
						}

						dst += 4;
						src += 1;
						blocks += 1;
					} break;

					case 0xE000: {
						const uint8_t * cb = codebook + ((uint32_t)(word & 0xFFFu) << 3);

						for (r = 0; r < 2; ++r) {
							for (int c = 0; c < 4; ++c) {
								if (*cb) *dst = *cb;
								++dst;
								++cb;
							}
							dst -= 4u;
							dst += bufwidth;
						}

						dst -= 2u * bufwidth;

						dst += 4u;
						src += 1;
						blocks += 1;

					} break;
					}
				}

			} else {
				const uint8_t * cb = codebook + ((uint32_t)*src << 3);

				for (r = 0; r < 2; ++r) {
					for (int c = 0; c < 4; ++c) {
						*dst++ = *cb++;
					}
					dst -= 4u;
					dst += bufwidth;
				}

				dst -= 2u * bufwidth;

				dst += 4u;
				src += 1;
				blocks += 1;
			}

			if (blocks % blocksperrow == 0u) {
				dst = row_base + 2u * bufwidth;
				row_base = dst;
			}

			if (blocks >= total) {
				return;
			}
		}
	}
}

// FUNCTION: GAMEMD 0x007C57D0
void __cdecl UnVQ2_C0_4x2_KEY(unsigned char * codebook, unsigned char * pointers, unsigned char * buffer, unsigned long blocksperrow, unsigned long numrows, unsigned long bufwidth)
{
	uint16_t * src = (uint16_t *)pointers;
	uint8_t * dst = (uint8_t *)buffer;
	uint8_t * row_base = (uint8_t *)buffer;
	uint32_t blocks = 0;
	uint32_t total = numrows * blocksperrow;
	int r;
	uint32_t count;

	if (total > 0u) {

		while (1) {

			uint16_t word = *src;

			if ((word & 0x8000u) != 0) {

				int32_t op = word & 0xF000u;

				if (op <= 0xC000) {

					if (op != 0xC000) {

						switch (op) {

						case 0xB000: {
							count = ((uint32_t)word >> 8) & 0xFu;

							{
								uint32_t n = 4u * count;

								for (r = 0; r < 2; ++r) {
									for (uint32_t k = 0; k < n; ++k) {
										*dst++ = (uint8_t)word;
									}
									dst -= n;
									dst += bufwidth;
								}

								dst -= 2u * bufwidth;
							}

							dst += 4u * count;
							blocks += count;
							src += 1;
						} break;

						case 0xA000: {
							count = word & 0xFFFu;
							src += 1;
							uint16_t word2 = *src;

							if ((word2 & 0x8000u) != 0) {
								int sub = word2 & 0xC000;

								switch (sub) {

								case 0xC000: {
									const uint8_t * cb = codebook + ((uint32_t)(word2 & 0xFFFu) << 3);

									for (r = 0; r < 2; ++r) {
										uint32_t nn;
										for (nn = 0; nn < count; nn++) {
											for (int c = 0; c < 4; ++c) {
												*dst = *cb;
												++dst;
												++cb;
											}
											cb -= 4;
										}
										cb += 4;
										dst -= 4u * count;
										dst += bufwidth;
									}

									dst += 4u * count;
									dst -= 2u * bufwidth;
									src += 1;
									blocks += count;

								} break;

								case 0x8000: {
									{
										uint32_t n = 4u * count;

										for (r = 0; r < 2; ++r) {
											for (uint32_t k = 0; k < n; ++k) {
												*dst++ = (uint8_t)word2;
											}
											dst -= n;
											dst += bufwidth;
										}

										dst -= 2u * bufwidth;
									}

									dst += 4u * count;
									blocks += count;
									src += 1;

								} break;
								}

							} else {
								const uint8_t * cb = codebook + ((uint32_t)word2 << 3);

								for (r = 0; r < 2; ++r) {
									uint32_t nn;
									for (nn = 0; nn < count; nn++) {
										for (int c = 0; c < 4; ++c) {
											*dst++ = *cb++;
										}
										cb -= 4;
									}
									cb += 4;
									dst -= 4u * count;
									dst += bufwidth;
								}

								dst += 4u * count;
								dst -= 2u * bufwidth;
								src += 1;
								blocks += count;
							}

						} break;
						}

					} else {
						count = word & 0xFFFu;
						src += 1;
						for (uint32_t pairs = count >> 1; pairs != 0u; pairs--) {
							uint16_t word2 = *src;
							uint8_t hi = (uint8_t)(word2 >> 8);

							for (r = 0; r < 2; ++r) {
								for (int c = 0; c < 4; ++c) {
									*dst++ = hi;
								}
								dst -= 4u;
								dst += bufwidth;
							}
							dst -= 2u * bufwidth;
							dst += 4;

							uint8_t lo = (uint8_t)word2 & 0xFF;

							for (r = 0; r < 2; ++r) {
								for (int c = 0; c < 4; ++c) {
									*dst++ = lo;
								}
								dst -= 4u;
								dst += bufwidth;
							}
							dst -= 2u * bufwidth;
							dst += 4;

							src += 1;
						}

						blocks += count;
					}

				} else {

					switch (op) {

					case 0xF000: {
						for (r = 0; r < 2; ++r) {
							for (int c = 0; c < 4; ++c) {
								*dst++ = (uint8_t)word;
							}
							dst -= 4u;
							dst += bufwidth;
						}
						dst -= 2u * bufwidth;

						dst += 4;
						src += 1;
						blocks += 1;
					} break;

					case 0xE000: {
						const uint8_t * cb = codebook + ((uint32_t)(word & 0xFFFu) << 3);

						for (r = 0; r < 2; ++r) {
							for (int c = 0; c < 4; ++c) {
								*dst = *cb;
								++dst;
								++cb;
							}
							dst -= 4u;
							dst += bufwidth;
						}

						dst -= 2u * bufwidth;

						dst += 4u;
						src += 1;
						blocks += 1;

					} break;
					}
				}

			} else {
				const uint8_t * cb = codebook + ((uint32_t)*src << 3);

				for (r = 0; r < 2; ++r) {
					for (int c = 0; c < 4; ++c) {
						*dst++ = *cb++;
					}
					dst -= 4u;
					dst += bufwidth;
				}

				dst -= 2u * bufwidth;

				dst += 4u;
				src += 1;
				blocks += 1;
			}

			if (blocks % blocksperrow == 0u) {
				dst = row_base + 2u * bufwidth;
				row_base = dst;
			}

			if (blocks >= total) {
				return;
			}
		}
	}
}
