// best natural 93.5% (290/290 B), no overlay; splice after Zone_Span (0x0056CB90, ends before 0x0056D3A0) in src/map.cpp; rechecked 24 Nov. Same bytes as 0x0056D100_regs.cpp.
// 24 Nov (fresh pass, then meastes' base-RA2 0x0054F7C0 port): the port (int zone, Cell const & Get_Cell_Zone, explicit `true`) is 89.4% either operand order.
// The order key is the leaf symbol only: parameter names, const, mixed bool/char/unsigned char bridge and callee types, pointer from/to,
// int-vs-enum mzone, casts on the call results, reference/pointer aliases of to_bridge, early copies of either bridge: all 89.4 (folded or propagated).
// Inline wrappers (free or member Cell const & overload) evaluate right operand first but their result ranks as named (93.5); localprobe 0..17 flat on the 89.4 form.
// Early extra refs of this/mzone do not reach the tail tie (tail allocated alone). Zero-code bridge forms all add code: `?:`/`!!`/`== true` 96.8, `& true` 97.2.
// Tail registers only: exe this=esi (loaded after push to), mzone=edi, to-zone=ebp; ours to-zone=esi, this=edi, mzone=ebp.
// Measured model: candidates take esi, edi, ebx, ebp in priority order. Priority is block-weighted ref count; at 2 refs each
// a named or inline-hoisted to-zone wins the tie over this and mzone, a compiler temporary comes last. The exe is
// (this, mzone, to-zone): either the to-zone is a temporary of an in-place two-call compare, or this and mzone each
// have a third ref (a third call after the compare gives every exe register, with extra code).
// In-place two-call compares are ordered by the last argument's leaf: locals first (latest defined first), then
// parameters by ascending slot. from_bridge (slot 4) precedes to_bridge (slot 5), so the exe's to-first order
// cannot come from two plain calls on these parameters; base RA2 (game.exe 0x0054F7C0) has exactly that from-first form.
// Diagnostics: from-call given leavemap 99.2%; bridges swapped 98.4%; flocal/tlocal passed as bridges 98.4%.

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
