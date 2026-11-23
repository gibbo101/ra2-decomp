// 0x0058C800 99.8% (1678/1678), whole file otherwise unchanged; side test through the file's existing In_Zone_Map_Bounds (two callers), Create_Seed_Region still single-use; only diff lea esi,[eax+ecx] vs [ecx+eax] in the first side test.
// Without Create_Seed_Region (written inline) it falls to 86.2%. Insert before 0x0058CE90.
// Adapted from RobinDigitalPro's RA2 1.001 level-2 reconstruction (reference/ra2-help, update 2026-11-23):
// SPDX-License-Identifier: GPL-3.0-or-later. Copyright 2025 Electronic Arts Inc. Copyright 2026 OpenTS contributors.
// Contains material derived from Electronic Arts source code; EA's GPLv3 Section 7 additional terms apply.

static inline MapRegionClass * Create_Seed_Region(CellClass * cptr, int cell_height, bool has_water)
{
	MapRegionClass * region = new MapRegionClass(cptr->Fetch_CellID());
	region->CellHeight = cell_height;
	region->ContainsWater = has_water;
	return(region);
}

// FUNCTION: GAMEMD 0x0058C800
MapRegionClass * MapRegionClass::Create_Region_From_Cell(CellClass * cptr)
{
	int i;
	int region_id = MapRegionClass::TotalCount;

	DynamicVectorClass<Cell> cells;
	cells.Set_Growth_Step(50000);
	cells.Add(cptr->Fetch_CellID());

	bool has_water = cptr->Is_Tile_With_Water() || cptr->Is_Tile_Clear_To_Green_LAT();
	int cell_height = cptr->Height;
	int processed_count = 0;

	int pending = cells.Count();
	while (pending) {
		Cell cell = cells[pending - 1];
		cells.Delete_Index(pending - 1);

		CellData::Set_Spread(cell, region_id);
		CellData::Set_Region(cell, region_id);

		Cell ncell;
		for (i = 0; i < FACING_COUNT; i++) {
			ncell = Adjacent_Cell(cell, (FacingType)i);
			CellData & ndata = MapRegionClass::Get_Cell_Data(ncell);
			if (My_In_Radar(ncell)) {
				if (ndata.RegionID == -1 && ndata.SpreadID != region_id) {
					ndata.SpreadID = region_id;
					if (Map[ncell].Height == cell_height) {
						int n_has_water = Map[ncell].Is_Tile_With_Water() || Map[ncell].Is_Tile_Clear_To_Green_LAT();
						if (!(has_water ^ n_has_water)) {
							cells.Add(ncell);
						}
					}
				}
			}
		}

		processed_count++;
		pending = cells.Count();
	}

	if (processed_count < 75 && !has_water) {
		bool reset_done = false;

		if (region_id == 0) {
			DynamicVectorClass<Cell> * region_cells = RandomMapGen.Build_Region_Border_Cell_List(0);
			Cell region_cell = (*region_cells)[Pick_Random_UInt(0, region_cells->Count() - 1)];

			Cell ncell;
			for (i = 0; i < FACING_COUNT; i++) {
				ncell = Adjacent_Cell(region_cell, (FacingType)i);
				if (My_In_Radar(ncell)) {
					if (Map[ncell].Height != Map[region_cell].Height && !Map[ncell].Is_Tile_With_Water()) {
						int height = Map[ncell].Height;

						RandomMapGen.Clear_Region_Cells(0, -1, height, ISOTILE_CLEAR);

						reset_done = true;
						break;
					}
				}
			}

			delete region_cells;
		} else {
			Cell side_cell = cptr->Fetch_CellID() - Cell(1, 0);
			if (!In_Zone_Map_Bounds(side_cell)) {
				side_cell = cptr->Fetch_CellID() - Cell(0, 1);
			}

			if (My_In_Radar(side_cell)) {
				int id = CellData::Get_Region(side_cell);
				int height = Map[side_cell].Height;

				RandomMapGen.Clear_Region_Cells(region_id, id, height, ISOTILE_CLEAR);

				reset_done = true;
			}
		}

		if (reset_done) {
			return(NULL);
		}
	}

	MapRegionClass * region = Create_Seed_Region(cptr, cell_height, has_water);
	region->Recount_Cells();
	return(region);
}
