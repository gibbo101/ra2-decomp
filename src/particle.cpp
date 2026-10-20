// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#define INCLUDE_COM
#include "always.h"
#include "geometry.h"
#include "particle.h"
#include "_alpha.h"
#include "_convert.h"
#include "_map.h"
#include "_rect.h"
#include "_rules.h"
#include "_surface.h"
#include "_tactica.h"
#include "_zbuffer.h"
#include "abuffer.h"
#include "building.h"
#include "cell.h"
#include "convert.h"
#include "draw.h"
#include "dsurface.h"
#include "globals.h"
#include "goptions.h"
#include "house.h"
#include "houstype.h"
#include "inline.h"
#include "mouse.h"
#include "partsys.h"
#include "psystype.h"
#include "ptype.h"
#include "rgb.h"
#include "rules.h"
#include "scenario.h"
#include "sun.h"
#include "surface.h"
#include "swizzle.h"
#include "tactical.h"
#include "tracker.h"
#include "unit.h"
#include "wave.h"
#include "zbuffer.h"
#include "draw.hh"

namespace Unsorted { extern int CurrentFrame; extern bool ArmageddonMode; }
namespace FPSCounter { extern unsigned CurrentFrameRate; }
namespace Detail { unsigned GetMinFrameRate(void); }

LEPTON ParticleClass::SmokeWindX[FACING_COUNT] = { 0, 2, 2, 1, 0, -2, -2, -2 };
LEPTON ParticleClass::SmokeWindY[FACING_COUNT] = { -2, -2, 0, 2, 2, 2, 0, -2 };
LEPTON ParticleClass::GasWindX[FACING_COUNT] = { 0, 2, 2, 2, 0, -2, -2, -2 };
LEPTON ParticleClass::GasWindY[FACING_COUNT] = { -2, -2, 0, 2, 2, 2, 0, -2 };

// FUNCTION: 0x0062B5E0
ParticleClass::ParticleClass(ParticleTypeClass const * type, Coord const & origin, Coord const & target, ParticleSystemClass * partsys) :
	Class(type),
	Color(0,0,0),
	ColorAccum(0),
	ColorIndex(0),
	GasDrift(0,0,0),
	GasVelocity(0,0,-1),
	UnusedCoord1(0,0,0),
	Speed(Class->Velocity),
	FireTarget(target),
	FireOrigin(origin),
	FireMoveDelta(COORD_NONE),
	MovementDirection(target.X - origin.X, target.Y - origin.Y, target.Z - origin.Z),
	PrecisePosition(origin.X, origin.Y, origin.Z),
	System(partsys),
	StateAIAdvance(Class->StateAIAdvance),
	IsFireBelowGround(false),
	StateAI(Class->StartStateAI),
	Translucency(Class->Translucency),
	WasSaved(false),
	IsToDie(false)
{
	Create_ID();
	Particles.Add(this);

	if (Class->BehavesLike == BEHAVIOR_SMOKE) {
		Speed += Scen->RandomNumber() % 2;
	}

	RemainingDC = Class->MaxDC;
	if (Class->BehavesLike == BEHAVIOR_RAILGUN) {
		RemainingEC = Class->MaxEC + abs((Scen->RandomNumber()) % 10);
	} else {
		int random = Scen->RandomNumber();
		RemainingEC = Class->MaxEC + abs(random % Class->MaxEC);
	}

	Vector3 normalized;
	Coord coord(origin.X, origin.Y, origin.Z);
	if (coord.Z <= Map.Get_Height_GL(origin)) {
		coord.Z = Map.Get_Height_GL(origin);
	}
	PositionCoord = coord;

	float length = MovementDirection.Length();
	if (length != 0.0) {
		normalized = MovementDirection / length;
	} else {
		normalized = MovementDirection;
	}
	MovementDirection = normalized;

	if (Class->IsNormalized) {
		Coord delta(origin.X - target.X, origin.Y - target.Y, 0);

		float abs_vx = abs(MovementDirection.X * Speed);
		float abs_vy = abs(MovementDirection.Y * Speed);

		float min_time = 9999.0f;

		if (abs_vx > 1e-6) {
			min_time = abs(delta.X) / abs_vx;
		}

		if (abs_vy > 1e-6) {
			min_time = MIN(min_time, abs(delta.Y) / abs_vy);
		}

		StateAIAdvance = (int)(min_time / (Class->FinalDamageState + 1) + 1.0);
	}

	if (Class->ColorList.Count() > 0) {
		if (Class->StartColor1 == RGBClass(0,0,0) && Class->StartColor2 == RGBClass(0,0,0)) {
			Color = Class->ColorList[0];
		} else {
			Color = RGBClass(NoInitClass()).Lerp(Class->StartColor1, Class->StartColor2, Random_Double(0.0,1.0));
		}
	}

	Unlimbo(coord);
}

