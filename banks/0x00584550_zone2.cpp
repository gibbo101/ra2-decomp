// best 60.1% raw (71.1% normalised, 2290/2290 bytes), no overlay; 0x00584550_zone.cpp with the final pass written through Subzone_Of (same score); insert before 0x00584E50 in src/map.cpp, which now defines Subzone_Of once above 0x00581140. Zone_Of is single-use (does not land as is). inlinebudget: no differing decisions; flipsweep local/call/expr 0..6 at start, after size, before basex: no flip; dropping track 49.2, track in pass 2 53.8, basey first 55.7, set before collected 60.0; permute.py 22 min best raw 57.4.
static inline CellZoneStruct * Zone_Of(MapClass * map, Cell const & cell)
{
	return(&map->CellZones[map->Get_Cell_Zone_Index(cell)]);
}

// FUNCTION: GAMEMD 0x00584550
void MapClass::Update_Cell_Subzones(Cell const & cell)
{
	if (In_Local_Radar(cell)) {

		int y;
		int x;
		for (int subzone = 2; subzone >= 0; subzone--) {
			int size = 1 << (subzone + 1);
			Rect bounds(cell.X - cell.X % size, cell.Y - cell.Y % size, size, size);
			DynamicVectorClass<unsigned short> collected;
			SUBZONE_CONNECTION_HASH_SET * set = SubzoneConnectionHashTable[subzone];
			for (int bucket = 0; bucket < set->NumBuckets; bucket++) {
				set->Buckets[bucket].Clear();
			}

			DynamicVectorClass<SubzoneTrackingStruct> * track = &SubzoneTracking[subzone];
			for (y = bounds.Y; y < bounds.Y + bounds.Height; y++) {
				for (x = bounds.X; x < bounds.Width + bounds.X; x++) {
					CellSubzoneStruct * subptr = Subzone_Of(this, Cell(x, y));
					unsigned short subid = subptr->SubzoneID[subzone];
					if (subid != 0) {
						int found;
						for (found = collected.Count() - 1; found >= 0; found--) {
							if (collected[found] == subid) {
								break;
							}
						}
						if (found == -1) {
							collected.Add(subid);
						}
					}
					subptr->SubzoneID[subzone] = 0;
					subptr->ZoneID = Zone_Of(this, Cell(x, y))->ZoneID;
				}
			}
			for (int idx = collected.Count() - 1; idx >= 0; idx--) {
				unsigned short subid = collected[idx];
				SubzoneTrackingStruct * entry = &SubzoneTracking[subzone][subid];
				for (int i = entry->Connections.Count() - 1; i >= 0; i--) {
					SubzoneTrackingStruct * neighbor = &SubzoneTracking[subzone][entry->Connections[i].SubzoneID];
					int j;
					for (j = neighbor->Connections.Count() - 1; j >= 0; j--) {
						if (neighbor->Connections[j].SubzoneID == subid) {
							neighbor->Connections.Delete_Index(j);
							break;
						}
					}
				}
				entry->Connections.Clear();
			}
			int entry_count = SubzoneTrackingEntryCount[subzone];
			for (y = bounds.Y; y < bounds.Y + bounds.Height; y++) {
				for (x = bounds.X; x < bounds.Width + bounds.X; x++) {

					if (In_Local_Radar(Cell(x, y))) {

						CellSubzoneStruct * subptr = Subzone_Of(this, Cell(x, y));
						int passability = Zone_Of(this, Cell(x, y))->Passability;
						if (passability != PASSABLE_OUTSIDE && subptr->SubzoneID[subzone] == 0) {

							Subzone_Span(subptr, subzone, entry_count, bounds, Cell(x, y));

							track->Add(SubzoneTrackingStruct());

							SubzoneTrackingStruct * added = &(*track)[entry_count];
							added->ParentSubzoneID = (subzone == 2) ? 0 : subptr->SubzoneID[subzone + 1];
							added->Passability = (PassabilityType)passability;
							added->Connections.Set_Growth_Step(16);
							added->ThreatRegion = Cell_Region(Cell(x, y));

							entry_count++;
							if (entry_count == 0) {
								Reset_All_Subzones();
								return;
							}
						}
					}
				}
			}
			SubzoneTrackingEntryCount[subzone] = entry_count;
			for (int connection_index = ZoneConnections.Count() - 1; connection_index >= 0; connection_index--) {
				ZoneConnectionClass & connection = ZoneConnections[connection_index];
				if ((connection.From.X >= bounds.X && connection.From.X < bounds.Width + bounds.X
					&& connection.From.Y >= bounds.Y && connection.From.Y < bounds.Y + bounds.Height)
					|| (connection.To.X >= bounds.X && connection.To.X < bounds.Width + bounds.X
					&& connection.To.Y >= bounds.Y && connection.To.Y < bounds.Y + bounds.Height)) {
					if (connection.IsPassable) {
						Register_Zone_Connection_Entries(connection, subzone);
					}
				}
			}
			for (int bucket_index = 0; bucket_index < 256; bucket_index++) {
				if (set->Buckets[bucket_index].Count() > 0) {
					SUBZONE_CONNECTION_HASH_SET::ObjectType * object = &set->Buckets[bucket_index][0];
					for (int index = set->Buckets[bucket_index].Count(); index > 0; index--) {
						unsigned packed = object->Value.SubzoneID;
						WORD low = LOWORD(packed);
						WORD high = HIWORD(packed);
						bool costly = object->Value.IsCrossBlock;

						SubzoneConnectionStruct conn;
						conn.SubzoneID = high;
						conn.IsCrossBlock = costly;
						(*track)[low].Connections.Add(conn);

						SubzoneConnectionStruct conn2;
						conn2.SubzoneID = low;
						conn2.IsCrossBlock = costly;
						(*track)[high].Connections.Add(conn2);

						object++;
					}
				}
			}
		}
		int basex = cell.X - cell.X % 8;
		int basey = cell.Y - cell.Y % 8;
		for (y = basey; y < basey + 8; y++) {
			for (x = basex; x < basex + 8; x++) {
				if (In_Local_Radar(Cell(x, y))) {
					CellSubzoneStruct * subptr = Subzone_Of(this, Cell(x, y));
					for (int level = 0; level < 2; level++) {
						SubzoneTracking[level][subptr->SubzoneID[level]].ParentSubzoneID = subptr->SubzoneID[level + 1];
					}
				}
			}
		}
		Search.Reset();
	}
}


