// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).
#pragma once

#include "buff.h"
#include "xsurface.h"

class BSurface : public XSurface
{
		typedef XSurface BASECLASS;

	friend class DisplayClass;
	public:
		BSurface(int width, int height, int bbp, void * buffer=NULL) :
			BASECLASS(width, height),
			BBP(bbp),
			Buff(buffer, width * height * bbp)
		{
		}

		virtual void * Lock(Point2D point = Point2D(0, 0)) const override
		{
			BASECLASS::Lock();
			return(((char*)Buff.Get_Buffer()) + point.Y * Stride() + point.X * Bytes_Per_Pixel());
		}

		virtual int Bytes_Per_Pixel(void) const override {return(BBP);}
		virtual int Stride(void) const override {return(Get_Width() * BBP);}

	protected:

		int BBP;

		Buffer Buff;
};
