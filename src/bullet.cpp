// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#define INCLUDE_COM
#include "always.h"
#include "bullet.h"
#include "_convert.h"
#include "_map.h"
#include "_rules.h"
#include "_surface.h"
#include "_tactica.h"
#include "aircraft.h"
#include "anim.h"
#include "building.h"
#include "bullettype.h"
#include "ccrand.h"
#include "cell.h"
#include "combat.h"
#include "convert.h"
#include "draw.h"
#include "empulse.h"
#include "fly.h"
#include "house.h"
#include "houstype.h"
#include "infantry.h"
#include "inline.h"
#include "lightcon.h"
#include "map.h"
#include "missile.h"
#include "notify.h"
#include "overtype.h"
#include "partsys.h"
#include "rules.h"
#include "scheme.h"
#include "stimer.h"
#include "sun.h"
#include "swizzle.h"
#include "tactical.h"
#include "techno.h"
#include "tracker.h"
#include "unit.h"
#include "voxdrsys.h"
#include "warhead.h"
#include "weapon.h"

DynamicVectorClass<BulletClass *> BulletClass::ScalableBullets;

extern Cell DAT_0089DDF0;

// FUNCTION: 0x004662E0 $E107
// FUNCTION: 0x00466320 $E104

// FUNCTION: 0x00466380
BulletClass::BulletClass(void) :
	BASECLASS(),
	Class(NULL),
	Payback(NULL),
	IsInaccurate(false),
	Fuse(),
	field_A4(true),
	IsLaunching(true),
	IsBright(false),
	BounceCount(0),
	CourseLockCounter(0),
	TarCom(NULL),
	MaxSpeed(0),
	InheritedColor(-1),
	ClosureSamples(0),
	SmoothedClosure(0),
	Warhead(NULL),
	AnimFrame(0),
	AnimRate(0),
	WeaponType(NULL),
	LastMapCoords(-1, -1),
	NextAnim(NULL),
	SpawnNextAnim(false)
{
	Create_ID();
	SourceCoords.X = 0;
	SourceCoords.Y = 0;
	SourceCoords.Z = 0;
	TargetCoords.X = 0;
	TargetCoords.Y = 0;
	TargetCoords.Z = 0;
	Array.Add(this);
}

// FUNCTION: 0x004664C0
void BulletClass::Construct(BulletTypeClass const *type, AbstractClass *target, TechnoClass *payback, int strength, WarheadTypeClass const *warhead, int max_speed, bool bright)
{
	Class = (BulletTypeClass *)type;
	Payback = payback;
	TarCom = target;
	MaxSpeed = max_speed;
	Warhead = (WarheadTypeClass *)warhead;
	IsBright = bright;
	Strength = strength;
	AnimFrame = 0;
	AnimRate = Class->AnimRate;
	if (Class->FirersPalette && Payback != NULL) {
		InheritedColor = Payback->House->Scheme;
	} else {
		InheritedColor = -1;
	}
	DamageMultiplier = 256;
	NextAnim = NULL;
	SpawnNextAnim = false;
}

// FUNCTION: 0x00466560 BulletClass::~BulletClass
BulletClass::~BulletClass(void)
{
	Detach_This_From_All(this, true);

	if (SpawnNextAnim) {
		PointerExpiredNotification::NotifyInvalidAnim.Delete(this);
	}

	if (GameActive) {
		BulletClass::Limbo();
	}

	Class = NULL;
	Payback = NULL;
	NextAnim = NULL;
	Array.Delete(this);
}

// FUNCTION: 0x00466660
Cell const * BulletClass::Occupy_List(bool) const
{
	static Cell const _list[] = { REFRESH_EOL };

	return(_list);
}

// FUNCTION: 0x004666B0 $E110

// FUNCTION: 0x004666C0
bool BulletClass::Mark(MarkType mark)
{
	if (BASECLASS::Mark(mark)) {
		return(true);
	}
	return(false);
}

// FUNCTION: 0x00468000
int BulletClass::Shape_Number(void) const
{
	int shapenum = 0;

	if (!Class->NoRotate) {
		shapenum = TechnoClass::BodyShape[DirType(fastmath::atan2(-Velocity.Y, Velocity.X)).As_Dir32()];
	}

	if (Class->AnimLow || Class->AnimHigh) {
		shapenum = AnimFrame;
	}

	return(shapenum);
}

