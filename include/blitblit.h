// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).
#pragma once

#include "_alpha.h"
#include "_zbuffer.h"
#include "assert.h"
#include "blitter.h"
#include "abuffer.h"
#include "dsurface.h"
#include "zbuffer.h"

#include <cstring>

template<class T>
class BlitPlain : public Blitter {
	public:
		virtual void BlitForward(void * dest, void const * source, int length, int z_min = 0, void * z_buff = NULL, void * a_buff = NULL, int alpha_level = 1000, int warp_offset = 0) const override
		{
			memcpy(dest, source, length*sizeof(T));
		}
		virtual void BlitBackward(void * dest, void const * source, int length, int z_min = 0, void * z_buff = NULL, void * a_buff = NULL, int alpha_level = 1000) const override
		{
			memmove(dest, source, length*sizeof(T));
		}
		virtual void Blit_Copy_Tinted(void * dest, void const * source, int length, int z_min, void * z_buff, void * a_buff, int alpha_level, int warp_offset, unsigned short tint) const override
		{
			memcpy(dest, source, length*sizeof(T));
		}
		virtual void Blit_Move_Tinted(void * dest, void const * source, int length, int z_min, void * z_buff, void * a_buff, int alpha_level, unsigned short tint) const override
		{
			memmove(dest, source, length*sizeof(T));
		}
};

template<class T>
class BlitTrans : public Blitter {
	public:
		virtual void BlitForward(void * dest, void const * source, int length, int z_min = 0, void * z_buff = NULL, void * a_buff = NULL, int alpha_level = 1000, int warp_offset = 0) const override
		{
			for (int index = 0; index < length; index++) {
				T color = *(T const *)source;
				source = ((T *)source) + 1;
				if (color != 0) *((T *)dest) = color;
				dest = ((T *)dest) + 1;
			}
		}

		virtual void BlitBackward(void * dest, void const * source, int length, int z_min = 0, void * z_buff = NULL, void * a_buff = NULL, int alpha_level = 1000) const override {BlitForward(dest, source, length, z_min, z_buff, a_buff, alpha_level);}
		virtual void Blit_Copy_Tinted(void * dest, void const * source, int length, int z_min, void * z_buff, void * a_buff, int alpha_level, int warp_offset, unsigned short tint) const override {BlitForward(dest, source, length, z_min, z_buff, a_buff, alpha_level);}
		virtual void Blit_Move_Tinted(void * dest, void const * source, int length, int z_min, void * z_buff, void * a_buff, int alpha_level, unsigned short tint) const override {BlitForward(dest, source, length, z_min, z_buff, a_buff, alpha_level);}
};

template<class T>
class BlitPlainXlat : public Blitter {
	public:
		BlitPlainXlat(T const * translator) : TranslateTable(translator) {assert(TranslateTable != NULL);}
		virtual void BlitForward(void * dest, void const * source, int length, int z_min = 0, void * z_buff = NULL, void * a_buff = NULL, int alpha_level = 1000, int warp_offset = 0) const override
		{
			for (int index = 0; index < length; index++) {
				unsigned char color = *(unsigned char const *)source;
				source = ((unsigned char const *)source)+1;
				*((T *)dest) = TranslateTable[color];
				dest = ((T *)dest) + 1;
			}
		}

		virtual void BlitBackward(void * dest, void const * source, int length, int z_min = 0, void * z_buff = NULL, void * a_buff = NULL, int alpha_level = 1000) const override {BlitForward(dest, source, length, z_min, z_buff, a_buff, alpha_level);}
		virtual void Blit_Copy_Tinted(void * dest, void const * source, int length, int z_min, void * z_buff, void * a_buff, int alpha_level, int warp_offset, unsigned short tint) const override {BlitForward(dest, source, length, z_min, z_buff, a_buff, alpha_level);}
		virtual void Blit_Move_Tinted(void * dest, void const * source, int length, int z_min, void * z_buff, void * a_buff, int alpha_level, unsigned short tint) const override {BlitForward(dest, source, length, z_min, z_buff, a_buff, alpha_level);}

	private:
		T const * TranslateTable;
};

template<class T>
class BlitPlainXlatAlpha : public Blitter {
	public:
		BlitPlainXlatAlpha(T const * translator, int intensity_levels) : TranslateTable(translator), AlphaLightingRemap(NULL) {assert(TranslateTable != NULL); AlphaLightingRemap = AlphaLightingRemapInit.Init(intensity_levels);}
		virtual ~BlitPlainXlatAlpha(void) override { AlphaLightingRemapInit.Deinit(AlphaLightingRemap); AlphaLightingRemap = NULL; }

		virtual void BlitForward(void * dest, void const * source, int length, int z_min = 0, void * z_buff = NULL, void * a_buff = NULL, int alpha_level = 1000, int warp_offset = 0) const override
		{
			unsigned short *ap = (unsigned short *)a_buff;
			unsigned short *alphatable = AlphaLightingRemap->Get_Table(alpha_level);

			for (int index = 0; index < length; index++) {
				unsigned char color = *(unsigned char const *)source;
				source = ((unsigned char const *)source)+1;
				*((T *)dest) = TranslateTable[color | alphatable[*ap]];
				dest = ((T *)dest) + 1;

				ap++;
				ap = Blit_Wrap_A_Buffer(ap);
			}
		}

		virtual void BlitBackward(void * dest, void const * source, int length, int z_min = 0, void * z_buff = NULL, void * a_buff = NULL, int alpha_level = 1000) const override {BlitForward(dest, source, length, z_min, z_buff, a_buff, alpha_level);}
		virtual void Blit_Copy_Tinted(void * dest, void const * source, int length, int z_min, void * z_buff, void * a_buff, int alpha_level, int warp_offset, unsigned short tint) const override {BlitForward(dest, source, length, z_min, z_buff, a_buff, alpha_level);}
		virtual void Blit_Move_Tinted(void * dest, void const * source, int length, int z_min, void * z_buff, void * a_buff, int alpha_level, unsigned short tint) const override {BlitForward(dest, source, length, z_min, z_buff, a_buff, alpha_level);}

	private:
		T const * TranslateTable;
		AlphaLightingRemapClass *AlphaLightingRemap;
};

template<class T>
class BlitPlainXlatZRead : public Blitter {
	public:
		BlitPlainXlatZRead(T const * translator) : TranslateTable(translator) {assert(TranslateTable != NULL);}
		virtual void BlitForward(void * dest, void const * source, int length, int z_min = 0, void * z_buff = NULL, void * a_buff = NULL, int alpha_level = 1000, int warp_offset = 0) const override
		{
			unsigned short *zb = (unsigned short *)z_buff;

			for (int i = 0; i < length; ++i) {
				unsigned char value = *(unsigned char *)source;

				if (z_min < *zb++) {
					*(T *)dest = TranslateTable[value];
				}

				source = (unsigned char *)source + 1;
				dest = (T *)dest + 1;

				zb = Blit_Wrap_Z_Buffer(zb);
			}
		}

		virtual void BlitBackward(void * dest, void const * source, int length, int z_min = 0, void * z_buff = NULL, void * a_buff = NULL, int alpha_level = 1000) const override {BlitForward(dest, source, length, z_min, z_buff, a_buff, alpha_level);}
		virtual void Blit_Copy_Tinted(void * dest, void const * source, int length, int z_min, void * z_buff, void * a_buff, int alpha_level, int warp_offset, unsigned short tint) const override {BlitForward(dest, source, length, z_min, z_buff, a_buff, alpha_level);}
		virtual void Blit_Move_Tinted(void * dest, void const * source, int length, int z_min, void * z_buff, void * a_buff, int alpha_level, unsigned short tint) const override {BlitForward(dest, source, length, z_min, z_buff, a_buff, alpha_level);}

	private:
		T const * TranslateTable;
};

template<class T>
class BlitPlainXlatZReadWrite : public Blitter {
	public:
		BlitPlainXlatZReadWrite(T const * translator) : TranslateTable(translator) {assert(TranslateTable != NULL);}
		virtual void BlitForward(void * dest, void const * source, int length, int z_min = 0, void * z_buff = NULL, void * a_buff = NULL, int alpha_level = 1000, int warp_offset = 0) const override
		{
			unsigned short *zb = (unsigned short *)z_buff;

			for (int i = 0; i < length; ++i) {
				unsigned char value = *(unsigned char *)source;

				if (z_min < *zb) {
					*(T *)dest = TranslateTable[value];

					*zb = (unsigned char)z_min;
				}

				zb++;
				source = (unsigned char *)source + 1;
				dest = (T *)dest + 1;

				zb = Blit_Wrap_Z_Buffer(zb);
			}
		}

		virtual void BlitBackward(void * dest, void const * source, int length, int z_min = 0, void * z_buff = NULL, void * a_buff = NULL, int alpha_level = 1000) const override {BlitForward(dest, source, length, z_min, z_buff, a_buff, alpha_level);}
		virtual void Blit_Copy_Tinted(void * dest, void const * source, int length, int z_min, void * z_buff, void * a_buff, int alpha_level, int warp_offset, unsigned short tint) const override {BlitForward(dest, source, length, z_min, z_buff, a_buff, alpha_level);}
		virtual void Blit_Move_Tinted(void * dest, void const * source, int length, int z_min, void * z_buff, void * a_buff, int alpha_level, unsigned short tint) const override {BlitForward(dest, source, length, z_min, z_buff, a_buff, alpha_level);}

	private:
		T const * TranslateTable;
};

template<class T>
class BlitTransXlat : public Blitter {
	public:
		BlitTransXlat(T const * translator) : TranslateTable(translator) {assert(TranslateTable != NULL);}
		virtual void BlitForward(void * dest, void const * source, int length, int z_min = 0, void * z_buff = NULL, void * a_buff = NULL, int alpha_level = 1000, int warp_offset = 0) const override
		{
			for (int index = 0; index < length; index++) {
				unsigned char color = *(unsigned char const *)source;
				source = ((unsigned char const *)source)+1;
				if (color != 0) {
					*((T *)dest) = TranslateTable[color];
				}
				dest = ((T *)dest) + 1;
			}
		}

		virtual void BlitBackward(void * dest, void const * source, int length, int z_min = 0, void * z_buff = NULL, void * a_buff = NULL, int alpha_level = 1000) const override {BlitForward(dest, source, length, z_min, z_buff, a_buff, alpha_level);}
		virtual void Blit_Copy_Tinted(void * dest, void const * source, int length, int z_min, void * z_buff, void * a_buff, int alpha_level, int warp_offset, unsigned short tint) const override {BlitForward(dest, source, length, z_min, z_buff, a_buff, alpha_level);}
		virtual void Blit_Move_Tinted(void * dest, void const * source, int length, int z_min, void * z_buff, void * a_buff, int alpha_level, unsigned short tint) const override {BlitForward(dest, source, length, z_min, z_buff, a_buff, alpha_level);}

	private:
		T const * TranslateTable;
};

template<class T>
class BlitTransXlatAlpha : public Blitter {
	public:
		BlitTransXlatAlpha(T const * translator, int intensity_levels) : TranslateTable(translator), AlphaLightingRemap(NULL) {assert(TranslateTable != NULL); AlphaLightingRemap = AlphaLightingRemapInit.Init(intensity_levels);}
		virtual ~BlitTransXlatAlpha(void) override { AlphaLightingRemapInit.Deinit(AlphaLightingRemap); AlphaLightingRemap = NULL; }

		virtual void BlitForward(void * dest, void const * source, int length, int z_min = 0, void * z_buff = NULL, void * a_buff = NULL, int alpha_level = 1000, int warp_offset = 0) const override
		{
			unsigned short* ap = (unsigned short*)(a_buff);
			const unsigned short* aLUT = AlphaLightingRemap->Get_Table(alpha_level);

			for (int index = 0; index < length; index++) {
				unsigned char color = *(unsigned char const *)source;
				source = ((unsigned char const *)source)+1;
				if (color != 0) {
					*((T *)dest) = TranslateTable[color | aLUT[*ap]];
				}
				dest = ((T *)dest) + 1;

				ap++;
				ap = Blit_Wrap_A_Buffer(ap);
			}
		}

		virtual void BlitBackward(void * dest, void const * source, int length, int z_min = 0, void * z_buff = NULL, void * a_buff = NULL, int alpha_level = 1000) const override {BlitForward(dest, source, length, z_min, z_buff, a_buff, alpha_level);}
		virtual void Blit_Copy_Tinted(void * dest, void const * source, int length, int z_min, void * z_buff, void * a_buff, int alpha_level, int warp_offset, unsigned short tint) const override {BlitForward(dest, source, length, z_min, z_buff, a_buff, alpha_level);}
		virtual void Blit_Move_Tinted(void * dest, void const * source, int length, int z_min, void * z_buff, void * a_buff, int alpha_level, unsigned short tint) const override {BlitForward(dest, source, length, z_min, z_buff, a_buff, alpha_level);}

	private:
		T const * TranslateTable;
		AlphaLightingRemapClass *AlphaLightingRemap;
};

template<class T>
class BlitTransXlatAlphaZRead : public Blitter {
	public:
		BlitTransXlatAlphaZRead(T const * translator, int intensity_levels) : TranslateTable(translator), AlphaLightingRemap(NULL) {AlphaLightingRemap = AlphaLightingRemapInit.Init(intensity_levels);}
		virtual ~BlitTransXlatAlphaZRead(void) override { AlphaLightingRemapInit.Deinit(AlphaLightingRemap); AlphaLightingRemap = NULL; }

