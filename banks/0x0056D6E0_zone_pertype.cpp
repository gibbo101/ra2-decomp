// 53.4% raw (73.3% normalised, 856/804 bytes), no overlay: lower raw score and 52 bytes long, banked because the exe's block order supports it; insert before 0x0056DA10 in src/map.cpp. In the exe the train-bridge block jumps back into the bridge block's index test (jmp 0x0056D916), which is two separate identical tests cross-jumped; ours cannot merge them because the two copies take mirrored scratch registers (set in edx / table entry in ecx, then the reverse; the exe has ecx for the set). No effect: index as one outer variable, as a function-level variable, or written out with no variable (byte-identical); a store sweep 0..7 after the facing definition (154 differing lines at every count).
// FUNCTION: GAMEMD 0x0056D6E0
void MapClass::Compute_Zone_Connections(void)
{
	static int _bridge_subtiles1[] = {
		7, 7, -1, 7, 7, -1, 4, 4,
		4, 4,  4, 2, 2,  2, 2, 2
	};
	static FacingType _facings[] = {
		FACING_E, FACING_E, FACING_NONE, FACING_S, FACING_S, FACING_NONE, FACING_E, FACING_E,
		FACING_E, FACING_E,    FACING_E, FACING_S, FACING_S,    FACING_S, FACING_S, FACING_S
	};
	static int _bridge_subtiles2[] = {
		-1, -1, 4, -1, -1, 2, 4, 4,
		 4,  4, 4,  2,  2, 2, 2, 2
	};

	ZoneConnections.Clear();
	Reset_Iterator();
	CellClass * cptr = Iterate();

	while (cptr != NULL) {

		if (!cptr->Is_Tile_Bridge() && !cptr->Is_Tile_Train_Bridge()) {

			if (cptr->Has_Tunnel()) {
				if ((cptr->Adjacent_Cell(FACING_E).Has_Tunnel() && cptr->Adjacent_Cell(FACING_W).Has_Tunnel()) ||
					(cptr->Adjacent_Cell(FACING_S).Has_Tunnel() && cptr->Adjacent_Cell(FACING_N).Has_Tunnel())) {

					Cell cell = cptr->Get_Tunnel()->Exit;
					if (Map_Cell_Index(cptr->CellID) < Map_Cell_Index(cell)) {
						Cell exit = cptr->Get_Tunnel()->Exit;
						ZoneConnectionClass con;
						con.From = cptr->CellID;
						con.To = exit;
						con.IsPassable = true;
						con.Type = 1;
						ZoneConnections.Add(con);
					}
				}
			}

		} else {

			int faceindex;
			if (cptr->Is_Tile_Bridge()) {
				faceindex = cptr->ITType - IsometricTileTypeClass::BridgeSet;
			} else {
				faceindex = cptr->ITType - IsometricTileTypeClass::TrainBridgeSet;
			}

			if (_bridge_subtiles1[faceindex] == cptr->SubTile) {

				bool crossed = false;
				bool passable = true;
				FacingType facing = _facings[faceindex];

				CellClass * bptr = cptr;
				for (;;) {
					bptr = &bptr->Adjacent_Cell(facing);

					if (In_Radar(bptr->CellID)) {

						if (!crossed) {

							if (bptr->Is_Tile_Bridge()) {
								int index = bptr->ITType - IsometricTileTypeClass::BridgeSet;
								if (index != -1 && _bridge_subtiles2[index] == bptr->SubTile) {
									crossed = true;
								}

							} else if (bptr->Is_Tile_Train_Bridge()) {
								int index = bptr->ITType - IsometricTileTypeClass::TrainBridgeSet;
								if (index != -1 && _bridge_subtiles2[index] == bptr->SubTile) {
									crossed = true;
								}
							} else if (!bptr->IsUnderBridge) {
								passable = false;
							}
							continue;
						}

					} else if (!crossed) {
						break;
					}

					ZoneConnectionClass con;
					con.From = cptr->CellID;
					con.To = bptr->Adjacent_Cell(Facing_Sub((unsigned char)facing, FACING_180)).CellID;
					con.IsPassable = passable;
					con.Type = 0;
					ZoneConnections.Add(con);
					break;
				}
			}
		}

		cptr = Iterate();
	}
}
