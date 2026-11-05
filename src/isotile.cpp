// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#define INCLUDE_COM
#include "always.h"
#include "geometry.h"
#include "isotile.h"
#include "_isotile.h"
#include "_map.h"
#include "anim.h"
#include "cell.h"
#include "globals.h"
#include "isotype.h"
#include "overtype.h"
#include "swizzle.h"
#include "tracker.h"
#include "vector.h"

// SYNTHETIC: GAMEMD 0x00543120 SYMBOL
// _$E2
// SYNTHETIC: GAMEMD 0x00543150 SYMBOL
// _$E5
// SYNTHETIC: GAMEMD 0x00543170 SYMBOL
// _$E8
// SYNTHETIC: GAMEMD 0x00543190 SYMBOL
// _$E11
// SYNTHETIC: GAMEMD 0x005431B0 SYMBOL
// _$E14
// SYNTHETIC: GAMEMD 0x005431D0 SYMBOL
// _$E17
// SYNTHETIC: GAMEMD 0x005431F0 SYMBOL
// _$E20
// SYNTHETIC: GAMEMD 0x00543230 SYMBOL
// _$E23
// SYNTHETIC: GAMEMD 0x00543260 SYMBOL
// _$E26
// SYNTHETIC: GAMEMD 0x00543290 SYMBOL
// _$E29
// SYNTHETIC: GAMEMD 0x005432C0 SYMBOL
// _$E32
// SYNTHETIC: GAMEMD 0x005432D0 SYMBOL
// _$E35
// SYNTHETIC: GAMEMD 0x005432F0 SYMBOL
// _$E38
// SYNTHETIC: GAMEMD 0x00543310 SYMBOL
// _$E47

// FUNCTION: GAMEMD 0x00543330
bool IsometricTileClass::Mark(MarkType mark)
{
	IsoTileSet * set = (IsoTileSet *)Get_Image_Data();
	if (set != NULL && BASECLASS::Mark(mark)) {

		for (int y = 0; y < Class->Height; y++) {
			for (int x = 0; x < Class->Width; x++) {
				Cell cell = PositionCell + Cell(x, y);

				if (Map.In_Radar(cell)) {
					CellClass *cptr = &Map[cell];
					int subtile = Class->SubTile_Index(x, y);
					if (set->Tiles[subtile] != NULL) {
						if (mark == MARK_UP) {
							if (cptr->ITType == Class->HeapID && cptr->SubTile == subtile) {
								cptr->ITType = TILE_NONE;
								cptr->SubTile = 0;
								cptr->Height -= set->Tiles[subtile]->Height;
							}
						} else if (mark == MARK_DOWN || mark == MARK_DOWN_FORCED) {
							const IsometricTileType tile_type = Class->HeapID;
							if (tile_type == TILE_CLEAR) {
								cptr->ITType = TILE_NONE;
								cptr->SubTile = 0;
							} else {
								cptr->ITType = tile_type;
								cptr->SubTile = subtile;
								cptr->Ramp = Class->Ramp_Type(subtile);
								if (cptr->ITType == IsometricTileTypeClass::SlopeSetPieces + 5) {
									switch (cptr->SubTile) {
										case 0:
										case 3:
										case 6:
										case 9:
											cptr->ITType = IsometricTileType(IsometricTileTypeClass::RampStart + 1);
											cptr->SubTile = 0;
											break;
										default:
											break;
									}
								}
								if (cptr->ITType == IsometricTileTypeClass::SlopeSetPieces2 + 5) {
									switch (cptr->SubTile) {
										case 0:
										case 3:
										case 6:
										case 9:
											cptr->ITType = IsometricTileType(IsometricTileTypeClass::MMRampBase + 1);
											cptr->SubTile = 0;
											break;
										default:
											break;
									}
								}
								if (cptr->ITType == IsometricTileTypeClass::SlopeSetPieces + 8 && cptr->SubTile <= 3) {
									cptr->ITType = IsometricTileTypeClass::RampStart;
									cptr->SubTile = 0;
								}
								if (cptr->ITType == IsometricTileTypeClass::SlopeSetPieces2 + 8 && cptr->SubTile <= 3) {
									cptr->ITType = IsometricTileTypeClass::MMRampBase;
									cptr->SubTile = 0;
								}
							}
							cptr->Smudge = SMUDGE_NONE;
							cptr->SmudgeData = 0;
							const OverlayType overlay = cptr->Overlay;
							if (overlay >= OVERLAY_LARGE_TIBERIUM01 && overlay < OVERLAY_LARGE_TIBERIUM12) {
								cptr->IsAnimAttached = false;
								Coord ccoord = cptr->Center_Coord() + Coord(CELL_LEPTON_W + (CELL_LEPTON_W >> 1), CELL_LEPTON_H + (CELL_LEPTON_H >> 1), 0);
								for (int i = 0; i < Anims.Count(); i++) {
									AnimClass *anim = Anims[i];
									if (anim->Type == OverlayTypes[cptr->Overlay]->CellAnim) {
										const Coord position = anim->PositionCoord;
										if (position == ccoord) {
											delete anim;
											break;
										}
									}
								}
							}
							cptr->Overlay = OVERLAY_NONE;
							cptr->OverlayData = 0;
							cptr->Height += set->Tiles[subtile]->Height;
							cptr->Fixup_LAT();
							cptr->Adjacent_Cell(FACING_N).Fixup_LAT();
							cptr->Adjacent_Cell(FACING_E).Fixup_LAT();
							cptr->Adjacent_Cell(FACING_S).Fixup_LAT();
							cptr->Adjacent_Cell(FACING_W).Fixup_LAT();
						}
						cptr->Recalc_Attributes();
					}
				}
			}
		}

		if (mark == MARK_DOWN || mark == MARK_DOWN_FORCED) {
			IsDown = false;
			delete this;
		}
		return(true);
	}
	return(false);
}

