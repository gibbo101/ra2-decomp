// best 70.2% (764/795 bytes), no overlay; insert after 0x00481130 in src/cell.cpp. Only diff: TPoint2D::Length inside the inlined Spot_Index stays a call (inline budget).
// Diagnostics: all calls match (97.7%, 795/795) only with Spot_Index richer (~50 trivial statements before it) AND one extra inline site in Get_Gate after Cell_Building().
// FUNCTION: GAMEMD 0x00481180
Coord CellClass::Closest_Free_Spot(Coord const & xcoord, bool any, bool bridge, bool use_cell_coord) const
{
	int spot_index = Spot_Index(xcoord);

	static unsigned char _sequence[5][4] = {
		{1,2,3,4},
		{0,2,3,4},
		{0,1,4,3},
		{0,1,4,2},
		{0,2,3,1}
	};

	static unsigned char _alternate[4][4] = {
		{1,2,3,4},
		{2,3,4,1},
		{3,4,1,2},
		{4,1,2,3},
	};

	Coord coord;
	if (use_cell_coord) {
		Coord center = Center_Coord();
		coord = Coord(center.X - (center.X & 0xFF), center.Y - (center.Y & 0xFF), center.Z);
	} else {
		coord = Coord(xcoord.X - (xcoord.X & 0xFF), xcoord.Y - (xcoord.Y & 0xFF), xcoord.Z);
	}

	bool is_vehicle_occupied;
	if (bridge) {
		is_vehicle_occupied = BridgeFlag.Occupy.Vehicle;
	} else {
		is_vehicle_occupied = Flag.Occupy.Vehicle;
	}

	if (!any) {
		if (is_vehicle_occupied || (Flag.Occupy.Monolith && (!Get_Gate() || !Get_Gate()->Is_Gate_Open()))) {
			return(COORD_NONE);
		}
	}

	if (any || Is_Spot_Free(spot_index, bridge)) {
		coord = coord + StoppingCoordAbs[spot_index];
		coord.Z = Map.Get_Height_GL(xcoord);
		if (bridge) {
			coord.Z += BRIDGE_LEPTON_HEIGHT;
		}
		return(coord);
	}

	unsigned char * sequence;
	if (spot_index == 0) {
		sequence = &_alternate[Random_Pick(0, 3)][0];
	} else {
		sequence = &_sequence[spot_index][0];
	}
	for (int index = 0; index < 4; index++) {
		int pos = *sequence++;

		if (Is_Spot_Free(pos, bridge)) {
			coord = coord + StoppingCoordAbs[pos];
			coord.Z = Map.Get_Height_GL(xcoord);
			if (bridge) {
				coord.Z += BRIDGE_LEPTON_HEIGHT;
			}
			return(coord);
		}
	}

	return(COORD_NONE);
}

