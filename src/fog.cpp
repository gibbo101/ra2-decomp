// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#define NDEBUG
#define INCLUDE_COM
#include "always.h"
#include "geometry.h"
#include "fog.h"
#include "_convert.h"
#include "_map.h"
#include "_rect.h"
#include "_surface.h"
#include "_tactica.h"
#include "anim.h"
#include "animtype.h"
#include "building.h"
#include "builtype.h"
#include "cell.h"
#include "convert.h"
#include "coord.h"
#include "crc.h"
#include "draw.h"
#include "globals.h"
#include "goptions.h"
#include "house.h"
#include "inline.h"
#include "lightcon.h"
#include "objtype.h"
#include "rect.h"
#include "scheme.h"
#include "shapeset.h"
#include "smudtype.h"
#include "sun.h"
#include "swizzle.h"
#include "tactical.h"
#include "terrain.h"
#include "vector.h"
#include <math.h>

// SYNTHETIC: GAMEMD 0x004D0540 SYMBOL
// _$E47
// SYNTHETIC: GAMEMD 0x004D0560 SYMBOL
// _$E2
// SYNTHETIC: GAMEMD 0x004D0590 SYMBOL
// _$E5
// SYNTHETIC: GAMEMD 0x004D05B0 SYMBOL
// _$E8
// SYNTHETIC: GAMEMD 0x004D05D0 SYMBOL
// _$E11
// SYNTHETIC: GAMEMD 0x004D05F0 SYMBOL
// _$E14
// SYNTHETIC: GAMEMD 0x004D0610 SYMBOL
// _$E17
// SYNTHETIC: GAMEMD 0x004D0630 SYMBOL
// _$E20
// SYNTHETIC: GAMEMD 0x004D0670 SYMBOL
// _$E23
// SYNTHETIC: GAMEMD 0x004D06A0 SYMBOL
// _$E26
// SYNTHETIC: GAMEMD 0x004D06D0 SYMBOL
// _$E29
// SYNTHETIC: GAMEMD 0x004D0700 SYMBOL
// _$E32
// SYNTHETIC: GAMEMD 0x004D0710 SYMBOL
// _$E35
// SYNTHETIC: GAMEMD 0x004D0730 SYMBOL
// _$E38
// SYNTHETIC: GAMEMD 0x004D0750 SYMBOL
// _$E134
// SYNTHETIC: GAMEMD 0x004D0790 SYMBOL
// _$E131
DynamicVectorClass<FoggedObjectClass *> FoggedObjectClass::FoggyObjects;

// SYNTHETIC: GAMEMD 0x004D07F0 SYMBOL
// _$E140
// SYNTHETIC: GAMEMD 0x004D0820 SYMBOL
// _$E137
IndexClass<int, FoggedObjectClass *> FoggedObjectClass::FoggedObjectIndex;

// FUNCTION: GAMEMD 0x004D0860
FoggedObjectClass::FoggedObjectClass(NoInitClass const & x) :
	BASECLASS(x)
{
}

// FUNCTION: GAMEMD 0x004D08B0
FoggedObjectClass::FoggedObjectClass(void) :
	BASECLASS(),
	Overlay(OVERLAY_NONE),
	House(NULL),
	OverlayData(0),
	RTTI(RTTI_NONE),
	Position(0,0,0),
	BoundingRect(0,0,0,0),
	CellHeight(0),
	Smudge(SMUDGE_NONE),
	SmudgeData(0),
	Records(),
	CanDraw(true)
{
	FoggyObjects.Add(this);
}

// FUNCTION: GAMEMD 0x004D0980
FoggedObjectClass::FoggedObjectClass(Coord const & coord, OverlayType type, int data) :
	BASECLASS(),
	Overlay(type),
	House(NULL),
	OverlayData(data),
	RTTI(RTTI_OVERLAY),
	Position(coord),
	CellHeight(Map[coord].Height),
	Smudge(SMUDGE_NONE),
	SmudgeData(0),
	Records(),
	CanDraw(true)
{
	CellClass * cellptr = &Map[coord];
	BoundingRect = Union(cellptr->Overlay_Render_Rect(), cellptr->Overlay_Shadow_Render_Rect());
	BoundingRect.X += TacticalMap->TacPixelX;
	BoundingRect.Y += TacticalMap->TacPixelY;
	BoundingRect -= TacticalRect.Top_Left();

	FoggyObjects.Add(this);

	Cell cell = Position.As_Cell();
	int id = cell.As_Int() - RTTI + INT_MAX;
	FoggedObjectIndex.Add_Index(id, this);
}

