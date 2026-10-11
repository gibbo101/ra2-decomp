// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"
#include "msanim.h"
#include "_mixfile.h"
#include "_palette.h"
#include "_surface.h"
#include "ccfile.h"
#include "convert.h"
#include "data.h"
#include "dbgprint.h"
#include "draw.h"
#include "dsaudio.h"
#include "globals.h"
#include "goptions.h"
#include "mixfile.h"
#include "movies.h"
#include "msfont.h"
#include "ownrdraw.h"
#include "pcx.h"
#include "shapeset.h"
#include "srfcache.h"

// FUNCTION: 0x005CB840
void MSShapeAnim::Set_Start_Frame(unsigned frame)
{
	StartFrame = frame;
	if (StartFrame >= (unsigned)Shape->Get_Count()) {
		StartFrame = Shape->Get_Count() - 1;
	}
}

// FUNCTION: 0x005CB860
void MSShapeAnim::Set_Stop_Frame(unsigned frame)
{
	StopFrame = frame;
	if (StopFrame >= (unsigned)Shape->Get_Count()) {
		StopFrame = Shape->Get_Count() - 1;
	}
}

// FUNCTION: 0x005CC850
Rect MSButtonAnim::Get_Rect(void) const
{
	return(Area);
}

// FUNCTION: 0x005CD0E0
void MSVQAnim::Redraw(Surface * surface, const Rect * rect)
{
	if (Movie != NULL && !Done) {
		if (rect == NULL || Intersect(*rect, Movie->StretchRect).Is_Valid()) {
			surface->Blit_From(Movie->InitialRect, *AlternateSurface, Movie->StretchRect);
		}
	}
}

// FUNCTION: 0x005CD290
Rect MSVQAnim::Get_Rect(void) const
{
	static Rect _rect_none(0,0,0,0);
	return(Movie != NULL ? Movie->StretchRect : _rect_none);
}

// FUNCTION: 0x005CE2B0
Rect MSPrintAnim::Get_Rect(void) const
{
	return(Area);
}

// FUNCTION: 0x005CE2E0
void MSPrintAnim::Set_Transient(bool transient)
{
	if (transient != Transient) {
		Transient = transient;
	}
}

// FUNCTION: 0x005CEA80
Rect MSPCXAnim::Get_Rect(void) const
{
	return(Area);
}
