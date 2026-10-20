// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#define INCLUDE_COM
#include "always.h"
#include "terrain.h"
#include "_convert.h"
#include "_map.h"
#include "_rules.h"
#include "_surface.h"
#include "_tactica.h"
#include "anim.h"
#include "ccrand.h"
#include "cell.h"
#include "combat.h"
#include "draw.h"
#include "findmake.h"
#include "globals.h"
#include "incdec.h"
#include "inline.h"
#include "lightcon.h"
#include "rect.h"
#include "rules.h"
#include "scenario.h"
#include "shapeset.h"
#include "sun.h"
#include "swizzle.h"
#include "tactical.h"
#include "terrtype.h"
#include "tracker.h"
#include "warhead.h"
#include "draw.hh"
#include <cstdio>

void TerrainClass::Start_To_Crumble(void)
{
	if (!IsCrumbling) {
		IsCrumbling = true;
		Set_Rate(2);
		Set_Stage(0);
	}
}


// FUNCTION: 0x0071B7B0
TerrainClass::~TerrainClass(void)
{
	Detach_This_From_All(this);
	Terrains.Delete(this);

	if (GameActive && Class) {
		IsActive = true;
		TerrainClass::Limbo();
	}

	TargetTracker.Remove_Index(Fetch_ID());
}


// FUNCTION: 0x0071B920
ResultType TerrainClass::Take_Damage(int & damage, int distance, WarheadTypeClass const * warhead, TechnoClass * source, bool forced, bool no_crew, HouseClass * house)
{
	ResultType res = RESULT_NONE;

	if (!warhead) return(RESULT_NONE);

	if (warhead->IsWoodDestroyer && !Class->IsImmune) {

		res = BASECLASS::Take_Damage(damage, distance, warhead, source, forced, no_crew, house);

		if (res == RESULT_ALREADY_DESTROYED) {
			return(res);
		}

		if (res == RESULT_DESTROYED) {

			if (Class->IsTiberiumSpawn) {

				static int const _damage = 100;

				new AnimClass(Combat_Anim(_damage, Rule->C4Warhead, Map[Get_Coord()].Land_Type(), Get_Coord()), Get_Coord(), 0, 1, ShapeFlags_Type(SHAPE_CENTER|SHAPE_WIN_REL|SHAPE_ZGRAD), Get_Explosion_Z(Get_Coord()));
				Map.DamageArea(Get_Coord(), _damage, NULL, Rule->C4Warhead, true, NULL);
				Chain_Reaction_Damage(Get_Cell());
			} else if (IsOnFire) {
				Shorten_Attached_Anims(this);
			} else {
				Start_To_Crumble();
			}

			TacticalMap->Register_Dirty_Area(Get_Render_Rect(), false);

			Detach_All();
			Delete_Me();
		}
	}
	return(res);
}


// FUNCTION: 0x0071BB90
TerrainClass::TerrainClass(TerrainTypeClass const * type, Cell const & cell) :
	BASECLASS(),
	Class((TerrainTypeClass *)type),
	IsOnFire(false),
	IsCrumbling(false),
	Unused1(0),
	Unused2(0),
	RenderPixelPos(0,0)
{
	Create_ID();
	Strength = Class->MaxStrength;
	if (cell != CELL_NONE) {
		if (!Unlimbo(Coord(cell))) {
			Delete_Me();
		}
	}

	Set_Rate(0);
	Terrains.Add(this);
	TargetTracker.Add_Index(Fetch_ID(), this);
}


// FUNCTION: 0x0071BDF0
TerrainClass::TerrainClass(void) :
	BASECLASS(),
	Class(NULL),
	IsOnFire(false),
	IsCrumbling(false),
	Unused1(0),
	Unused2(0),
	RenderPixelPos(0,0)
{
	Terrains.Add(this);
	TargetTracker.Add_Index(Fetch_ID(), this);
}


