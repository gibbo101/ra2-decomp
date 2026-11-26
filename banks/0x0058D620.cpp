// 0x0058D620 76.7% raw, 95.7% normalised (4004/4012), no overlay beyond the header line below; every inlining decision matches the exe and the other 306 functions of mapgen.cpp stay MATCH.
// Splice into src/mapgen.cpp: replace Get_Region 0x0058D0A0, Increase_Height 0x0058D5A0 and Grow_Water_Region 0x005A08D0 with the bodies below, insert Split_Region after Increase_Height, add 1 to every _$E marker numbered 205 or more (main of 26 Nov), drop the now-unused forward declaration of Delete_Forest_Search above Grow_Water_Region, and declare `void Increase_Height(int cell_height);` in include/mapgen.h.
// The splice flips Generate_Urban_Buildings 0x005A6510 to 86.8% by declaration weight; moving `extern bool DAT_0082B030;`, `extern const double DAT_007ED898;` and `extern const double DAT_007ED990;` from their late spots into the file's top extern block (after `extern int DAT_00ABECD0;`) restores it with the other 306 functions still MATCH (needs 3 or more weight above Split_Region's marker; weight added after about line 1600 does nothing).
// Over the 95.6% bank: `int i = 0;` with heap_size folded into the new[] gives the exe's cells-loop bottom (this reloaded before i). Left: the new_regions loop top (exe reloads i past a jmp, region in eax; ours keeps i in eax) and frame slot order.
// Landing blocker beyond this function: Grow_Water_Region below drops Delete_Forest_Search, leaving that helper one caller (Place_Forest); Place_Forest without it scores 98.8%.
// FUNCTION: GAMEMD 0x0058D0A0
int MapRegionClass::CellData::Get_Region(Cell const &cell)
{
	if (RMGCellData == NULL) {
		return(-1);
	}
	return(RMGCellData[cell.X + cell.Y * (int)MapCellStride].RegionID);
}

// FUNCTION: GAMEMD 0x0058D5A0
void MapRegionClass::Increase_Height(int cell_height)
{
	Map.Reset_Iterator();
	CellClass *cptr = Map.Iterate();
	while (cptr != NULL) {
		int rid = CellData::Get_Region(cptr->Fetch_CellID());
		if (rid == ID) {
			cptr->Height += cell_height;
		}
		cptr = Map.Iterate();
	}
}