// FUNCTION: GAMEMD 0x004D0C40
FoggedObjectClass::FoggedObjectClass(Coord const & coord, SmudgeType type, int data) :
	BASECLASS(),
	Overlay(OVERLAY_NONE),
	House(NULL),
	OverlayData(0),
	RTTI(RTTI_SMUDGE),
	CellHeight(Map[coord].Height),
	Smudge(type),
	SmudgeData(data),
	Records(),
	CanDraw(true)
{
	Coord crd;
	Point2D point;
	CellClass * cellptr = &Map[coord];
	crd = Coord(coord);
	crd.Z = LEVEL_LEPTON_H * cellptr->Height;
	Position = crd;

	TacticalMap->Coord_To_Pixel(crd, point);
	point += Point2D(ISO_TILE_PIXEL_W / -2, ISO_TILE_PIXEL_H / -2);
	BoundingRect = Rect(point.X, point.Y, ISO_TILE_PIXEL_W, ISO_TILE_PIXEL_H);
	BoundingRect.X += TacticalMap->TacPixelX;
	BoundingRect.Y += TacticalMap->TacPixelY;

	FoggyObjects.Add(this);

	Cell cell = Position.As_Cell();
	int id = cell.As_Int() - RTTI + INT_MAX;
	FoggedObjectIndex.Add_Index(id, this);
}

// FUNCTION: GAMEMD 0x004D0EF0
FoggedObjectClass::FoggedObjectClass(BuildingClass * object, bool fade) :
	BASECLASS(),
	Overlay(OVERLAY_NONE),
	House(object->House),
	OverlayData(0),
	RTTI(RTTI_BUILDING),
	Position(object->PositionCoord),
	CellHeight(Map[(Coord const &)object->PositionCoord].Height),
	Smudge(SMUDGE_NONE),
	SmudgeData(0),
	Records(),
	CanDraw(fade)
{
	BoundingRect = object->Get_Render_Rect();

	FoggyObjects.Add(this);

	Cell cell = Position.As_Cell();
	int id = cell.As_Int() - RTTI + INT_MAX;
	FoggedObjectIndex.Add_Index(id, this);

	object->IsFogged = true;
	int shapenum = object->Shape_Number();

	DrawRecord record(object->Class, shapenum, 0, 0);
	if (object->Class->IsLaserFence && (object->LaserFenceFrame == 12 || object->LaserFenceFrame == 8)) {
		record.HeightAdjust = 1;
	} else {
		record.HeightAdjust = 0;
		if (object->Class->IsFirestormWall) {
			record.HeightAdjust = 1;
		}
	}
	Records.Add(record);

	for (int i = 0; i < BANIM_COUNT; i++) {
		if (object->Anims[i] != NULL) {
			object->Anims[i]->IsFogged = true;
			AnimTypeClass *atype = object->Anims[i]->Type;
			BoundingRect = Union(BoundingRect, object->Anims[i]->Get_Visual_Rect());
			record.TypeClass = atype;
			record.FrameNumber = atype->Start + object->Anims[i]->Animation.Fetch_Stage();
			record.HeightAdjust = 0;
			record.ZAdjust = object->Anims[i]->ZAdjust;
			Records.Add(record);
		}
	}

	TacticalMap->Register_Dirty_Area(BoundingRect);
	BoundingRect.X += TacticalMap->TacPixelX;
	BoundingRect.Y += TacticalMap->TacPixelY;
}