		virtual void BlitForward(void * dest, void const * source, int length, int z_min = 0, void * z_buff = NULL, void * a_buff = NULL, int alpha_level = 1000, int warp_offset = 0) const override
		{
			unsigned short *zb = (unsigned short *)z_buff;
			unsigned short *ap = (unsigned short *)a_buff;
			const unsigned short* aLUT = AlphaLightingRemap->Get_Table(alpha_level);

			for (int index = 0; index < length; index++) {
				if (z_min < *zb++) {
					unsigned char color = *(unsigned char const *)source;
					if (color != 0) {
						*((T *)dest) = TranslateTable[color | aLUT[*ap]];
					}
				}
				source = ((unsigned char *)source) + 1;
				dest = ((T *)dest) + 1;

				zb = Blit_Wrap_Z_Buffer(zb);
				ap++;
				ap = Blit_Wrap_A_Buffer(ap);
			}
		}

		virtual void BlitBackward(void * dest, void const * source, int length, int z_min = 0, void * z_buff = NULL, void * a_buff = NULL, int alpha_level = 1000) const override {BlitForward(dest, source, length, z_min, z_buff, a_buff, alpha_level);}
		virtual void Blit_Copy_Tinted(void * dest, void const * source, int length, int z_min, void * z_buff, void * a_buff, int alpha_level, int warp_offset, unsigned short tint) const override;
		virtual void Blit_Move_Tinted(void * dest, void const * source, int length, int z_min, void * z_buff, void * a_buff, int alpha_level, unsigned short tint) const override {Blit_Copy_Tinted(dest, source, length, z_min, z_buff, a_buff, alpha_level, 0, tint);}

	private:
		T const * TranslateTable;
		AlphaLightingRemapClass *AlphaLightingRemap;
};

template<class T>
class BlitTransXlatAlphaZReadWrite : public Blitter {
	public:
		BlitTransXlatAlphaZReadWrite(T const * translator, int intensity_levels) : TranslateTable(translator), AlphaLightingRemap(NULL) {AlphaLightingRemap = AlphaLightingRemapInit.Init(intensity_levels);}
		virtual ~BlitTransXlatAlphaZReadWrite(void) override { AlphaLightingRemapInit.Deinit(AlphaLightingRemap); AlphaLightingRemap = NULL; }

		virtual void BlitForward(void * dest, void const * source, int length, int z_min = 0, void * z_buff = NULL, void * a_buff = NULL, int alpha_level = 1000, int warp_offset = 0) const override
		{
			unsigned short *zb = (unsigned short *)z_buff;
			unsigned short *ap = (unsigned short *)a_buff;
			const unsigned short* aLUT = AlphaLightingRemap->Get_Table(alpha_level);

			for (int index = 0; index < length; index++) {
				if (z_min < *zb) {
					unsigned char color = *(unsigned char const *)source;
					if (color != 0) {
						*((T *)dest) = TranslateTable[color | aLUT[*ap]];
						*zb = z_min;
					}
				}
				source = ((unsigned char *)source) + 1;
				dest = ((T *)dest) + 1;

				zb++;
				zb = Blit_Wrap_Z_Buffer(zb);
				ap++;
				ap = Blit_Wrap_A_Buffer(ap);
			}
		}

		virtual void BlitBackward(void * dest, void const * source, int length, int z_min = 0, void * z_buff = NULL, void * a_buff = NULL, int alpha_level = 1000) const override {BlitForward(dest, source, length, z_min, z_buff, a_buff, alpha_level);}
		virtual void Blit_Copy_Tinted(void * dest, void const * source, int length, int z_min, void * z_buff, void * a_buff, int alpha_level, int warp_offset, unsigned short tint) const override {BlitForward(dest, source, length, z_min, z_buff, a_buff, alpha_level);}
		virtual void Blit_Move_Tinted(void * dest, void const * source, int length, int z_min, void * z_buff, void * a_buff, int alpha_level, unsigned short tint) const override {BlitForward(dest, source, length, z_min, z_buff, a_buff, alpha_level);}

	private:
		T const * TranslateTable;
		AlphaLightingRemapClass *AlphaLightingRemap;
};

template<class T>
class BlitTransXlatMultWriteAlpha : public Blitter {
	public:
		BlitTransXlatMultWriteAlpha(void) {}
		virtual void BlitForward(void * dest, void const * source, int length, int z_min = 0, void * z_buff = NULL, void * a_buff = NULL, int alpha_level = 1000, int warp_offset = 0) const override
		{
			unsigned short *ap = (unsigned short *)a_buff;

			for (int i = 0; i < length; ++i) {
				unsigned char value = *(unsigned char *)source;
				source = (unsigned char *)source + 1;

				if (value != 0) {
					*(unsigned short *)ap = MIN(z_min + alpha_level * value, 255);
				}

				ap = ap + 1;

				ap = Blit_Wrap_A_Buffer(ap);
			}
		}

		virtual void BlitBackward(void * dest, void const * source, int length, int z_min = 0, void * z_buff = NULL, void * a_buff = NULL, int alpha_level = 1000) const override {BlitForward(dest, source, length, z_min, z_buff, a_buff, alpha_level);}
		virtual void Blit_Copy_Tinted(void * dest, void const * source, int length, int z_min, void * z_buff, void * a_buff, int alpha_level, int warp_offset, unsigned short tint) const override {BlitForward(dest, source, length, z_min, z_buff, a_buff, alpha_level);}
		virtual void Blit_Move_Tinted(void * dest, void const * source, int length, int z_min, void * z_buff, void * a_buff, int alpha_level, unsigned short tint) const override {BlitForward(dest, source, length, z_min, z_buff, a_buff, alpha_level);}
};

template<class T>
class BlitTransXlatWriteAlpha : public Blitter {
	public:
		BlitTransXlatWriteAlpha(void) {}
		virtual void BlitForward(void * dest, void const * source, int length, int z_min = 0, void * z_buff = NULL, void * a_buff = NULL, int alpha_level = 1000, int warp_offset = 0) const override
		{
			unsigned short *ap = (unsigned short *)a_buff;

			for (int i = 0; i < length; ++i) {
				unsigned char value = *(unsigned char *)source;
				source = (unsigned char *)source + 1;

				if (value != 0) {
					*(unsigned short *)ap = MIN(z_min + value, 255);
				}

				ap = ap + 1;

				ap = Blit_Wrap_A_Buffer(ap);
			}
		}

		virtual void BlitBackward(void * dest, void const * source, int length, int z_min = 0, void * z_buff = NULL, void * a_buff = NULL, int alpha_level = 1000) const override {BlitForward(dest, source, length, z_min, z_buff, a_buff, alpha_level);}
		virtual void Blit_Copy_Tinted(void * dest, void const * source, int length, int z_min, void * z_buff, void * a_buff, int alpha_level, int warp_offset, unsigned short tint) const override {BlitForward(dest, source, length, z_min, z_buff, a_buff, alpha_level);}
		virtual void Blit_Move_Tinted(void * dest, void const * source, int length, int z_min, void * z_buff, void * a_buff, int alpha_level, unsigned short tint) const override {BlitForward(dest, source, length, z_min, z_buff, a_buff, alpha_level);}
};

template<class T>
class BlitTransXlatZRead : public Blitter {
	public:
		BlitTransXlatZRead(T const * translator) : TranslateTable(translator) {assert(TranslateTable != NULL);}
		virtual void BlitForward(void * dest, void const * source, int length, int z_min = 0, void * z_buff = NULL, void * a_buff = NULL, int alpha_level = 1000, int warp_offset = 0) const override
		{
			unsigned short *zb = (unsigned short *)z_buff;

			for (int i = 0; i < length; ++i) {

				if (z_min < *zb++) {
					unsigned char value = *(unsigned char *)source;
					if (value != 0) {
						*(T *)dest = TranslateTable[(unsigned char)value];
					}
				}
				source = ((unsigned char *)source) + 1;
				dest = ((T *)dest) + 1;

				zb = Blit_Wrap_Z_Buffer(zb);
			}
		}

		virtual void BlitBackward(void * dest, void const * source, int length, int z_min = 0, void * z_buff = NULL, void * a_buff = NULL, int alpha_level = 1000) const override {BlitForward(dest, source, length, z_min, z_buff, a_buff, alpha_level);}
		virtual void Blit_Copy_Tinted(void * dest, void const * source, int length, int z_min, void * z_buff, void * a_buff, int alpha_level, int warp_offset, unsigned short tint) const override {BlitForward(dest, source, length, z_min, z_buff, a_buff, alpha_level);}
		virtual void Blit_Move_Tinted(void * dest, void const * source, int length, int z_min, void * z_buff, void * a_buff, int alpha_level, unsigned short tint) const override {BlitForward(dest, source, length, z_min, z_buff, a_buff, alpha_level);}

	private:
		T const * TranslateTable;
};

template<class T>
class BlitTransXlatZReadWrite : public Blitter {
	public:
		BlitTransXlatZReadWrite(T const * translator) : TranslateTable(translator) {assert(TranslateTable != NULL);}
		virtual void BlitForward(void * dest, void const * source, int length, int z_min = 0, void * z_buff = NULL, void * a_buff = NULL, int alpha_level = 1000, int warp_offset = 0) const override
		{
			unsigned short *zb = (unsigned short *)z_buff;

			for (int i = 0; i < length; ++i) {

				if (z_min < *zb) {
					unsigned char value = *(unsigned char *)source;
					if (value != 0) {
						*(T *)dest = TranslateTable[(unsigned char)value];
						*zb = z_min;
					}
				}
				zb++;
				source = ((unsigned char *)source) + 1;
				dest = ((T *)dest) + 1;

				zb = Blit_Wrap_Z_Buffer(zb);
			}
		}

		virtual void BlitBackward(void * dest, void const * source, int length, int z_min = 0, void * z_buff = NULL, void * a_buff = NULL, int alpha_level = 1000) const override {BlitForward(dest, source, length, z_min, z_buff, a_buff, alpha_level);}
		virtual void Blit_Copy_Tinted(void * dest, void const * source, int length, int z_min, void * z_buff, void * a_buff, int alpha_level, int warp_offset, unsigned short tint) const override {BlitForward(dest, source, length, z_min, z_buff, a_buff, alpha_level);}
		virtual void Blit_Move_Tinted(void * dest, void const * source, int length, int z_min, void * z_buff, void * a_buff, int alpha_level, unsigned short tint) const override {BlitForward(dest, source, length, z_min, z_buff, a_buff, alpha_level);}

	private:
		T const * TranslateTable;
};

template<class T>
class BlitTransRemapXlat : public Blitter {
	public:
		BlitTransRemapXlat(unsigned char const * remapper, T const * translator) : RemapTable(remapper), TranslateTable(translator) {assert(RemapTable != NULL);assert(TranslateTable != NULL);}
		virtual void BlitForward(void * dest, void const * source, int length, int z_min = 0, void * z_buff = NULL, void * a_buff = NULL, int alpha_level = 1000, int warp_offset = 0) const override
		{
			for (int index = 0; index < length; index++) {
				unsigned char color = *(unsigned char const *)source;
				source = ((unsigned char const *)source)+1;
				if (color != 0) {
					*((T *)dest) = TranslateTable[RemapTable[color]];
				}
				dest = ((T *)dest) + 1;
			}
		}

		virtual void BlitBackward(void * dest, void const * source, int length, int z_min = 0, void * z_buff = NULL, void * a_buff = NULL, int alpha_level = 1000) const override {BlitForward(dest, source, length, z_min, z_buff, a_buff, alpha_level);}
		virtual void Blit_Copy_Tinted(void * dest, void const * source, int length, int z_min, void * z_buff, void * a_buff, int alpha_level, int warp_offset, unsigned short tint) const override {BlitForward(dest, source, length, z_min, z_buff, a_buff, alpha_level);}
		virtual void Blit_Move_Tinted(void * dest, void const * source, int length, int z_min, void * z_buff, void * a_buff, int alpha_level, unsigned short tint) const override {BlitForward(dest, source, length, z_min, z_buff, a_buff, alpha_level);}

	private:
		unsigned char const * RemapTable;
		T const * TranslateTable;
};

template<class T>
class BlitTransZRemapXlat : public Blitter {
	public:
		BlitTransZRemapXlat(unsigned char const * const * remapper, T const * translator) : RemapTable(remapper), TranslateTable(translator) {assert(RemapTable != NULL);assert(TranslateTable != NULL);}
		virtual void BlitForward(void * dest, void const * source, int length, int z_min = 0, void * z_buff = NULL, void * a_buff = NULL, int alpha_level = 1000, int warp_offset = 0) const override
		{
			unsigned char const * rtable = *RemapTable;
			for (int index = 0; index < length; index++) {
				unsigned char color = *(unsigned char const *)source;
				source = ((unsigned char const *)source)+1;
				if (color != 0) {
					*((T *)dest) = TranslateTable[rtable[color]];
				}
				dest = ((T *)dest) + 1;
			}
		}

