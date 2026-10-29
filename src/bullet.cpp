// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#define INCLUDE_COM
#include "always.h"
#include "geometry.h"
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
#include "radsite.h"
#include "rules.h"
#include "scheme.h"
#include "stimer.h"
#include "sun.h"
#include "swizzle.h"
#include "tactical.h"
#include "techno.h"
#include "tracker.h"
#include "trajhelp.h"
#include "unit.h"
#include "voxdrsys.h"
#include "warhead.h"
#include "weapon.h"

// SYNTHETIC: GAMEMD 0x004660D0 SYMBOL
// _$E2
// SYNTHETIC: GAMEMD 0x00466100 SYMBOL
// _$E5
// SYNTHETIC: GAMEMD 0x00466120 SYMBOL
// _$E8
// SYNTHETIC: GAMEMD 0x00466140 SYMBOL
// _$E11
// SYNTHETIC: GAMEMD 0x00466160 SYMBOL
// _$E14
// SYNTHETIC: GAMEMD 0x00466180 SYMBOL
// _$E17
// SYNTHETIC: GAMEMD 0x004661A0 SYMBOL
// _$E20
// SYNTHETIC: GAMEMD 0x004661E0 SYMBOL
// _$E23
// SYNTHETIC: GAMEMD 0x00466210 SYMBOL
// _$E26
// SYNTHETIC: GAMEMD 0x00466240 SYMBOL
// _$E29
// SYNTHETIC: GAMEMD 0x00466270 SYMBOL
// _$E32
// SYNTHETIC: GAMEMD 0x00466280 SYMBOL
// _$E35
// SYNTHETIC: GAMEMD 0x004662A0 SYMBOL
// _$E38
// SYNTHETIC: GAMEMD 0x004662C0 SYMBOL
// _$E47

DynamicVectorClass<BulletClass *> BulletClass::ScalableBullets;

extern Cell DAT_0089DDF0;
extern int DAT_0089DE70;
extern int DAT_0081AF00;
extern ULONG COMRefCount;


// SYNTHETIC: GAMEMD 0x004662E0 SYMBOL
// _$E166
// SYNTHETIC: GAMEMD 0x00466320 SYMBOL
// _$E163

// FUNCTION: GAMEMD 0x00466380
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

// FUNCTION: GAMEMD 0x004664C0
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

// FUNCTION: GAMEMD 0x00466560
// BulletClass::~BulletClass
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

// FUNCTION: GAMEMD 0x00466660
Cell const * BulletClass::Occupy_List(bool) const
{
	static Cell const _list[] = { REFRESH_EOL };

	return(_list);
}

// SYNTHETIC: GAMEMD 0x004666B0 SYMBOL
// _$E41

// FUNCTION: GAMEMD 0x004666C0
bool BulletClass::Mark(MarkType mark)
{
	if (BASECLASS::Mark(mark)) {
		return(true);
	}
	return(false);
}

// FUNCTION: GAMEMD 0x00468000
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

// FUNCTION: GAMEMD 0x00468090
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

// FUNCTION: GAMEMD 0x00468430
void BulletClass::LoseTarget(void)
{
	Cell cell = TarCom->Center_Coord().As_Cell();
	if (!ScenarioInit && !TarCom->In_Air() && cell != DAT_0089DDF0) {
		TarCom = &Map[cell];
	} else {
		TarCom = NULL;
	}
}

// FUNCTION: GAMEMD 0x004684E0
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

// FUNCTION: GAMEMD 0x00468B90
LayerType BulletClass::In_Which_Layer(void) const
{
	return(Class->Flat ? LAYER_SURFACE : LAYER_AIR);
}

