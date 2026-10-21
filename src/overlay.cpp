// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#define INCLUDE_COM
#include "always.h"
#include "geometry.h"
#include "rect.h"
#include "overlay.h"
#include "_map.h"
#include "_rect.h"
#include "_rules.h"
#include "_surface.h"
#include "_tactica.h"
#include "anim.h"
#include "bsurface.h"
#include "building.h"
#include "ccini.h"
#include "cell.h"
#include "dbgprint.h"
#include "draw.h"
#include "globals.h"
#include "house.h"
#include "ini.h"
#include "inline.h"
#include "lcwpipe.h"
#include "lcwstraw.h"
#include "lightcon.h"
#include "overtype.h"
#include "rules.h"
#include "scenario.h"
#include "scheme.h"
#include "session.h"
#include "surface.h"
#include "swizzle.h"
#include "tactical.h"
#include "tiberium.h"
#include "tracker.h"
#include "tube.h"
#include "vector.h"
#include "vein.h"
#include "xpipe.h"
#include "xstraw.h"
#include "overlay.hh"

// SYNTHETIC: GAMEMD 0x005FC170 SYMBOL
// _$E2
// SYNTHETIC: GAMEMD 0x005FC1A0 SYMBOL
// _$E5
// SYNTHETIC: GAMEMD 0x005FC1C0 SYMBOL
// _$E8
// SYNTHETIC: GAMEMD 0x005FC1E0 SYMBOL
// _$E11
// SYNTHETIC: GAMEMD 0x005FC200 SYMBOL
// _$E14
// SYNTHETIC: GAMEMD 0x005FC220 SYMBOL
// _$E17
// SYNTHETIC: GAMEMD 0x005FC240 SYMBOL
// _$E20
// SYNTHETIC: GAMEMD 0x005FC280 SYMBOL
// _$E23
// SYNTHETIC: GAMEMD 0x005FC2B0 SYMBOL
// _$E26
// SYNTHETIC: GAMEMD 0x005FC2E0 SYMBOL
// _$E29
// SYNTHETIC: GAMEMD 0x005FC310 SYMBOL
// _$E32
// SYNTHETIC: GAMEMD 0x005FC320 SYMBOL
// _$E35
// SYNTHETIC: GAMEMD 0x005FC340 SYMBOL
// _$E38
// SYNTHETIC: GAMEMD 0x005FC360 SYMBOL
// _$E41

// FUNCTION: GAMEMD 0x005FC380
OverlayClass::OverlayClass(OverlayTypeClass const * ttype, Cell const & pos, HousesType house) :
	Class((OverlayTypeClass *)ttype)
{
	Create_ID();

	Array.Add(this);

	if (pos != CELL_NONE) {
		ToOwn = house;
		Coord coord(pos);
		CellClass & cellref = Map[coord.As_Cell()];
		if (cellref.Cell_Terrain(false) == NULL) {
			BASECLASS::Unlimbo(coord, DIR_N);
		}
		ToOwn = HOUSE_NONE;
	}
}

// FUNCTION: GAMEMD 0x005FC4D0
// FUNCTION: GAMEMD 0x005FDF70
OverlayClass::~OverlayClass(void)
{
	Detach_This_From_All(this, true);
	Array.Delete(this);

	if (GameActive) {
		OverlayClass::Limbo();
	}
	Class = NULL;
}

// FUNCTION: GAMEMD 0x005FD270
bool OverlayClass::Unlimbo(Coord const & coord, Dir256 facing)
{
	CellClass & cellref = Map[coord.As_Cell()];
	if (cellref.Cell_Terrain(false) != NULL) {
		return(false);
	}
	return(BASECLASS::Unlimbo(coord, facing));
}

