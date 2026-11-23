// best 93.5% (290/290 bytes), no overlay; splice before 0x0056D230 in src/map.cpp. Only the tail registers differ (exe: this esi loaded after the push of to, mzone edi, the to-zone temporary ebp).
// The exe is the single-expression compare with the to call evaluated first: `to_bridge != false` as the to call's last argument gives every exe register (96.8%, plus its test/setne); VC6 orders the calls by that argument's leaf, and a parameter or a copy-propagated local always puts p4 (from) first.

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