// FUNCTION: GAMEMD 0x00468BB0
bool BulletClass::Is_Forced_To_Explode(Coord & coord) const
{
	coord = PositionCoord;
	CellClass * cell = &Map[coord];

	if (Class->SubjectToCliffs || Class->SubjectToWalls) {
		CellClass * source = &Map[SourceCoords];
		CellClass * target = &Map[TargetCoords];
		CellClass * current = &Map[LastMapCoords];
		HouseClass * owner = (HouseClass *)Payback;
		if (Payback != NULL) {
			owner = Payback->House;
		}
		if (TrajectoryHelper::GetObstacle(source, target, current, coord, Class, owner)) {
			return(true);
		}
	}

	if (HeightAGL <= -DAT_0089DE70 * 4) {
		return(true);
	}

	if (TarCom != NULL && Class->FlakScatter) {
		Coord pos = PositionCoord;
		if (pos.Z < TarCom->As_Coord().Z && HeightAGL < 0) {
			return(true);
		}
	}

	if (Class->Level && cell->On_Ground()) {
		return(true);
	}

	if (Class->IsAntiAircraft && TarCom != NULL && TarCom->In_Air() && Distance(TarCom) < 128) {
		return(true);
	}

	return(false);
}

static inline int Coord_Dist(Coord const & a, Coord const & b)
{
	Coord c = a - b;
	return(c.Length());
}

// FUNCTION: GAMEMD 0x00468D80
void BulletClass::Bullet_Explodes(bool forced)
{
	Coord coord = PositionCoord;
	TechnoClass * target = NULL;
	if (TarCom != NULL && TarCom->In_Air()) {
		target = (TechnoClass *)TarCom;
	}

	if (!Class->IsInaccurate) {
		if (TarCom != NULL) {
			if (Coord_Dist(coord, TarCom->Center_Coord()) < 32 && !Class->IsAirburst && !Class->IsInaccurate) {
				coord = TarCom->Center_Coord();
			}
		}

		if (!Warhead->IsEMEffect && !Class->IsAirburst) {
			if (!forced && !Class->IsArcing && !Is_Homing() && Fuse.Fuse_Target() != COORD_NONE) {
				coord = Fuse.Fuse_Target();
			}

			if (target != NULL && target->In_Which_Layer() != LAYER_GROUND) {
				if (Distance(TarCom) < 128) {
					coord = target->Target_Coord();
				}
			} else if (TarCom != NULL && Distance(TarCom) < 42) {
				coord = TarCom->As_Coord();
				if (TarCom->Fetch_RTTI() == RTTI_BUILDING) {
					Point3D const & offset = ((BuildingClass *)TarCom)->Class->TargetCoordOffset;
					if (offset.X != 0 || offset.Y != 0 || offset.Z != 0) {
						coord = ((ObjectClass *)TarCom)->Target_Coord();
					}
				}
			}
		}
	}

	if (!Class->IsAirburst) {
		Coord newcoord = coord;
		for (int i = 0; i < Class->Cluster; i++) {
			Detonate(coord);
			if (!IsActive) {
				break;
			}
			coord = Coord_Scatter(newcoord, Random_Pick(CELL_LEPTON, CELL_LEPTON * 2));
		}
	} else {
		Detonate(coord);
	}
}

// FUNCTION: GAMEMD 0x0046ADE0
void BulletClass::ApplyRadiationToCell(Cell cell, int radius, int amount)
{
	CellClass * cptr = &Map[cell];
	RadSiteClass * site = cptr->FUN_00487C80();
	if (site == NULL) {
		site = new RadSiteClass();
		site->SetBaseCell(&cell);
		site->SetSpread(radius);
		site->SetRadLevel(amount);
		site->Activate();
		cptr->FUN_00487C70(site);
	} else {
		site->Add(amount);
	}
}

// FUNCTION: GAMEMD 0x0046AE70
HRESULT STDMETHODCALLTYPE BulletClass::Load(IStream * stream)
{
	HRESULT result = BASECLASS::Load(stream);
	if (SUCCEEDED(result)) {
		new (this) BulletClass(NoInitClass());
		if (NextAnim != NULL) {
			PointerExpiredNotification::NotifyInvalidAnim.Add(this);
		}

		Swizzle_Pointer(&Class);
		Swizzle_Pointer(&Payback);
		Swizzle_Pointer(&Warhead);
		Swizzle_Pointer(&TarCom);
		Swizzle_Pointer(&WeaponType);
		Swizzle_Pointer(&NextAnim);
	}
	return(result);
}

// FUNCTION: GAMEMD 0x0046AFB0
HRESULT STDMETHODCALLTYPE BulletClass::Save(IStream * stream, BOOL cleardirty)
{
	return(BASECLASS::Save(stream, cleardirty));
}