// FUNCTION: GAMEMD 0x0058D620
bool MapRegionClass::Split_Region(void)
{
	int i = 0;
	int j;
	MapRegionClass * region;

	if (Unsplittable) {
		return(false);
	}

	CellNode * nodes = new CellNode[(2 * CellCount) + 10];
	PriorityQueueClass<CellNode> * queue = new PriorityQueueClass<CellNode>((2 * CellCount) + 10);
	queue->Clear();

	CellData::Reset_Spreads(false);

	int spread_count = 0;
	for (i = Cells.Count() - 1; i >= 0; i--) {
		MapRegionClass::Get_Cell_Data(Cells[i]).RegionID = -2;
	}

	unsigned int target_count = Pick_Random_UInt(CellCount / 8, CellCount / 3);
	unsigned int seed_index = Pick_Random_UInt(0, Cells.Count() - 1);

	int node_count = 1;
	Cell seed = Cells[seed_index];
	nodes[0].Element = seed;
	nodes[0].Score = 0.0f;
	MapRegionClass::Get_Cell_Data(seed).SpreadID = -3;
	queue->Insert(nodes[0]);

	CellNode * node = queue->Extract_Min();
	double dist = (unsigned int)RMGRandom() * (DEG_TO_RAD(360) / UINT_MAX);

	while (node != NULL) {
		if (spread_count >= (int)target_count) {
			break;
		}

		CellData::Set_Region(node->Element, -3);

		int cell_index = node->Element.X + MapCellStride * node->Element.Y;
		CellNode * newnode = &nodes[node_count];

		for (FacingType dir = FACING_FIRST; dir < FACING_COUNT; dir = (FacingType)(dir + FACING_90)) {
			Cell ncell = Adjacent_Cell(node->Element, dir);
			if (My_In_Radar(ncell)) {
				if (CellData::Get_Region(ncell) == -2 && CellData::Get_Spread(ncell) != -3 && Map[ncell].Is_Tile_Clear()) {
					newnode->Element = ncell;
					newnode->Score = Get_Angle_Score(seed, ncell, dist);
					MapRegionClass::Get_Cell_Data(cell_index + AStarFacingToOffset[dir]).SpreadID = -3;
					node_count++;
					queue->Insert(*newnode++);
				}
			}
		}

		spread_count++;
		dist += Sample_Normal(0, DEG_TO_RAD(22.5));
		node = queue->Extract_Min();
	}

	CellData::Reset_Spreads(false);

	CellNode * remaining = queue->Extract_Min();
	DynamicVectorClass<Cell> ncells;

	while (remaining != NULL) {
		Cell c = remaining->Element;
		MapRegionClass::Get_Cell_Data(c).RegionID = -3;
		remaining = queue->Extract_Min();
	}

	delete[] nodes;
	delete queue;

	DynamicVectorClass<MapRegionClass *> new_regions;
	for (i = Cells.Count() - 1; i >= 0; i--) {
		Cell cell = Cells[i];
		int cell_index = cell.X + MapCellStride * cell.Y;
		if (MapRegionClass::Get_Cell_Data(cell_index).RegionID < -1) {
			int old_id = MapRegionClass::Get_Cell_Data(cell_index).RegionID;

			region = new MapRegionClass(cell);
			region->Cells.Add(cell);
			region->CellCount++;
			new_regions.Add(region);
			MapRegionClass::Get_Cell_Data(cell_index).RegionID = region->ID;

			DynamicVectorClass<Cell> open;
			open.Set_Growth_Step(100000);
			open.Add(cell);

			while (open.Count() != 0) {
				int index = open.Count() - 1;
				Cell c = open[index];
				open.Delete_Index(index);

				for (j = 0; j < FACING_COUNT; j++) {
					Cell ncell = Adjacent_Cell(c, (FacingType)j);
					if (My_In_Radar(ncell)) {
						CellData & ndata = MapRegionClass::Get_Cell_Data(ncell);
						if (ndata.RegionID == old_id) {
							region->Cells.Add(ncell);
							region->CellCount++;
							ndata.RegionID = region->ID;
							open.Add(ncell);
						}
					}
				}
			}
		}
	}

	for (i = new_regions.Count() - 1; i >= 0; i--) {
		region = new_regions[i];
		if (region->Discardable) {
			continue;
		}

		DynamicVectorClass<Cell> * cells = region->Build_Border_Cell_List2();
		bool * adjacent = new bool[MapRegionClass::TotalCount];
		for (j = 0; j < MapRegionClass::TotalCount; j++) {
			adjacent[j] = false;
		}

		for (j = cells->Count() - 1; j >= 0; j--) {
			Cell c = (*cells)[j];
			for (int m = 0; m < FACING_COUNT; m++) {
				Cell ncell = Adjacent_Cell(c, (FacingType)m);
				if (My_In_Radar(ncell)) {
					int id = MapRegionClass::Get_Cell_Data(ncell).RegionID;
					if (id >= 0 && id != region->ID) {
						adjacent[id] = true;
					}
				}
			}
		}

		delete cells;

		MapRegionClass * merge_target = NULL;
		int merge_height = -1;
		int max_count = -1;
		int min_height = -1;
		int max_height = -1;

		for (j = 0; j < MapRegionClass::TotalCount; j++) {
			if (adjacent[j]) {
				MapRegionClass * other = MapRegionClass::From_ID(j);

				if (!other->Discardable) {
					if (other->CellCount > max_count && !other->ContainsWater) {
						max_count = other->CellCount;
						merge_target = other;
						merge_height = other->CellHeight;
					}

					if (min_height == -1) {
						min_height = other->CellHeight;
						max_height = other->CellHeight;
					} else {
						if (other->CellHeight > max_height) {
							max_height = other->CellHeight;
						}
						if (other->CellHeight < min_height) {
							min_height = other->CellHeight;
						}
					}
				}
			}
		}

		DynamicVectorClass<int> heights;
		int diff = max_height - min_height;
		if (diff != 0) {
			if (diff != 4) {
				if (diff == 8) {
					heights.Add((max_height + min_height) / 2);
				}
			} else {
				heights.Add(max_height);
				heights.Add(min_height);
			}
		} else {
			if (min_height >= 4) {
				heights.Add(min_height - 4);
			}
			if (max_height <= 7) {
				heights.Add(max_height + 4);
			}
		}

		if (FUN_0058BD20(RandomMapGen.SeedData.MapType)) {
			for (j = heights.Count() - 1; j >= 0; j--) {
				if (heights[j] == 0) {
					heights.Delete_Index(j);
				}
			}
		}

		delete[] adjacent;

		if (region->CellCount > 100) {
			int old_height = region->CellHeight;
			unsigned int max_index = (unsigned int)heights.Count() - 1;
			unsigned int index = Pick_Random_UInt(0, max_index);

			region->CellHeight = heights[index];
			int height_diff = region->CellHeight - old_height;
			region->Increase_Height(height_diff);
		} else {
			if (merge_target != NULL) {
				int old_height = region->CellHeight;
				region->CellHeight = merge_height;
				int height_diff = region->CellHeight - old_height;
				region->Increase_Height(height_diff);

				for (j = region->Cells.Count() - 1; j >= 0; j--) {
					Cell c = region->Cells[j];
					merge_target->Cells.Add(c);
					merge_target->CellCount++;
					MapRegionClass::Get_Cell_Data(c).RegionID = merge_target->ID;
				}

				region->Discardable = true;
			}
		}
	}

	for (i = MapRegionClass::MapRegions.Count() - 1; i >= 0; i--) {
		region = MapRegionClass::MapRegions[i];
		if (region != this && region->Discardable) {
			delete MapRegionClass::MapRegions[i];
		}
	}

	SplitCount++;
	return(true);
}