// FUNCTION: 0x00468090
void BulletClass::Draw_It(Point2D const & point, Rect const & cliprect) const
{
	if (Scen->Special.IsFogOfWar && Map.Is_Fogged(Get_Coord())) return;

	if (Class->IsInvisible) return;

	if (SpawnNextAnim) return;

	if (Class->IsVoxel) {
		Matrix3D matrix(true);
		matrix.Rotate_Z(Velocity.Get_Yaw().As_Radian32());
		matrix.Rotate_Y(-Velocity.Get_Pitch().As_Radian32());
		Draw_Voxel(Class->Voxel, Get_Isometric_View_Matrix() * matrix, point, cliprect, 0, SHAPE_ALPHA, NORMAL_LIGHT);

	} else {
		ShapeSet const * shapeptr = (ShapeSet const *)Get_Image_Data();
		if (shapeptr == NULL) return;

		int shapenum = Shape_Number();

		int height_agl = HeightAGL;
		int height_gl = Map.Get_Height_GL(Get_Coord());

		if (!IsOnBridge && Map[Get_Coord()].IsUnderBridge && height_agl >= BRIDGE_LEPTON_HEIGHT) {
			height_agl -= BRIDGE_LEPTON_HEIGHT;
			height_gl += BRIDGE_LEPTON_HEIGHT;
		}

		if (height_agl > 0 && Class->IsShadow) {
			Point2D shadowpoint = point + Point2D(0, TacticalMap->Z_Lepton_To_Pixel(height_agl));
			Draw_Shape(*LogicalSurface, *NormalDrawer, shapeptr, shapenum, shadowpoint, cliprect, ShapeFlags_Type(SHAPE_DARKEN|SHAPE_CENTER|SHAPE_WIN_REL|SHAPE_ZGRAD), 0, -10 - TacticalMap->Z_Lepton_To_Pixel(height_gl));
		}

		ConvertClass * drawer = NormalDrawer;
		if (Class->IsAnimPalette) {
			drawer = AnimDrawer;
		} else if (Class->FirersPalette) {
			if (InheritedColor != -1) {
				drawer = ColorSchemes[InheritedColor]->Converter;
			} else {
				drawer = ColorSchemes[PlayerPtr->Scheme]->Converter;
			}
		}
		Draw_Shape(*LogicalSurface, *drawer, shapeptr, shapenum, point, cliprect, ShapeFlags_Type(SHAPE_CENTER|SHAPE_WIN_REL|SHAPE_ALPHA|SHAPE_ZGRAD), 0, -30 - TacticalMap->Z_Lepton_To_Pixel(Height));
	}
}

// FUNCTION: 0x00468430
void BulletClass::LoseTarget(void)
{
	Cell cell = TarCom->Center_Coord().As_Cell();
	if (!ScenarioInit && !TarCom->In_Air() && cell != DAT_0089DDF0) {
		TarCom = &Map[cell];
	} else {
		TarCom = NULL;
	}
}

// FUNCTION: 0x004684E0
void BulletClass::Detach(AbstractClass const * target, bool all)
{
	BASECLASS::Detach(target, all);

	if (Payback == target) {
		Payback = NULL;
	}
	if (TarCom == target) {
		LoseTarget();
	}
	if (Class == target) {
		Class = NULL;
	}
	if (WeaponType == target) {
		WeaponType = NULL;
	}
	if (NextAnim == target) {
		NextAnim = NULL;
	}

	if (all && target->Fetch_RTTI() == RTTI_BULLET) {
		BulletClass * bullet = (BulletClass *)target;
		if (bullet->Class->Scalable) {
			int tracked = ScalableBullets.Count() + ScalableBullets.Count() + ScalableBullets.Count();
			for (int i = 0; i < ScalableBullets.Count(); i++) {
				if (bullet == ScalableBullets[i]) {
					ScalableBullets.Delete(bullet);
					return;
				}
			}
		}
	}
}

// FUNCTION: 0x00468B90
LayerType BulletClass::In_Which_Layer(void) const
{
	return(Class->Flat ? LAYER_SURFACE : LAYER_AIR);
}

// FUNCTION: 0x0046AFB0
HRESULT STDMETHODCALLTYPE BulletClass::Save(IStream * stream, BOOL cleardirty)
{
	return(BASECLASS::Save(stream, cleardirty));
}

// FUNCTION: 0x0046B560
HRESULT STDMETHODCALLTYPE BulletClass::GetClassID(CLSID * retval)
{
	if (retval == NULL) return(E_POINTER);
	*retval = CLSID_BulletClass;
	return(S_OK);
}
