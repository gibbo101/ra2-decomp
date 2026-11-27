// 93.5% raw, 94.3% normalised (290/290), 16 differing lines (tail registers: exe this=esi after push of to, mzone=edi, to-zone=ebp); same body as 0x0056D100.cpp. Gauge flat: localprobe +0..+63 all 93.5%; declprobe inl before 0-16, mem before 0-6, mfn/func/enum/type before 0-12 and inl head 0-10 one hash 58012c; stackslots no slot diffs. Splice after Zone_Span, before 0x0056D3A0 in src/map.cpp.
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
