// best 65.0% raw, 76.8% normalised (809/824 B), no overlay; splice before Set_Ground_Position (above 0x006D8260) in src/tactical.cpp. Site 1 now matches (pixel built by member assignment from screen); every call matches. Left: the middle's registers (exe: draw in al, height kept in edx, diff in ecx, Elevation and level re-read; ours: draw in cl, Elevation cached in al, height re-read) and the slot order that follows from them
// FUNCTION: GAMEMD 0x006D7F20
void Tactical::Draw_Tiles(Cell const & cell, Rect const & cliprect)
{
	CellClass * basecell = &Map[cell];

	Coord coord = Coord_Whole(Coord(cell));
	Point2D screen;
	Coord_To_Pixel(coord, screen);
	if (basecell->Height < 2) {
		Point2D pixel;
		pixel.X = screen.X - ISO_TILE_PIXEL_W / 2;
		pixel.Y = screen.Y;
		basecell->Draw_It(pixel, TacticalRect, 0);
	}

	int x = cell.X;
	int y = cell.Y;
	for (int level = 0; level < 24; level += 2) {
		for (int col = 0; col < 3; col++) {
			int cx = x + ((col + 1) >> 1);
			int cy = y - (col >> 1) + 1;
			if (Map.In_Radar(Cell(cx, cy))) {
				CellClass * cellptr = &Map[Cell(cx, cy)];
				bool draw = false;
				int height = cellptr->Height;
				bool overlap = false;

				if (col != 1) {
					if (height == level + 1) {
						draw = true;
						overlap = true;
					}
				} else if (height > level && height < level + 4) {
					draw = true;
					overlap = true;
				}

				if (cellptr->Elevation > 0 && !draw) {
					int diff = height - level;
					if (col != 1) {
						if (diff < 0 && cellptr->Elevation + diff >= 0) {
							draw = true;
						} else {
							draw = overlap;
						}
					} else if (diff + 1 > 0) {
						draw = overlap;
					} else {
						draw = true;
						if (cellptr->Elevation + diff + 1 <= 0) {
							draw = overlap;
						}
					}
				}

				if (height == level) {
					if (!draw) {
						int subtile = cellptr->SubTile;
						int ittype = cellptr->ITType;
						if (ittype == ISOTILE_NONE || ittype == ISOTILE_NONE_LEGACY) {
							continue;
						}
						IsometricTileTypeClass * type = IsometricTileTypes[ittype];
						if (type == NULL) {
							continue;
						}
						IsoTileSet const * set = (IsoTileSet const *)type->Get_Image_Data();
						if (set == NULL) {
							continue;
						}
						int last = set->Tile_Count() - 1;
						if (subtile >= last) {
							subtile = last;
						}
						IsoTileRecord const * record = set->Fetch_Record_Pointer_Unsafe(subtile);
						if (record == NULL || !record->IsHasExtraData) {
							continue;
						}
					}
				} else if (!draw) {
					continue;
				}

				Cell above(cx, cy);
				coord = Coord_Whole(Coord(above));
				Point2D pixel;
				Coord_To_Pixel(coord, pixel);
				Map[above].Draw_It(pixel - Point2D(ISO_TILE_PIXEL_W / 2, 0), TacticalRect, 0);
			}
		}
		x++;
		y++;
	}
}