		virtual void BlitBackward(void * dest, void const * source, int length, int z_min = 0, void * z_buff = NULL, void * a_buff = NULL, int alpha_level = 1000) const override {BlitForward(dest, source, length, z_min, z_buff, a_buff, alpha_level);}
		virtual void Blit_Copy_Tinted(void * dest, void const * source, int length, int z_min, void * z_buff, void * a_buff, int alpha_level, int warp_offset, unsigned short tint) const override {BlitForward(dest, source, length, z_min, z_buff, a_buff, alpha_level);}
		virtual void Blit_Move_Tinted(void * dest, void const * source, int length, int z_min, void * z_buff, void * a_buff, int alpha_level, unsigned short tint) const override {BlitForward(dest, source, length, z_min, z_buff, a_buff, alpha_level);}

	private:
		unsigned char const * const * RemapTable;
		T const * TranslateTable;
};

template<class T>
class BlitTransZRemapXlatAlpha : public Blitter {
	public:
		BlitTransZRemapXlatAlpha(unsigned char const * const * remapper, T const * translator, int intensity_levels) : RemapTable(remapper), TranslateTable(translator), AlphaLightingRemap(NULL) {assert(RemapTable != NULL);assert(TranslateTable != NULL); AlphaLightingRemap = AlphaLightingRemapInit.Init(intensity_levels);}
		virtual ~BlitTransZRemapXlatAlpha(void) override { AlphaLightingRemapInit.Deinit(AlphaLightingRemap); AlphaLightingRemap = NULL; }

		virtual void BlitForward(void * dest, void const * source, int length, int z_min = 0, void * z_buff = NULL, void * a_buff = NULL, int alpha_level = 1000, int warp_offset = 0) const override
		{
			unsigned char const * rtable = *RemapTable;
			unsigned short* ap = (unsigned short*)(a_buff);
			const unsigned short* aLUT = AlphaLightingRemap->Get_Table(alpha_level);

			for (int index = 0; index < length; index++) {
				unsigned char color = *(unsigned char const *)source;
				source = ((unsigned char const *)source)+1;
				if (color != 0) {
					*((T *)dest) = TranslateTable[rtable[color] | aLUT[*ap]];
				}
				dest = ((T *)dest) + 1;

				ap++;
				ap = Blit_Wrap_A_Buffer(ap);
			}
		}

		virtual void BlitBackward(void * dest, void const * source, int length, int z_min = 0, void * z_buff = NULL, void * a_buff = NULL, int alpha_level = 1000) const override {BlitForward(dest, source, length, z_min, z_buff, a_buff, alpha_level);}
		virtual void Blit_Copy_Tinted(void * dest, void const * source, int length, int z_min, void * z_buff, void * a_buff, int alpha_level, int warp_offset, unsigned short tint) const override {BlitForward(dest, source, length, z_min, z_buff, a_buff, alpha_level);}
		virtual void Blit_Move_Tinted(void * dest, void const * source, int length, int z_min, void * z_buff, void * a_buff, int alpha_level, unsigned short tint) const override {BlitForward(dest, source, length, z_min, z_buff, a_buff, alpha_level);}

	private:
		unsigned char const * const * RemapTable;
		T const * TranslateTable;
		AlphaLightingRemapClass *AlphaLightingRemap;
};

template<class T>
class BlitTransZRemapXlatAlphaZRead : public Blitter {
	public:
		BlitTransZRemapXlatAlphaZRead(unsigned char const * const * remapper, T const * translator, int intensity_levels) : RemapTable(remapper), TranslateTable(translator), AlphaLightingRemap(NULL) {assert(RemapTable != NULL);assert(TranslateTable != NULL); AlphaLightingRemap = AlphaLightingRemapInit.Init(intensity_levels);}
		virtual ~BlitTransZRemapXlatAlphaZRead(void) override { AlphaLightingRemapInit.Deinit(AlphaLightingRemap); AlphaLightingRemap = NULL; }

		virtual void BlitForward(void * dest, void const * source, int length, int z_min = 0, void * z_buff = NULL, void * a_buff = NULL, int alpha_level = 1000, int warp_offset = 0) const override
		{
			unsigned short *zb = (unsigned short *)z_buff;
			unsigned short *ap = (unsigned short *)a_buff;
			unsigned char const * rtable = *RemapTable;
			const unsigned short* aLUT = AlphaLightingRemap->Get_Table(alpha_level);

			for (int index = 0; index < length; index++) {
				if (z_min < *zb++) {
					unsigned char color = *(unsigned char const *)source;
					if (color != 0) {
						*((T *)dest) = TranslateTable[rtable[color] | aLUT[*ap]];
					}
				}
				source = ((unsigned char *)source) + 1;
				dest = ((T *)dest) + 1;

				zb = Blit_Wrap_Z_Buffer(zb);
				ap++;
				ap = Blit_Wrap_A_Buffer(ap);
			}
		}

		virtual void BlitBackward(void * dest, void const * source, int length, int z_min = 0, void * z_buff = NULL, void * a_buff = NULL, int alpha_level = 1000) const override {BlitForward(dest, source, length, z_min, z_buff, a_buff, alpha_level);}
		virtual void Blit_Copy_Tinted(void * dest, void const * source, int length, int z_min, void * z_buff, void * a_buff, int alpha_level, int warp_offset, unsigned short tint) const override {BlitForward(dest, source, length, z_min, z_buff, a_buff, alpha_level);}
		virtual void Blit_Move_Tinted(void * dest, void const * source, int length, int z_min, void * z_buff, void * a_buff, int alpha_level, unsigned short tint) const override {BlitForward(dest, source, length, z_min, z_buff, a_buff, alpha_level);}

	private:
		unsigned char const * const * RemapTable;
		T const * TranslateTable;
		AlphaLightingRemapClass *AlphaLightingRemap;
};

template<class T>
class BlitTransZRemapXlatAlphaZReadWrite : public Blitter {
	public:
		BlitTransZRemapXlatAlphaZReadWrite(unsigned char const * const * remapper, T const * translator, int intensity_levels) : RemapTable(remapper), AlphaLightingRemap(NULL), TranslateTable(translator) {assert(RemapTable != NULL);assert(TranslateTable != NULL); AlphaLightingRemap = AlphaLightingRemapInit.Init(intensity_levels);}
		virtual ~BlitTransZRemapXlatAlphaZReadWrite(void) override { AlphaLightingRemapInit.Deinit(AlphaLightingRemap); AlphaLightingRemap = NULL; }

		virtual void BlitForward(void * dest, void const * source, int length, int z_min = 0, void * z_buff = NULL, void * a_buff = NULL, int alpha_level = 1000, int warp_offset = 0) const override
		{
			unsigned short *zb = (unsigned short *)z_buff;
			unsigned short *ap = (unsigned short *)a_buff;
			const unsigned short* aLUT = AlphaLightingRemap->Get_Table(alpha_level);
			unsigned char const * rtable = *RemapTable;

			for (int index = 0; index < length; index++) {
				if (z_min < *zb) {
					unsigned char color = *(unsigned char const *)source;
					if (color != 0) {
						*((T *)dest) = TranslateTable[rtable[color] | aLUT[*ap]];
						*zb = z_min;
					}
				}
				source = ((unsigned char *)source) + 1;
				dest = ((T *)dest) + 1;

				zb++;

				zb = Blit_Wrap_Z_Buffer(zb);
			}
		}

		virtual void BlitBackward(void * dest, void const * source, int length, int z_min = 0, void * z_buff = NULL, void * a_buff = NULL, int alpha_level = 1000) const override {BlitForward(dest, source, length, z_min, z_buff, a_buff, alpha_level);}
		virtual void Blit_Copy_Tinted(void * dest, void const * source, int length, int z_min, void * z_buff, void * a_buff, int alpha_level, int warp_offset, unsigned short tint) const override {BlitForward(dest, source, length, z_min, z_buff, a_buff, alpha_level);}
		virtual void Blit_Move_Tinted(void * dest, void const * source, int length, int z_min, void * z_buff, void * a_buff, int alpha_level, unsigned short tint) const override {BlitForward(dest, source, length, z_min, z_buff, a_buff, alpha_level);}

	private:
		unsigned char const * const * RemapTable;
		T const * TranslateTable;
		AlphaLightingRemapClass *AlphaLightingRemap;
};

template<class T>
class BlitTransZRemapXlatZRead : public Blitter {
	public:
		BlitTransZRemapXlatZRead(unsigned char const * const * remapper, T const * translator) : RemapTable(remapper), TranslateTable(translator) {assert(RemapTable != NULL);assert(TranslateTable != NULL);}
		virtual void BlitForward(void * dest, void const * source, int length, int z_min = 0, void * z_buff = NULL, void * a_buff = NULL, int alpha_level = 1000, int warp_offset = 0) const override
		{
			unsigned short *zb = (unsigned short *)z_buff;
			unsigned char const * rtable = *RemapTable;

			for (int i = 0; i < length; ++i) {

				if (z_min < *zb++) {
					unsigned char value = *(unsigned char *)source;
					if (value != 0) {
						*(T *)dest = TranslateTable[rtable[(unsigned char)value]];
					}
				}
				source = ((unsigned char *)source) + 1;
				dest = ((T *)dest) + 1;

				zb = Blit_Wrap_Z_Buffer(zb);
			}
		}

		virtual void BlitBackward(void * dest, void const * source, int length, int z_min = 0, void * z_buff = NULL, void * a_buff = NULL, int alpha_level = 1000) const override {BlitForward(dest, source, length, z_min, z_buff, a_buff, alpha_level);}
		virtual void Blit_Copy_Tinted(void * dest, void const * source, int length, int z_min, void * z_buff, void * a_buff, int alpha_level, int warp_offset, unsigned short tint) const override {BlitForward(dest, source, length, z_min, z_buff, a_buff, alpha_level);}
		virtual void Blit_Move_Tinted(void * dest, void const * source, int length, int z_min, void * z_buff, void * a_buff, int alpha_level, unsigned short tint) const override {BlitForward(dest, source, length, z_min, z_buff, a_buff, alpha_level);}

	private:
		unsigned char const * const * RemapTable;
		T const * TranslateTable;
};

template<class T>
class BlitTransZRemapXlatZReadWrite : public Blitter {
	public:
		BlitTransZRemapXlatZReadWrite(unsigned char const * const * remapper, T const * translator) : RemapTable(remapper), TranslateTable(translator) {assert(RemapTable != NULL);assert(TranslateTable != NULL);}
		virtual void BlitForward(void * dest, void const * source, int length, int z_min = 0, void * z_buff = NULL, void * a_buff = NULL, int alpha_level = 1000, int warp_offset = 0) const override
		{
			unsigned short *zb = (unsigned short *)z_buff;
			unsigned char const * rtable = *RemapTable;

			for (int i = 0; i < length; ++i) {

				if (z_min < *zb) {
					unsigned char value = *(unsigned char *)source;
					if (value != 0) {
						*(T *)dest = TranslateTable[rtable[(unsigned char)value]];
						*zb = z_min;
					}
				}
				zb++;
				source = ((unsigned char *)source) + 1;
				dest = ((T *)dest) + 1;

				zb = Blit_Wrap_Z_Buffer(zb);
			}
		}

		virtual void BlitBackward(void * dest, void const * source, int length, int z_min = 0, void * z_buff = NULL, void * a_buff = NULL, int alpha_level = 1000) const override {BlitForward(dest, source, length, z_min, z_buff, a_buff, alpha_level);}
		virtual void Blit_Copy_Tinted(void * dest, void const * source, int length, int z_min, void * z_buff, void * a_buff, int alpha_level, int warp_offset, unsigned short tint) const override {BlitForward(dest, source, length, z_min, z_buff, a_buff, alpha_level);}
		virtual void Blit_Move_Tinted(void * dest, void const * source, int length, int z_min, void * z_buff, void * a_buff, int alpha_level, unsigned short tint) const override {BlitForward(dest, source, length, z_min, z_buff, a_buff, alpha_level);}

	private:
		unsigned char const * const * RemapTable;
		T const * TranslateTable;
};

template<class T>
class BlitTransDarken : public Blitter {
	public:
		BlitTransDarken(T mask) : Mask(mask) {}
		virtual void BlitForward(void * dest, void const * source, int length, int z_min = 0, void * z_buff = NULL, void * a_buff = NULL, int alpha_level = 1000, int warp_offset = 0) const override
		{
			for (int index = 0; index < length; index++) {
				unsigned char color = *(unsigned char const *)source;
				source = ((unsigned char const *)source)+1;
				if (color != 0) {
					*((T *)dest) = (T)((((*(T *)dest) >> 1) & Mask));
				}
				dest = ((T *)dest) + 1;
			}
		}

		virtual void BlitBackward(void * dest, void const * source, int length, int z_min = 0, void * z_buff = NULL, void * a_buff = NULL, int alpha_level = 1000) const override {BlitForward(dest, source, length, z_min, z_buff, a_buff, alpha_level);}
		virtual void Blit_Copy_Tinted(void * dest, void const * source, int length, int z_min, void * z_buff, void * a_buff, int alpha_level, int warp_offset, unsigned short tint) const override {BlitForward(dest, source, length, z_min, z_buff, a_buff, alpha_level);}
		virtual void Blit_Move_Tinted(void * dest, void const * source, int length, int z_min, void * z_buff, void * a_buff, int alpha_level, unsigned short tint) const override {BlitForward(dest, source, length, z_min, z_buff, a_buff, alpha_level);}

	private:
		T Mask;
};