// FUNCTION: 0x0071BFB0
bool TerrainClass::Mark(MarkType mark)
{
	if (BASECLASS::Mark(mark)) {
		Cell cell = Get_Cell();

		switch (mark) {
			case MARK_UP:
				Map.RemoveContentAt(&cell, (TechnoClass *)this);
				break;

			case MARK_DOWN:
			case MARK_DOWN_FORCED:
				Map.AddContentAt(&cell, (TechnoClass *)this);
				TacticalMap->Coord_To_Pixel(Render_Coord(), RenderPixelPos);
				RenderPixelPos += Point2D(TacticalMap->TacPixelX, TacticalMap->TacPixelY);
				break;

			default:
				break;
		}
		return(true);
	}
	return(false);
}


// FUNCTION: 0x0071C070
void TerrainClass::Clear_Occupy_Bit(Coord const & coord)
{
	int bits = Scen->Theater != THEATER_SNOW ? Class->TemperateOccupationBits : Class->SnowOccupationBits;

	CellClass &cell = Map[coord.As_Cell()];

	if (bits & 1) {
		cell.Flag.Composite &= ~(1 << 2);
	}

	if (bits & 2) {
		cell.Flag.Composite &= ~(1 << 3);
	}

	if (bits & 4) {
		cell.Flag.Composite &= ~(1 << 4);
	}
}


// FUNCTION: 0x0071C110
void TerrainClass::Set_Occupy_Bit(Coord const & coord)
{
	int bits = Scen->Theater != THEATER_SNOW ? Class->TemperateOccupationBits : Class->SnowOccupationBits;

	CellClass &cell = Map[coord.As_Cell()];

	if (bits & 1) {
		cell.Flag.Composite |= (1 << 2);
	}

	if (bits & 2) {
		cell.Flag.Composite |= (1 << 3);
	}

	if (bits & 4) {
		cell.Flag.Composite |= (1 << 4);
	}
}


// FUNCTION: 0x0071C1B0
void TerrainClass::Draw_It(Point2D const & point, Rect const & cliprect) const
{
	ShapeSet const * shapedata;

	Cell cell = Get_Cell();
	CellClass & cellptr = Map[cell];

	shapedata = (ShapeSet const *)Get_Image_Data();
	if (shapedata) {
		int	shapenum = 0;

		if (Class->IsAnimated) {
			shapenum = Fetch_Stage();
		} else if (IsCrumbling) {
			shapenum = Fetch_Stage() + IsCrumbling;
		} else {
			if (Strength < 2) {
				shapenum++;
			}
		}

		Point2D drawpoint = point;
		int zadjust = -TacticalMap->Z_Lepton_To_Pixel(Height);

		if (cellptr.Drawer == NULL) {
			cellptr.Init_Drawer();
		}


		int tint;
		ConvertClass * drawer;

		if (Class->IsTiberiumSpawn) {
			drawer = TiberiumDrawer;
			tint = cellptr.Brightness;
			drawpoint -= Point2D(0,16);
		} else {
			drawer = cellptr.Drawer;
			tint = cellptr.TileBrightness;
		}

		ShapeFlags_Type flags = ShapeFlags_Type(SHAPE_CENTER|SHAPE_WIN_REL|SHAPE_ALPHA);
		if (Is_Animating()) {
			flags = ShapeFlags_Type(flags | SHAPE_ZGRAD);
		} else {
			flags = ShapeFlags_Type(flags | SHAPE_ZWRITE);
		}
		Draw_Shape(*LogicalSurface, *drawer, shapedata, shapenum, drawpoint, cliprect, flags, NULL, zadjust - 12, ZGRAD_90DEG, tint);
		if (DrawShapeShadows) {
			Draw_Shape(*LogicalSurface, *cellptr.Drawer, shapedata, shapenum + shapedata->Get_Count() / 2, drawpoint, cliprect, ShapeFlags_Type(flags|SHAPE_DARKEN), NULL, zadjust - 3);
		}
	}
}


