// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"
#include "blowfish.h"
#include "assert.h"
#include "blit.h"
#include "bsurface.h"
#include "_alpha.h"
#include "_zbuffer.h"
#include "rect.h"
#ifndef NO_BLOWFISH_DLL
#include "iblowfish.h"
#endif

// FUNCTION: GAMEMD 0x004371B0
int Buffer_Size(Surface & surface, int width, int height)
{
	return(width * height * surface.Bytes_Per_Pixel());
}

// FUNCTION: GAMEMD 0x004371D0
bool To_Buffer(Surface const & surface, Rect const & rect, Buffer & buffer)
{
	if (!rect.Is_Valid()) return(false);

	BSurface from(rect.Width, rect.Height, surface.Bytes_Per_Pixel(), buffer);
	return(from.Blit_From(Rect(0, 0, rect.Width, rect.Height), surface, rect));
}

// FUNCTION: GAMEMD 0x00437290
bool From_Buffer(Surface & surface, Rect const & rect, Buffer const & buffer)
{
	if (!rect.Is_Valid()) return(false);

	BSurface from(rect.Width, rect.Height, surface.Bytes_Per_Pixel(), buffer);
	return(surface.Blit_From(rect, from, Rect(0, 0, rect.Width, rect.Height)));
}

// FUNCTION: GAMEMD 0x00437350
bool Bit_Blit(Surface & dest, Rect const & destrect, Surface const & source, Rect const & sourcerect, Blitter const & blitter, int zdepth, ZGradientType zgrad, int alpha, int unknown)
{
	return(Bit_Blit(dest, dest.Get_Rect(), destrect, source, source.Get_Rect(), sourcerect, blitter, zdepth, zgrad, alpha, unknown));
}