template<class T>
class BlitTransDarkenZRead : public Blitter {
	public:
		BlitTransDarkenZRead(T mask) : Mask(mask) {}
		virtual void BlitForward(void * dest, void const * source, int length, int z_min = 0, void * z_buff = NULL, void * a_buff = NULL, int alpha_level = 1000, int warp_offset = 0) const override
		{
			unsigned short *zb = (unsigned short *)z_buff;

			for (int i = 0; i < length; ++i) {

				if (z_min < *zb++) {
					unsigned char value = *(unsigned char *)source;
					if (value != 0) {
						*((T *)dest) = (T)((((*(T *)dest) >> 1) & Mask));
					}
				}
				source = ((unsigned char *)source) + 1;
				dest = ((T *)dest) + 1;

				zb = Blit_Wrap_Z_Buffer(zb);
			}
		}

		virtual void BlitBackward(void * dest, void const * source, int length, int z_min = 0, void * z_buff = NULL, void * a_buff = NULL, int alpha_level = 1000) const override {BlitForward(dest, source, length, z_min, z_buff, a_buff, alpha_level);}
		virtual void Blit_Copy_Tinted(void * dest, void const * source, int length, int z_min, void * z_buff, void * a_buff, int alpha_level, int warp_offset, unsigned short tint) const override {BlitForward(dest, source, length, z_min, z_buff, a_buff, alpha_level);}
		virtual void Blit_Move_Tinted(void * dest, void const * source, int length, int z_min, void * z_buff, void * a_buff, int alpha_level, unsigned short tint) const override {BlitForward(dest, source, length, z_min, z_buff, a_buff, alpha_level);}

	private:
		T Mask;
};

template<class T>
class BlitTransDarkenZReadWrite : public Blitter {
	public:
		BlitTransDarkenZReadWrite(T mask) : Mask(mask) {}
		virtual void BlitForward(void * dest, void const * source, int length, int z_min = 0, void * z_buff = NULL, void * a_buff = NULL, int alpha_level = 1000, int warp_offset = 0) const override
		{
			unsigned short *zb = (unsigned short *)z_buff;

			for (int i = 0; i < length; ++i) {

				if (z_min < *zb) {
					unsigned char value = *(unsigned char *)source;
					if (value != 0) {
						*((T *)dest) = (T)((((*(T *)dest) >> 1) & Mask));
						*zb = z_min;
					}
				}
				zb++;
				source = ((unsigned char *)source) + 1;
				dest = ((T *)dest) + 1;

				zb = Blit_Wrap_Z_Buffer(zb);
			}
		}

		virtual void BlitBackward(void * dest, void const * source, int length, int z_min = 0, void * z_buff = NULL, void * a_buff = NULL, int alpha_level = 1000) const override {BlitForward(dest, source, length, z_min, z_buff, a_buff, alpha_level);}
		virtual void Blit_Copy_Tinted(void * dest, void const * source, int length, int z_min, void * z_buff, void * a_buff, int alpha_level, int warp_offset, unsigned short tint) const override {BlitForward(dest, source, length, z_min, z_buff, a_buff, alpha_level);}
		virtual void Blit_Move_Tinted(void * dest, void const * source, int length, int z_min, void * z_buff, void * a_buff, int alpha_level, unsigned short tint) const override {BlitForward(dest, source, length, z_min, z_buff, a_buff, alpha_level);}

	private:
		T Mask;
};

template<class T>
class BlitTransRemapDest : public Blitter {
	public:
		BlitTransRemapDest(T const * remap) : RemapTable(remap) {}
		virtual void BlitForward(void * dest, void const * source, int length, int z_min = 0, void * z_buff = NULL, void * a_buff = NULL, int alpha_level = 1000, int warp_offset = 0) const override
		{
			for (int index = 0; index < length; index++) {
				unsigned char color = *(unsigned char const *)source;
				source = ((unsigned char const *)source)+1;
				if (color != 0) {
					*((T *)dest) = RemapTable[*((T *)dest)];
				}
				dest = ((T *)dest) + 1;
			}
		}

		virtual void BlitBackward(void * dest, void const * source, int length, int z_min = 0, void * z_buff = NULL, void * a_buff = NULL, int alpha_level = 1000) const override {BlitForward(dest, source, length, z_min, z_buff, a_buff, alpha_level);}
		virtual void Blit_Copy_Tinted(void * dest, void const * source, int length, int z_min, void * z_buff, void * a_buff, int alpha_level, int warp_offset, unsigned short tint) const override {BlitForward(dest, source, length, z_min, z_buff, a_buff, alpha_level);}
		virtual void Blit_Move_Tinted(void * dest, void const * source, int length, int z_min, void * z_buff, void * a_buff, int alpha_level, unsigned short tint) const override {BlitForward(dest, source, length, z_min, z_buff, a_buff, alpha_level);}

	private:
		T const * RemapTable;
};

template<class T>
class BlitDarken : public Blitter {
	public:
		BlitDarken(T mask) : Mask(mask) {}
		virtual void BlitForward(void * dest, void const * source, int length, int z_min = 0, void * z_buff = NULL, void * a_buff = NULL, int alpha_level = 1000, int warp_offset = 0) const override
		{
			for (int index = 0; index < length; index++) {
				*((T *)dest) = (T)(((*(T *)dest) >> 1) & Mask);
				dest = ((T *)dest) + 1;
			}
		}

		virtual void BlitBackward(void * dest, void const * source, int length, int z_min = 0, void * z_buff = NULL, void * a_buff = NULL, int alpha_level = 1000) const override {BlitForward(dest, source, length, z_min, z_buff, a_buff, alpha_level);}
		virtual void Blit_Copy_Tinted(void * dest, void const * source, int length, int z_min, void * z_buff, void * a_buff, int alpha_level, int warp_offset, unsigned short tint) const override {BlitForward(dest, source, length, z_min, z_buff, a_buff, alpha_level);}
		virtual void Blit_Move_Tinted(void * dest, void const * source, int length, int z_min, void * z_buff, void * a_buff, int alpha_level, unsigned short tint) const override {BlitForward(dest, source, length, z_min, z_buff, a_buff, alpha_level);}

	private:
		T Mask;
};

template<class T>
class BlitTransLucent50 : public Blitter {
	public:
		BlitTransLucent50(T const * translator, T mask) : TranslateTable(translator), Mask(mask) {assert(TranslateTable != NULL);}
		virtual void BlitForward(void * dest, void const * source, int length, int z_min = 0, void * z_buff = NULL, void * a_buff = NULL, int alpha_level = 1000, int warp_offset = 0) const override
		{
			for (int index = 0; index < length; index++) {
				unsigned char color = *(unsigned char const *)source;
				source = ((unsigned char *)source) + 1;
				if (color != 0) {
					*((T *)dest) = (T)((((*(T *)dest) >> 1) & Mask) + ((TranslateTable[color] >> 1) & Mask));
				}
				dest = ((T *)dest) + 1;
			}
		}

		virtual void BlitBackward(void * dest, void const * source, int length, int z_min = 0, void * z_buff = NULL, void * a_buff = NULL, int alpha_level = 1000) const override {BlitForward(dest, source, length, z_min, z_buff, a_buff, alpha_level);}
		virtual void Blit_Copy_Tinted(void * dest, void const * source, int length, int z_min, void * z_buff, void * a_buff, int alpha_level, int warp_offset, unsigned short tint) const override {BlitForward(dest, source, length, z_min, z_buff, a_buff, alpha_level);}
		virtual void Blit_Move_Tinted(void * dest, void const * source, int length, int z_min, void * z_buff, void * a_buff, int alpha_level, unsigned short tint) const override {BlitForward(dest, source, length, z_min, z_buff, a_buff, alpha_level);}

	private:
		T const * TranslateTable;
		T Mask;
};

template<class T>
class BlitTransLucent50Alpha : public Blitter {
	public:
		BlitTransLucent50Alpha(T const * translator, int intensity_levels, T mask) : TranslateTable(translator), AlphaLightingRemap(NULL), Mask(mask) {AlphaLightingRemap = AlphaLightingRemapInit.Init(intensity_levels);}
		virtual ~BlitTransLucent50Alpha(void) override { AlphaLightingRemapInit.Deinit(AlphaLightingRemap); AlphaLightingRemap = NULL; }

		virtual void BlitForward(void * dest, void const * source, int length, int z_min = 0, void * z_buff = NULL, void * a_buff = NULL, int alpha_level = 1000, int warp_offset = 0) const override
		{
			unsigned short *ap = (unsigned short *)a_buff;
			const unsigned short* aLUT = AlphaLightingRemap->Get_Table(alpha_level);

			for (int index = 0; index < length; index++) {
				unsigned char color = *(unsigned char const *)source;
				source = ((unsigned char *)source) + 1;
				if (color != 0) {
					*((T *)dest) = (T)((((*(T *)dest) >> 1) & Mask) + ((TranslateTable[color | aLUT[*ap]] >> 1) & Mask));
				}
				dest = ((T *)dest) + 1;

				ap++;
				ap = Blit_Wrap_A_Buffer(ap);
			}
		}

		virtual void BlitBackward(void * dest, void const * source, int length, int z_min = 0, void * z_buff = NULL, void * a_buff = NULL, int alpha_level = 1000) const override {BlitForward(dest, source, length, z_min, z_buff, a_buff, alpha_level);}
		virtual void Blit_Copy_Tinted(void * dest, void const * source, int length, int z_min, void * z_buff, void * a_buff, int alpha_level, int warp_offset, unsigned short tint) const override {BlitForward(dest, source, length, z_min, z_buff, a_buff, alpha_level);}
		virtual void Blit_Move_Tinted(void * dest, void const * source, int length, int z_min, void * z_buff, void * a_buff, int alpha_level, unsigned short tint) const override {BlitForward(dest, source, length, z_min, z_buff, a_buff, alpha_level);}

	private:
		T const * TranslateTable;
		AlphaLightingRemapClass *AlphaLightingRemap;
		T Mask;
};

template<class T>
class BlitTransLucent50AlphaZRead : public Blitter {
	public:
		BlitTransLucent50AlphaZRead(T const * translator, int intensity_levels, T mask) : TranslateTable(translator), AlphaLightingRemap(NULL), Mask(mask) {AlphaLightingRemap = AlphaLightingRemapInit.Init(intensity_levels);}
		virtual ~BlitTransLucent50AlphaZRead(void) override { AlphaLightingRemapInit.Deinit(AlphaLightingRemap); AlphaLightingRemap = NULL; }

		virtual void BlitForward(void * dest, void const * source, int length, int z_min = 0, void * z_buff = NULL, void * a_buff = NULL, int alpha_level = 1000, int warp_offset = 0) const override
		{
			unsigned short *zb = (unsigned short *)z_buff;
			unsigned short *ap = (unsigned short *)a_buff;
			const unsigned short* aLUT = AlphaLightingRemap->Get_Table(alpha_level);

			for (int index = 0; index < length; index++) {
				if (z_min < *zb++) {
					unsigned char color = *(unsigned char const *)source;
					if (color != 0) {
						T qsource = (T)(((TranslateTable[color | aLUT[*ap]] >> 1) & Mask));
						T qdest = (T)((((*(T *)dest) >> 1) & Mask));
						*((T *)dest) = (T)(qdest + qsource);
					}
				}
				source = ((unsigned char *)source) + 1;
				dest = ((T *)dest) + 1;

				zb = Blit_Wrap_Z_Buffer(zb);
				ap++;
				ap = Blit_Wrap_A_Buffer(ap);
			}
		}

		virtual void BlitBackward(void * dest, void const * source, int length, int z_min = 0, void * z_buff = NULL, void * a_buff = NULL, int alpha_level = 1000) const override {BlitForward(dest, source, length, z_min, z_buff, a_buff, alpha_level);}
		virtual void Blit_Copy_Tinted(void * dest, void const * source, int length, int z_min, void * z_buff, void * a_buff, int alpha_level, int warp_offset, unsigned short tint) const override {BlitForward(dest, source, length, z_min, z_buff, a_buff, alpha_level);}
		virtual void Blit_Move_Tinted(void * dest, void const * source, int length, int z_min, void * z_buff, void * a_buff, int alpha_level, unsigned short tint) const override {BlitForward(dest, source, length, z_min, z_buff, a_buff, alpha_level);}

	private:
		T const * TranslateTable;
		AlphaLightingRemapClass *AlphaLightingRemap;
		T Mask;
};

template<class T>
class BlitTransLucent50AlphaZReadWarp : public Blitter {
	public:
		BlitTransLucent50AlphaZReadWarp(T const * translator, int intensity_levels, T mask) : TranslateTable(translator), AlphaLightingRemap(NULL), Mask(mask) {AlphaLightingRemap = AlphaLightingRemapInit.Init(intensity_levels);}
		virtual ~BlitTransLucent50AlphaZReadWarp(void) override { AlphaLightingRemapInit.Deinit(AlphaLightingRemap); AlphaLightingRemap = NULL; }

		virtual void BlitForward(void * dest, void const * source, int length, int z_min = 0, void * z_buff = NULL, void * a_buff = NULL, int alpha_level = 1000, int warp_offset = 0) const override
		{
			unsigned short *zb = (unsigned short *)z_buff;
			unsigned short *ap = (unsigned short *)a_buff;
			const unsigned short* aLUT = AlphaLightingRemap->Get_Table(alpha_level);

			for (int index = 0; index < length; index++) {
				if (z_min < *zb++) {
					unsigned char color = *(unsigned char const *)source;
					if (color != 0) {
						*((T *)dest) = (T)((((((T *)dest)[warp_offset]) >> 1) & Mask) + ((TranslateTable[color | aLUT[*ap]] >> 1) & Mask));
					}
				}
				source = ((unsigned char *)source) + 1;
				dest = ((T *)dest) + 1;

				zb = Blit_Wrap_Z_Buffer(zb);
				ap++;
				ap = Blit_Wrap_A_Buffer(ap);
			}
		}

