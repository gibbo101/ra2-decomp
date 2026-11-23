// best 97.1% raw (2109/2109), no overlay; insert before 0x00587C50 in src/map.cpp; left: search at [esp+0x2c] (any form that assigns search after the From/To if/else), index in eax and _facings loaded last, ecx/edx at ZoneConnections
// FUNCTION: GAMEMD 0x00587410
bool MapClass::Can_Repair_Bridge(Cell const & cell)
{
	static short _x[] = {
		1, 1, 0, 2, 2, 2, 0, 0,
		0, 0, 0, 2, 2, 2, 2, 2
	};
	static short _y[] = {
		2, 2, 2, 1, 1, 0, 2, 2,
		2, 2, 2, 0, 0, 0, 0, 0
	};
	static FacingType _facings[] = {
		FACING_E, FACING_E, FACING_W, FACING_S, FACING_S, FACING_N, FACING_NONE, FACING_NONE,
		FACING_NONE, FACING_NONE, FACING_NONE, FACING_NONE, FACING_NONE, FACING_NONE, FACING_NONE, FACING_NONE
	};

	bool high = false;
	bool rail = false;
	CellClass * target = NULL;
	int index = -1;

	for (int y = -2; y < 3; y++) {
		for (int x = -2; x < 3; x++) {
			int tile = Map[Cell(x, y) + cell].ITType;
			if (tile >= IsometricTileTypeClass::TrainBridgeSet && tile < IsometricTileTypeClass::TrainBridgeSet + 16) {
				high = true;
				target = &Map[Cell(x, y) + cell];
				index = tile - IsometricTileTypeClass::TrainBridgeSet;
			} else if (tile >= IsometricTileTypeClass::BridgeSet && tile < IsometricTileTypeClass::BridgeSet + 16) {
				high = true;
				target = &Map[Cell(x, y) + cell];
				index = tile - IsometricTileTypeClass::BridgeSet;
			} else if (Is_Low_Bridge(Cell(x, y) + cell)) {
				high = false;
				rail = false;
				target = &Map[Cell(x, y) + cell];
			} else if (Is_Low_Bridge2(Cell(x, y) + cell)) {
				high = false;
				rail = true;
				target = &Map[Cell(x, y) + cell];
			}
		}
	}

	if (high) {
		IsometricTileTypeClass * isotype = IsometricTileTypes[target->ITType];
		Cell topleft = target->CellID - Cell(target->SubTile % isotype->Width, target->SubTile / isotype->Width);
		FacingType dir = _facings[index];
		Cell search = Cell(_x[index], _y[index]) + topleft;

		if (dir != FACING_NONE) {
			Cell entry = search;
			while (true) {
				int found = Zone_Connection_Index(search, 3, 0);
				if (found < 0) {
					break;
				}
				ZoneConnectionClass * con = &ZoneConnections[found];
				if (!con->IsPassable) {
					return true;
				}
				Cell next;
				if (con->From == entry) {
					entry = con->To;
					next = Adjacent_Cell(entry, dir);
				} else {
					if (con->To != entry) {
						return true;
					}
					entry = con->From;
					next = Adjacent_Cell(entry, dir);
				}
				search = next;
			}
		}
		return false;
	}

	if (target == NULL) {
		return false;
	}

	if (rail) {
		int overlay = target->Overlay;
		Cell walk;
		Cell current;
		if ((overlay >= 0xCD && overlay <= 0xD5) || (overlay >= 0xDF && overlay <= 0xE2) || overlay == 0xE7) {
			walk = target->CellID;
			while (Is_Low_Bridge2(walk)) {
				int end = Map[walk].Overlay;
				if (end == 0xE7 || end == 0xE8) {
					return true;
				}
				walk -= Cell(1, 0);
			}
			current = target->CellID;
			while (Is_Low_Bridge2(current)) {
				int end = Map[current].Overlay;
				if (end == 0xE7 || end == 0xE8) {
					return true;
				}
				current.X++;
			}
			return false;
		}
		current = target->CellID;
		while (Is_Low_Bridge2(current)) {
			int end = Map[current].Overlay;
			if (end == 0xE7 || end == 0xE8) {
				return true;
			}
			current.Y++;
		}
		walk = target->CellID;
		while (Is_Low_Bridge2(walk)) {
			int end = Map[walk].Overlay;
			if (end == 0xE7 || end == 0xE8) {
				return true;
			}
			walk -= Cell(0, 1);
		}
		return false;
	} else {
	int overlay = target->Overlay;
	Cell walk;
	Cell current;
	if ((overlay >= 0x4A && overlay <= 0x52) || (overlay >= 0x5C && overlay <= 0x5F) || overlay == 0x64) {
		walk = target->CellID;
		while (Is_Low_Bridge(walk)) {
			int end = Map[walk].Overlay;
			if (end == 0x64 || end == 0x65) {
				return true;
			}
			walk -= Cell(1, 0);
		}
		current = target->CellID;
		while (Is_Low_Bridge(current)) {
			int end = Map[current].Overlay;
			if (end == 0x64 || end == 0x65) {
				return true;
			}
			current.X++;
		}
		return false;
	}
	current = target->CellID;
	while (Is_Low_Bridge(current)) {
		int end = Map[current].Overlay;
		if (end == 0x64 || end == 0x65) {
		return true;
		}
		current.Y++;
	}
	walk = target->CellID;
	while (Is_Low_Bridge(walk)) {
		int end = Map[walk].Overlay;
		if (end == 0x64 || end == 0x65) {
		return true;
		}
		walk -= Cell(0, 1);
	}
	return false;
	}
}
