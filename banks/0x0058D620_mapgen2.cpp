// 0x0058D620 natural best 70.9% raw (4061/4012), no overlay; replace Increase_Height 0x0058D5A0 with these two functions and shift every _$E marker from 206 up by one; unchanged from 0x0058D620_mapgen.cpp.
// inlinebudget 23 Nov: 4 decisions differ; Build_Border_Cell_List2 (exe calls) and Get_Region in both inlined Increase_Height loops never flip with sites or spend, so not budget; operator+ in the border loop flips with 8-16 units spent before it.
// FUNCTION: GAMEMD 0x0058D5A0
void MapRegionClass::Increase_Height(char cell_height)
{
	Map.Reset_Iterator();
	CellClass *cptr = Map.Iterate();
	while (cptr != NULL) {
		Cell cell = cptr->Fetch_CellID();
		int rid = CellData::Get_Region(cell);
		if (rid == ID) {
			cptr->Height += cell_height;
		}
		cptr = Map.Iterate();
	}
}

// FUNCTION: GAMEMD 0x0058D620
bool MapRegionClass::Split_Region(void)
{
	int i, j;

	if (Unsplittable) {
		return(false);
	}

	int heap_size = (2 * CellCount) + 10;
	CellNode * nodes = new CellNode[heap_size];
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

			MapRegionClass * region = new MapRegionClass(cell);
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

				for (int m = 0; m < FACING_COUNT; m++) {
					Cell ncell = Adjacent_Cell(c, (FacingType)m);
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
		MapRegionClass * region = new_regions[i];
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
		int min_height = -1;
		int max_height = -1;
		int merge_height = -1;
		int max_count = -1;

		for (j = 0; j < MapRegionClass::TotalCount; j++) {
			if (adjacent[j]) {
				MapRegionClass * other = MapRegionClass::From_ID(j);

				if (!other->Discardable) {
					if (other->CellCount > max_count && !other->ContainsWater) {
						max_count = other->CellCount;
						merge_height = other->CellHeight;
						merge_target = other;
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

		if (RandomMapGen.SeedData.MapType == 3 || RandomMapGen.SeedData.MapType == 4) {
			for (int h = heights.Count() - 1; h >= 0; h--) {
				if (heights[h] == 0) {
					heights.Delete_Index(h);
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

				for (int j = region->Cells.Count() - 1; j >= 0; j--) {
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
		MapRegionClass * region = MapRegionClass::MapRegions[i];
		if (region != this && region->Discardable) {
			delete region;
		}
	}

	SplitCount++;
	return(true);
}