// FUNCTION: 0x0062BB20
ParticleClass::ParticleClass(void) :
	Class(NULL),
	Color(0,0,0),
	ColorAccum(0),
	ColorIndex(0),
	GasDrift(0,0,0),
	GasVelocity(0,0,-1),
	UnusedCoord1(0,0,0),
	Speed(0),
	FireTarget(COORD_NONE),
	FireOrigin(COORD_NONE),
	FireMoveDelta(COORD_NONE),
	MovementDirection(0,0,0),
	PrecisePosition(0,0,0),
	System(NULL),
	StateAIAdvance(0),
	IsFireBelowGround(false),
	StateAI(0),
	Translucency(0),
	WasSaved(false),
	IsToDie(false)
{
	Particles.Add(this);
}

// FUNCTION: 0x0062BCC0
ParticleClass::~ParticleClass(void)
{
	Limbo();
	Detach_This_From_All(this);
	Class = NULL;
	Particles.Delete(this);
}

// FUNCTION: 0x0062C3A0
void ParticleClass::Railgun_Behavior_AI(void)
{
	Vector3 direction(MovementDirection.X, MovementDirection.Y, MovementDirection.Z);
	float speed = Speed;
	Vector3 velocity = speed * direction;
	Speed += Random_Double(-0.5, 0.5) * 0.1;
	PrecisePosition += velocity;
	Coord pos(PrecisePosition.X, PrecisePosition.Y, PrecisePosition.Z);
	PositionCoord = pos;
	ColorAccum += Class->ColorSpeed + Random_Double(0.0, 0.05);
	if (ColorAccum > 1.0) {
		if (ColorIndex < Class->ColorList.Count() - 2) {
			ColorIndex++;
			ColorAccum = 0;
		} else {
			ColorAccum = 1.0;
		}
	}
}

// FUNCTION: 0x0062C540
void ParticleClass::Smoke_Behavior_AI(void)
{
	if (Unsorted::CurrentFrame & 1) {
		if ((abs(Scen->RandomNumber()) & 3) == 0) {
			int x = 0;
			int y = 0;
			if ((abs(Scen->RandomNumber()) & 1) == 0) {
				x = (int)abs(Scen->RandomNumber()) % 3 - 1;
			} else {
				y = (int)abs(Scen->RandomNumber()) % 3 - 1;
			}
			GasDrift += Point2D(x,y);
			if (GasDrift.X < -5) {
				GasDrift.X = -5;
			} else if (GasDrift.X > 5) {
				GasDrift.X = 5;
			}
			if (GasDrift.Y < -5) {
				GasDrift.Y = -5;
			} else if (GasDrift.Y > 5) {
				GasDrift.Y = 5;
			}
		}
		GasDrift.Z = 0;
	}

	if (StateAI < Class->EndStateAI) {
		if (((Class->MaxEC - RemainingEC + Fetch_ID()) % (Class->StateAIAdvance + (Fetch_ID() & 1))) == 0) {
			StateAI++;
		}
		if (StateAI == Class->EndStateAI && Class->DeleteOnStateLimit) {
			IsToDie = true;
		}
	}
	if (Speed > 3.0) {
		Speed = Speed - Class->Deacc;
	}
}