		virtual void BlitBackward(void * dest, void const * source, int length, int z_min = 0, void * z_buff = NULL, void * a_buff = NULL, int alpha_level = 1000) const override {BlitForward(dest, source, length, z_min, z_buff, a_buff, alpha_level);}
		virtual void Blit_Copy_Tinted(void * dest, void const * source, int length, int z_min, void * z_buff, void * a_buff, int alpha_level, int warp_offset, unsigned short tint) const override {BlitForward(dest, source, length, z_min, z_buff, a_buff, alpha_level);}
		virtual void Blit_Move_Tinted(void * dest, void const * source, int length, int z_min, void * z_buff, void * a_buff, int alpha_level, unsigned short tint) const override {BlitForward(dest, source, length, z_min, z_buff, a_buff, alpha_level);}

	private:
		T const * TranslateTable;
		AlphaLightingRemapClass *AlphaLightingRemap;
		T Mask;
};

template<class T>
class BlitTransLucent50AlphaZReadWrite : public Blitter {
	public:
		BlitTransLucent50AlphaZReadWrite(T const * translator, int intensity_levels, T mask) : TranslateTable(translator), AlphaLightingRemap(NULL), Mask(mask) {AlphaLightingRemap = AlphaLightingRemapInit.Init(intensity_levels);}
		virtual ~BlitTransLucent50AlphaZReadWrite(void) override { AlphaLightingRemapInit.Deinit(AlphaLightingRemap); AlphaLightingRemap = NULL; }

		virtual void BlitForward(void * dest, void const * source, int length, int z_min = 0, void * z_buff = NULL, void * a_buff = NULL, int alpha_level = 1000, int warp_offset = 0) const override
		{
			unsigned short *zb = (unsigned short *)z_buff;
			unsigned short *ap = (unsigned short *)a_buff;
			const unsigned short* aLUT = AlphaLightingRemap->Get_Table(alpha_level);

			for (int index = 0; index < length; index++) {
				if (z_min < *zb) {
					unsigned char color = *(unsigned char const *)source;
					if (color != 0) {
						*((T *)dest) = (T)((((*(T *)dest) >> 1) & Mask) + ((TranslateTable[color | aLUT[*ap]] >> 1) & Mask));
						*zb = z_min;
					}
				}
				source = ((unsigned char *)source) + 1;
				dest = ((T *)dest) + 1;

				zb++;
				zb = Blit_Wrap_Z_Buffer(zb);
				ap++;
				ap = Blit_Wrap_A_Buffer(ap);
			}
		}

		virtual void BlitBackward(void * dest, void const * source, int length, int z_min = 0, void * z_buff = NULL, void * a_buff = NULL, int alpha_level = 1000) const override {BlitForward(dest, source, length, z_min, z_buff, a_buff, alpha_level);}
		virtual void Blit_Copy_Tinted(void * dest, void const * source, int length, int z_min, void * z_buff, void * a_buff, int alpha_level, int warp_offset, unsigned short tint) const override {BlitForward(dest, source, length, z_min, z_buff, a_buff, alpha_level);}
		virtual void Blit_Move_Tinted(void * dest, void const * source, int length, int z_min, void * z_buff, void * a_buff, int alpha_level, unsigned short tint) const override {BlitForward(dest, source, length, z_min, z_buff, a_buff, alpha_level);}

	private:
		T const * TranslateTable;
		AlphaLightingRemapClass *AlphaLightingRemap;
		T Mask;
};

template<class T>
class BlitTransLucent50ZRead : public Blitter {
	public:
		BlitTransLucent50ZRead(T const * translator, T mask) : TranslateTable(translator), Mask(mask) {assert(TranslateTable != NULL);}
		virtual void BlitForward(void * dest, void const * source, int length, int z_min = 0, void * z_buff = NULL, void * a_buff = NULL, int alpha_level = 1000, int warp_offset = 0) const override
		{
			unsigned short *zb = (unsigned short *)z_buff;
			for (int index = 0; index < length; index++) {
				if (z_min < *zb++) {
					unsigned char color = *(unsigned char const *)source;
					if (color != 0) {
						*((T *)dest) = (T)((((*(T *)dest) >> 1) & Mask) + ((TranslateTable[color] >> 1) & Mask));
					}
				}
				source = ((unsigned char *)source) + 1;
				dest = ((T *)dest) + 1;

				zb = Blit_Wrap_Z_Buffer(zb);
			}
		}

		virtual void BlitBackward(void * dest, void const * source, int length, int z_min = 0, void * z_buff = NULL, void * a_buff = NULL, int alpha_level = 1000) const override {BlitForward(dest, source, length, z_min, z_buff, a_buff, alpha_level);}
		virtual void Blit_Copy_Tinted(void * dest, void const * source, int length, int z_min, void * z_buff, void * a_buff, int alpha_level, int warp_offset, unsigned short tint) const override {BlitForward(dest, source, length, z_min, z_buff, a_buff, alpha_level);}
		virtual void Blit_Move_Tinted(void * dest, void const * source, int length, int z_min, void * z_buff, void * a_buff, int alpha_level, unsigned short tint) const override {BlitForward(dest, source, length, z_min, z_buff, a_buff, alpha_level);}

	private:
		T const * TranslateTable;
		T Mask;
};

template<class T>
class BlitTransLucent50ZReadWarp : public Blitter {
	public:
		BlitTransLucent50ZReadWarp(T const * translator, T mask) : TranslateTable(translator), Mask(mask) {assert(TranslateTable != NULL);}
		virtual void BlitForward(void * dest, void const * source, int length, int z_min = 0, void * z_buff = NULL, void * a_buff = NULL, int alpha_level = 1000, int warp_offset = 0) const override
		{
			unsigned short *zb = (unsigned short *)z_buff;
			for (int index = 0; index < length; index++) {
				if (z_min < *zb++) {
					unsigned char color = *(unsigned char const *)source;
					if (color != 0) {
						*((T *)dest) = (T)((((((T *)dest)[warp_offset]) >> 1) & Mask) + ((TranslateTable[color] >> 1) & Mask));
					}
				}
				source = ((unsigned char *)source) + 1;
				dest = ((T *)dest) + 1;

				zb = Blit_Wrap_Z_Buffer(zb);
			}
		}

		virtual void BlitBackward(void * dest, void const * source, int length, int z_min = 0, void * z_buff = NULL, void * a_buff = NULL, int alpha_level = 1000) const override {BlitForward(dest, source, length, z_min, z_buff, a_buff, alpha_level);}
		virtual void Blit_Copy_Tinted(void * dest, void const * source, int length, int z_min, void * z_buff, void * a_buff, int alpha_level, int warp_offset, unsigned short tint) const override {BlitForward(dest, source, length, z_min, z_buff, a_buff, alpha_level);}
		virtual void Blit_Move_Tinted(void * dest, void const * source, int length, int z_min, void * z_buff, void * a_buff, int alpha_level, unsigned short tint) const override {BlitForward(dest, source, length, z_min, z_buff, a_buff, alpha_level);}

	private:
		T const * TranslateTable;
		T Mask;
};

template<class T>
class BlitTransLucent50ZReadWrite : public Blitter {
	public:
		BlitTransLucent50ZReadWrite(T const * translator, T mask) : TranslateTable(translator), Mask(mask) {assert(TranslateTable != NULL);}
		virtual void BlitForward(void * dest, void const * source, int length, int z_min = 0, void * z_buff = NULL, void * a_buff = NULL, int alpha_level = 1000, int warp_offset = 0) const override
		{
			unsigned short *zb = (unsigned short *)z_buff;
			for (int index = 0; index < length; index++) {
				if (z_min < *zb) {
					unsigned char color = *(unsigned char const *)source;
					if (color != 0) {
						*((T *)dest) = (T)((((*(T *)dest) >> 1) & Mask) + ((TranslateTable[color] >> 1) & Mask));
						*zb = z_min;
					}
				}
				zb++;
				source = ((unsigned char *)source) + 1;
				dest = ((T *)dest) + 1;

				zb = Blit_Wrap_Z_Buffer(zb);
			}
		}

		virtual void BlitBackward(void * dest, void const * source, int length, int z_min = 0, void * z_buff = NULL, void * a_buff = NULL, int alpha_level = 1000) const override {BlitForward(dest, source, length, z_min, z_buff, a_buff, alpha_level);}
		virtual void Blit_Copy_Tinted(void * dest, void const * source, int length, int z_min, void * z_buff, void * a_buff, int alpha_level, int warp_offset, unsigned short tint) const override {BlitForward(dest, source, length, z_min, z_buff, a_buff, alpha_level);}
		virtual void Blit_Move_Tinted(void * dest, void const * source, int length, int z_min, void * z_buff, void * a_buff, int alpha_level, unsigned short tint) const override {BlitForward(dest, source, length, z_min, z_buff, a_buff, alpha_level);}

	private:
		T const * TranslateTable;
		T Mask;
};

template<class T>
class BlitTranslucent50NonzeroAlpha : public Blitter {
	public:
		BlitTranslucent50NonzeroAlpha(T const * translator, T mask) : TranslateTable(translator), Mask(mask) {assert(TranslateTable != NULL);}
		virtual void BlitForward(void * dest, void const * source, int length, int z_min = 0, void * z_buff = NULL, void * a_buff = NULL, int alpha_level = 1000, int warp_offset = 0) const override
		{
			unsigned short *ap = (unsigned short *)a_buff;

			for (int i = 0; i < length; ++i) {
				unsigned char value = *(unsigned char *)source;
				source = (unsigned char *)source + 1;

				if (value != 0 && *ap != 0) {
					*(T *)dest = ((*(T *)dest / 2) & Mask) + ((TranslateTable[value] / 2) & Mask);
				}

				ap = ap + 1;
				dest = (T *)dest + 1;

				ap = Blit_Wrap_A_Buffer(ap);
			}
		}

		virtual void BlitBackward(void * dest, void const * source, int length, int z_min = 0, void * z_buff = NULL, void * a_buff = NULL, int alpha_level = 1000) const override {BlitForward(dest, source, length, z_min, z_buff, a_buff, alpha_level);}
		virtual void Blit_Copy_Tinted(void * dest, void const * source, int length, int z_min, void * z_buff, void * a_buff, int alpha_level, int warp_offset, unsigned short tint) const override {BlitForward(dest, source, length, z_min, z_buff, a_buff, alpha_level);}
		virtual void Blit_Move_Tinted(void * dest, void const * source, int length, int z_min, void * z_buff, void * a_buff, int alpha_level, unsigned short tint) const override {BlitForward(dest, source, length, z_min, z_buff, a_buff, alpha_level);}

	private:
		T const * TranslateTable;
		T Mask;
};

template<class T>
class BlitTranslucent50ZeroAlpha : public Blitter {
	public:
		BlitTranslucent50ZeroAlpha(T const * translator, T mask) : TranslateTable(translator), Mask(mask) {assert(TranslateTable != NULL);}
		virtual void BlitForward(void * dest, void const * source, int length, int z_min = 0, void * z_buff = NULL, void * a_buff = NULL, int alpha_level = 1000, int warp_offset = 0) const override
		{
			unsigned short *ap = (unsigned short *)a_buff;

			for (int i = 0; i < length; ++i) {
				unsigned char value = *(unsigned char *)source;
				source = (unsigned char *)source + 1;

				if (value != 0 && *ap == 0) {
					*(T *)dest = ((*(T *)dest / 2) & Mask) + ((TranslateTable[value] / 2) & Mask);
				}

				ap = ap + 1;
				dest = (T *)dest + 1;

				ap = Blit_Wrap_A_Buffer(ap);
			}
		}

		virtual void BlitBackward(void * dest, void const * source, int length, int z_min = 0, void * z_buff = NULL, void * a_buff = NULL, int alpha_level = 1000) const override {BlitForward(dest, source, length, z_min, z_buff, a_buff, alpha_level);}
		virtual void Blit_Copy_Tinted(void * dest, void const * source, int length, int z_min, void * z_buff, void * a_buff, int alpha_level, int warp_offset, unsigned short tint) const override {BlitForward(dest, source, length, z_min, z_buff, a_buff, alpha_level);}
		virtual void Blit_Move_Tinted(void * dest, void const * source, int length, int z_min, void * z_buff, void * a_buff, int alpha_level, unsigned short tint) const override {BlitForward(dest, source, length, z_min, z_buff, a_buff, alpha_level);}

	private:
		T const * TranslateTable;
		T Mask;
};

template<class T>
class BlitTransLucent25 : public Blitter {
	public:
		BlitTransLucent25(T const * translator, T mask) : TranslateTable(translator), Mask(mask) {assert(TranslateTable != NULL);}
		virtual void BlitForward(void * dest, void const * source, int length, int z_min = 0, void * z_buff = NULL, void * a_buff = NULL, int alpha_level = 1000, int warp_offset = 0) const override
		{
			for (int index = 0; index < length; index++) {
				unsigned char color = *(unsigned char const *)source;
				source = ((unsigned char *)source) + 1;
				if (color != 0) {
					T qsource = (T)(((TranslateTable[color] >> 2) & Mask));
					T qdest = (T)((((*(T *)dest) >> 2) & Mask));
					*((T *)dest) = (T)(qdest + qsource + qsource + qsource);
				}
				dest = ((T *)dest) + 1;
			}
		}

