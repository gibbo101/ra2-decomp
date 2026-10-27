// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"
#include "geometry.h"
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

// SYNTHETIC: GAMEMD 0x0053C820 SYMBOL
// _$E2
// SYNTHETIC: GAMEMD 0x0053C850 SYMBOL
// _$E5
// SYNTHETIC: GAMEMD 0x0053C870 SYMBOL
// _$E8
// SYNTHETIC: GAMEMD 0x0053C890 SYMBOL
// _$E11
// SYNTHETIC: GAMEMD 0x0053C8B0 SYMBOL
// _$E14
// SYNTHETIC: GAMEMD 0x0053C8D0 SYMBOL
// _$E17
// SYNTHETIC: GAMEMD 0x0053C8F0 SYMBOL
// _$E20
// SYNTHETIC: GAMEMD 0x0053C930 SYMBOL
// _$E23
// SYNTHETIC: GAMEMD 0x0053C960 SYMBOL
// _$E26
// SYNTHETIC: GAMEMD 0x0053C990 SYMBOL
// _$E29
// SYNTHETIC: GAMEMD 0x0053C9C0 SYMBOL
// _$E32
// SYNTHETIC: GAMEMD 0x0053C9D0 SYMBOL
// _$E35
// SYNTHETIC: GAMEMD 0x0053C9F0 SYMBOL
// _$E38
// SYNTHETIC: GAMEMD 0x0053CA10 SYMBOL
// _$E42

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

// SYNTHETIC: GAMEMD 0x0053CA30 SYMBOL
// _$E134
double WaveSpeed = ((WaveRange - WaveStartDistance) + WaveBandWidth) * WaveSpeedScale;
// SYNTHETIC: GAMEMD 0x0053CA50 SYMBOL
// _$E137
double WavePhaseSpeed = WaveSpeed;

// SYNTHETIC: GAMEMD 0x0053CA70 SYMBOL
// _$E143
DynamicVectorClass<IonBlastClass *> IonBlastClass::IonBlasts;

int Point_To_Spiral_Index(Point2D point);

void Calculate_Index_To_Surface_LUT(int stride);

Point2D Spiral_Index_To_Point(int index);

// SYNTHETIC: GAMEMD 0x0053CAB0 SYMBOL
// _$E140

// FUNCTION: GAMEMD 0x0053CB10
IonBlastClass::IonBlastClass(Coord coord) :
	Lifetime(0),
	Position(coord),
	DisableIonBeam(0)
{
	IonBlasts.Add(this);
}

// FUNCTION: GAMEMD 0x0053CB90
IonBlastClass::~IonBlastClass(void)
{
	IonBlasts.Delete(this);
}

// FUNCTION: GAMEMD 0x0053D310
void IonBlastClass::Update_All(void)
{
	for (int i = IonBlasts.Count() - 1; i >= 0; i--) {
		IonBlasts[i]->AI();
	}
}

// FUNCTION: GAMEMD 0x0053D550
void IonBlastClass::Clear_All(void)
{
	for (int i = 0; i < ARRAY_SIZE(BlastSurfaces); i++) {
		delete BlastSurfaces[i];
		BlastSurfaces[i] = NULL;
	}
}

// FUNCTION: GAMEMD 0x0053D850
void IonBlastClass::Draw_All(void)
{
	if (LogicalSurface->Stride() != BlastSurfaceStride) {
		Calculate_Index_To_Surface_LUT(LogicalSurface->Stride());
	}
	for (int i = IonBlasts.Count() - 1; i >= 0; i--) {
		IonBlasts[i]->Draw_It();
	}
}

// FUNCTION: GAMEMD 0x0053D8E0
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

// FUNCTION: GAMEMD 0x0053D960
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

// FUNCTION: GAMEMD 0x0053D9F0
void Calculate_Index_To_Surface_LUT(int stride)
{
	SpiralIndexToSurfaceLUT[0] = 0;

	BlastSurfaceStride = stride;

	for (int index = 1; index < ARRAY_SIZE(SpiralIndexToSurfaceLUT); index++) {
		Point2D point = Spiral_Index_To_Point(index);
		SpiralIndexToSurfaceLUT[index] = point.X + point.Y * stride;
	}
}

// TEMPLATE: GAMEMD 0x0053DA40
// ?Clear@?$DynamicVectorClass@PAVIonBlastClass@@@@UAEXXZ
template class DynamicVectorClass<IonBlastClass *>;

// TEMPLATE: GAMEMD 0x0053DA70
// ?ID@?$DynamicVectorClass@PAVIonBlastClass@@@@UAEHPBQAVIonBlastClass@@@Z
// TEMPLATE: GAMEMD 0x0053DA90
// ?GetItem@?$VectorClass@PAVIonBlastClass@@@@UBEPAVIonBlastClass@@H@Z
// TEMPLATE: GAMEMD 0x0053DAA0
// ??8?$VectorClass@PAVIonBlastClass@@@@UBE_NABV0@@Z
// TEMPLATE: GAMEMD 0x0053DAE0
// ?Resize@?$VectorClass@PAVIonBlastClass@@@@UAE_NHPBQAVIonBlastClass@@@Z
// TEMPLATE: GAMEMD 0x0053DB90
// ?Clear@?$VectorClass@PAVIonBlastClass@@@@UAEXXZ
// TEMPLATE: GAMEMD 0x0053DBC0
// ?ID@?$VectorClass@PAVIonBlastClass@@@@UAEHABQAVIonBlastClass@@@Z
// TEMPLATE: GAMEMD 0x0053DC00
// ?ID@?$VectorClass@PAVIonBlastClass@@@@UAEHPBQAVIonBlastClass@@@Z
// TEMPLATE: GAMEMD 0x0053DC20
// ?Resize@?$DynamicVectorClass@PAVIonBlastClass@@@@UAE_NHPBQAVIonBlastClass@@@Z
// TEMPLATE: GAMEMD 0x0053DCD0
// ?ID@?$DynamicVectorClass@PAVIonBlastClass@@@@UAEHABQAVIonBlastClass@@@Z
// SYNTHETIC: GAMEMD 0x0053DD00
// ??_G?$VectorClass@PAVIonBlastClass@@@@UAEPAXI@Z
// SYNTHETIC: GAMEMD 0x0053DD50
// ??_G?$DynamicVectorClass@PAVIonBlastClass@@@@UAEPAXI@Z
// TEMPLATE: GAMEMD 0x0053DDA0
// ?Delete_Index@?$DynamicVectorClass@PAVIonBlastClass@@@@QAE_NH@Z