// FUNCTION: 0x0062CB10
void ParticleClass::Fire_Behavior_AI(void)
{
	if (Speed > 0.0) {
		double rndval = (double)(((Scen->RandomNumber() % 10) - 5) / 50.0) + 1.0;

		Vector3 motion = rndval * MovementDirection;
		FireMoveDelta = Coord(motion.X * Speed, motion.Y * Speed, motion.Z * Speed);
		if (StateAI < Class->EndStateAI) {
			if (((Class->MaxEC - RemainingEC + Fetch_ID()) % (StateAIAdvance + (Fetch_ID() & 1))) == 0) {
				StateAI++;
				if (StateAI == Class->Translucent25State) {
					Translucency = 25;
				}
				if (StateAI == Class->Translucent50State) {
					Translucency = 50;
				}
			}
			if (StateAI == Class->EndStateAI) {
				if (Class->DeleteOnStateLimit) {
					IsToDie = true;
					return;
				}
			}
		}
		Speed -= Class->Deacc;
		RemainingDC--;
		if (RemainingDC == 0 && Class->Damage && StateAI <= (char)Class->FinalDamageState) {
			RemainingDC = Class->MaxDC;
			CellClass *cellptr = &Map[(Coord const &)PositionCoord];
			bool onbridge = cellptr->IsUnderBridge && PositionCoord.Z >= LEVEL_LEPTON_H * (cellptr->Height + BRIDGE_CELL_HEIGHT);
			ObjectClass *optr = cellptr->Cell_Occupier(onbridge);
			while (optr != NULL) {
				int dist = Distance(optr->PositionCoord);
				const ParticleTypeClass *ptype = Class;
				int damage = ptype->Damage;
				if ( optr->Strength > 0 && optr->IsActive && optr != System->Source_Object() )
				{
					optr->Take_Damage(damage, dist / 10, ptype->Warhead, 0, 0, 0);
				}
				optr = optr->Next;
			}
		}
	} else {
		Speed = 0.0;
		IsToDie = true;
	}
}

// FUNCTION: 0x0062CE40
void ParticleClass::Behavior_AI(void)
{
	switch (Class->BehavesLike) {
		case BEHAVIOR_GAS:
			Gas_Behavior_AI();
			break;
		case BEHAVIOR_SMOKE:
			Smoke_Behavior_AI();
			break;
		case BEHAVIOR_FIRE:
			Fire_Behavior_AI();
			break;
		case BEHAVIOR_SPARK:
			Spark_Behavior_AI();
			break;
		case BEHAVIOR_RAILGUN:
			Railgun_Behavior_AI();
			break;
		default:
			break;
	}

	RemainingEC--;
	if (RemainingEC == 0) {
		IsToDie = true;
	}
}

// FUNCTION: 0x0062CEC0
void ParticleClass::Draw_It(Point2D const & point, Rect const & cliprect) const
{
	if (FPSCounter::CurrentFrameRate < Detail::GetMinFrameRate() && Class->Damage == 0) {
		return;
	}
	if (Options.DetailLevel != 0 || Class->BehavesLike != BEHAVIOR_SMOKE && Class->BehavesLike != BEHAVIOR_SPARK) {
		if (Unsorted::ArmageddonMode || MainWindow == 0 || !Scen->Special.IsFogOfWar || !Map.Is_Fogged((Coord const &)PositionCoord)) {

			if (Class->BehavesLike != BEHAVIOR_SPARK && Class->BehavesLike != BEHAVIOR_RAILGUN) {
				int height_offset = -15 - TacticalMap->Z_Lepton_To_Pixel(Height);
				ShapeSet const * shape = (ShapeSet const *)Get_Image_Data();
				if (shape != NULL) {
					ShapeFlags_Type flags = ShapeFlags_Type(SHAPE_ALPHA|SHAPE_ZGRAD);
					int shapenum = Shape_Number();
					if (Options.DetailLevel == 2) {
						if (Translucency == 25) {
							flags = ShapeFlags_Type(SHAPE_TRANSLUCENT25|SHAPE_ALPHA|SHAPE_ZGRAD);
						} else if (Translucency == 50) {
							flags = ShapeFlags_Type(SHAPE_TRANSLUCENT50|SHAPE_ALPHA|SHAPE_ZGRAD);
						} else if (Translucency >= 75) {
							flags = ShapeFlags_Type(SHAPE_TRANSLUCENT75|SHAPE_ALPHA|SHAPE_ZGRAD);
						}
					}
					Draw_Shape(*LogicalSurface, *AnimDrawer, shape, shapenum, point, cliprect, (ShapeFlags_Type)(flags|SHAPE_CENTER|SHAPE_WIN_REL|SHAPE_ALPHA), 0, height_offset, ZGRAD_90DEG);
				}
			} else {
				Point2D pixel;
				TacticalMap->Coord_To_Pixel(PositionCoord, pixel);
				pixel.Y += TacticalRect.Y;
				if (cliprect.Is_Point_Within(pixel)) {
					Point2D alpha_point = pixel - Point2D(0, AlphaBuffer->Get_Bounds().Y);
					unsigned short * aptr = (unsigned short*)AlphaBuffer->Get_Buffer_Offset(alpha_point);
					int alpha = *aptr;
					if (alpha != 0) {
						Point2D depth_point = pixel - Point2D(0, DepthBuffer->Get_Bounds().Y);
						int zdepth = (unsigned short)(DepthBuffer->Get_Bounds().Y + DepthBuffer->Get_Scroll_Delta(pixel.Y)) - TacticalMap->Z_Lepton_To_Pixel(PositionCoord.Z) - 50;
						int depth = *(unsigned short*)DepthBuffer->Get_Buffer_Offset(depth_point);
						if (zdepth < depth) {
							RGBClass color1 = ColorIndex == 0 ? Color : Class->ColorList[ColorIndex];
							RGBClass color2 = Class->ColorList[ColorIndex + 1];
							int red = color1.Get_Red() * (1.0 - ColorAccum) + color2.Get_Red() * ColorAccum;
							int green = color1.Get_Green() * (1.0 - ColorAccum) + color2.Get_Green() * ColorAccum;
							int blue = color1.Get_Blue() * (1.0 - ColorAccum) + color2.Get_Blue() * ColorAccum;
							if (alpha < 127) {
								red = (int)(red * alpha) >> 7;
								green = (int)(green * alpha) >> 7;
								blue = (int)(blue * alpha) >> 7;
							}
							LogicalSurface->Put_Pixel(pixel, (unsigned short)DSurface::Build_Hicolor_Pixel(red, green, blue));
						}
					}
				}

			}

		}
	}
}

