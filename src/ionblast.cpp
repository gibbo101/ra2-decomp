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
const double WaveAmplitudeFalloff = 51.0;
const double WaveRockScale = DEG_TO_RAD(360);
const double WaveSpeedScale = 0.0125;
const double WaveRange = 512.0;

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

static inline float Ion_Vector_Length(Vector3 const &v)
{
	return (float)fastmath::sqrt(
		(double)v.X * v.X +
		(double)v.Y * v.Y +
		(double)v.Z * v.Z);
}
// FUNCTION: GAMEMD 0x0053CBE0
void IonBlastClass::AI()
{
	if (Lifetime >= 79) {
		delete this;
		return;
	}

	if (DisableIonBeam != 1) {
		Point2D point;
		TacticalMap->Coord_To_Pixel(Position, point);

		if (Lifetime == 0) {
			if (Map[Position].Land_Type() == 2) {
				new AnimClass(
					Rule->SplashList[Rule->SplashList.Count() - 1],
					Position + Coord(0, 0, 5), 0, 1,
					ShapeFlags_Type(SHAPE_CENTER | SHAPE_WIN_REL), 0);
			} else {
				new AnimClass(
					Rule->IonBlast, Position + Coord(0, 0, 5), 0, 1,
					ShapeFlags_Type(SHAPE_CENTER | SHAPE_WIN_REL), 0);
			}

			new AnimClass(
				Rule->IonBeam, Position + Coord(0, 0, 5), 0, 1,
				ShapeFlags_Type(SHAPE_CENTER | SHAPE_WIN_REL), 0);

			if (Map[Position].IsUnderBridge) {
				Coord coord = Coord(
					Position.X, Position.Y,
					BRIDGE_LEPTON_HEIGHT + Position.Z);
				Explosion_Damage(
					coord, Rule->IonCannonDamage, 0,
					Rule->IonCannonWarhead, 1);
			}

			Explosion_Damage(
				Position, Rule->IonCannonDamage, 0,
				Rule->IonCannonWarhead, 1);
			Combat_Lighting(
				Position, Rule->IonCannonDamage,
				Rule->IonCannonWarhead, 0, 0);
		}

		Cell const &center = Position.As_Cell();

		for (int x_offset = -3; x_offset < 4; x_offset++) {
			for (int y_offset = -3; y_offset < 4; y_offset++) {
				CellClass &cell = Map[center + Cell(x_offset, y_offset)];
				ObjectClass *occupier = cell.Cell_Occupier();

				while (occupier != 0) {
					if (occupier->Fetch_RTTI() == RTTI_INFANTRY ||
						occupier->Fetch_RTTI() == RTTI_UNIT) {
						FootClass *foot = (FootClass *)occupier;
						Point2D occupier_point;
						TacticalMap->Coord_To_Pixel(
							occupier->Get_Coord(), occupier_point);

						Point2D diff = occupier_point - point;
						int screen_dist = (int)fastmath::sqrt(
							(double)diff.Y * diff.Y +
							(double)diff.X * diff.X) + 8;

						if (screen_dist < 256) {
							char *displacement =
								(char *)BlastSurfaces[Lifetime]->Lock(
									Point2D(screen_dist + 256, 128));

							if (*displacement > 0) {
								foot->Set_Speed(0.0);
								int draw_offset =
									Spiral_Index_To_Point(*displacement).Y;
								foot->IonBlastYDrawOffset = 2 * draw_offset;
							}

							VoxelLibrary *voxel;
							if (foot->Techno_Type_Class()->Voxel.VoxLib != NULL &&
								(voxel = foot->Techno_Type_Class()->Voxel.VoxLib,
								 !voxel->Load_Failed()) &&
								*displacement >= 0) {
								Coord blast_coord = Position;
								Coord occupier_coord = occupier->Get_Coord();

								Vector3 to_blast;
								to_blast.X =
									(float)(blast_coord.X - occupier_coord.X);
								to_blast.Y =
									(float)(occupier_coord.Y - blast_coord.Y);
								to_blast.Z =
									(float)(blast_coord.Z - occupier_coord.Z);
								Vector3 flat = to_blast;

								double facing_rad =
									foot->PrimaryFacing.Current().As_Radian();
								float facing_sin =
									(float)fastmath::sin(facing_rad);
								Vector3 facing(
									(float)fastmath::cos(facing_rad),
									facing_sin, 0.0f);

								float blast_dist = Ion_Vector_Length(to_blast);

								if (fabs(blast_dist) > 0.00002) {
									flat.Z = 0.0;
									double flat_len = fastmath::sqrt(
										flat.X * flat.X + flat.Y * flat.Y);

									Vector3 normal;
									if (flat_len != 0.0) {
										normal = flat / flat_len;
									} else {
										normal = flat;
									}

									float forwards = normal * facing;
									float sideways = Ion_Vector_Length(
										Vector3::Cross_Product(normal, facing));

									float c2 = facing.X;
									float s2 = facing_sin;
									float c3 = facing.X;
									float s3 = facing_sin;
									if (fabs(c2 * forwards - s2 * sideways -
											 normal.X) > 0.0002 ||
										fabs(c3 * sideways + s3 * forwards -
											 normal.Y) > 0.0002) {
										sideways = -sideways;
									}

									double decay =
										blast_dist + WaveAmplitudeFalloff;
									double height =
										(fastmath::sin(
											(blast_dist -
											 (double)Lifetime * WavePhaseSpeed +
											 38.0) * WaveFrequency) * 3.5 + 3.0) *
										WaveAmplitudeFalloff;
									double slope =
										((fastmath::cos(
											(blast_dist -
											 (double)Lifetime * WavePhaseSpeed +
											 38.0) * WaveFrequency) *
										  WaveFrequency *
										  WaveAmplitudeFalloff * 3.5) *
										 decay - height) /
										(decay * decay);

									foot->AngleRotatedSideways =
										(float)(sideways * slope * WaveRockScale);
									foot->AngleRotatedForwards =
										(float)(-(forwards * slope *
												  WaveRockScale));
								}
							}
						}
					}
					occupier = occupier->Next;
				}
			}
		}
	}

	Lifetime++;
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