// FUNCTION: GAMEMD 0x004D1370
FoggedObjectClass::FoggedObjectClass(TerrainClass * object) :
	BASECLASS(),
	Overlay(OVERLAY_NONE),
	House(NULL),
	OverlayData(0),
	RTTI(RTTI_TERRAIN),
	Position(object->PositionCoord),
	CellHeight(Map[(Coord const &)object->PositionCoord].Height),
	Smudge(SMUDGE_NONE),
	SmudgeData(0),
	Records(),
	CanDraw(true)
{
	BoundingRect = object->Get_Render_Rect();
	BoundingRect.X += TacticalMap->TacPixelX;
	BoundingRect.Y += TacticalMap->TacPixelY;

	FoggyObjects.Add(this);

	Records.Add(DrawRecord(object->Class, 0, 0, 0));

	Cell cell = Position.As_Cell();
	int id = cell.As_Int() - RTTI + INT_MAX;
	FoggedObjectIndex.Add_Index(id, this);
}

FoggedObjectClass::~FoggedObjectClass(void)
{
	FoggyObjects.Delete(this);

	Cell cell = Get_Cell();
	int id = cell.As_Int() - RTTI + INT_MAX;
	FoggedObjectIndex.Remove_Index(id);

	if (RTTI == RTTI_BUILDING) {
		BuildingClass *bptr = Map[cell].Cell_Building();
		if (bptr != NULL) {
			bptr->IsFogged = false;
			for (int i = 0; i < BANIM_COUNT; i++) {
				if (bptr->Anims[i] != NULL) {
					bptr->Anims[i]->IsFogged = false;
				}
			}
		}
	}

	if (TacticalMap != NULL) {
		Rect rect = BoundingRect;
		rect -= Point2D(TacticalMap->TacPixelX, TacticalMap->TacPixelY);
		TacticalMap->Register_Dirty_Area(rect);
	}
}