// FUNCTION: 0x0062D2A0
void ParticleClass::Smoke_Motion_AI(void)
{
	Coord coord = Get_Coord();

	if (Class->WindEffect > 0 && (Unsorted::CurrentFrame % (10 / Class->WindEffect)) == 0) {
		coord += Coord(SmokeWindX[Rule->WindDirection], SmokeWindY[Rule->WindDirection]);
	}

	if ((unsigned)Unsorted::CurrentFrame % 2 != 0) {

		coord.X += GasVelocity.X;
		coord.Y += GasVelocity.Y;

		int height = Map.Get_Height_GL(coord);
		if (coord.Z > height + 5) {
			int descend = MIN(coord.Z - (height + 5), 2);
			coord.Z -= descend;
		}

		coord += GasDrift;

		if (coord.Z < Map.Get_Height_GL(coord) + 5) {
			coord.Z = Map.Get_Height_GL(coord) + 5;
		}
	}

	Set_Coord(coord);
}

// FUNCTION: 0x0062D3F0
void ParticleClass::Gas_Motion_AI(void)
{
	static const double _level_scale = 2.5;

	Coord coord = PositionCoord;
	Coord previous_coord = coord;

	int wind_effect = Class->WindEffect;

	Point2D wind;
	wind.X = GasWindX[Rule->WindDirection] * wind_effect;
	wind.Y = GasWindY[Rule->WindDirection] * wind_effect;

	coord.X += wind.X;
	coord.Y += wind.Y;
	coord.Z += (int)Speed;
	coord += GasDrift;

	if (Map[previous_coord].IsUnderBridge) {
		int height = Map.Get_Height_GL(previous_coord) + BRIDGE_LEPTON_HEIGHT;
		if (previous_coord.Z < height && coord.Z >= height - LEVEL_LEPTON_H * _level_scale) {
			IsToDie = true;
			return;
		}
	}

	Set_Coord(coord);
}

// FUNCTION: 0x0062D510
void ParticleClass::Fire_Motion_AI(void)
{
	Coord coord = PositionCoord;
	Coord previous_coord = coord;
	if (Speed > 0.0) {
		coord += FireMoveDelta;
	}

	if (Speed > 0.0) {
		int height = Map.Get_Height_GL(previous_coord);
		if (Map.Get_Height_GL(coord) > height) {
			IsFireBelowGround = true;
			IsToDie = true;
		}
	}

	Set_Coord(coord);
}

