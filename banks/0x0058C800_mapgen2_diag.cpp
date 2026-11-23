// 0x0058C800 DIAGNOSTIC, not landable: MATCH (1678/1678), whole file 307/307 otherwise unchanged; one dead call of a 40-term inline before `int processed_count = 0;`.
// The same probe at 40-48 terms matches anywhere before the small-region block; insert before 0x0058CE90.
static inline int Inline_Probe_S(int a) { return (a ^ 1) + (a ^ 2) + (a ^ 3) + (a ^ 4) + (a ^ 5) + (a ^ 6) + (a ^ 7) + (a ^ 8) + (a ^ 9) + (a ^ 10) + (a ^ 11) + (a ^ 12) + (a ^ 13) + (a ^ 14) + (a ^ 15) + (a ^ 16) + (a ^ 17) + (a ^ 18) + (a ^ 19) + (a ^ 20) + (a ^ 21) + (a ^ 22) + (a ^ 23) + (a ^ 24) + (a ^ 25) + (a ^ 26) + (a ^ 27) + (a ^ 28) + (a ^ 29) + (a ^ 30) + (a ^ 31) + (a ^ 32) + (a ^ 33) + (a ^ 34) + (a ^ 35) + (a ^ 36) + (a ^ 37) + (a ^ 38) + (a ^ 39) + (a ^ 40); }

// FUNCTION: GAMEMD 0x0058C800
MapRegionClass * MapRegionClass::Create_Region_From_Cell(CellClass * cell_ptr)
{
	int i;
	int region_id = MapRegionClass::TotalCount;

	DynamicVectorClass<Cell> cells;
	cells.Set_Growth_Step(50000);
	cells.Add(cell_ptr->Fetch_CellID());

	bool has_water =
		cell_ptr->Is_Tile_With_Water() ||
		cell_ptr->Is_Tile_Clear_To_Green_LAT();
	int cell_height = cell_ptr->Height;
	int spend0 = Inline_Probe_S(0);
	int processed_count = 0;

	for (int active = cells.Count(); active != 0; active = cells.Count()) {
		Cell cell = cells[active - 1];
		cells.Delete_Index(active - 1);

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
						int n_has_water =
							Map[ncell].Is_Tile_With_Water() ||
							Map[ncell].Is_Tile_Clear_To_Green_LAT();
						if (!(has_water ^ n_has_water)) {
							cells.Add(ncell);
						}
					}
				}
			}
		}
		processed_count++;
	}

	if (processed_count < 75 && !has_water) {
		bool reset_done = false;

		if (region_id == 0) {
			DynamicVectorClass<Cell> * region_cells =
				RandomMapGen.Build_Region_Border_Cell_List(0);
			Cell region_cell =
				(*region_cells)[Pick_Random_UInt(0, region_cells->Count() - 1)];

			Cell ncell;
			for (i = 0; i < FACING_COUNT; i++) {
				ncell = Adjacent_Cell(region_cell, (FacingType)i);
				if (My_In_Radar(ncell)) {
					if (Map[ncell].Height != Map[region_cell].Height &&
						!Map[ncell].Is_Tile_With_Water()) {
						RandomMapGen.Clear_Region_Cells(0, -1, Map[ncell].Height, ISOTILE_CLEAR);
						reset_done = true;
						break;
					}
				}
			}

			delete region_cells;
		} else {
			Cell side_cell;
			side_cell = cell_ptr->Fetch_CellID() + Cell(-1, 0);
			if (!My_In_Radar(side_cell)) {
				side_cell = cell_ptr->Fetch_CellID() + Cell(0, -1);
			}

			if (My_In_Radar(side_cell)) {
				int id = CellData::Get_Region(side_cell);
				int height = Map[side_cell].Height;
				RandomMapGen.Clear_Region_Cells(
					region_id, id, height, ISOTILE_CLEAR);
				reset_done = true;
			}
		}

		if (reset_done) {
			return 0;
		}
	}

	MapRegionClass * region = new MapRegionClass(cell_ptr->Fetch_CellID());
	region->CellHeight = cell_height;
	region->ContainsWater = has_water;
	region->Recount_Cells();
	return region;
}
