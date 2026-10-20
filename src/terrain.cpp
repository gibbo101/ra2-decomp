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
#include "msanim.h"
#include "overlay.h"
#include "overtype.h"
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


// FUNCTION: 0x0071C730
void TerrainClass::AI(void)
{
	BASECLASS::AI();

	if (Class->IsAnimated) {
		if (Fetch_Rate() == 0) {
			double r = (abs(Scen->RandomNumber()) % 1000000) / 1000000.0;
			if (r < Class->AnimationProbability) {
				Set_Stage(0);
				Set_Rate(Class->AnimationRate);
			}
		}
	}

	if (StageClass::Graphic_Logic()) {
		if (IsCrumbling && Fetch_Stage() == (((ShapeSet const *)Class->Get_Image_Data())->Get_Count())-1) {
			Delete_Me();
			return;
		}

		if (Class->IsTiberiumSpawn && Class->IsAnimated && Fetch_Stage() == (((ShapeSet const *)Class->Get_Image_Data())->Get_Count() / 2)) {
			Set_Stage(0);
			Set_Rate(0);
			Map[Get_Coord()].Spread_Tiberium(true);
		}
	}
}


// FUNCTION: 0x0071C8E0
void TerrainClass::Start_To_Crumble(void)
{
	if (!IsCrumbling) {
		IsCrumbling = true;
		Set_Rate(2);
		Set_Stage(0);
	}
}


// FUNCTION: 0x0071C930
bool TerrainClass::Limbo(void)
{
	if (!IsInLimbo) {
		Cell cell = Get_Cell();
		FacingType facing = FACING_FIRST;
		while (facing < FACING_COUNT) {
			CellClass & c = Map[Adjacent_Cell(cell, facing)];
			c.AdjacentObjectCount--;
			facing++;
		}
		Map[Get_Coord()].Flag.Occupy.Monolith = false;
	}
	Cell cell = Get_Cell();
	bool result = BASECLASS::Limbo();
	Map[cell].Recalc_Attributes();
	if (!ScenarioInit) {
		Map.Update_Cell_Zone(cell);
		Map.Update_Cell_Subzones(cell);
		Map.Radar_Background(cell);
	}
	return(result);
}


// FUNCTION: 0x0071CA70
void TerrainClass::Read_INI(CCINIClass const & ini)
{
	TerrainClass * tptr;

	const_cast<CCINIClass &>(ini).Reset();
	int len = ini.Entry_Count(INI_NAME);

	for (int index = 0; index < len; index++) {
		char const * entry = ini.Get_Entry(INI_NAME, index);
		TerrainTypeClass const * terrain = TGet_Class<TerrainTypeClass>(ini, INI_NAME, entry, NULL);

		Cell cell;
		if (NewINIFormat >= 4) {
			int val = atoi(entry);
			cell = Cell(val % 1000, val / 1000);
		} else {
			int val = atoi(entry);
			cell = Cell(val % 128, val / 128);
		}

		if (terrain != NULL) {
			tptr = new TerrainClass(terrain, cell);
		}
	}
}


// FUNCTION: 0x0071CB90
void TerrainClass::Write_INI(CCINIClass & ini)
{
	ini.Clear(INI_NAME);

	for (int index = 0; index < Terrains.Count(); index++) {
		TerrainClass * terrain;

		terrain = Terrains[index];
		if (terrain != NULL && !terrain->IsInLimbo && terrain->IsActive) {
			char	uname[10];
			Cell cell = terrain->Get_Cell();
			sprintf(uname, "%d", cell.X + cell.Y * 1000);
			TPut_Class<TerrainTypeClass>(ini, INI_NAME, uname, terrain->Class);
		}
	}
}