// FUNCTION: GAMEMD 0x004D1890
void Draw_Fogged_Objects(Rect const & rect)
{
	if (MainWindow == NULL) {
		return;
	}

	for (int index = 0; index < FoggedObjectClass::FoggedObjectIndex.Count(); index++) {

		FoggedObjectClass * data = FoggedObjectClass::FoggedObjectIndex.Fetch_By_Position(index);

		if (!data->CanDraw) {
			continue;
		}

		Rect rectangle = data->BoundingRect;
		rectangle.X -= TacticalMap->TacPixelX;
		rectangle.Y -= TacticalMap->TacPixelY;
		rectangle += TacticalRect.Top_Left();

		Rect clipped = Intersect(rect, rectangle);
		if (!clipped.Is_Valid()) {
			continue;
		}

		switch (data->RTTI) {

			case RTTI_OVERLAY: {
				CellClass * cellptr = &Map[(Coord)data->Position];

				OverlayType saveoverlay = cellptr->Overlay;
				int saveoverlaydata = cellptr->OverlayData;
				cellptr->Overlay = data->Overlay;
				cellptr->OverlayData = data->OverlayData;

				Point2D point;
				TacticalMap->Coord_To_Pixel(Coord_Whole(cellptr->CellID), point);
				point.X += ISO_TILE_PIXEL_W / -2;

				cellptr->Draw_Overlay(point, rect);
				cellptr->Draw_Overlay_Shadow(point, rect);

				cellptr->Overlay = saveoverlay;
				cellptr->OverlayData = saveoverlaydata;
				break;
			}

			case RTTI_TERRAIN: {
				Coord * position = &data->Position;
				ObjectTypeClass * type = data->Records[0].TypeClass;
				CellClass * cellptr = &Map[(Coord)*position];
				int shapenum = data->Records[0].FrameNumber;
				ShapeSet const * shape = (ShapeSet const *)type->Get_Image_Data();

				Point2D xy;
				TacticalMap->Coord_To_Pixel((Coord)*position, xy);
				xy += TacticalRect.Top_Left() - rect.Top_Left();

				int zadjust = -TacticalMap->Z_Lepton_To_Pixel(((Coord)*position).Z);

				if (cellptr->Drawer == NULL) {
					cellptr->Init_Drawer(NULL, 0x10000, 0, NORMAL_LIGHT, NORMAL_LIGHT, NORMAL_LIGHT);
				}

				Draw_Shape(*LogicalSurface, *cellptr->Drawer, shape, shapenum, xy, rect, ShapeFlags_Type(SHAPE_CENTER|SHAPE_WIN_REL|SHAPE_ALPHA|SHAPE_ZWRITE), NULL, zadjust - 4, ZGRAD_90DEG, cellptr->TileBrightness);

				if (DrawShapeShadows) {
					Draw_Shape(*LogicalSurface, *cellptr->Drawer, shape, shapenum + shape->Get_Count() / 2, xy, rect, ShapeFlags_Type(SHAPE_DARKEN|SHAPE_CENTER|SHAPE_WIN_REL|SHAPE_ZWRITE), NULL, zadjust - 2, ZGRAD_GROUND, NORMAL_LIGHT);
				}
				break;
			}

			case RTTI_BUILDING: {
				for (int record = 0; record < data->Records.Count(); record++) {

					BuildingTypeClass * type = (BuildingTypeClass *)data->Records[record].TypeClass;
					int shapenum = data->Records[record].FrameNumber;
					ShapeSet const * shape = (ShapeSet const *)type->Get_Image_Data();
					unsigned char heightadjust = data->Records[record].HeightAdjust;
					LightConvertClass * converter = ColorSchemes[data->House->Scheme]->Converter;

					if (record == 0) {

						Coord * position = &data->Position;
						Point2D drawpoint;
						TacticalMap->Coord_To_Pixel((Coord)*position - Coord(CELL_LEPTON_W / 2, CELL_LEPTON_H / 2, 0), drawpoint);
						CellClass * cellptr = &Map[(Coord)*position];
						drawpoint += TacticalRect.Top_Left() - rect.Top_Left();

						if (type->IsInvisibleInGame) {
							break;
						}

						int zadjust = type->NormalZAdjust;

						Point2D shapepoint = drawpoint;
						int height = drawpoint.Y + shape->Get_Height() / 2;
						Rect cliprect = rect;
						if (cliprect.Height > height) {
							cliprect.Height = height;
						}

						Point2D zdrawpoint(144, 172);
						zdrawpoint += type->ZShapePointMove;
						Point2D zsizeoffset(type->Width() * CELL_LEPTON - CELL_LEPTON, type->Height() * CELL_LEPTON - CELL_LEPTON);
						zdrawpoint -= TacticalMap->Coord_To_Pixel_Absolute(zsizeoffset);

						ShapeSet const * zshapefile = (ShapeSet const *)BuildingTypeClass::BuildingZShape;

						int zlepton = -TacticalMap->Z_Lepton_To_Pixel(((Coord)*position).Z);
						int brightness = Map[(Coord)*position].Brightness + type->ExtraLight;

						if (cellptr->Drawer == NULL) {
							cellptr->Init_Drawer(NULL, 0x10000, 0, NORMAL_LIGHT, NORMAL_LIGHT, NORMAL_LIGHT);
						}
						ConvertClass * drawconvert = type->IsTerrainPalette ? (ConvertClass *)cellptr->Drawer : (ConvertClass *)converter;

						if (cliprect.Height > 0) {
							if (heightadjust) {
								Draw_Shape(*LogicalSurface, *drawconvert, shape, shapenum, shapepoint, cliprect, ShapeFlags_Type(SHAPE_CENTER|SHAPE_WIN_REL|SHAPE_ALPHA|SHAPE_ZWRITE), NULL, zlepton - 2, ZGRAD_GROUND, brightness);
								Draw_Shape(*LogicalSurface, *drawconvert, shape, shapenum + shape->Get_Count() / 2, shapepoint, cliprect, ShapeFlags_Type(SHAPE_DARKEN|SHAPE_CENTER|SHAPE_WIN_REL|SHAPE_ALPHA|SHAPE_ZWRITE), NULL, zlepton - 2, ZGRAD_GROUND, NORMAL_LIGHT);
							} else {
								Draw_Shape(*LogicalSurface, *drawconvert, shape, shapenum, shapepoint, cliprect, ShapeFlags_Type(SHAPE_CENTER|SHAPE_WIN_REL|SHAPE_ALPHA|SHAPE_ZWRITE), NULL, zlepton + zadjust - 2, ZGRAD_90DEG, brightness, 0, zshapefile, 0, zdrawpoint);
								Draw_Shape(*LogicalSurface, *drawconvert, shape, shapenum + shape->Get_Count() / 2, shapepoint, cliprect, ShapeFlags_Type(SHAPE_DARKEN|SHAPE_CENTER|SHAPE_WIN_REL|SHAPE_ALPHA|SHAPE_ZWRITE), NULL, zlepton - 2, ZGRAD_GROUND, NORMAL_LIGHT);
							}
						}

						if (type->BibShape != NULL) {
							Draw_Shape(*LogicalSurface, *drawconvert, type->BibShape, shapenum, drawpoint, rect, ShapeFlags_Type(SHAPE_CENTER|SHAPE_WIN_REL|SHAPE_ALPHA|SHAPE_ZWRITE), NULL, zlepton - 3, ZGRAD_GROUND, brightness);
						}

					} else {

						AnimTypeClass * anim = (AnimTypeClass *)type;

						if (anim != NULL && anim->Fetch_RTTI() == RTTI_ANIMTYPE && anim->DetailLevel <= Options.DetailLevel && shape != NULL) {

							Point3D * position = &data->Position;
							Point2D drawpoint;
							TacticalMap->Coord_To_Pixel((Coord)*position - Coord(CELL_LEPTON_W / 2, CELL_LEPTON_H / 2, 0), drawpoint);
							drawpoint += TacticalRect.Top_Left() - rect.Top_Left();

							int brightness = NORMAL_LIGHT;
							if (!anim->UseNormalLight) {
								brightness = Map[(Coord)*position].Brightness;
							}

							ConvertClass * animconvert;
							if (converter != NULL && anim->ShouldUseCellDrawer) {
								animconvert = (ConvertClass *)converter;
							} else {
								animconvert = AnimDrawer;
							}

							int zadjust = data->Records[record].ZAdjust;

							if (anim->Flat) {
								Draw_Shape(*LogicalSurface, *animconvert, shape, shapenum, drawpoint, rect, ShapeFlags_Type(SHAPE_CENTER|SHAPE_WIN_REL|SHAPE_ALPHA|SHAPE_ZGRAD), NULL, zadjust - TacticalMap->Z_Lepton_To_Pixel(((Coord)*position).Z) - 2, ZGRAD_GROUND, brightness);
							} else {
								Draw_Shape(*LogicalSurface, *animconvert, shape, shapenum, drawpoint, rect, ShapeFlags_Type(SHAPE_CENTER|SHAPE_WIN_REL|SHAPE_ALPHA|SHAPE_ZGRAD), NULL, zadjust - TacticalMap->Z_Lepton_To_Pixel(((Coord)*position).Z) - 2, ZGRAD_90DEG, brightness);
							}
						}
					}
				}
				break;
			}

			case RTTI_SMUDGE: {
				Point2D point = data->BoundingRect.Top_Left() - Point2D(TacticalMap->TacPixelX, TacticalMap->TacPixelY) + (TacticalRect.Top_Left() + Point2D(ISO_TILE_PIXEL_W / 2, 0)) - rect.Top_Left();

				ShapeSet const * shape = (ShapeSet const *)SmudgeTypes[data->Smudge]->Get_Image_Data();
				if (shape != NULL) {
					Point3D * position = &data->Position;
					CellClass * cellptr = &Map[(Coord)*position];
					int brightness = cellptr->TileBrightness;
					Draw_Shape(*LogicalSurface, *cellptr->Drawer, shape, data->SmudgeData, point, rect, ShapeFlags_Type(SHAPE_CENTER|SHAPE_WIN_REL|SHAPE_ALPHA), NULL, -TacticalMap->Z_Lepton_To_Pixel(((Coord)*position).Z), ZGRAD_90DEG, brightness);
				}
				break;
			}

			default:
				break;
		}
	}
}

