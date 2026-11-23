// best 54.8% raw (73.0% normalised, 806/804 bytes), no overlay, body unchanged from the near_mapx bank; insert before 0x0056DA10 in src/map.cpp. Exe keeps cptr in ebp from the prologue and facing at [esp+0x18]; ours spills cptr and gives facing ebx. No effect: the walk step written at entry and in each arm, as a for-increment, or both (byte-identical); named locals 0..8 (flat). The radar-bounds inline already has the exe's shape (x+y temporary in a callee-saved register plus the [esp+0x20] store), so no header change is indicated. See 0x0056D6E0_zone_pertype.cpp for the form the exe's block order supports.
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

							int index;
							if (bptr->Is_Tile_Bridge()) {
								index = bptr->ITType - IsometricTileTypeClass::BridgeSet;
							} else if (bptr->Is_Tile_Train_Bridge()) {
								index = bptr->ITType - IsometricTileTypeClass::TrainBridgeSet;
							} else {
								if (!bptr->IsUnderBridge) {
									passable = false;
								}
								continue;
							}
							if (index != -1 && _bridge_subtiles2[index] == bptr->SubTile) {
								crossed = true;
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