		virtual void BlitBackward(void * dest, void const * source, int length, int z_min = 0, void * z_buff = NULL, void * a_buff = NULL, int alpha_level = 1000) const override {BlitForward(dest, source, length, z_min, z_buff, a_buff, alpha_level);}
		virtual void Blit_Copy_Tinted(void * dest, void const * source, int length, int z_min, void * z_buff, void * a_buff, int alpha_level, int warp_offset, unsigned short tint) const override {BlitForward(dest, source, length, z_min, z_buff, a_buff, alpha_level);}
		virtual void Blit_Move_Tinted(void * dest, void const * source, int length, int z_min, void * z_buff, void * a_buff, int alpha_level, unsigned short tint) const override {BlitForward(dest, source, length, z_min, z_buff, a_buff, alpha_level);}

	private:
		T const * TranslateTable;
		T Mask;
};

template<class T>
class BlitTransLucent25Alpha : public Blitter {
	public:
		BlitTransLucent25Alpha(T const * translator, int intensity_levels, T mask) : TranslateTable(translator), AlphaLightingRemap(NULL), Mask(mask) {AlphaLightingRemap = AlphaLightingRemapInit.Init(intensity_levels);}
		virtual ~BlitTransLucent25Alpha(void) override { AlphaLightingRemapInit.Deinit(AlphaLightingRemap); AlphaLightingRemap = NULL; }

		virtual void BlitForward(void * dest, void const * source, int length, int z_min = 0, void * z_buff = NULL, void * a_buff = NULL, int alpha_level = 1000, int warp_offset = 0) const override
		{
			unsigned short *ap = (unsigned short *)a_buff;
			const unsigned short* aLUT = AlphaLightingRemap->Get_Table(alpha_level);

			for (int index = 0; index < length; index++) {
				unsigned char color = *(unsigned char const *)source;
				source = ((unsigned char *)source) + 1;
				if (color != 0) {
					T qsource = (T)(((TranslateTable[color | aLUT[*ap]] >> 2) & Mask));
					T qdest = (T)((((*(T *)dest) >> 2) & Mask));
					*((T *)dest) = (T)(qdest + qsource + qsource + qsource);
				}
				dest = ((T *)dest) + 1;

				ap++;
				ap = Blit_Wrap_A_Buffer(ap);
			}
		}

		virtual void BlitBackward(void * dest, void const * source, int length, int z_min = 0, void * z_buff = NULL, void * a_buff = NULL, int alpha_level = 1000) const override {BlitForward(dest, source, length, z_min, z_buff, a_buff, alpha_level);}
		virtual void Blit_Copy_Tinted(void * dest, void const * source, int length, int z_min, void * z_buff, void * a_buff, int alpha_level, int warp_offset, unsigned short tint) const override {BlitForward(dest, source, length, z_min, z_buff, a_buff, alpha_level);}
		virtual void Blit_Move_Tinted(void * dest, void const * source, int length, int z_min, void * z_buff, void * a_buff, int alpha_level, unsigned short tint) const override {BlitForward(dest, source, length, z_min, z_buff, a_buff, alpha_level);}

	private:
		T const * TranslateTable;
		AlphaLightingRemapClass *AlphaLightingRemap;
		T Mask;
};

template<class T>
class BlitTransLucent25AlphaZRead : public Blitter {
	public:
		BlitTransLucent25AlphaZRead(T const * translator, int intensity_levels, T mask) : TranslateTable(translator), AlphaLightingRemap(NULL), Mask(mask) {AlphaLightingRemap = AlphaLightingRemapInit.Init(intensity_levels);}
		virtual ~BlitTransLucent25AlphaZRead(void) override { AlphaLightingRemapInit.Deinit(AlphaLightingRemap); AlphaLightingRemap = NULL; }

		virtual void BlitForward(void * dest, void const * source, int length, int z_min = 0, void * z_buff = NULL, void * a_buff = NULL, int alpha_level = 1000, int warp_offset = 0) const override
		{
			unsigned short *zb = (unsigned short *)z_buff;
			unsigned short *ap = (unsigned short *)a_buff;
			const unsigned short* aLUT = AlphaLightingRemap->Get_Table(alpha_level);

			for (int index = 0; index < length; index++) {
				if (z_min < *zb++) {
					unsigned char color = *(unsigned char const *)source;
					if (color != 0) {
						T qsource = (T)(((TranslateTable[color | aLUT[*ap]] >> 2) & Mask));
						T qdest = (T)((((*(T *)dest) >> 2) & Mask));
						*((T *)dest) = (T)(qdest + qsource + qsource + qsource);
					}
				}
				source = ((unsigned char *)source) + 1;
				dest = ((T *)dest) + 1;

				zb = Blit_Wrap_Z_Buffer(zb);
				ap++;
				ap = Blit_Wrap_A_Buffer(ap);
			}
		}

		virtual void BlitBackward(void * dest, void const * source, int length, int z_min = 0, void * z_buff = NULL, void * a_buff = NULL, int alpha_level = 1000) const override {BlitForward(dest, source, length, z_min, z_buff, a_buff, alpha_level);}
		virtual void Blit_Copy_Tinted(void * dest, void const * source, int length, int z_min, void * z_buff, void * a_buff, int alpha_level, int warp_offset, unsigned short tint) const override {BlitForward(dest, source, length, z_min, z_buff, a_buff, alpha_level);}
		virtual void Blit_Move_Tinted(void * dest, void const * source, int length, int z_min, void * z_buff, void * a_buff, int alpha_level, unsigned short tint) const override {BlitForward(dest, source, length, z_min, z_buff, a_buff, alpha_level);}

	private:
		T const * TranslateTable;
		AlphaLightingRemapClass *AlphaLightingRemap;
		T Mask;
};

template<class T>
class BlitTransLucent25AlphaZReadWarp : public Blitter {
	public:
		BlitTransLucent25AlphaZReadWarp(T const * translator, int intensity_levels, T mask) : TranslateTable(translator), AlphaLightingRemap(NULL), Mask(mask) {AlphaLightingRemap = AlphaLightingRemapInit.Init(intensity_levels);}
		virtual ~BlitTransLucent25AlphaZReadWarp(void) override { AlphaLightingRemapInit.Deinit(AlphaLightingRemap); AlphaLightingRemap = NULL; }

		virtual void BlitForward(void * dest, void const * source, int length, int z_min = 0, void * z_buff = NULL, void * a_buff = NULL, int alpha_level = 1000, int warp_offset = 0) const override
		{
			unsigned short *zb = (unsigned short *)z_buff;
			unsigned short *ap = (unsigned short *)a_buff;
			const unsigned short* aLUT = AlphaLightingRemap->Get_Table(alpha_level);

			for (int index = 0; index < length; index++) {
				if (z_min < *zb++) {
					unsigned char color = *(unsigned char const *)source;
					if (color != 0) {
						T qsource = (T)(((TranslateTable[color | aLUT[*ap]] >> 2) & Mask));
						T qdest = (T)(((((T *)dest)[warp_offset] >> 2) & Mask));
						*((T *)dest) = (T)(qdest + qsource + qsource + qsource);
					}
				}
				source = ((unsigned char *)source) + 1;
				dest = ((T *)dest) + 1;

				zb = Blit_Wrap_Z_Buffer(zb);
				ap++;
				ap = Blit_Wrap_A_Buffer(ap);
			}
		}

		virtual void BlitBackward(void * dest, void const * source, int length, int z_min = 0, void * z_buff = NULL, void * a_buff = NULL, int alpha_level = 1000) const override {BlitForward(dest, source, length, z_min, z_buff, a_buff, alpha_level);}
		virtual void Blit_Copy_Tinted(void * dest, void const * source, int length, int z_min, void * z_buff, void * a_buff, int alpha_level, int warp_offset, unsigned short tint) const override {BlitForward(dest, source, length, z_min, z_buff, a_buff, alpha_level);}
		virtual void Blit_Move_Tinted(void * dest, void const * source, int length, int z_min, void * z_buff, void * a_buff, int alpha_level, unsigned short tint) const override {BlitForward(dest, source, length, z_min, z_buff, a_buff, alpha_level);}

	private:
		T const * TranslateTable;
		AlphaLightingRemapClass *AlphaLightingRemap;
		T Mask;
};

template<class T>
class BlitTransLucent25AlphaZReadWrite : public Blitter {
	public:
		BlitTransLucent25AlphaZReadWrite(T const * translator, int intensity_levels, T mask) : TranslateTable(translator), AlphaLightingRemap(NULL), Mask(mask) {AlphaLightingRemap = AlphaLightingRemapInit.Init(intensity_levels);}
		virtual ~BlitTransLucent25AlphaZReadWrite(void) override { AlphaLightingRemapInit.Deinit(AlphaLightingRemap); AlphaLightingRemap = NULL; }

		virtual void BlitForward(void * dest, void const * source, int length, int z_min = 0, void * z_buff = NULL, void * a_buff = NULL, int alpha_level = 1000, int warp_offset = 0) const override
		{
			unsigned short *zb = (unsigned short *)z_buff;
			unsigned short *ap = (unsigned short *)a_buff;
			const unsigned short* aLUT = AlphaLightingRemap->Get_Table(alpha_level);

			for (int index = 0; index < length; index++) {
				if (z_min < *zb) {
					unsigned char color = *(unsigned char const *)source;
					if (color != 0) {
						T qsource = (T)(((TranslateTable[color | aLUT[*ap]] >> 2) & Mask));
						T qdest = (T)((((*(T *)dest) >> 2) & Mask));
						*((T *)dest) = (T)(qdest + qsource + qsource + qsource);
						*zb = z_min;
					}
				}
				source = ((unsigned char *)source) + 1;
				dest = ((T *)dest) + 1;

				zb++;
				zb = Blit_Wrap_Z_Buffer(zb);
				ap++;
				ap = Blit_Wrap_A_Buffer(ap);
			}
		}

		virtual void BlitBackward(void * dest, void const * source, int length, int z_min = 0, void * z_buff = NULL, void * a_buff = NULL, int alpha_level = 1000) const override {BlitForward(dest, source, length, z_min, z_buff, a_buff, alpha_level);}
		virtual void Blit_Copy_Tinted(void * dest, void const * source, int length, int z_min, void * z_buff, void * a_buff, int alpha_level, int warp_offset, unsigned short tint) const override {BlitForward(dest, source, length, z_min, z_buff, a_buff, alpha_level);}
		virtual void Blit_Move_Tinted(void * dest, void const * source, int length, int z_min, void * z_buff, void * a_buff, int alpha_level, unsigned short tint) const override {BlitForward(dest, source, length, z_min, z_buff, a_buff, alpha_level);}

	private:
		T const * TranslateTable;
		AlphaLightingRemapClass *AlphaLightingRemap;
		T Mask;
};

template<class T>
class BlitTransLucent25ZRead : public Blitter {
	public:
		BlitTransLucent25ZRead(T const * translator, T mask) : TranslateTable(translator), Mask(mask) {assert(TranslateTable != NULL);}
		virtual void BlitForward(void * dest, void const * source, int length, int z_min = 0, void * z_buff = NULL, void * a_buff = NULL, int alpha_level = 1000, int warp_offset = 0) const override
		{
			unsigned short *zb = (unsigned short *)z_buff;
			for (int index = 0; index < length; index++) {
				if (z_min < *zb++) {
					unsigned char color = *(unsigned char const *)source;
					if (color != 0) {
						T qsource = (T)(((TranslateTable[color] >> 2) & Mask));
						T qdest = (T)((((*(T *)dest) >> 2) & Mask));
						*((T *)dest) = (T)(qdest + qsource + qsource + qsource);
					}
				}
				source = ((unsigned char *)source) + 1;
				dest = ((T *)dest) + 1;

				zb = Blit_Wrap_Z_Buffer(zb);
			}
		}

		virtual void BlitBackward(void * dest, void const * source, int length, int z_min = 0, void * z_buff = NULL, void * a_buff = NULL, int alpha_level = 1000) const override {BlitForward(dest, source, length, z_min, z_buff, a_buff, alpha_level);}
		virtual void Blit_Copy_Tinted(void * dest, void const * source, int length, int z_min, void * z_buff, void * a_buff, int alpha_level, int warp_offset, unsigned short tint) const override {BlitForward(dest, source, length, z_min, z_buff, a_buff, alpha_level);}
		virtual void Blit_Move_Tinted(void * dest, void const * source, int length, int z_min, void * z_buff, void * a_buff, int alpha_level, unsigned short tint) const override {BlitForward(dest, source, length, z_min, z_buff, a_buff, alpha_level);}

	private:
		T const * TranslateTable;
		T Mask;
};

