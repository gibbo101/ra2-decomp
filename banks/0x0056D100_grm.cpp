// best 93.5% (290/290 bytes), no overlay; splice before 0x0056D230 in src/map.cpp; tail registers differ (exe this=esi loaded after the push of to, mzone=edi, tozone=ebp).
// ts-game.exe 0x005156E0 and RA2 game.exe 0x0054F7C0 hold the single-expression form's code (from first, 89.4% here); YR alone evaluates the to call first and keeps its result in a compiler temporary.
// VC6 orders the two calls by their argument leaves, last argument first: lower frame address first (locals newest first, then parameters in order), so parameter 4 always goes before parameter 5.
// The exe's code comes out only when both calls sit in one expression and the to call goes first by that rule; a named local or an inline's result takes esi instead.

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