// FUNCTION: 0x0071C360
void TerrainClass::Editor_Draw_It(Point2D const & point, Rect const & cliprect) const
{
	ShapeSet const * shapedata;

	Cell cell = Get_Cell();
	CellClass & cellptr = Map[cell];

	shapedata = (ShapeSet const *)Get_Image_Data();
	if (shapedata) {
		int	shapenum = 0;

		if (IsCrumbling) {
			shapenum = Fetch_Stage() + IsCrumbling;
		}

		Point2D drawpoint = point;
		int zadjust = -TacticalMap->Z_Lepton_To_Pixel(Height);

		if (cellptr.Drawer == NULL) {
			cellptr.Init_Drawer();
		}


		int tint;
		ConvertClass * drawer;

		if (Class->IsTiberiumSpawn) {
			drawer = TiberiumDrawer;
			tint = cellptr.Brightness;
			drawpoint -= Point2D(0,16);
		} else {
			drawer = cellptr.Drawer;
			tint = cellptr.TileBrightness;
		}

		ShapeFlags_Type flags = ShapeFlags_Type(SHAPE_CENTER|SHAPE_WIN_REL|SHAPE_ALPHA);
		Draw_Shape(*LogicalSurface, *drawer, shapedata, shapenum, drawpoint, cliprect, flags, NULL, zadjust - 4, ZGRAD_90DEG, tint);
		if (DrawShapeShadows) {
			flags = ShapeFlags_Type(flags & ~SHAPE_ALPHA);
			Draw_Shape(*LogicalSurface, *cellptr.Drawer, shapedata, shapenum + shapedata->Get_Count() / 2, drawpoint, cliprect, ShapeFlags_Type(flags|SHAPE_DARKEN), NULL, zadjust - 2);
		}
	}
}


// FUNCTION: 0x0071C4D0
MoveType TerrainClass::Can_Enter_Cell(CellClass const * cell, FacingType, int cell_height, CellClass const *, bool) const
{
	Cell const * offset = Occupy_List();
	Cell location = cell->CellID;

	while (*offset != REFRESH_EOL) {
		if (Class->IsWaterBased) {
			if (!Map[location + *offset++].Is_Clear_To_Build(SPEED_FLOAT)) {
				return(MOVE_NO);
			}
		} else {
			if (!Map[location + *offset++].Is_Clear_To_Build()) {
				return(MOVE_NO);
			}
		}
	}
	return(MOVE_OK);
}


// FUNCTION: 0x0071C5B0
bool TerrainClass::Catch_Fire(void)
{
	if (!IsCrumbling && !IsOnFire && Class->Armor == ARMOR_WOOD && !Class->IsTiberiumSpawn) {
		int randomnum = Scen->RandomNumber();
		AnimClass * anim = new AnimClass(Rule->TreeFire[randomnum & 1], Center_Coord() + Coord(0, 0, 80), 0, 255);
		if (anim) {
			anim->SetOwnerObject(this);
		}
		anim->ZAdjust -= 20;
		IsOnFire = true;
		return(true);
	}
	return(false);
}


// FUNCTION: 0x0071C6B0
void TerrainClass::Fire_Out(void)
{
	if (IsOnFire) {
		IsOnFire = false;
		if (!IsCrumbling && !Strength) {
			Detach_All();
			Mark(MARK_CHANGE);
			Start_To_Crumble();
		}
	}
}


// FUNCTION: 0x0071CF30
HRESULT STDMETHODCALLTYPE TerrainClass::Save(IStream * stream, BOOL cleardirty)
{
	return(BASECLASS::Save(stream, cleardirty));
}

// FUNCTION: 0x0071D310
HRESULT STDMETHODCALLTYPE TerrainClass::GetClassID(CLSID * retval)
{
	if (retval == NULL) return(E_POINTER);
	*retval = CLSID_TerrainClass;
	return(S_OK);
}