// FUNCTION: GAMEMD 0x004D2370
void Update_Fogged_Objects(void)
{
	for (int i = 0; i < FoggedObjectClass::FoggyObjects.Count(); i++) {
		FoggedObjectClass *fptr = FoggedObjectClass::FoggyObjects[i];
		if (fptr->Get_Head_Record_Occupy_List() != NULL) {
			const Cell * list = fptr->Get_Head_Record_Occupy_List();
			Cell cell = fptr->Get_Cell();
			while (*list != REFRESH_EOL) {
				Cell ncell = *list + cell;
				Map[ncell].IsFogged = true;
				list++;
			}
		} else {
			Map[(Coord)fptr->Position].IsFogged = true;
		}
	}
}

// FUNCTION: GAMEMD 0x004D24A0
HRESULT STDMETHODCALLTYPE FoggedObjectClass::Save(IStream * stream, BOOL cleardirty)
{
	HRESULT result = BASECLASS::Save(stream, cleardirty);
	if (SUCCEEDED(result)) {
		int index;

		int count = Records.Count();
		result = stream->Write(&count, sizeof(count), NULL);
		if (FAILED(result)) {
			return(result);
		}

		for (index = 0; index < count; index++) {
			result = stream->Write(&Records[index], sizeof(Records[index]), NULL);
			if (FAILED(result)) {
				return(S_OK);
			}
		}

		result = S_OK;
	}
	return(result);
}

