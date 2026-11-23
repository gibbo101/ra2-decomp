// best flag-free 99.5% raw, 100.0% normalised (2109/2109), no overlay; insert before 0x00587C50 in src/map.cpp; while (true) with a `search = next` join gives the exe's forward cross-jump, but search lands at [esp+0x2c] where the exe has offset's slot [esp+0x14] (3 instructions); the only MATCH forms keep a loop test on a local that is constant on every path (bool flag, or a count never stepped)
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
		FACING_E,    FACING_E,    FACING_W,    FACING_S,    FACING_S,    FACING_N,    FACING_NONE, FACING_NONE,
		FACING_NONE, FACING_NONE, FACING_NONE, FACING_NONE, FACING_NONE, FACING_NONE, FACING_NONE, FACING_NONE
	};

	bool found = false;
	bool concrete = false;
	CellClass * cellptr = NULL;
	int offset = -1;

	for (int y = -2; y < 3; y++) {
		for (int x = -2; x < 3; x++) {
			int ittype = Map[Cell(Cell(x, y) + cell)].ITType;

			if (ittype >= IsometricTileTypeClass::TrainBridgeSet && ittype < IsometricTileTypeClass::TrainBridgeSet + 16) {
				found = true;
				cellptr = &Map[Cell(Cell(x, y) + cell)];
				offset = ittype - IsometricTileTypeClass::TrainBridgeSet;
			} else if (ittype >= IsometricTileTypeClass::BridgeSet && ittype < IsometricTileTypeClass::BridgeSet + 16) {
				found = true;
				cellptr = &Map[Cell(Cell(x, y) + cell)];
				offset = ittype - IsometricTileTypeClass::BridgeSet;
			} else if (Is_Low_Bridge(Cell(x, y) + cell)) {
				found = false;
				concrete = false;
				cellptr = &Map[Cell(Cell(x, y) + cell)];
			} else if (Is_Low_Bridge2(Cell(x, y) + cell)) {
				found = false;
				concrete = true;
				cellptr = &Map[Cell(Cell(x, y) + cell)];
			}
		}
	}

	if (found) {
		IsometricTileTypeClass * isotype = IsometricTileTypes[cellptr->ITType];
		Cell topleft = cellptr->CellID - Cell(cellptr->SubTile % isotype->Width, cellptr->SubTile / isotype->Width);
		Cell search = Cell(_x[offset], _y[offset]) + topleft;
		FacingType dir = _facings[offset];

		if (dir != FACING_NONE) {
			Cell entry = search;
			while (true) {
				int index = Zone_Connection_Index(search, 3, 0);
				if (index < 0) {
					break;
				}
				if (!ZoneConnections[index].IsPassable) {
					return(true);
				}
				ZoneConnectionClass * con = &ZoneConnections[index];
				Cell next;
				if (con->From == entry) {
					entry = con->To;
					next = Adjacent_Cell(entry, dir);
				} else if (con->To == entry) {
					entry = con->From;
					next = Adjacent_Cell(entry, dir);
				} else {
					return(true);
				}
				search = next;
			}
		}
		return(false);
	}

	if (cellptr == NULL) {
		return(false);
	}

	if (concrete) {
		int overlay = cellptr->Overlay;
		Cell walk;
		Cell scan;
		if (overlay >= LB2(1) && overlay <= LB2(9) || overlay >= LB2(19) && overlay <= LB2(22) || overlay == LB2(27)) {
			walk = cellptr->CellID;
			while (Is_Low_Bridge2(walk)) {
				int end = Map[walk].Overlay;
				if (end == LB2(27) || end == LB2(28)) {
					return(true);
				}
				walk -= Cell(1, 0);
			}
			scan = cellptr->CellID;
			while (Is_Low_Bridge2(scan)) {
				int end = Map[scan].Overlay;
				if (end == LB2(27) || end == LB2(28)) {
					return(true);
				}
				scan.X++;
			}
			return(false);
		}
		scan = cellptr->CellID;
		while (Is_Low_Bridge2(scan)) {
			int end = Map[scan].Overlay;
			if (end == LB2(27) || end == LB2(28)) {
				return(true);
			}
			scan.Y++;
		}
		walk = cellptr->CellID;
		while (Is_Low_Bridge2(walk)) {
			int end = Map[walk].Overlay;
			if (end == LB2(27) || end == LB2(28)) {
				return(true);
			}
			walk -= Cell(0, 1);
		}
		return(false);
	} else {
		int overlay = cellptr->Overlay;
		Cell walk;
		Cell scan;
		if (overlay >= OVERLAY_LOWBRIDGE_01 && overlay <= OVERLAY_LOWBRIDGE_09 || overlay >= OVERLAY_LOWBRIDGE_19 && overlay <= OVERLAY_LOWBRIDGE_22 || overlay == OVERLAY_LOWBRIDGE_27) {
			walk = cellptr->CellID;
			while (Is_Low_Bridge(walk)) {
				int end = Map[walk].Overlay;
				if (end == OVERLAY_LOWBRIDGE_27 || end == OVERLAY_LOWBRIDGE_28) {
					return(true);
				}
				walk -= Cell(1, 0);
			}
			scan = cellptr->CellID;
			while (Is_Low_Bridge(scan)) {
				int end = Map[scan].Overlay;
				if (end == OVERLAY_LOWBRIDGE_27 || end == OVERLAY_LOWBRIDGE_28) {
					return(true);
				}
				scan.X++;
			}
			return(false);
		}
		scan = cellptr->CellID;
		while (Is_Low_Bridge(scan)) {
			int end = Map[scan].Overlay;
			if (end == OVERLAY_LOWBRIDGE_27 || end == OVERLAY_LOWBRIDGE_28) {
				return(true);
			}
			scan.Y++;
		}
		walk = cellptr->CellID;
		while (Is_Low_Bridge(walk)) {
			int end = Map[walk].Overlay;
			if (end == OVERLAY_LOWBRIDGE_27 || end == OVERLAY_LOWBRIDGE_28) {
				return(true);
			}
			walk -= Cell(0, 1);
		}
		return(false);
	}
}