// FUNCTION: GAMEMD 0x004373B0
bool Bit_Blit(Surface & dest, Rect const & dcliprect, Rect const & ddrect, Surface const & source, Rect const & scliprect, Rect const & ssrect, Blitter const & blitter, int zdepth, ZGradientType zgrad, int alpha, unsigned short tint, int)
{
	Rect srect = ssrect;
	Rect drect = ddrect;
	bool overlapped = false;
	void * dbuffer = NULL;
	void * sbuffer = NULL;
	int zbuffer_offset = 0;
	int abuffer_offset = 0;
	int current_z = 0;
	int zbuffer_pitch = 0;
	int abuffer_pitch = 0;
	int z_fraction = 0;
	int z_step_denominator = 0;
	int z_step_numerator = 0;
	int z_step_ratio = 0;
	int warp_offset = 0;
	ZGradStruct * gradient = &ZGradients[zgrad];
	Rect origdrect = ddrect;

	if (!XSurface::Prep_For_Blit(dest, dcliprect, drect, source, scliprect, srect, overlapped, dbuffer, sbuffer)) {
		return(false);
	}

	if (AlphaBuffer != NULL) {
		Point2D pt = dcliprect.Top_Left() + drect.Top_Left();
		pt.Y -= AlphaBuffer->Bounds.Y;
		abuffer_offset = AlphaBuffer->Get_Buffer_Offset(pt);
		warp_offset = ((pt.Y ^ pt.X) & 1) | ((pt.Y & 1) << 1);
	}

	if (DepthBuffer != NULL) {
		Point2D pt = dcliprect.Top_Left() + drect.Top_Left();
		pt.Y -= DepthBuffer->Bounds.Y;
		zbuffer_offset = DepthBuffer->Get_Buffer_Offset(pt);

		z_fraction = 0;
		z_step_denominator = gradient->StepDenominator;
		z_step_numerator = gradient->StepNumerator;
		z_step_ratio = z_step_numerator / z_step_denominator;

		if (gradient->IsTopDown) {
			current_z = (unsigned short)DepthBuffer->Get_Scroll_Delta(dcliprect.Y + drect.Y - DepthBuffer->Bounds.Y);
			current_z += zdepth;
			current_z = (current_z / gradient->LineIncrement) * gradient->LineIncrement;
		}

		else {
			current_z = (unsigned short)DepthBuffer->Get_Scroll_Delta(dcliprect.Y + drect.Y + drect.Height - 1 - DepthBuffer->Bounds.Y);
			current_z += zdepth;

			current_z = (current_z / z_step_ratio) * z_step_ratio;
			current_z -= drect.Height / z_step_ratio;
			z_fraction = z_step_numerator - drect.Height % z_step_ratio;

			if (z_fraction == z_step_numerator) {
				z_fraction = 0;
				current_z += gradient->WrapIncrement;
			}
		}
	}

	if (drect.Width * dest.Bytes_Per_Pixel() == dest.Stride() && dest.Stride() == source.Stride()) {
		int length = MIN(srect.Height*srect.Width, drect.Height*drect.Width);
		if (overlapped) {
			blitter.BlitBackward(dbuffer, sbuffer, length);
		} else {
			blitter.BlitForward(dbuffer, sbuffer, length);
		}
	} else {
		int sstride = source.Stride();
		int dstride = dest.Stride();
		if (DepthBuffer != NULL) {
			zbuffer_pitch = DepthBuffer->Get_Buffer_Width();
		}
		if (AlphaBuffer != NULL) {
			abuffer_pitch = AlphaBuffer->Get_Buffer_Width();
		}
		if (overlapped) {
			sstride = -sstride;
			dstride = -dstride;
			zbuffer_pitch = -zbuffer_pitch;
			abuffer_pitch = -abuffer_pitch;
			sbuffer = ((char*)sbuffer) + (srect.Height-1) * source.Stride();
			dbuffer = ((char*)dbuffer) + (drect.Height-1) * dest.Stride();
			if (DepthBuffer != NULL) {
				zbuffer_offset = (zbuffer_offset + 2 * srect.Height) - 2 * DepthBuffer->Get_Buffer_Width();
				zbuffer_offset = DepthBuffer->Wrap_Overflow(zbuffer_offset);
				current_z -= srect.Height - 1;
			}
			if (AlphaBuffer != NULL) {
				abuffer_offset = (abuffer_offset + 2 * srect.Height) - 2 * AlphaBuffer->Get_Buffer_Width();
				abuffer_offset = AlphaBuffer->Wrap_Overflow(abuffer_offset);
			}
		}

		int height = MIN(srect.Height, drect.Height);
		if (overlapped) {
			for (int y = 0; y < height; y++) {
				if (tint) {
					blitter.Blit_Move_Tinted(dbuffer, sbuffer, srect.Width, current_z, (void *)zbuffer_offset, (void *)abuffer_offset, alpha, tint);
				} else {
					blitter.BlitBackward(dbuffer, sbuffer, srect.Width, current_z, (void *)zbuffer_offset, (void *)abuffer_offset, alpha);
				}
				dbuffer = (void*)(((char*)dbuffer) + dstride);
				sbuffer = (void*)(((char*)sbuffer) + sstride);
				if (DepthBuffer != NULL) {
					zbuffer_offset += 2 * zbuffer_pitch;
					zbuffer_offset = DepthBuffer->Wrap_Underflow(zbuffer_offset);
					z_fraction += z_step_denominator;
					if (z_fraction >= z_step_numerator) {
						current_z += gradient->WrapIncrement;
						z_fraction -= z_step_numerator;
					}
				}
				if (AlphaBuffer != NULL) {
					abuffer_offset += 2 * abuffer_pitch;
					abuffer_offset = AlphaBuffer->Wrap_Underflow(abuffer_offset);
				}
			}
		} else {
			for (int y = 0; y < height; y++) {
				if (tint) {
					blitter.Blit_Copy_Tinted(dbuffer, sbuffer, srect.Width, current_z, (void *)zbuffer_offset, (void *)abuffer_offset, alpha, warp_offset, tint);
				} else {
					blitter.BlitForward(dbuffer, sbuffer, srect.Width, current_z, (void *)zbuffer_offset, (void *)abuffer_offset, alpha, warp_offset);
				}
				warp_offset ^= 3;
				dbuffer = (void*)(((char*)dbuffer) + dstride);
				sbuffer = (void*)(((char*)sbuffer) + sstride);
				if (DepthBuffer != NULL) {
					zbuffer_offset += 2 * zbuffer_pitch;
					zbuffer_offset = DepthBuffer->Wrap_Overflow(zbuffer_offset);
					z_fraction += z_step_denominator;
					if (z_fraction >= z_step_numerator) {
						current_z += gradient->WrapIncrement;
						z_fraction -= z_step_numerator;
					}
				}
				if (AlphaBuffer != NULL) {
					abuffer_offset += 2 * abuffer_pitch;
					abuffer_offset = AlphaBuffer->Wrap_Overflow(abuffer_offset);
				}
			}
		}
	}

	dest.Unlock();
	source.Unlock();

	return(true);
}

