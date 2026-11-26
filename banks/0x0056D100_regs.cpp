// best natural 93.5% (290/290 B), no overlay; same source as 0x0056D100_grm.cpp (splice before 0x0056D230 in src/map.cpp). Identical bytes under the RTM C2 (8168) and the 8168 front end.
// Register model, recalibrated on SP3 (a stack parameter's or reloaded this's load is not a reference): the tail is a three-way tie at 2 refs (this reload, mzone, to-zone); SP3 gives a named to-zone the tie (esi), the exe's to-zone loses it, as a compiler temporary does (to_bridge != false: exe registers, 96.8%). Needs the to-call first as an unnamed temporary, which the leaf order forbids for two plain calls, or +1 ref to both this and mzone.
// Flat here: localprobe 0..9, uninitialised/0-initialised tozone, (tozone = call) == call, swapped/negated/ternary single expressions (89.4/87.7), (int) cast, bool local.

// FUNCTION: GAMEMD 0x0056D100
bool MapClass::Is_Same_Cell_Zone(Cell const & from, Cell const & to, MZoneType mzone, bool from_bridge, bool to_bridge, bool leavemap)
{
	if (mzone == MZONE_NONE) {
		return(true);
	}

	bool flocal = Map.In_Local_Radar(from);
	bool finradar = Map.In_Radar(from);

	if (!flocal && finradar) {
		return(true);
	}

	bool tlocal = Map.In_Local_Radar(to);
	bool tinradar = Map.In_Radar(to);

	if (leavemap && flocal && !tlocal && tinradar) {
		return(true);
	}

	int tozone = Get_Cell_Zone(&to, mzone, to_bridge);
	return(Get_Cell_Zone(&from, mzone, from_bridge) == tozone);
}