// FUNCTION: 0x0062D5E0
void ParticleClass::Motion_AI(void)
{
	switch (Class->BehavesLike)
	{
		case BEHAVIOR_GAS:
			Smoke_Motion_AI();
			break;
		case BEHAVIOR_SMOKE:
			Gas_Motion_AI();
			break;
		case BEHAVIOR_FIRE:
			Fire_Motion_AI();
			break;
		default:
			break;
	}
}

// FUNCTION: 0x0062D6F0
bool ParticleClass::Mark(MarkType mark)
{
	if (BASECLASS::Mark(mark)) {
		return(true);
	}
	return(false);
}

// FUNCTION: 0x0062D710
Cell const * ParticleClass::Occupy_List(bool placement) const
{
	static Cell const _list[] = {REFRESH_EOL};
	return(_list);
}

// FUNCTION: 0x0062D760 $E155

// FUNCTION: 0x0062D770
LayerType ParticleClass::In_Which_Layer(void) const
{
	return(LAYER_AIR);
}

// FUNCTION: 0x0062D780
int Approximate_Distance(Point2D & pt)
{
	return((int)(abs(pt.X) + abs(pt.Y)) >> 1);
}

// FUNCTION: 0x0062D7A0
HRESULT STDMETHODCALLTYPE ParticleClass::Load(IStream * stream)
{
	HRESULT result = BASECLASS::Load(stream);
	if (SUCCEEDED(result)) {
		new (this) ParticleClass(NoInitClass());

		Swizzle_Pointer(&Class);
		Swizzle_Pointer(&System);

		result = S_OK;
	}
	return(result);
}

// FUNCTION: 0x0062D810
HRESULT STDMETHODCALLTYPE ParticleClass::Save(IStream * stream, BOOL cleardirty)
{
	HRESULT result = BASECLASS::Save(stream, cleardirty);
	WasSaved = true;
	return(result);
}

// FUNCTION: 0x0062D830
int ParticleClass::Shape_Number(void) const
{
	Point2D fire_start;
	Point2D fire_end;
	FacingType face;

	int behavior = Class->BehavesLike;
	if (behavior >= BEHAVIOR_GAS) {
		if (behavior <= BEHAVIOR_SMOKE) {
			goto state;
		}
		if (behavior == BEHAVIOR_FIRE) {
			TacticalMap->Coord_To_Pixel(FireOrigin, fire_start);
			TacticalMap->Coord_To_Pixel(FireTarget, fire_end);
			face = Facing_Between_Points(fire_start, fire_end);
			if (face == FACING_E || face == FACING_W) {
				return(StateAI + 2 * Class->EndStateAI);
			}
			if (face == FACING_S || face == FACING_N) {
				goto state;
			}
			if (face == FACING_NE || face == FACING_SW) {
				return(StateAI + Class->EndStateAI);
			}
			if (face == FACING_SE || face == FACING_NW) {
				return(StateAI + 3 * Class->EndStateAI);
			}
		}
	}

	return(0);
state:
	return(StateAI);
}

// FUNCTION: 0x0062D930
HRESULT STDMETHODCALLTYPE ParticleClass::GetClassID(CLSID * retval)
{
	if (retval == NULL) return(E_POINTER);
	*retval = CLSID_ParticleClass;
	return(S_OK);
}

// FUNCTION: 0x0062D970
int ParticleClass::Fetch_Object_Size(void) const
{
	return(sizeof(*this));
}

// FUNCTION: 0x0062D980
RTTIType ParticleClass::Fetch_RTTI(void) const
{
	return(RTTI_PARTICLE);
}

// FUNCTION: 0x0062D990
ObjectTypeClass const * ParticleClass::Class_Of(void) const
{
	return(Class);
}

// FUNCTION: 0x0062D9A0 ParticleClass::~ParticleClass

// FUNCTION: 0x0062DA40 $E2
// FUNCTION: 0x0062DA70 $E5

// FUNCTION: 0x0062DA90 $E8
// FUNCTION: 0x0062DAB0 $E11
// FUNCTION: 0x0062DAD0 $E14
// FUNCTION: 0x0062DAF0 $E17
// FUNCTION: 0x0062DB10 $E20
// FUNCTION: 0x0062DB50 $E23
// FUNCTION: 0x0062DB80 $E26
// FUNCTION: 0x0062DBB0 $E29
// FUNCTION: 0x0062DBE0 $E32
// FUNCTION: 0x0062DBF0 $E35
// FUNCTION: 0x0062DC10 $E38
// FUNCTION: 0x0062DC30 $E47