// FUNCTION: GAMEMD 0x004379A0
bool RLE_Blit(Surface & dest, Rect const & destrect, Surface const & source, Rect const & sourcerect, RLEBlitter const & blitter, int zdepth, ZGradientType zgrad, int alpha, int unknown)
{
	return(RLE_Blit(dest, dest.Get_Rect(), destrect, source, source.Get_Rect(), sourcerect, blitter, zdepth, zgrad, alpha, unknown, NULL, Point2D(0, 0)));
}

// FUNCTION: GAMEMD 0x00437A10
bool RLE_Blit(Surface & dest, Rect const & dcliprect, Rect const & ddrect, Surface const & source, Rect const & scliprect, Rect const & ssrect, RLEBlitter const & blitter, int zdepth, ZGradientType zgrad, int alpha, int, Surface * zshape, Point2D zpoint, unsigned short tint)
{
	static char _temp_buf[256];

	int zbuffer_offset = 0;
	int abuffer_offset = 0;
	char * zshapelock = NULL;
	int zbuffer_pitch = 0;
	int abuffer_pitch = 0;
	int current_z = 0;

	Rect srect = ssrect;
	Rect drect = ddrect;

	int z_fraction = 0;
	int z_step_denominator = 0;
	int z_step_numerator = 0;
	int z_step_ratio;

	int zshapew = 0;
	int warp_offset = 0;

	ZGradStruct * grad = &ZGradients[zgrad];

	if (!Blit_Clip(drect, dcliprect, srect, scliprect)) {
		return(false);
	}

	int leftmargin = srect.X - scliprect.X;
	int topmargin = srect.Y - scliprect.Y;

	void * dbuffer = dest.Lock(dcliprect.Top_Left() + drect.Top_Left());
	if (dbuffer == NULL) return(false);

	if (DepthBuffer != NULL) {
		Point2D pt = dcliprect.Top_Left() + drect.Top_Left();
		pt.Y -= DepthBuffer->Bounds.Y;
		zbuffer_offset = DepthBuffer->Get_Buffer_Offset(pt);
		zbuffer_pitch = DepthBuffer->Get_Buffer_Width();
		z_fraction = 0;
		z_step_denominator = grad->StepDenominator;
		z_step_numerator = grad->StepNumerator;
		z_step_ratio = z_step_numerator / z_step_denominator;

		if (grad->IsTopDown) {
			current_z = (unsigned short)DepthBuffer->Get_Scroll_Delta(pt.Y);
			current_z += zdepth;
			current_z = (current_z / grad->LineIncrement) * grad->LineIncrement;
		} else {
			current_z = (unsigned short)DepthBuffer->Get_Scroll_Delta(dcliprect.Y + ddrect.Y + ddrect.Height - 1 - DepthBuffer->Bounds.Y);
			current_z += zdepth;
			if (zshape == NULL) {
				current_z = (current_z / z_step_ratio) * z_step_ratio;
				int zy = ddrect.Y + ddrect.Height - drect.Y;
				current_z -= zy / z_step_ratio;
				z_fraction = z_step_numerator - zy % z_step_ratio;
				if (z_fraction == z_step_numerator) {
					z_fraction = 0;
					current_z += grad->WrapIncrement;
				}
			}
		}
		if (zshape != NULL) {
			Rect zrect = zshape->Get_Rect();
			zshapew = zrect.Width;
			zshapelock = (char *)zshape->Lock(Point2D(zrect.X, zrect.Y) + zpoint + Point2D(leftmargin, topmargin));
		} else {
			zshapelock = _temp_buf;
		}
	}

	if (AlphaBuffer != NULL) {
		Point2D pt = dcliprect.Top_Left() + drect.Top_Left();
		pt.Y -= AlphaBuffer->Bounds.Y;
		abuffer_offset = AlphaBuffer->Get_Buffer_Offset(pt);
		abuffer_pitch = AlphaBuffer->Get_Buffer_Width();
		warp_offset = ((pt.Y ^ pt.X) & 1) | ((pt.Y & 1) << 1);
	}

	void * sbuffer = source.Lock();
	if (sbuffer == NULL) {
		dest.Unlock();
		return(false);
	}

	while (topmargin > 0) {
		warp_offset ^= 3;
		sbuffer = ((unsigned char *)sbuffer) + (*(unsigned short *)sbuffer);
		topmargin--;
	}

	int dstride = dest.Stride();
	int height = MIN(srect.Height, drect.Height);

	for (int y = 0; y < height; y++) {
		if (tint) {
			blitter.Blit_Copy_Tinted(dbuffer, ((unsigned short *)sbuffer) + 1, srect.Width, leftmargin, current_z, (void *)zbuffer_offset, (void *)abuffer_offset, alpha, warp_offset, (void *)zshapelock, tint);
		} else {
			blitter.Blit(dbuffer, ((unsigned short *)sbuffer) + 1, srect.Width, leftmargin, current_z, (void *)zbuffer_offset, (void *)abuffer_offset, alpha, warp_offset, (void *)zshapelock);
		}

		warp_offset ^= 3;
		sbuffer = ((unsigned char *)sbuffer) + (*(unsigned short *)sbuffer);
		dbuffer = (void *)(((char *)dbuffer) + dstride);
		if (DepthBuffer != NULL) {
			zbuffer_offset += 2 * zbuffer_pitch;
			zbuffer_offset = DepthBuffer->Wrap_Overflow(zbuffer_offset);
			if (zshape != NULL) {
				zshapelock += zshapew;
			} else {
				z_fraction += z_step_denominator;
				if (z_fraction >= z_step_numerator) {
					current_z += grad->WrapIncrement;
					z_fraction -= z_step_numerator;
				}
			}
			abuffer_offset += 2 * abuffer_pitch;
			abuffer_offset = AlphaBuffer->Wrap_Overflow(abuffer_offset);
		}
	}

	dest.Unlock();
	source.Unlock();

	return(true);
}