// FUNCTION: 0x0071CC50
bool TerrainClass::Render(Rect & cliprect, bool forced, bool extras_only) const
{
	if (Debug_Map || !MainWindow || ((forced || IsToDisplay) && IsDown && !IsInLimbo)) {
		IsToDisplay = false;

		Point2D point;

		cliprect = Intersect(cliprect, TacticalRect);

		Rect rect = ((TerrainClass *)this)->Get_Render_Rect() + TacticalRect.Top_Left();

		if (cliprect.Is_Overlapping(rect)) {

			TacticalMap->Coord_To_Pixel(Render_Coord(), point);

			if (cliprect.X > TacticalRect.X) {
				point.X += TacticalRect.X - cliprect.X;
			}
			if (cliprect.Y > TacticalRect.Y) {
				point.Y += TacticalRect.Y - cliprect.Y;
			}

			Draw_It(point, cliprect);

			return(true);
		}
	}

	return(false);
}


// FUNCTION: 0x0071CDA0
HRESULT STDMETHODCALLTYPE TerrainClass::Load(IStream * stream)
{
	TargetTracker.Remove_Index(Fetch_ID());
	HRESULT result = BASECLASS::Load(stream);
	if (SUCCEEDED(result)) {
		new (this) TerrainClass(NoInitClass());

		TargetTracker.Add_Index(Fetch_ID(), this);
		Swizzle_Pointer(&Class);
	}
	return(result);
}


// FUNCTION: 0x0071CF30
HRESULT STDMETHODCALLTYPE TerrainClass::Save(IStream * stream, BOOL cleardirty)
{
	return(BASECLASS::Save(stream, cleardirty));
}

// FUNCTION: 0x0071CF50
void TerrainClass::Compute_CRC(CRCEngine &crc) const
{
	BASECLASS::Compute_CRC(crc);

	crc((RTTIType)Class->RTTI);
	crc(Class->Fetch_ID());
	crc(IsOnFire);
	crc(IsCrumbling);
	crc(Unused1);
	crc(Unused2);
}


// FUNCTION: 0x0071CFD0
void TerrainClass::Detach(AbstractClass const * target, bool all)
{
	BASECLASS::Detach(target, all);
	if (Class == target) {
		Class = NULL;
	}
}


// FUNCTION: 0x0071D000
bool TerrainClass::Unlimbo(Coord const & coord, Dir256 dir)
{
	if (BASECLASS::Unlimbo(coord, dir)) {
		Cell cell = coord.As_Cell();
		for (FacingType facing = FACING_FIRST; facing < FACING_COUNT; facing++) {
			CellClass * cptr = &Map[Adjacent_Cell(cell, facing)];
			cptr->AdjacentObjectCount++;
		}

		TacticalMap->Coord_To_Pixel(Render_Coord(), RenderPixelPos);
		RenderPixelPos += Point2D(TacticalMap->TacPixelX, TacticalMap->TacPixelY);

		if (Map[coord].Overlay != OVERLAY_NONE && OverlayTypes[Map[coord].Overlay]->IsTiberium) {
			Map[coord].Overlay = OVERLAY_NONE;
			Map[coord].OverlayData = 0;
		}
		return(true);
	}
	return(false);
}


// FUNCTION: 0x0071D150
ObjectTypeClass const * TerrainClass::Class_Of(void) const
{
	return(Class);
}


// FUNCTION: 0x0071D160
Rect TerrainClass::Get_Render_Rect(void)
{
	Point2D drawpoint = RenderPixelPos - Point2D(TacticalMap->TacPixelX, TacticalMap->TacPixelY);
	SHPStruct const * sdata = (SHPStruct const *)TerrainClass::Get_Image_Data();
	if (sdata == NULL) {
		return(RECT_NONE);
	}

	Rect rect1 = sdata->GetFrameBounds(0);
	Rect rect2 = sdata->GetFrameBounds(sdata->Get_Count() / 2);

	int width = sdata->Get_Width();
	int height = sdata->Get_Height();

	Rect a = Union(rect1, rect2);
	return(Rect(drawpoint.X + a.X - width / 2, drawpoint.Y + a.Y - height / 2, a.Width, a.Height));
}


// FUNCTION: 0x0071D310
HRESULT STDMETHODCALLTYPE TerrainClass::GetClassID(CLSID * retval)
{
	if (retval == NULL) return(E_POINTER);
	*retval = CLSID_TerrainClass;
	return(S_OK);
}