// FUNCTION: GAMEMD 0x005A08D0
bool MapGeneratorClass::Grow_Water_Region(int region_id, float spread_scale, Rect const & bounds, Cell const & origin, bool claim_frontier)
{
	CellNode * nodes = new CellNode[max(2 * LocalHeight * LocalWidth, 100)];
	PriorityQueueClass<CellNode> * queue = new PriorityQueueClass<CellNode>(max(2 * LocalHeight * LocalWidth, 100));

	int marker_index = 0;
	bool result_flag = true;

	double angle = 0.0;
	if (bounds.X != 0) {
		angle = 0.0;
	} else if (bounds.Width != MAP_CELL_W) {
		angle = M_PI;
	}
	if (bounds.Y != 0) {
		angle = 3 * M_PI_2;
	} else if (bounds.Height != MAP_CELL_H) {
		angle = M_PI_2;
	}

	Clear_Cell_Data_Spreads();

	DynamicVectorClass<Cell> * seed_cells = Build_Region_Border_Cell_List(region_id);
	int seed_count = seed_cells->Count();

	for (int seed_index = 0; seed_index < seed_count; seed_index++) {
		Cell seed = (*seed_cells)[seed_index];
		if (bounds.Is_Point_Within(Point2D(seed.X, seed.Y))) {

			nodes[marker_index].Element = seed;
			nodes[marker_index].Score = Get_Angle_Score(origin, seed, angle);
			queue->Insert(nodes[marker_index++]);
		}
	}

	delete seed_cells;

	double scale = log((double)seed_count);
	scale = max(1.0, scale);

	int spread_count = (int)(1.0f / ((float)(1.0 / scale) * spread_scale) * 0.5f);
	spread_count += Pick_Random_UInt(0, spread_count / 2);

	CellNode * node = queue->Extract_Min();

	int spread_step = 0;
	while (spread_step < spread_count && result_flag && node != NULL) {

		MapRegionClass::CellData & ndata = MapRegionClass::Get_Cell_Data(node->Element);
		if (!ndata.RegionID) {
			if (Map[node->Element].Is_Tile_Clear()) {
				ndata.CanExpand = true;
				ndata.RegionID = region_id;
			}
		}

		for (FacingType dir = FACING_FIRST; dir < FACING_COUNT; dir = FacingType(dir + FACING_90)) {
			Cell newcell = Adjacent_Cell(node->Element, dir);
			if (My_In_Radar(newcell)) {
				if (MapRegionClass::CellData::Get_Region(newcell) == 0 &&
					MapRegionClass::Get_Cell_Data(newcell).SpreadID != region_id &&
					newcell.X >= bounds.X && newcell.X < bounds.X + bounds.Width && newcell.Y >= bounds.Y && newcell.Y < bounds.Y + bounds.Height &&
					Map[newcell].Is_Tile_Clear()) {
					nodes[marker_index].Element = newcell;
					nodes[marker_index].Score = Get_Angle_Score(origin, newcell, angle);
					Set_Cell_Data_Spread(newcell, region_id);
					queue->Insert(nodes[marker_index++]);
				} else {
					if (MapRegionClass::CellData::Get_Region(newcell) != 0 && MapRegionClass::CellData::Get_Region(newcell) != region_id) {
						result_flag = false;
					}
				}
			}
		}

		spread_step++;
		angle += Sample_Normal(0, M_PI_4);
		node = queue->Extract_Min();
	}

	if (claim_frontier) {
		for (CellNode * remaining = queue->Extract_Min(); remaining != NULL && result_flag; remaining = queue->Extract_Min()) {

			if (MapRegionClass::CellData::Get_Region(remaining->Element) == 0) {
				MapRegionClass::Get_Cell_Data(remaining->Element).RegionID = region_id;
			} else if (MapRegionClass::CellData::Get_Region(remaining->Element) != region_id) {
				result_flag = false;
			}

		}
	}

	delete[] nodes;
	delete queue;

	return(result_flag);
}
