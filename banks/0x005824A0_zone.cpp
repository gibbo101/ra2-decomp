// best 40.6% raw (55.0% normalised, 2194/2184 bytes), no overlay, body unchanged from the near_map2 bank; insert before 0x00582D30 in src/map.cpp. Left: registers rotated from the prologue (exe: end esi, x edi, begin ebx; ours: begin esi, end edi, x ebx) and the frame slot order. A store sweep 0..7 at the start of the body shows no threshold (693 differing lines at 0, 699 to 707 at 1..7).
static unsigned Zone_Pack32(int zone1, int zone2)
{
	return(zone2 | (zone1 << 16));
}

static unsigned Zone_Pack8(int zone1, int zone2)
{
	return(zone2 & 0xF | ((zone1 & 0xF) << 4));
}

// FUNCTION: GAMEMD 0x005824A0
int MapClass::Subzone_Span(CellSubzoneStruct * seed, int subzone_level, int subzone_id, Rect const & bounds, Cell const & cell)
{
	int zone = seed->ZoneID;

	CellSubzoneStruct * begin = seed;
	CellSubzoneStruct * end = seed;
	SUBZONE_CONNECTION_HASH_SET::ObjectType entry;

	int x = cell.X;
	int y = cell.Y;

	int xmin = bounds.X;
	int ymin = bounds.Y;
	int last_adjacent = -1;
	int begin_x = x;
	int xmax = bounds.Width + bounds.X - 1;
	int end_x = x;
	int ymax = bounds.Height + bounds.Y - 1;

	SUBZONE_CONNECTION_HASH_SET * table = SubzoneConnectionHashTable[subzone_level];
	int prev_height = seed->Height;
	while (true) {
		if (x < xmin) {
			break;
		}
		if (abs(begin->Height - prev_height) >= 2) {
			break;
		}
		begin->SubzoneID[subzone_level] = (short)subzone_id;
		prev_height = begin->Height;
		int prev_zone = begin[-1].ZoneID;
		begin--;
		x--;
		if (prev_zone != zone) {
			break;
		}
	}

	begin_x = x;
	int begin_subzone = begin->SubzoneID[subzone_level];
	if (begin_subzone > 0 && abs(begin->Height - prev_height) < 2 && begin_subzone != last_adjacent) {
		Cell probe;
		probe = Cell(x, y);
		if (In_Local_Radar(probe, true)) {
			probe = Cell(x + 1, y);
			if (In_Local_Radar(probe, true)) {
				int packed_pair = Zone_Pack32(begin_subzone, (unsigned short)subzone_id);
				entry.Value.IsCrossBlock = false;
				entry.Key = entry.Value.SubzoneID = packed_pair;
				table->Add_Object(Zone_Pack8(begin_subzone, (unsigned short)subzone_id), entry);
				last_adjacent = begin_subzone;
			}
		}
	}
	prev_height = seed->Height;
	while (end->ZoneID == zone) {
		if (end_x > xmax) {
			break;
		}
		if (abs(end->Height - prev_height) >= 2) {
			break;
		}
		end->SubzoneID[subzone_level] = (short)subzone_id;
		prev_height = end->Height;
		end++;
		end_x++;
	}

	int end_subzone = end->SubzoneID[subzone_level];
	if (end_subzone > 0 && abs(end->Height - prev_height) < 2 && end_subzone != last_adjacent) {
		Cell probe;
		probe.X = end_x;
		probe.Y = y;
		if (In_Local_Radar(probe, true)) {
			probe = Cell(end_x - 1, y);
			if (In_Local_Radar(probe, true)) {
				int packed_pair = Zone_Pack32(end_subzone, (unsigned short)subzone_id);
				entry.Value.IsCrossBlock = false;
				entry.Key = entry.Value.SubzoneID = packed_pair;
				table->Add_Object(Zone_Pack8(end_subzone, (unsigned short)subzone_id), entry);
				last_adjacent = end_subzone;
			}
		}
	}

	int skip = end - seed - 1;
	int stride = PlayRect.Width + PlayRect.Height + 1;

	CellSubzoneStruct * above = &begin[-stride];
	CellSubzoneStruct * below = &begin[stride];
	x = begin_x;

	Cell span_cell;
	span_cell.X = 0;
	span_cell.Y = 0;
	while (x <= end_x) {
		int shadow_subzone = above->SubzoneID[subzone_level];
		CellSubzoneStruct * span_ptr;
		if (x == begin_x) {
			Cell left;
			left.Y = y;
			left.X = x + 1;
			span_cell = left;
			span_ptr = &above[stride + 1];
		} else if (x < end_x) {
			Cell middle;
			middle.X = x;
			middle.Y = y;
			span_cell = middle;
			span_ptr = &above[stride];
		} else {
			Cell right;
			right.X = x - 1;
			right.Y = y;
			span_cell = right;
			span_ptr = &above[stride - 1];
		}

		if (shadow_subzone != 0 || y <= ymin || x < xmin || x > xmax) {
			if (shadow_subzone != (unsigned short)subzone_id && shadow_subzone != last_adjacent) {
				if (shadow_subzone != 0) {
					if (abs(span_ptr->Height - above->Height) < 2 && In_Local_Radar(span_cell, true)) {
						Cell shadow_cell;
						shadow_cell.Y = y - 1;
						shadow_cell.X = x;
						if (In_Local_Radar(shadow_cell, true)) {
							int packed_pair = Zone_Pack32(shadow_subzone, (unsigned short)subzone_id);
							if (x < xmin) {
								entry.Value.IsCrossBlock = true;
							} else {
								entry.Value.IsCrossBlock = false;
								if (x > xmax) {
									entry.Value.IsCrossBlock = true;
								}
							}
							entry.Key = entry.Value.SubzoneID = packed_pair;
							table->Add_Object(Zone_Pack8(shadow_subzone, (unsigned short)subzone_id), entry);
							last_adjacent = shadow_subzone;
						}
					}
				}
			}
			above++;
			x++;
		} else if (above->ZoneID == zone) {
			if (abs(span_ptr->Height - above->Height) < 2) {
				Cell shadow_cell;
				shadow_cell.X = x;
				shadow_cell.Y = y - 1;
				Subzone_Span(above, subzone_level, subzone_id, bounds, shadow_cell);
				continue;
			}
			above++;
			x++;
		} else {
			above++;
			x++;
		}
	}
	x = begin_x;
	while (x <= end_x) {
		int shadow_subzone = below->SubzoneID[subzone_level];
		CellSubzoneStruct * span_ptr;
		if (x == begin_x) {
			Cell left;
			left.X = x + 1;
			left.Y = y;
			span_cell = left;
			span_ptr = &below[-stride + 1];
		} else if (x < end_x) {
			Cell middle;
			middle.X = x;
			middle.Y = y;
			span_cell = middle;
			span_ptr = &below[-stride];
		} else {
			Cell right;
			span_ptr = &below[-stride - 1];
			right.X = x - 1;
			right.Y = y;
			span_cell = right;
		}

		if (shadow_subzone != 0 || y >= ymax || x < xmin || x > xmax) {
			if (shadow_subzone != (unsigned short)subzone_id && shadow_subzone != last_adjacent) {
				if (shadow_subzone != 0) {
					if (abs(below->Height - span_ptr->Height) < 2 && In_Local_Radar(span_cell, true)) {
						Cell shadow_cell;
						shadow_cell.X = x;
						shadow_cell.Y = y + 1;
						if (In_Local_Radar(shadow_cell, true)) {
							int packed_pair = Zone_Pack32(shadow_subzone, (unsigned short)subzone_id);
							if (x < xmin) {
								entry.Value.IsCrossBlock = true;
							} else {
								entry.Value.IsCrossBlock = false;
								if (x > xmax) {
									entry.Value.IsCrossBlock = true;
								}
							}
							entry.Key = entry.Value.SubzoneID = packed_pair;
							table->Add_Object(Zone_Pack8(shadow_subzone, (unsigned short)subzone_id), entry);
							last_adjacent = shadow_subzone;
						}
					}
				}
			}
			below++;
			x++;
		} else if (below->ZoneID == zone) {
			if (abs(below->Height - span_ptr->Height) < 2) {
				Subzone_Span(below, subzone_level, subzone_id, bounds, Cell(x, y + 1));
				continue;
			}
			below++;
			x++;
		} else {
			below++;
			x++;
		}
	}

	return(skip);
}