// FUNCTION: GAMEMD 0x005FD2E0
void OverlayClass::Read_INI(CCINIClass & ini)
{
	ini.Reset();

	if (NewINIFormat > 1) {

		BSurface temp_surface(640, 400, 2);
		temp_surface.Fill(0);

		int len = ini.Get_UUBlock("OverlayPack", temp_surface.Lock(), temp_surface.Get_Width() * temp_surface.Get_Height() * temp_surface.Bytes_Per_Pixel());

		if (len > 0) {
			BufferStraw bpipe(temp_surface.Lock(), len);
			LCWStraw uncomp(LCWStraw::DECOMPRESS);
			uncomp.Get_From(&bpipe);

			for (int y = 0; y < MAP_CELL_H; y++) {
				for (int x = 0; x < MAP_CELL_W; x++) {
					Cell cell(x, y);

					OverlayType classid = OVERLAY_NONE;

					uncomp.Get(&(char&)classid, sizeof(char));
					if (classid != OVERLAY_NONE) {
						classid = OverlayType(classid & 0x00FF);
					}

					if (classid != OVERLAY_NONE && (OverlayTypes[classid]->Get_Image_Data() != NULL || OverlayTypes[classid]->CellAnim)) {

						if (Session.Type == GAME_NORMAL || !OverlayTypes[classid]->IsCrate) {

							if (Map.In_Radar(cell)) {
								unsigned char old_overlay_data = (&Map[cell])->OverlayData;
								new OverlayClass(OverlayTypes[classid], cell);

								if ((int)classid == OVERLAY_BRIDGE1 || (int)classid == OVERLAY_BRIDGE2 ||
									(int)classid == OVERLAY_BRIDGEHEAD21 || (int)classid == OVERLAY_BRIDGEHEAD22) {
									(&Map[cell])->OverlayData = old_overlay_data;
								}
							}
						}
					}
				}
			}
			temp_surface.Unlock();
		}
		temp_surface.Unlock();

		len = ini.Get_UUBlock("OverlayDataPack", temp_surface.Lock(), temp_surface.Get_Width() * temp_surface.Get_Height());

		if (len > 0) {
			BufferStraw databpipe(temp_surface.Lock(), len);
			LCWStraw datauncomp(LCWStraw::DECOMPRESS);
			datauncomp.Get_From(&databpipe);

			for (int y = 0; y < MAP_CELL_H; y++) {
				for (int x = 0; x < MAP_CELL_W; x++) {
					Cell cell(x, y);
					unsigned char overlay_data = 0;
					datauncomp.Get(&(char &)overlay_data, sizeof(char));

					if (Map.In_Radar(cell)) {
						CellClass *cellptr = &Map[cell];
						cellptr->OverlayData = overlay_data;
					}
				}
			}
			temp_surface.Unlock();
		}
		temp_surface.Unlock();
	}
	Process_Deferred_Deletion();
}

// FUNCTION: GAMEMD 0x005FD6A0
void OverlayClass::Write_INI(CCINIClass & ini)
{
	ini.Clear(INI_NAME);
	ini.Clear("OverlayPack");

	BufferPipe bpipe(AlternateSurface->Lock(), AlternateSurface->Get_Width() * AlternateSurface->Get_Height());
	LCWPipe comppipe(LCWPipe::COMPRESS);

	comppipe.Put_To(&bpipe);

	int total = 0;
	int y;
	for (y = 0; y < MAP_CELL_H; y++) {
		for (int x = 0; x < MAP_CELL_W; x++) {
			total += comppipe.Put(&(char&)(Map[Cell(x, y)].Overlay), sizeof(char));
		}
	}
	if (total) {
		ini.Put_UUBlock("OverlayPack", AlternateSurface->Lock(), total);
		AlternateSurface->Unlock();
	}
	AlternateSurface->Unlock();

	ini.Clear("OverlayDataPack");

	BufferPipe databpipe(AlternateSurface->Lock(), AlternateSurface->Get_Width() * AlternateSurface->Get_Height());
	LCWPipe datacomppipe(LCWPipe::COMPRESS);

	datacomppipe.Put_To(&databpipe);

	total = 0;
	for (y = 0; y < MAP_CELL_H; y++) {
		for (int x = 0; x < MAP_CELL_W; x++) {
			total += datacomppipe.Put(&(char&)(Map[Cell(x, y)].OverlayData), sizeof(char));
		}
	}
	if (total) {
		ini.Put_UUBlock("OverlayDataPack", AlternateSurface->Lock(), total);
		AlternateSurface->Unlock();
	}
	AlternateSurface->Unlock();
}

// FUNCTION: GAMEMD 0x005FD8F0
HRESULT STDMETHODCALLTYPE OverlayClass::Load(IStream * stream)
{
	HRESULT result = BASECLASS::Load(stream);
	if (SUCCEEDED(result)) {
		new (this) OverlayClass(NoInitClass());

		Swizzle_Pointer(&Class);
	}
	return(result);
}

// FUNCTION: GAMEMD 0x005FD950
HRESULT STDMETHODCALLTYPE OverlayClass::Save(IStream * stream, BOOL cleardirty)
{
	HRESULT result = BASECLASS::Save(stream, cleardirty);
	return(result);
}

