// best 98.0% raw/normalised (824/824 B, 10 diff lines), natural, no overlay; splice before 0x006D8260 in src/tactical.cpp. 0x006D7F20.cpp with the twins' head 'Point2D screen; Coord coord = Coord_Whole(Coord(cell));' (TibSun/Robin; drops Tile_Center) and Robin's site 2 (named screen, then pixel = screen - Point2D(30, 0)): same bytes. Left: col != 1 test (exe test ecx,ecx on diff) and the site-2 R2I result register
// FUNCTION: GAMEMD 0x006D7F20
void Tactical::Draw_Tiles(Cell const & cell, Rect const & cliprect)
{
	CellClass * basecell = &Map[cell];

	Point2D screen;
	Coord coord = Coord_Whole(Coord(cell));
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

				if (col != 1) {
					if (height == level + 1) {
						draw = true;
					}
				} else if (height > level && height < level + 4) {
					draw = true;
				}

				if (cellptr->Elevation > 0 && draw == false) {
					int diff = height - level;
					if (col != 1) {
						if (height < level && cellptr->Elevation + diff >= 0) {
							draw = true;
						}
					} else if (diff + 1 <= 0 && cellptr->Elevation + diff + 1 > 0) {
						draw = true;
					}
				}

				if (height == level) {
					if (draw == false) {
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
						if (record == NULL) {
							continue;
						}
						if (!record->IsHasExtraData) {
							continue;
						}
						draw = true;
					}
				} else if (draw == false) {
					continue;
				}

				Cell above(cx, cy);
				coord = Coord_Whole(Coord(above));
				Point2D screen;
				Coord_To_Pixel(coord, screen);
				Point2D pixel = screen - Point2D(ISO_TILE_PIXEL_W / 2, 0);
				Map[above].Draw_It(pixel, TacticalRect, 0);
			}
		}
		x++;
		y++;
	}
}