template<class T>
class BlitTransLucent25ZReadWarp : public Blitter {
	public:
		BlitTransLucent25ZReadWarp(T const * translator, T mask) : TranslateTable(translator), Mask(mask) {assert(TranslateTable != NULL);}
		virtual void BlitForward(void * dest, void const * source, int length, int z_min = 0, void * z_buff = NULL, void * a_buff = NULL, int alpha_level = 1000, int warp_offset = 0) const override
		{
			unsigned short *zb = (unsigned short *)z_buff;
			for (int index = 0; index < length; index++) {
				if (z_min < *zb++) {
					unsigned char color = *(unsigned char const *)source;
					if (color != 0) {
						T qsource = (T)(((TranslateTable[color] >> 2) & Mask));
						T qdest = (T)(((((T *)dest)[warp_offset] >> 2) & Mask));
						*((T *)dest) = (T)(qdest + qsource + qsource + qsource);
					}
				}
				source = ((unsigned char *)source) + 1;
				dest = ((T *)dest) + 1;

				zb = Blit_Wrap_Z_Buffer(zb);
			}
		}

		virtual void BlitBackward(void * dest, void const * source, int length, int z_min = 0, void * z_buff = NULL, void * a_buff = NULL, int alpha_level = 1000) const override {BlitForward(dest, source, length, z_min, z_buff, a_buff, alpha_level);}
		virtual void Blit_Copy_Tinted(void * dest, void const * source, int length, int z_min, void * z_buff, void * a_buff, int alpha_level, int warp_offset, unsigned short tint) const override {BlitForward(dest, source, length, z_min, z_buff, a_buff, alpha_level);}
		virtual void Blit_Move_Tinted(void * dest, void const * source, int length, int z_min, void * z_buff, void * a_buff, int alpha_level, unsigned short tint) const override {BlitForward(dest, source, length, z_min, z_buff, a_buff, alpha_level);}

	private:
		T const * TranslateTable;
		T Mask;
};

template<class T>
class BlitTransLucent25ZReadWrite : public Blitter {
	public:
		BlitTransLucent25ZReadWrite(T const * translator, T mask) : TranslateTable(translator), Mask(mask) {assert(TranslateTable != NULL);}
		virtual void BlitForward(void * dest, void const * source, int length, int z_min = 0, void * z_buff = NULL, void * a_buff = NULL, int alpha_level = 1000, int warp_offset = 0) const override
		{
			unsigned short *zb = (unsigned short *)z_buff;
			for (int index = 0; index < length; index++) {
				if (z_min < *zb) {
					unsigned char color = *(unsigned char const *)source;
					if (color != 0) {
						T qsource = (T)(((TranslateTable[color] >> 2) & Mask));
						T qdest = (T)((((*(T *)dest) >> 2) & Mask));
						*((T *)dest) = (T)(qdest + qsource + qsource + qsource);
						*zb = z_min;
					}
				}
				zb++;
				source = ((unsigned char *)source) + 1;
				dest = ((T *)dest) + 1;

				zb = Blit_Wrap_Z_Buffer(zb);
			}
		}

		virtual void BlitBackward(void * dest, void const * source, int length, int z_min = 0, void * z_buff = NULL, void * a_buff = NULL, int alpha_level = 1000) const override {BlitForward(dest, source, length, z_min, z_buff, a_buff, alpha_level);}
		virtual void Blit_Copy_Tinted(void * dest, void const * source, int length, int z_min, void * z_buff, void * a_buff, int alpha_level, int warp_offset, unsigned short tint) const override {BlitForward(dest, source, length, z_min, z_buff, a_buff, alpha_level);}
		virtual void Blit_Move_Tinted(void * dest, void const * source, int length, int z_min, void * z_buff, void * a_buff, int alpha_level, unsigned short tint) const override {BlitForward(dest, source, length, z_min, z_buff, a_buff, alpha_level);}

	private:
		T const * TranslateTable;
		T Mask;
};

template<class T>
class BlitTransLucent75 : public Blitter {
	public:
		BlitTransLucent75(T const * translator, T mask) : TranslateTable(translator), Mask(mask) {assert(TranslateTable != NULL);}
		virtual void BlitForward(void * dest, void const * source, int length, int z_min = 0, void * z_buff = NULL, void * a_buff = NULL, int alpha_level = 1000, int warp_offset = 0) const override
		{
			for (int index = 0; index < length; index++) {
				unsigned char color = *(unsigned char const *)source;
				source = ((unsigned char *)source) + 1;
				if (color != 0) {
					T qsource = (T)(((TranslateTable[color] >> 2) & Mask));
					T qdest = (T)(((*(T *)dest) >> 2) & Mask);
					*((T *)dest) = (T)(qdest + qdest + qdest + qsource);
				}
				dest = ((T *)dest) + 1;
			}
		}

		virtual void BlitBackward(void * dest, void const * source, int length, int z_min = 0, void * z_buff = NULL, void * a_buff = NULL, int alpha_level = 1000) const override {BlitForward(dest, source, length, z_min, z_buff, a_buff, alpha_level);}
		virtual void Blit_Copy_Tinted(void * dest, void const * source, int length, int z_min, void * z_buff, void * a_buff, int alpha_level, int warp_offset, unsigned short tint) const override {BlitForward(dest, source, length, z_min, z_buff, a_buff, alpha_level);}
		virtual void Blit_Move_Tinted(void * dest, void const * source, int length, int z_min, void * z_buff, void * a_buff, int alpha_level, unsigned short tint) const override {BlitForward(dest, source, length, z_min, z_buff, a_buff, alpha_level);}

	private:
		T const * TranslateTable;
		T Mask;
};

template<class T>
class BlitTransLucent75Alpha : public Blitter {
	public:
		BlitTransLucent75Alpha(T const * translator, int intensity_levels, T mask) : TranslateTable(translator), AlphaLightingRemap(NULL), Mask(mask) {AlphaLightingRemap = AlphaLightingRemapInit.Init(intensity_levels);}
		virtual ~BlitTransLucent75Alpha(void) override { AlphaLightingRemapInit.Deinit(AlphaLightingRemap); AlphaLightingRemap = NULL; }

		virtual void BlitForward(void * dest, void const * source, int length, int z_min = 0, void * z_buff = NULL, void * a_buff = NULL, int alpha_level = 1000, int warp_offset = 0) const override
		{
			unsigned short *ap = (unsigned short *)a_buff;
			const unsigned short* aLUT = AlphaLightingRemap->Get_Table(alpha_level);

			for (int index = 0; index < length; index++) {
				unsigned char color = *(unsigned char const *)source;
				source = ((unsigned char *)source) + 1;
				if (color != 0) {
					T qsource = (T)(((TranslateTable[color | aLUT[*ap]] >> 2) & Mask));
					T qdest = (T)(((*(T *)dest) >> 2) & Mask);
					*((T *)dest) = (T)(qdest + qdest + qdest + qsource);
				}
				dest = ((T *)dest) + 1;

				ap++;
				ap = Blit_Wrap_A_Buffer(ap);
			}
		}

		virtual void BlitBackward(void * dest, void const * source, int length, int z_min = 0, void * z_buff = NULL, void * a_buff = NULL, int alpha_level = 1000) const override {BlitForward(dest, source, length, z_min, z_buff, a_buff, alpha_level);}
		virtual void Blit_Copy_Tinted(void * dest, void const * source, int length, int z_min, void * z_buff, void * a_buff, int alpha_level, int warp_offset, unsigned short tint) const override {BlitForward(dest, source, length, z_min, z_buff, a_buff, alpha_level);}
		virtual void Blit_Move_Tinted(void * dest, void const * source, int length, int z_min, void * z_buff, void * a_buff, int alpha_level, unsigned short tint) const override {BlitForward(dest, source, length, z_min, z_buff, a_buff, alpha_level);}

	private:
		T const * TranslateTable;
		AlphaLightingRemapClass *AlphaLightingRemap;
		T Mask;
};

template<class T>
class BlitTransLucent75AlphaZRead : public Blitter {
	public:
		BlitTransLucent75AlphaZRead(T const * translator, int intensity_levels, T mask) : TranslateTable(translator), AlphaLightingRemap(NULL), Mask(mask) {AlphaLightingRemap = AlphaLightingRemapInit.Init(intensity_levels);}
		virtual ~BlitTransLucent75AlphaZRead(void) override { AlphaLightingRemapInit.Deinit(AlphaLightingRemap); AlphaLightingRemap = NULL; }

		virtual void BlitForward(void * dest, void const * source, int length, int z_min = 0, void * z_buff = NULL, void * a_buff = NULL, int alpha_level = 1000, int warp_offset = 0) const override
		{
			unsigned short *zb = (unsigned short *)z_buff;
			unsigned short *ap = (unsigned short *)a_buff;
			const unsigned short* aLUT = AlphaLightingRemap->Get_Table(alpha_level);

			for (int index = 0; index < length; index++) {
				if (z_min < *zb++) {
					unsigned char color = *(unsigned char const *)source;
					if (color != 0) {
						*((T *)dest) = (T)((3 * (((*(T *)dest) >> 2) & Mask)) + ((TranslateTable[color | aLUT[*ap]] >> 2) & Mask));
					}
				}
				source = ((unsigned char *)source) + 1;
				dest = ((T *)dest) + 1;

				zb = Blit_Wrap_Z_Buffer(zb);
				ap++;
				ap = Blit_Wrap_A_Buffer(ap);
			}
		}

		virtual void BlitBackward(void * dest, void const * source, int length, int z_min = 0, void * z_buff = NULL, void * a_buff = NULL, int alpha_level = 1000) const override {BlitForward(dest, source, length, z_min, z_buff, a_buff, alpha_level);}
		virtual void Blit_Copy_Tinted(void * dest, void const * source, int length, int z_min, void * z_buff, void * a_buff, int alpha_level, int warp_offset, unsigned short tint) const override {BlitForward(dest, source, length, z_min, z_buff, a_buff, alpha_level);}
		virtual void Blit_Move_Tinted(void * dest, void const * source, int length, int z_min, void * z_buff, void * a_buff, int alpha_level, unsigned short tint) const override {BlitForward(dest, source, length, z_min, z_buff, a_buff, alpha_level);}

	private:
		T const * TranslateTable;
		AlphaLightingRemapClass *AlphaLightingRemap;
		T Mask;
};

template<class T>
class BlitTransLucent75AlphaZReadWarp : public Blitter {
	public:
		BlitTransLucent75AlphaZReadWarp(T const * translator, int intensity_levels, T mask) : TranslateTable(translator), AlphaLightingRemap(NULL), Mask(mask) {AlphaLightingRemap = AlphaLightingRemapInit.Init(intensity_levels);}
		virtual ~BlitTransLucent75AlphaZReadWarp(void) override { AlphaLightingRemapInit.Deinit(AlphaLightingRemap); AlphaLightingRemap = NULL; }

		virtual void BlitForward(void * dest, void const * source, int length, int z_min = 0, void * z_buff = NULL, void * a_buff = NULL, int alpha_level = 1000, int warp_offset = 0) const override
		{
			unsigned short *zb = (unsigned short *)z_buff;
			unsigned short *ap = (unsigned short *)a_buff;
			const unsigned short* aLUT = AlphaLightingRemap->Get_Table(alpha_level);

			for (int index = 0; index < length; index++) {
				if (z_min < *zb++) {
					unsigned char color = *(unsigned char const *)source;
					if (color != 0) {
						T qsource = (T)(((TranslateTable[color | aLUT[*ap]] >> 2) & Mask));
						T qdest = (T)(((((T *)dest)[warp_offset] >> 2) & Mask));
						*((T *)dest) = (T)(qdest + qdest + qdest + qsource);
					}
				}
				source = ((unsigned char *)source) + 1;
				dest = ((T *)dest) + 1;

				zb = Blit_Wrap_Z_Buffer(zb);
				ap++;
				ap = Blit_Wrap_A_Buffer(ap);
			}
		}

		virtual void BlitBackward(void * dest, void const * source, int length, int z_min = 0, void * z_buff = NULL, void * a_buff = NULL, int alpha_level = 1000) const override {BlitForward(dest, source, length, z_min, z_buff, a_buff, alpha_level);}
		virtual void Blit_Copy_Tinted(void * dest, void const * source, int length, int z_min, void * z_buff, void * a_buff, int alpha_level, int warp_offset, unsigned short tint) const override {BlitForward(dest, source, length, z_min, z_buff, a_buff, alpha_level);}
		virtual void Blit_Move_Tinted(void * dest, void const * source, int length, int z_min, void * z_buff, void * a_buff, int alpha_level, unsigned short tint) const override {BlitForward(dest, source, length, z_min, z_buff, a_buff, alpha_level);}

	private:
		T const * TranslateTable;
		AlphaLightingRemapClass *AlphaLightingRemap;
		T Mask;
};

template<class T>
class BlitTransLucent75AlphaZReadWrite : public Blitter {
	public:
		BlitTransLucent75AlphaZReadWrite(T const * translator, int intensity_levels, T mask) : TranslateTable(translator), AlphaLightingRemap(NULL), Mask(mask) {AlphaLightingRemap = AlphaLightingRemapInit.Init(intensity_levels);}
		virtual ~BlitTransLucent75AlphaZReadWrite(void) override { AlphaLightingRemapInit.Deinit(AlphaLightingRemap); AlphaLightingRemap = NULL; }