// FUNCTION: GAMEMD 0x00543780
IsometricTileClass::IsometricTileClass(IsometricTileType type, Cell const & cell) :
	BASECLASS(),
	Class(IsometricTileTypes[type])
{
	if (cell != CELL_NONE) {
		Unlimbo(Coord(cell));
	}

	IsometricTiles.Add(this);
}

// FUNCTION: GAMEMD 0x00543880
IsometricTileClass::~IsometricTileClass(void)
{
	Detach_This_From_All(this, true);
	Limbo();
	Class = NULL;
	IsometricTiles.Delete(this);
	AbstractTypePtrTracker.Delete(this);
}

// SYNTHETIC: GAMEMD 0x00543B10
// ??_GIsometricTileClass@@UAEPAXI@Z

// FUNCTION: GAMEMD 0x00543990
HRESULT STDMETHODCALLTYPE IsometricTileClass::Load(IStream * stream)
{
	HRESULT result = BASECLASS::Load(stream);
	if (SUCCEEDED(result)) {
		new (this) IsometricTileClass(NoInitClass());

		Swizzle_Pointer(&Class);
	}
	return(result);
}

// FUNCTION: GAMEMD 0x005439F0
HRESULT STDMETHODCALLTYPE IsometricTileClass::Save(IStream * stream, BOOL cleardirty)
{
	HRESULT result = BASECLASS::Save(stream, cleardirty);
	if (SUCCEEDED(result)) {
		result = S_OK;
	}
	return(result);
}

// FUNCTION: GAMEMD 0x00543A10
bool IsometricTileClass::Unlimbo(Coord const & coord, Dir256 dir)
{
	IsInLimbo = false;
	PositionCoord = coord;
	return(Mark(MARK_DOWN));
}

// FUNCTION: GAMEMD 0x00543A40
bool IsometricTileClass::Limbo(void)
{
	if (!GameActive || IsInLimbo) {
		return(false);
	}
	Unselect();
	Detach_All(true);
	Mark(MARK_UP);
	IsInLimbo = true;
	IsToDisplay = false;
	return(true);
}

// FUNCTION: GAMEMD 0x00543A90
int IsometricTileClass::Fetch_Object_Size(void) const
{
	return(sizeof(*this));
}

// FUNCTION: GAMEMD 0x00543AA0
RTTIType IsometricTileClass::Fetch_RTTI(void) const
{
	return(RTTI_ISOTILE);
}

// FUNCTION: GAMEMD 0x00543AB0
HRESULT STDMETHODCALLTYPE IsometricTileClass::GetClassID(CLSID * retval)
{
	if (retval == NULL) return(E_POINTER);
	*retval = CLSID_IsometricTileClass;
	return(S_OK);
}

// FUNCTION: GAMEMD 0x00543AF0
ObjectTypeClass const * IsometricTileClass::Class_Of(void) const
{
	return(Class);
}

// FUNCTION: GAMEMD 0x00543B00
void IsometricTileClass::Draw_It(Point2D const & point, Rect const & cliprect) const
{
}