// FUNCTION: GAMEMD 0x005FD970
void OverlayClass::Editor_Draw_It(Point2D const & point, Rect const & cliprect) const
{
	ShapeSet const * shapefile = (ShapeSet const *)Class->Get_Image_Data();
	int yadjust = TacticalMap->Z_Lepton_To_Pixel(Map.Get_Height_GL(Get_Coord()));

	Point2D drawpoint = point + Overlay_Draw_Offset(OverlayType(OverlayTypes.ID(Class)));
	drawpoint.Y += TacticalRect.Y;

	if (Class->IsTiberium) {
		TiberiumClass const * tib = Tiberiums[Which_Tiberium_Type(OverlayType(OverlayTypes.ID(Class)))];
		ColorScheme const * scheme = ColorSchemes[tib->Color];
		int variety = 0;
		shapefile = (ShapeSet const *)OverlayTypes[tib->Overlay->HeapID + variety]->Get_Image_Data();
		if (!shapefile && Class->CellAnim) {
			shapefile = (ShapeSet const *)Class->CellAnim->Get_Image_Data();
		}
		Draw_Shape(*LogicalSurface, *(ConvertClass *)scheme->Converter, shapefile, 0, drawpoint, cliprect, (ShapeFlags_Type)(SHAPE_CENTER|SHAPE_WIN_REL|SHAPE_ALPHA), NULL, -2-yadjust, ZGRAD_GROUND);
	} else if (Class->IsVeins) {
		if (Class->HeapID == OVERLAY_VEINHOLE) {
			drawpoint.Y -= 38;
		}
		Draw_Shape(*LogicalSurface, *ColorSchemes[PlayerPtr->Scheme]->Converter, shapefile, 0, drawpoint, cliprect, (ShapeFlags_Type)(SHAPE_CENTER|SHAPE_WIN_REL|SHAPE_ALPHA), NULL, -2-yadjust, ZGRAD_GROUND, Map[Get_Coord()].TileBrightness);
	} else {
		CellClass *cellptr = NULL;
		if (Map.In_Radar(Get_Coord().As_Cell())) {
			cellptr = &Map[Get_Coord()];
		} else if (Map.In_Radar(TacticalMap->Pixel_To_Cell(drawpoint))) {
			cellptr = &Map[TacticalMap->Pixel_To_Cell(drawpoint)];
		} else {
			cellptr = &Map[Cell(1, Map.PlayRect.Width)];
		}
		Draw_Shape(*LogicalSurface, *(ConvertClass *)(cellptr->Drawer), shapefile, 0, drawpoint, cliprect, (ShapeFlags_Type)(SHAPE_CENTER|SHAPE_WIN_REL|SHAPE_ALPHA), NULL, -2-yadjust, ZGRAD_GROUND, cellptr->TileBrightness);
	}
}

// FUNCTION: GAMEMD 0x005FDCC0
Point2D Overlay_Draw_Offset(OverlayType overlay)
{
	OverlayTypeClass const & otype = *OverlayTypes[overlay];
	Point2D offset(0, 0);

	if (otype.IsTiberium || otype.IsWall || otype.HeapID == OVERLAY_VEINS || otype.IsCrate) {
		offset.Y -= 12;
	}
	if (otype.Land == LAND_RAILROAD) {
		offset.Y -= 1;
	}
	if (overlay == OVERLAY_VEINS) {
		offset.Y -= 1;
	}

	return(offset);
}

// FUNCTION: GAMEMD 0x005FDD20
TiberiumType Which_Tiberium_Type(OverlayType overlay)
{
	if (overlay != OVERLAY_NONE) {
		OverlayTypeClass const * otype = OverlayTypes[overlay];
		if (otype->IsTiberium) {
			for (int index = 0; index < Tiberiums.Count(); index++) {
				TiberiumClass const * tiberium = Tiberiums[index];

				if (overlay >= tiberium->Overlay->HeapID && overlay < tiberium->Overlay->HeapID + tiberium->Variety) {
					return(TiberiumType(tiberium->HeapID));
				}

				if (overlay >= tiberium->Overlay->HeapID + tiberium->Variety && overlay < tiberium->Overlay->HeapID + tiberium->Variety+tiberium->RampVariety) {
					return(tiberium->HeapID);
				}
			}

			DebugString("Overlay %s not really tiberium\n", otype->GivenName);
			return(TIBERIUM_RIPARIUS);
		}
	}
	return(TIBERIUM_NONE);
}

// FUNCTION: GAMEMD 0x005FDDE0
ObjectTypeClass const * OverlayClass::Class_Of(void) const
{
	return(Class);
}

// FUNCTION: GAMEMD 0x005FDDF0
void OverlayClass::Post_Read_Vein_Fixups(void)
{
	int index;
	Map.Reset_Iterator();
	CellClass *cellptr = Map.Iterate();

	DynamicVectorClass<CellClass *> solid_vein_cells;
	solid_vein_cells.Set_Growth_Step(1000);

	while (cellptr) {
		if (cellptr->Overlay == OVERLAY_VEINS) {
			if (cellptr->OverlayData >= OVERLAYDATA_FIRST_SOLID_VEIN) {
				solid_vein_cells.Add(cellptr);
			}
			cellptr->Overlay = OVERLAY_NONE;
			cellptr->OverlayData = 0;
		}
		cellptr = Map.Iterate();
	}

	for (index = solid_vein_cells.Count() - 1; index >= 0; index--) {
		CellClass *tmp = solid_vein_cells[index];
		if (tmp->Can_Place_Veins()) {
			tmp->Place_Veins();
		}
	}
}

// FUNCTION: GAMEMD 0x005FDF00
// OverlayClass::Fetch_Object_Size
// FUNCTION: GAMEMD 0x005FDF10
HRESULT STDMETHODCALLTYPE OverlayClass::GetClassID(CLSID * retval)
{
	if (retval == NULL) return(E_POINTER);
	*retval = CLSID_OverlayClass;
	return(S_OK);
}

// FUNCTION: GAMEMD 0x005FDF50
// OverlayClass::Fetch_RTTI
// TEMPLATE: GAMEMD 0x005FDF60
// ?Draw_It@OverlayClass@@UBEXABVPoint2D@@ABV?$TRect@H@@@Z

// FUNCTION: GAMEMD 0x005FE020
// XSurface::XSurface

