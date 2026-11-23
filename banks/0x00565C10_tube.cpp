// 99.7% raw, 99.6% normalised (5374/5374) with the overlay 0x00565C10_tube.h (tube.h plus an inline TubeClass::Shift holding both +=; the member name is a guess); replace the unmarked Set_Map_Dimensions in src/map.cpp, 0x00588C40 and 0x00588C10 MATCH beside it. Same form as 0x00565C10.cpp with the file-local Shift_Tube turned into the member. Left: the occupy-list loop keeps list->X in dx where the exe has cx (cell + *list, *list + cell, no ncell local and Get_Cell() all give the same 14 diff lines); a static member holding the whole loop gives 88.5%, a by-value parameter 91.6%
// FUNCTION: GAMEMD 0x00565C10
void MapClass::Set_Map_Dimensions(Rect const & rect, bool reset_cells, int cell_height, bool refresh_map)
{
	int i = 0;

	int w = PlayRect.Width;
	CellClass *cptr;
	CellClass *cells = NULL;
	CellClass *cells_base = NULL;
	int h = 2 * Map.PlayRect.Height + 8;

	Cell cell(CELL_NONE);

	for (i = 0; i < Objects.Count(); i++) {
		Objects[i]->Mark(MARK_UP);
	}

	for (i = 0; i < Array.Length(); i++) {
		if (!In_Radar(Cell(i % MAP_CELL_W, i / MAP_CELL_H))) {
			CellClass *c = Array[i];
			if (c != NULL) {
				c->Height = cell_height;
			}
		}
	}

	int cellcount = 0;

	if (!reset_cells) {
		cells = (CellClass *) new char[sizeof(CellClass) * w * h];
		cells_base = cells;
		Reset_Iterator();
		cptr = Iterate();
		cell = cptr->CellID;
		while (cptr != NULL) {
			cells[cellcount] = *cptr;
			cells[cellcount].BridgeDeckCell = NULL;
			cells[cellcount].UnusedCell = NULL;
			cptr = Iterate();
			cellcount++;
		}
	}

	PlayRect = rect;
	PlayRect.X = 0;
	PlayRect.Y = 0;

	if (PlayRect.Width > 63 || PlayRect.Height > 63) {
		Map.Set_Radar_Scale(2);
	}

	MapRegionClass::MapStartDiagonal = Map.PlayRect.Width;
	MapRegionClass::MapEndDiagonal = Map.PlayRect.Width + 2 * Map.PlayRect.Height;

	MapRect.X = 1;
	MapRect.Y = 1;
	int msize = rect.Height + rect.Width - 1;
	MapRect.Width = msize;
	MapRect.Height = msize;

	for (int y = 0; y < 2 * MapRect.Height + 2; y++) {
		for (int x = 0; x < MapRect.Width + 2; x++) {
			Cell cell(x, y);
			if (In_Radar(cell)) {
				int idx = (MAP_CELL_H * y) + x;
				if (Array[idx] == NULL) {
					CellClass *cc = new CellClass;
					cc->Set_CellID(cell);
					Array[idx] = cc;
				} else {
					CellClass *c = new(Array[idx]) CellClass;
					c->Set_CellID(cell);
				}
				Array[idx]->Height = cell_height;
			}
		}
	}

	bool freshcalled = false;

	if (!reset_cells) {
		Reset_Iterator();
		Cell icell = Iterate()->Fetch_CellID();
		Cell offset = (icell - cell) - Cell(rect.Y + rect.X, rect.Y - rect.X);

		for (int j = 0; j < cellcount; j++) {
			cptr = &cells_base[j];
			Cell scell = cptr->CellID;
			Cell shifted = scell + offset;

			CellClass *c = &(*this)[shifted];

			Cell dcell = c->Fetch_CellID();
			if (c != NULL) {
				*c = *cptr;
				c->Set_CellID(dcell);
			}

		}

		Reset_Iterator();
		cptr = Iterate();
		while (cptr != NULL) {

			Cell c = cptr->Fetch_CellID();

			if (c.X + c.Y < PlayRect.Width - 2 * rect.Y + 1 ||
				c.Y - c.X > PlayRect.Width + 2 * rect.X - 1 ||
				c.X + c.Y > PlayRect.Width + 2 * (h + PlayRect.Height - rect.Y - Map.PlayRect.Height) ||
				c.X - c.Y > PlayRect.Width + 2 * (w - rect.X - Map.PlayRect.Width) - 1) {

				int cellindex = c.X + c.Y * MAP_CELL_W;

				delete Array[cellindex];
				Array[cellindex] = new CellClass;
				Array[cellindex]->Set_CellID(c);
				Array[cellindex]->ITType = ISOTILE_NONE;
				Array[cellindex]->SubTile = 0;
				Array[cellindex]->Ramp = 0;
				Array[cellindex]->Overlay = OVERLAY_NONE;
				Array[cellindex]->Height = cell_height;
			}
			cptr = Iterate();
		}

		for (i = 0; i < WAYPT_COUNT; i++) {
			if (Scen->Is_Valid_Waypoint(i)) {
				Scen->Set_Waypoint(i, Scen->Get_Waypoint_Cell(i) + offset);
			}
		}

		for (i = 0; i < Tubes.Count(); i++) {
			Tubes[i]->Shift(offset);
		}

		Coord coord = Coord(offset, 0) - Coord(CELL_LEPTON_W / 2, CELL_LEPTON_H / 2, 0);

		for (i = 0; i < Objects.Count(); i++) {
			ObjectClass *obj = Objects[i];
			if (obj->RTTI != RTTI_ANIM) {
				obj->Set_Coord(obj->Get_Coord() + coord);
			}
		}

		for (i = Houses.Count() - 1; i >= 0; i--) {
			Houses[i]->FUN_0050D250(offset);
		}

		for (i = 0; i < Anims.Count(); i++) {
			AnimClass *anim = Anims[i];
			if (anim->OwnerObject == NULL) {
				anim->Set_Coord(coord + anim->Get_Coord());
			}
		}

		ScenarioInit++;

		for (i = 0; i < Objects.Count(); i++) {
			if (Objects[i]->RTTI != RTTI_PARTICLESYSTEM || ((ParticleSystemClass *)Objects[i])->Class->Behaves_Like() != PSYS_BEHAVIOR_GAS) {
				Cell cell = Objects[i]->PositionCell;
				const Cell *list = Objects[i]->Occupy_List();
				while (*list != REFRESH_EOL) {
					Cell ncell = cell + *list;
					if (!In_Radar(ncell)) {
						Objects[i]->Limbo();
						delete Objects[i];
						i--;
						break;
					}
					list++;
				}
			}
		}

		ScenarioInit--;

		for (i = 0; i < Array.Length(); i++) {
			CellClass *c1 = Array[i];
			CellClass **c2 = &Array[i];
			if (c1 != NULL) {
				if (!In_Radar(Cell(i % MAP_CELL_W, i / MAP_CELL_H))) {
					*c2 = NULL;
					delete c1;
				}
			}
		}

		Reset_Iterator();
		cptr = Iterate();
		while (cptr != NULL) {
			if (cptr->IsBridgeDeck && cptr->BridgeDeckCell == NULL) {
				FacingType facing = cptr->IsBridgeEastWest ? FACING_N : FACING_E;
				if (cptr->Overlay == OVERLAY_BRIDGE1 || cptr->Overlay == OVERLAY_BRIDGE2) {
					cptr->Set_Under_Bridge(facing);
				} else {
					cptr->Set_Under_Rail_Bridge(facing);
				}
			}
			cptr = Iterate();
		}

		if (refresh_map) {
			Fresh_Map();
			freshcalled = true;
		}

		for (i = 0; i < Objects.Count(); i++) {
			Objects[i]->Mark(MARK_DOWN);
		}

		Map.Flag_To_Redraw(GS_REDRAW_ALL);
	}

	if (!freshcalled && refresh_map) {
		Fresh_Map();
	}

	new(&InvalidCell) CellClass;
	operator delete(cells);
}