// TEMPLATE: GAMEMD 0x00437F10
// ??H?$TPoint2D@H@@QBE?BV0@ABV0@@Z

// TEMPLATE: GAMEMD 0x00437F30
// ?Top_Left@?$TRect@H@@QBE?AV?$TPoint2D@H@@XZ

// FUNCTION: GAMEMD 0x00437F50
BlowfishEngine::BlowfishEngine(void) :
	BlockCypher(CLSID_BlowfishObject)
{
}

// FUNCTION: GAMEMD 0x00437FC0
BlowfishEngine::~BlowfishEngine(void)
{
#ifdef NO_BLOWFISH_DLL
	if (IsKeyed) {
		Submit_Key(NULL, 0);
	}
#endif
}

// FUNCTION: GAMEMD 0x00437FD0
void BlowfishEngine::Submit_Key(void const * key, int length)
{
#ifndef NO_BLOWFISH_DLL
	BlockCypher->Set_Key(length, key);
	return;
#else
	assert(length <= MAX_KEY_LENGTH);

	memcpy(P_Encrypt, P_Init, sizeof(P_Init));
	memcpy(P_Decrypt, P_Init, sizeof(P_Init));
	memcpy(bf_S, S_Init, sizeof(S_Init));

	if (key == 0 || length == 0) {
		IsKeyed = false;
		return;
	}

	int j = 0;
	unsigned char const * key_ptr = (unsigned char const *)key;
	unsigned int * p_ptr = &P_Encrypt[0];
	for (int index = 0; index < ROUNDS+2; index++) {
		unsigned int data = 0;

		data = (data << CHAR_BIT) | key_ptr[j++ % length];
		data = (data << CHAR_BIT) | key_ptr[j++ % length];
		data = (data << CHAR_BIT) | key_ptr[j++ % length];
		data = (data << CHAR_BIT) | key_ptr[j++ % length];

		*p_ptr++ ^= data;
	}

	unsigned int left = 0x00000000L;
	unsigned int right = 0x00000000L;
	unsigned int * p_en = &P_Encrypt[0];
	unsigned int * p_de = &P_Decrypt[ROUNDS+1];
	for (int p_index = 0; p_index < ROUNDS+2; p_index += 2) {
		Sub_Key_Encrypt(left, right);

		*p_en++ = left;
		*p_en++ = right;

		*p_de-- = left;
		*p_de-- = right;
	}

	for (int sbox_index = 0; sbox_index < 4; sbox_index++) {
		for (int ss_index = 0; ss_index < UCHAR_MAX+1; ss_index += 2) {
			Sub_Key_Encrypt(left, right);
			bf_S[sbox_index][ss_index] = left;
			bf_S[sbox_index][ss_index + 1] = right;
		}
	}

	IsKeyed = true;
#endif
}

