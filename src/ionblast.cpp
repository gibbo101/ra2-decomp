// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"
#include "ionblast.h"
#include "_rect.h"
#include "_rules.h"
#include "_surface.h"
#include "_tactica.h"
#include "_zbuffer.h"
#include "anim.h"
#include "bsurface.h"
#include "building.h"
#include "cell.h"
#include "combat.h"
#include "foot.h"
#include "globals.h"
#include "goptions.h"
#include "infantry.h"
#include "rules.h"
#include "tactical.h"
#include "unit.h"
#include "vector.h"
#include "vector3.h"
#include "zbuffer.h"

int SpiralIndexToSurfaceLUT[289];

int BlastSurfaceStride = 0;

BSurface * BlastSurfaces[80];

const double WaveStartDistance = 0.0;
const double WaveBandWidth = 57.0;
const double WaveFrequency = 0.11;
const double WaveAmplitudeFalloff = 25.0;
const double WaveRockScale = DEG_TO_RAD(360);
const double WaveSpeedScale = 0.0125;
const double WaveRange = 256.0;

// FUNCTION: 0x0053CA30 $E89
double WaveSpeed = ((WaveRange - WaveStartDistance) + WaveBandWidth) * WaveSpeedScale;
// FUNCTION: 0x0053CA50 $E92
double WavePhaseSpeed = WaveSpeed;

// FUNCTION: 0x0053CA70 $E98
DynamicVectorClass<IonBlastClass *> IonBlastClass::IonBlasts;

int Point_To_Spiral_Index(Point2D point);

void Calculate_Index_To_Surface_LUT(int stride);

Point2D Spiral_Index_To_Point(int index);

// FUNCTION: 0x0053CAB0 $E95

// FUNCTION: 0x0053CB10
IonBlastClass::IonBlastClass(Coord coord) :
	Lifetime(0),
	Position(coord),
	DisableIonBeam(0)
{
	IonBlasts.Add(this);
}

// FUNCTION: 0x0053CB90
IonBlastClass::~IonBlastClass(void)
{
	IonBlasts.Delete(this);
}

// FUNCTION: 0x0053D310
void IonBlastClass::Update_All(void)
{
	for (int i = IonBlasts.Count() - 1; i >= 0; i--) {
		IonBlasts[i]->AI();
	}
}

// FUNCTION: 0x0053D550
void IonBlastClass::Clear_All(void)
{
	for (int i = 0; i < ARRAY_SIZE(BlastSurfaces); i++) {
		delete BlastSurfaces[i];
		BlastSurfaces[i] = NULL;
	}
}

// FUNCTION: 0x0053D850
void IonBlastClass::Draw_All(void)
{
	if (LogicalSurface->Stride() != BlastSurfaceStride) {
		Calculate_Index_To_Surface_LUT(LogicalSurface->Stride());
	}
	for (int i = IonBlasts.Count() - 1; i >= 0; i--) {
		IonBlasts[i]->Draw_It();
	}
}

// FUNCTION: 0x0053D8E0
Point2D Spiral_Index_To_Point(int index)
{
	index--;

	int layer = 1;

	if (index >= 8) {
		for (int i = 8; index >= i; i += 8) {
			index -= i;
			layer++;
		}
	}

	if (index < 2 * layer + 1) {
		return(Point2D(layer, index - layer));
	} else if (index < 4 * layer + 1) {
		return(Point2D(3 * layer - index, layer));
	} else if (index < 6 * layer + 1) {
		return(Point2D(-layer, 5 * layer - index));
	} else {
		return(Point2D(index - 7 * layer, -layer));
	}
}

// FUNCTION: 0x0053D960
int Point_To_Spiral_Index(Point2D point)
{
	if (point == Point2D(0,0)) {
		return(0);
	}

	int layer = MAX(abs(point.X), abs(point.Y));

	int index = 1;

	for (int i = 1; i < layer; i++) {
		index += 8 * i;
	}

	if (point.X == layer) {
		index += 1 * layer + point.Y;
	} else if (point.Y == layer) {
		index += 3 * layer - point.X;
	} else if (point.X == -layer) {
		index += 5 * layer - point.Y;
	} else {
		index += 7 * layer + point.X;
	}

	return(index);
}

// FUNCTION: 0x0053D9F0
void Calculate_Index_To_Surface_LUT(int stride)
{
	SpiralIndexToSurfaceLUT[0] = 0;

	BlastSurfaceStride = stride;

	for (int index = 1; index < ARRAY_SIZE(SpiralIndexToSurfaceLUT); index++) {
		Point2D point = Spiral_Index_To_Point(index);
		SpiralIndexToSurfaceLUT[index] = point.X + point.Y * stride;
	}
}

// FUNCTION: 0x0053DA40 ?$DynamicVectorClass@PAVIonBlastClass@@::Clear
template class DynamicVectorClass<IonBlastClass *>;

// FUNCTION: 0x0053DA70 ?$VectorClass@PAVIonBlastClass@@::ID
// FUNCTION: 0x0053DA90 ?$VectorClass@PAVIonBlastClass@@::GetItem
// FUNCTION: 0x0053DAA0 ?$VectorClass@PAVIonBlastClass@@::operator==
