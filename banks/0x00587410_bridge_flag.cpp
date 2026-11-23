// MATCH 100% (2109/2109) but held: the loop test `while (!broken)` is always true when tested (flag form, as on branch single-587410b); kept for reference beside the flag-free bank 0x00587410_bridge.cpp
// Tells whether a bridge within two cells of the cell has a broken span to repair.
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
			bool broken = false;
			while (!broken) {
				int index = Zone_Connection_Index(search, 3, 0);
				if (index < 0) {
					break;
				}
				if (!ZoneConnections[index].IsPassable) {
					broken = true;
					break;
				}
				ZoneConnectionClass * con = &ZoneConnections[index];
				if (con->From == entry) {
					entry = con->To;
					search = Adjacent_Cell(entry, dir);
				} else if (con->To == entry) {
					entry = con->From;
					search = Adjacent_Cell(entry, dir);
				} else {
					return(true);
				}
			}
			if (broken) {
				return(true);
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