// FUNCTION: GAMEMD 0x0046AFD0
ULONG STDMETHODCALLTYPE BulletClass::AddRef(void)
{
	COMRefCount++;
	return(InterlockedIncrement(&RefCount));
}

// FUNCTION: GAMEMD 0x0046AFF0
ULONG STDMETHODCALLTYPE BulletClass::Release(void)
{
	COMRefCount--;
	ULONG count = InterlockedDecrement(&RefCount);
	if (count == 0) {
		delete this;
	}
	return(count);
}

// FUNCTION: GAMEMD 0x0046B030
bool BulletClass::Is_Homing(void) const
{
	if (Class->ROT > 0) {
		return(true);
	}
	return(false);
}

// FUNCTION: GAMEMD 0x0046B050
BulletClass * __fastcall Create_Bullet(BulletTypeClass const * type, AbstractClass * target, TechnoClass * payback, int strength, WarheadTypeClass const * warhead, int max_speed, bool bright)
{
	LPVOID unk = NULL;
	if (FAILED(CoCreateInstance(CLSID_BulletClass, NULL, CLSCTX_INPROC_SERVER | CLSCTX_INPROC_HANDLER | CLSCTX_LOCAL_SERVER, IID_IUnknown, &unk))) {
		return(NULL);
	}

	BulletClass * bullet = (BulletClass *)unk;
	bullet->Construct(type, target, payback, strength, warhead, max_speed, bright);
	return(bullet);
}

// FUNCTION: GAMEMD 0x0046B0C0
void BulletClass::Draw_Voxel(VoxelDataStruct const & voxeldata, Matrix3D const & transform, Point2D const & drawpoint, Rect const & cliprect, int frame, ShapeFlags_Type flags, int brightness) const
{
	VoxelLibrary * voxlib = voxeldata.VoxLib;
	MotionLibrary * motlib = voxeldata.MotLib;

	VoxelDrawSystem::Precalculate_Light(voxlib, 0, 0, transform, VoxelLightSource);
	VoxelDrawSystem::Reset();

	for (unsigned int i = 0; i < voxlib->Get_Layer_Count(); i++) {
		Matrix3D matrix;
		if (motlib != NULL) {
			Matrix3D hva_transform = motlib->Get_Layer_Matrix(i, frame);
			matrix = transform * hva_transform;
		}
		VoxelDrawSystem::Prep_For_Object(voxlib, i, 0, VoxelCameraMatrix * matrix);
	}

	SurfaceRegion region = VoxelDrawSystem::Render();
	int extra = 0;
	ShapeFlags_Type f = ShapeFlags_Type(flags & ~SHAPE_REMAP);
	if (Class->Flat) {
		extra = DAT_0081AF00;
		f = ShapeFlags_Type(f | 0x2000);
	}

	Blit_Block(*LogicalSurface, *ColorSchemes[Class->Color]->Converter, *VoxelDrawSystem::Get_Surface(), region.Bounds, region.Point + drawpoint, cliprect, NULL, ColorSchemes[Class->Color]->Converter->Blitter_From_Flags(f), extra, Class->Flat ? ZGRAD_GROUND : ZGRAD_90DEG, brightness, 0, 0);
}

// FUNCTION: GAMEMD 0x0046B260
void BulletClass::Set_Weapon_Type(WeaponTypeClass * weapon)
{
	WeaponType = weapon;
}

// FUNCTION: GAMEMD 0x0046B270
WeaponTypeClass * BulletClass::Get_Weapon_Type(void) const
{
	return(WeaponType);
}

// FUNCTION: GAMEMD 0x0046B280
void BulletClass::InitScalable(void)
{
	ScalableBullets.Add(this);
	int extra = ScalableBullets.Count() - 5;
	if (extra < 0) {
		extra = 0;
	}
	Class->SetScaledSpawnDelay(Class->SpawnDelay + extra / 3);
}