// FUNCTION: GAMEMD 0x004D2510
HRESULT STDMETHODCALLTYPE FoggedObjectClass::Load(IStream * stream)
{
	HRESULT result = BASECLASS::Load(stream);
	if (SUCCEEDED(result)) {
		new (this) FoggedObjectClass(NoInitClass());

		Swizzle_Pointer(&House);

		int count = 0;
		result = stream->Read(&count, sizeof(count), NULL);
		if (FAILED(result)) {
			return(result);
		}

		for (int i = 0; i < count; i++) {
			DrawRecord record;

			result = stream->Read(&record, sizeof(record), NULL);
			if (FAILED(result)) {
				return(result);
			}

			Records.Add(record);
			Swizzle_Pointer(&Records[Records.Count()-1].TypeClass);
		}

		Cell cell = Get_Cell();
		int id = cell.As_Int() - RTTI + INT_MAX;
		FoggedObjectIndex.Add_Index(id, this);
	}
	return(result);
}

// FUNCTION: GAMEMD 0x004D2790
Cell const * FoggedObjectClass::Get_Head_Record_Occupy_List(void)
{
	if (RTTI == RTTI_BUILDING) {
		return(Records[0].TypeClass->Occupy_List());
	}
	return(NULL);
}

// FUNCTION: GAMEMD 0x004D27B0
RTTIType FoggedObjectClass::Fetch_RTTI(void) const
{
	return(RTTI_FOGGEDOBJECT);
}

// FUNCTION: GAMEMD 0x004D27C0
int FoggedObjectClass::Fetch_Object_Size(void) const
{
	return(sizeof((*this)));
}

// FUNCTION: GAMEMD 0x004D27D0
HRESULT STDMETHODCALLTYPE FoggedObjectClass::GetClassID(CLSID * retval)
{
	if (retval == NULL) return(E_POINTER);
	*retval = CLSID_FoggedObjectClass;
	return(S_OK);
}

// FUNCTION: GAMEMD 0x004D2810
void FoggedObjectClass::Compute_CRC(CRCEngine &crc) const
{
	BASECLASS::Compute_CRC(crc);
	crc(Overlay);
	crc(OverlayData);
	crc(RTTI);
	crc(Position.X);
	crc(Position.Y);
	crc(Position.Z);
	crc(BoundingRect.X);
	crc(BoundingRect.Y);
	crc(BoundingRect.Width);
	crc(BoundingRect.Height);
	crc(CellHeight);
	crc(Smudge);
	crc(SmudgeData);
	crc(Records.Count());
}

// FUNCTION: GAMEMD 0x004D28C0
ObjectTypeClass * FoggedObjectClass::Get_Head_Record_Object_Type(void)
{
	if (Records.Count() > 0) {
		return(Records[0].TypeClass);
	}
	return(NULL);
}

