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

// FUNCTION: GAMEMD 0x007C4730
void __cdecl UnVQ2_C0_4x4_TRANS(unsigned char * codebook, unsigned char * pointers, unsigned char * buffer, unsigned long blocksperrow, unsigned long numrows, unsigned long bufwidth)
{
	uint16_t * src = (uint16_t *)pointers;
	uint8_t * dst = (uint8_t *)buffer;
	uint32_t blocks = 0;
	uint32_t count;
	int r;
	uint8_t * row_base = (uint8_t *)buffer;
	uint32_t total = blocksperrow * numrows;

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
							src += 1;
							uint16_t word2 = *src;
							count = word & 0xFFFu;

							if ((word2 & 0x8000u) != 0) {
								int sub = word2 & 0xC000;

								switch (sub) {

								case 0xC000: {
									const uint8_t * cb = codebook + ((uint32_t)(word2 & 0xFFFu) << 4);

									for (r = 0; r < 4; ++r) {
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
									dst -= 4u * bufwidth;
									src += 1;
									blocks += count;

								} break;

								case 0x8000: {
									if ((uint8_t)word2) {
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
						for (uint32_t pairs = 0; pairs < (count >> 1); ++pairs) {
							uint16_t word2 = *src;
							uint8_t hi = (uint8_t)(word2 >> 8);

							if (hi != 0u) {
								for (r = 0; r < 4; ++r) {
									for (int c = 0; c < 4; ++c) {
										*dst++ = hi;
									}
									dst -= 4u;
									dst += bufwidth;
								}
								dst -= 4u * bufwidth;
							}
							dst += 4;

							uint8_t lo = (uint8_t)word2 & 0xFF;

							if (lo != 0u) {
								for (r = 0; r < 4; ++r) {
									for (int c = 0; c < 4; ++c) {
										*dst++ = lo;
									}
									dst -= 4u;
									dst += bufwidth;
								}
								dst -= 4u * bufwidth;
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
							for (r = 0; r < 4; ++r) {
								for (int c = 0; c < 4; ++c) {
									*dst++ = (uint8_t)word;
								}
								dst -= 4u;
								dst += bufwidth;
							}
							dst -= 4u * bufwidth;
						}

						dst += 4;
						src += 1;
						blocks += 1;
					} break;

					case 0xE000: {
						const uint8_t * cb = codebook + ((uint32_t)(word & 0xFFFu) << 4);

						for (r = 0; r < 4; ++r) {
							for (int c = 0; c < 4; ++c) {
								if (*cb) *dst = *cb;
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