// FUNCTION: GAMEMD 0x00438000
int BlowfishEngine::Encrypt(void const * plaintext, int length, void * cyphertext)
{
#ifndef NO_BLOWFISH_DLL
	BlockCypher->Encrypt(length, plaintext, cyphertext);
	return(length);
#else
	if (plaintext == 0 || length == 0) {
		return(0);
	}
	if (cyphertext == 0) cyphertext = (void *)plaintext;

	if (IsKeyed) {
		int blocks = length / BYTES_PER_BLOCK;

		for (int index = 0; index < blocks; index++) {
			Process_Block(plaintext, cyphertext, P_Encrypt);
			plaintext = ((char *)plaintext) + BYTES_PER_BLOCK;
			cyphertext = ((char *)cyphertext) + BYTES_PER_BLOCK;
		}
		int encrypted = blocks * BYTES_PER_BLOCK;

		if (encrypted < length) {
			memmove(cyphertext, plaintext, length - encrypted);
		}

		return(encrypted);
	}

	if (plaintext != cyphertext) {
		memmove(cyphertext, plaintext, length);
	}
	return(length);
#endif
}

// FUNCTION: GAMEMD 0x00438030
int BlowfishEngine::Decrypt(void const * cyphertext, int length, void * plaintext)
{
#ifndef NO_BLOWFISH_DLL
	BlockCypher->Decrypt(length, cyphertext, plaintext);
	return(length);
#else
	if (cyphertext == 0 || length == 0) {
		return(0);
	}
	if (plaintext == 0) plaintext = (void *)cyphertext;

	if (IsKeyed) {
		int blocks = length / BYTES_PER_BLOCK;

		for (int index = 0; index < blocks; index++) {
			Process_Block(cyphertext, plaintext, P_Decrypt);
			cyphertext = ((char *)cyphertext) + BYTES_PER_BLOCK;
			plaintext = ((char *)plaintext) + BYTES_PER_BLOCK;
		}
		int encrypted = blocks * BYTES_PER_BLOCK;

		if (encrypted < length) {
			memmove(plaintext, cyphertext, length - encrypted);
		}

		return(encrypted);
	}

	if (plaintext != cyphertext) {
		memmove(plaintext, cyphertext, length);
	}
	return(length);
#endif
}