// FUNCTION: GAMEMD 0x004D28D0
Cell FoggedObjectClass::Get_Cell(void)
{
	return(Position.As_Cell());
}

// SYNTHETIC: GAMEMD 0x004D2910
// ??_GFoggedObjectClass@@UAEPAXI@Z

// TEMPLATE: GAMEMD 0x004D2930
// ?Clear@?$DynamicVectorClass@UDrawRecord@FoggedObjectClass@@@@UAEXXZ
// TEMPLATE: GAMEMD 0x004D2960
// ?ID@?$DynamicVectorClass@UDrawRecord@FoggedObjectClass@@@@UAEHPBUDrawRecord@FoggedObjectClass@@@Z
template class DynamicVectorClass<FoggedObjectClass::DrawRecord>;

// TEMPLATE: GAMEMD 0x004D2980
// ?GetItem@?$VectorClass@UDrawRecord@FoggedObjectClass@@@@UBE?AUDrawRecord@FoggedObjectClass@@H@Z
// TEMPLATE: GAMEMD 0x004D29B0
// ??8?$VectorClass@UDrawRecord@FoggedObjectClass@@@@UBE_NABV0@@Z
// TEMPLATE: GAMEMD 0x004D2A10
// ?Resize@?$VectorClass@UDrawRecord@FoggedObjectClass@@@@UAE_NHPBUDrawRecord@FoggedObjectClass@@@Z
// TEMPLATE: GAMEMD 0x004D2B20
// ?Clear@?$VectorClass@UDrawRecord@FoggedObjectClass@@@@UAEXXZ
// TEMPLATE: GAMEMD 0x004D2B50
// ?ID@?$VectorClass@UDrawRecord@FoggedObjectClass@@@@UAEHABUDrawRecord@FoggedObjectClass@@@Z
// TEMPLATE: GAMEMD 0x004D2BA0
// ?ID@?$VectorClass@UDrawRecord@FoggedObjectClass@@@@UAEHPBUDrawRecord@FoggedObjectClass@@@Z
template class VectorClass<FoggedObjectClass::DrawRecord>;

// TEMPLATE: GAMEMD 0x004D2BC0
// ?Resize@?$DynamicVectorClass@UDrawRecord@FoggedObjectClass@@@@UAE_NHPBUDrawRecord@FoggedObjectClass@@@Z
// TEMPLATE: GAMEMD 0x004D2CD0
// ?ID@?$DynamicVectorClass@UDrawRecord@FoggedObjectClass@@@@UAEHABUDrawRecord@FoggedObjectClass@@@Z
template class DynamicVectorClass<FoggedObjectClass::DrawRecord>;

// SYNTHETIC: GAMEMD 0x004D2D10
// ??_G?$VectorClass@UDrawRecord@FoggedObjectClass@@@@UAEPAXI@Z
// SYNTHETIC: GAMEMD 0x004D2D60
// ??_G?$DynamicVectorClass@UDrawRecord@FoggedObjectClass@@@@UAEPAXI@Z
// TEMPLATE: GAMEMD 0x004D2DB0
// ??0?$VectorClass@UDrawRecord@FoggedObjectClass@@@@QAE@HPBUDrawRecord@FoggedObjectClass@@@Z
// TEMPLATE: GAMEMD 0x004D2E40
// ?Increase_Table_Size@?$IndexClass@HPAVFoggedObjectClass@@@@AAE_NH@Z
template class IndexClass<int, FoggedObjectClass *>;

// TEMPLATE: GAMEMD 0x004D2ED0
// ?search_compfunc@?$IndexClass@HPAVFoggedObjectClass@@@@CAHPBX0@Z

// TEMPLATE: GAMEMD 0x004D2F00
// ?Binary_Search@@YIPAUNodeElement@?$IndexClass@HPAVFoggedObjectClass@@@@PAU12@HABU12@@Z
template IndexClass<int, FoggedObjectClass *>::NodeElement * Binary_Search(IndexClass<int, FoggedObjectClass *>::NodeElement *, int, IndexClass<int, FoggedObjectClass *>::NodeElement const &);