		virtual void BlitForward(void * dest, void const * source, int length, int z_min = 0, void * z_buff = NULL, void * a_buff = NULL, int alpha_level = 1000, int warp_offset = 0) const override
		{
			unsigned short *zb = (unsigned short *)z_buff;
			unsigned short *ap = (unsigned short *)a_buff;
			const unsigned short* aLUT = AlphaLightingRemap->Get_Table(alpha_level);

			for (int index = 0; index < length; index++) {
				if (z_min < *zb) {
					unsigned char color = *(unsigned char const *)source;
					if (color != 0) {
						*((T *)dest) = (T)((3 * (((*(T *)dest) >> 2) & Mask)) + ((TranslateTable[color | aLUT[*ap]] >> 2) & Mask));
						*zb = z_min;
					}
				}
				source = ((unsigned char *)source) + 1;
				dest = ((T *)dest) + 1;

				zb++;
				zb = Blit_Wrap_Z_Buffer(zb);
				ap++;
				ap = Blit_Wrap_A_Buffer(ap);
			}
		}

		virtual void BlitBackward(void * dest, void const * source, int length, int z_min = 0, void * z_buff = NULL, void * a_buff = NULL, int alpha_level = 1000) const override {BlitForward(dest, source, length, z_min, z_buff, a_buff, alpha_level);}
		virtual void Blit_Copy_Tinted(void * dest, void const * source, int length, int z_min, void * z_buff, void * a_buff, int alpha_level, int warp_offset, unsigned short tint) const override {BlitForward(dest, source, length, z_min, z_buff, a_buff, alpha_level);}
		virtual void Blit_Move_Tinted(void * dest, void const * source, int length, int z_min, void * z_buff, void * a_buff, int alpha_level, unsigned short tint) const override {BlitForward(dest, source, length, z_min, z_buff, a_buff, alpha_level);}

	private:
		T const * TranslateTable;
		AlphaLightingRemapClass *AlphaLightingRemap;
		T Mask;
};

template<class T>
class BlitTransLucent75ZRead : public Blitter {
	public:
		BlitTransLucent75ZRead(T const * translator, T mask) : TranslateTable(translator), Mask(mask) {assert(TranslateTable != NULL);}
		virtual void BlitForward(void * dest, void const * source, int length, int z_min = 0, void * z_buff = NULL, void * a_buff = NULL, int alpha_level = 1000, int warp_offset = 0) const override
		{
			unsigned short *zb = (unsigned short *)z_buff;
			for (int index = 0; index < length; index++) {
				if (z_min < *zb++) {
					unsigned char color = *(unsigned char const *)source;
					if (color != 0) {
						T qsource = (T)(((TranslateTable[color] >> 2) & Mask));
						T qdest = (T)(((*(T *)dest) >> 2) & Mask);
						*((T *)dest) = (T)(qdest + qdest + qdest + qsource);
					}
				}
				source = ((unsigned char *)source) + 1;
				dest = ((T *)dest) + 1;

				zb = Blit_Wrap_Z_Buffer(zb);
			}
		}

		virtual void BlitBackward(void * dest, void const * source, int length, int z_min = 0, void * z_buff = NULL, void * a_buff = NULL, int alpha_level = 1000) const override {BlitForward(dest, source, length, z_min, z_buff, a_buff, alpha_level);}
		virtual void Blit_Copy_Tinted(void * dest, void const * source, int length, int z_min, void * z_buff, void * a_buff, int alpha_level, int warp_offset, unsigned short tint) const override {BlitForward(dest, source, length, z_min, z_buff, a_buff, alpha_level);}
		virtual void Blit_Move_Tinted(void * dest, void const * source, int length, int z_min, void * z_buff, void * a_buff, int alpha_level, unsigned short tint) const override {BlitForward(dest, source, length, z_min, z_buff, a_buff, alpha_level);}

	private:
		T const * TranslateTable;
		T Mask;
};

template<class T>
class BlitTransLucent75ZReadWarp : public Blitter {
	public:
		BlitTransLucent75ZReadWarp(T const * translator, T mask) : TranslateTable(translator), Mask(mask) {assert(TranslateTable != NULL);}
		virtual void BlitForward(void * dest, void const * source, int length, int z_min = 0, void * z_buff = NULL, void * a_buff = NULL, int alpha_level = 1000, int warp_offset = 0) const override
		{
			unsigned short *zb = (unsigned short *)z_buff;
			for (int index = 0; index < length; index++) {
				if (z_min < *zb++) {
					unsigned char color = *(unsigned char const *)source;
					if (color != 0) {
						T qsource = (T)(((TranslateTable[color] >> 2) & Mask));
						T qdest = (T)(((((T *)dest)[warp_offset]) >> 2) & Mask);
						*((T *)dest) = (T)(qdest + qdest + qdest + qsource);
					}
				}
				source = ((unsigned char *)source) + 1;
				dest = ((T *)dest) + 1;

				zb = Blit_Wrap_Z_Buffer(zb);
			}
		}

		virtual void BlitBackward(void * dest, void const * source, int length, int z_min = 0, void * z_buff = NULL, void * a_buff = NULL, int alpha_level = 1000) const override {BlitForward(dest, source, length, z_min, z_buff, a_buff, alpha_level);}
		virtual void Blit_Copy_Tinted(void * dest, void const * source, int length, int z_min, void * z_buff, void * a_buff, int alpha_level, int warp_offset, unsigned short tint) const override {BlitForward(dest, source, length, z_min, z_buff, a_buff, alpha_level);}
		virtual void Blit_Move_Tinted(void * dest, void const * source, int length, int z_min, void * z_buff, void * a_buff, int alpha_level, unsigned short tint) const override {BlitForward(dest, source, length, z_min, z_buff, a_buff, alpha_level);}

	private:
		T const * TranslateTable;
		T Mask;
};

template<class T>
class BlitTransLucent75ZReadWrite : public Blitter {
	public:
		BlitTransLucent75ZReadWrite(T const * translator, T mask) : TranslateTable(translator), Mask(mask) {assert(TranslateTable != NULL);}
		virtual void BlitForward(void * dest, void const * source, int length, int z_min = 0, void * z_buff = NULL, void * a_buff = NULL, int alpha_level = 1000, int warp_offset = 0) const override
		{
			unsigned short *zb = (unsigned short *)z_buff;
			for (int index = 0; index < length; index++) {
				if (z_min < *zb) {
					unsigned char color = *(unsigned char const *)source;
					if (color != 0) {
						T qsource = (T)(((TranslateTable[color] >> 2) & Mask));
						T qdest = (T)(((*(T *)dest) >> 2) & Mask);
						*((T *)dest) = (T)(qdest + qdest + qdest + qsource);
						*zb = z_min;
					}
				}
				zb++;
				source = ((unsigned char *)source) + 1;
				dest = ((T *)dest) + 1;

				zb = Blit_Wrap_Z_Buffer(zb);
			}
		}

		virtual void BlitBackward(void * dest, void const * source, int length, int z_min = 0, void * z_buff = NULL, void * a_buff = NULL, int alpha_level = 1000) const override {BlitForward(dest, source, length, z_min, z_buff, a_buff, alpha_level);}
		virtual void Blit_Copy_Tinted(void * dest, void const * source, int length, int z_min, void * z_buff, void * a_buff, int alpha_level, int warp_offset, unsigned short tint) const override {BlitForward(dest, source, length, z_min, z_buff, a_buff, alpha_level);}
		virtual void Blit_Move_Tinted(void * dest, void const * source, int length, int z_min, void * z_buff, void * a_buff, int alpha_level, unsigned short tint) const override {BlitForward(dest, source, length, z_min, z_buff, a_buff, alpha_level);}

	private:
		T const * TranslateTable;
		T Mask;
};

template<class T>
class BlitTranslucent75NonzeroAlpha : public Blitter {
	public:
		BlitTranslucent75NonzeroAlpha(T const * translator, T mask) : TranslateTable(translator), Mask(mask) {assert(TranslateTable != NULL);}
		virtual void BlitForward(void * dest, void const * source, int length, int z_min = 0, void * z_buff = NULL, void * a_buff = NULL, int alpha_level = 1000, int warp_offset = 0) const override
		{
			unsigned short *ap = (unsigned short *)a_buff;

			for (int i = 0; i < length; ++i) {
				unsigned char value = *(unsigned char *)source;
				source = (unsigned char *)source + 1;

				if (value != 0 && *ap != 0) {
					*(T *)dest = ((TranslateTable[value] >> 2) & Mask) + 3 * ((*(T *)dest >> 2) & Mask);
				}

				ap = ap + 1;
				dest = (T *)dest + 1;

				ap = Blit_Wrap_A_Buffer(ap);
			}
		}

		virtual void BlitBackward(void * dest, void const * source, int length, int z_min = 0, void * z_buff = NULL, void * a_buff = NULL, int alpha_level = 1000) const override {BlitForward(dest, source, length, z_min, z_buff, a_buff, alpha_level);}
		virtual void Blit_Copy_Tinted(void * dest, void const * source, int length, int z_min, void * z_buff, void * a_buff, int alpha_level, int warp_offset, unsigned short tint) const override {BlitForward(dest, source, length, z_min, z_buff, a_buff, alpha_level);}
		virtual void Blit_Move_Tinted(void * dest, void const * source, int length, int z_min, void * z_buff, void * a_buff, int alpha_level, unsigned short tint) const override {BlitForward(dest, source, length, z_min, z_buff, a_buff, alpha_level);}

	private:
		T const * TranslateTable;
		T Mask;
};

template<class T>
class BlitTranslucent75ZeroAlpha : public Blitter {
	public:
		BlitTranslucent75ZeroAlpha(T const * translator, T mask) : TranslateTable(translator), Mask(mask) {assert(TranslateTable != NULL);}
		virtual void BlitForward(void * dest, void const * source, int length, int z_min = 0, void * z_buff = NULL, void * a_buff = NULL, int alpha_level = 1000, int warp_offset = 0) const override
		{
			unsigned short *ap = (unsigned short *)a_buff;

			for (int i = 0; i < length; ++i) {
				unsigned char value = *(unsigned char *)source;
				source = (unsigned char *)source + 1;

				if (value != 0 && *ap == 0) {
					*(T *)dest = ((TranslateTable[value] >> 2) & Mask) + 3 * ((*(T *)dest >> 2) & Mask);
				}

				ap = ap + 1;
				dest = (T *)dest + 1;

				ap = Blit_Wrap_A_Buffer(ap);
			}
		}

		virtual void BlitBackward(void * dest, void const * source, int length, int z_min = 0, void * z_buff = NULL, void * a_buff = NULL, int alpha_level = 1000) const override {BlitForward(dest, source, length, z_min, z_buff, a_buff, alpha_level);}
		virtual void Blit_Copy_Tinted(void * dest, void const * source, int length, int z_min, void * z_buff, void * a_buff, int alpha_level, int warp_offset, unsigned short tint) const override {BlitForward(dest, source, length, z_min, z_buff, a_buff, alpha_level);}
		virtual void Blit_Move_Tinted(void * dest, void const * source, int length, int z_min, void * z_buff, void * a_buff, int alpha_level, unsigned short tint) const override {BlitForward(dest, source, length, z_min, z_buff, a_buff, alpha_level);}

	private:
		T const * TranslateTable;
		T Mask;
};

template<class T>
class BlitTranslucentWriteAlpha : public Blitter {
	public:
		BlitTranslucentWriteAlpha(T const * translator) : TranslateTable(translator) {assert(TranslateTable != NULL);}
		virtual void BlitForward(void * dest, void const * source, int length, int z_min = 0, void * z_buff = NULL, void * a_buff = NULL, int alpha_level = 1000, int warp_offset = 0) const override
		{
			unsigned short *ap = (unsigned short *)a_buff;

			for (int i = 0; i < length; ++i) {
				unsigned char value = *(unsigned char *)source;
				source = (unsigned char *)source + 1;

				if (value != 0) {
					int alpha1 = *ap;
					int alpha2 = 255 - alpha1;
					if (alpha1 == 255) {
						alpha1 = 256;
					}

					RGBClass rgb1 = DSurface::Deconstruct_Hicolor_Pixel(TranslateTable[value]);
					RGBClass rgb2 = DSurface::Deconstruct_Hicolor_Pixel(*(T *)dest);

					int r = ((rgb1.Get_Red() * alpha1) + (rgb2.Get_Red() * alpha2)) >> 8;
					int g = ((rgb1.Get_Green() * alpha1) + (rgb2.Get_Green() * alpha2)) >> 8;
					int b = ((rgb1.Get_Blue() * alpha1) + (rgb2.Get_Blue() * alpha2)) >> 8;

					*(T *)dest = DSurface::Build_Hicolor_Pixel(min(255, r), min(255, g), min(255, b));
				}

				dest = (T *)dest + 1;

				ap++;
				ap = Blit_Wrap_A_Buffer(ap);
			}
		}

		virtual void BlitBackward(void * dest, void const * source, int length, int z_min = 0, void * z_buff = NULL, void * a_buff = NULL, int alpha_level = 1000) const override {BlitForward(dest, source, length, z_min, z_buff, a_buff, alpha_level);}
		virtual void Blit_Copy_Tinted(void * dest, void const * source, int length, int z_min, void * z_buff, void * a_buff, int alpha_level, int warp_offset, unsigned short tint) const override {BlitForward(dest, source, length, z_min, z_buff, a_buff, alpha_level);}
		virtual void Blit_Move_Tinted(void * dest, void const * source, int length, int z_min, void * z_buff, void * a_buff, int alpha_level, unsigned short tint) const override {BlitForward(dest, source, length, z_min, z_buff, a_buff, alpha_level);}

	private:
		T const * TranslateTable;
};