// FUNCTION: GAMEMD 0x0046B310
void BulletClass::NukeMaker(void)
{
	AbstractClass * target = TarCom;
	CellClass * cptr = &Map[target->Center_Coord().As_Cell()];
	WeaponTypeClass * weapon = Weapons[WeaponTypeClass::From_Name("NukePayload")];

	BulletTypeClass const * type = weapon->Bullet;
	WarheadTypeClass const * warhead = weapon->WarheadPtr;
	int speed = weapon->MaxSpeed;
	BulletClass * bullet = Create_Bullet(type, target, Payback, weapon->Attack, warhead, speed, true);
	if (bullet != NULL) {
		bullet->Limbo();
		bullet->WeaponType = weapon;
		Coord coord = cptr->Center_Coord();
		coord = coord + Coord(0, 0, WeaponType->Bullet->DetonationAltitude);
		TVelocity3D<double> velocity(DIR_N, DIR_N, -1.0);
		bullet->Unlimbo(coord, velocity);
	}
}

// FUNCTION: GAMEMD 0x0046B560
HRESULT STDMETHODCALLTYPE BulletClass::GetClassID(CLSID * retval)
{
	if (retval == NULL) return(E_POINTER);
	*retval = CLSID_BulletClass;
	return(S_OK);
}

// FUNCTION: GAMEMD 0x0046B540
int BulletClass::Fetch_Object_Size(void) const
{
	return(sizeof(*this));
}

// FUNCTION: GAMEMD 0x0046B550
RTTIType BulletClass::Fetch_RTTI(void) const
{
	return(RTTI_BULLET);
}

// FUNCTION: GAMEMD 0x0046B5A0
void BulletClass::Assign_Target(AbstractClass * target)
{
	TarCom = target;
}

// FUNCTION: GAMEMD 0x0046B5B0
ObjectTypeClass const * BulletClass::Class_Of(void) const
{
	return(Class);
}

// SYNTHETIC: GAMEMD 0x0046B5C0
// ??_GBulletClass@@UAEPAXI@Z

// TEMPLATE: GAMEMD 0x0046B5E0
// ?Clear@?$DynamicVectorClass@PAVBulletClass@@@@UAEXXZ
// TEMPLATE: GAMEMD 0x0046B610
// ?ID@?$DynamicVectorClass@PAVBulletClass@@@@UAEHPBQAVBulletClass@@@Z
// TEMPLATE: GAMEMD 0x0046B630
// ?GetItem@?$VectorClass@PAVBulletClass@@@@UBEPAVBulletClass@@H@Z
template class DynamicVectorClass<BulletClass *>;

// TEMPLATE: GAMEMD 0x0046B640
// ??0?$CDTimerClass@VFrameTimerClass@@@@QAE@H@Z
template class CDTimerClass<FrameTimerClass>;

// TEMPLATE: GAMEMD 0x0046B660
// ??8?$VectorClass@PAVBulletClass@@@@UBE_NABV0@@Z
// TEMPLATE: GAMEMD 0x0046B6A0
// ?Resize@?$VectorClass@PAVBulletClass@@@@UAE_NHPBQAVBulletClass@@@Z
// TEMPLATE: GAMEMD 0x0046B750
// ?Clear@?$VectorClass@PAVBulletClass@@@@UAEXXZ
template class VectorClass<BulletClass *>;

// TEMPLATE: GAMEMD 0x0046B780
// ?ID@?$VectorClass@PAVBulletClass@@@@UAEHABQAVBulletClass@@@Z
// TEMPLATE: GAMEMD 0x0046B7C0
// ?ID@?$VectorClass@PAVBulletClass@@@@UAEHPBQAVBulletClass@@@Z
// TEMPLATE: GAMEMD 0x0046B7E0
// ?Resize@?$DynamicVectorClass@PAVBulletClass@@@@UAE_NHPBQAVBulletClass@@@Z
// TEMPLATE: GAMEMD 0x0046B890
// ?ID@?$DynamicVectorClass@PAVBulletClass@@@@UAEHABQAVBulletClass@@@Z
// SYNTHETIC: GAMEMD 0x0046B8C0
// ??_G?$VectorClass@PAVBulletClass@@@@UAEPAXI@Z
// SYNTHETIC: GAMEMD 0x0046B910
// ??_G?$DynamicVectorClass@PAVBulletClass@@@@UAEPAXI@Z

// TEMPLATE: GAMEMD 0x0046B960
// ??Y?$TVelocity3D@N@@QAEAAV0@ABV0@@Z
template class TVelocity3D<double>;

// FUNCTION: GAMEMD 0x0046B990
// BulletClass::AddRef

// FUNCTION: GAMEMD 0x0046B9A0
// BulletClass::Release
