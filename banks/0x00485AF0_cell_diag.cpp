// DIAGNOSTIC, never lands: 90.4% (2201/2191 B = exe + the 10-byte probe); without the probe line this natural form scores 71.9% (2185/2191 B)
// First block uses "- TacticalRect.TopLeft" with no named origin. One register-free instruction (a constant store to a global) anywhere between the two Unions of the
// Ramp == RAMP_NONE block flips every register to the exe's (this EBP, dir EBX, zero in EAX); what is left is the probe itself and two swapped spill slots in the second Union
// ([esp+0x48] and [esp+0x4c], rect2 Height and Y). Stores of a loaded value, stores to locals, dead stores, casts, named locals and trivial inline calls there do not flip it.
extern int Diag_Probe_Store;

// FUNCTION: GAMEMD 0x00485AF0
void CellClass::Place_Veins(void)
{
	Rect rect(0, 0, 0, 0);

	if (Can_Place_Veins()) {

		rect = Union(rect, Union(Overlay_Render_Rect(), Overlay_Shadow_Render_Rect()));

		Map.Radar_Background(CellID);
		Overlay = OVERLAY_VEINS;

		if (Ramp == RAMP_NONE) {
			OverlayData = abs(Scen->RandomNumber()) % 3 + OVERLAYDATA_FIRST_SOLID_VEIN;

			for (int dir = 0; dir < FACING_COUNT; dir += 2) {
				CellClass & adjacent = Adjacent_Cell((FacingType)dir);

				if (adjacent.Overlay == OVERLAY_VEINS && adjacent.OverlayData >= OVERLAYDATA_FIRST_SOLID_VEIN) {
					continue;
				}
				if (adjacent.Overlay != OVERLAY_VEINS && adjacent.Overlay != OVERLAY_NONE && OverlayTypes[adjacent.Overlay]->IsVeins) {
					continue;
				}

				if (adjacent.Ramp == RAMP_NONE) {
					int frame = adjacent.Get_Vein_Frame();
					if (adjacent.Overlay == OVERLAY_NONE || (frame <= 15 && adjacent.OverlayData / 3 != frame)) {
						Rect adjrect(0, 0, 0, 0);
						adjrect = Union(adjrect, Union(adjacent.Overlay_Render_Rect(), adjacent.Overlay_Shadow_Render_Rect()));
						adjacent.Overlay = OVERLAY_VEINS;
						adjacent.OverlayData = 3 * frame + abs(Scen->RandomNumber() % 3);
						Diag_Probe_Store = 5;	// DIAGNOSTIC ONLY, never lands
						adjacent.Recalc_Attributes();
						adjrect = Union(adjrect, Union(adjacent.Overlay_Render_Rect(), adjacent.Overlay_Shadow_Render_Rect())) - TacticalRect.TopLeft;
						TacticalMap->Register_Dirty_Area(adjrect);
					}
				} else {
					Rect adjrect(0, 0, 0, 0);
					adjrect = Union(adjrect, Union(adjacent.Overlay_Render_Rect(), adjacent.Overlay_Shadow_Render_Rect()));
					adjacent.Overlay = OVERLAY_VEINS;
					adjacent.OverlayData = 2 * adjacent.Ramp + (abs(Scen->RandomNumber()) & 1) + OVERLAYDATA_FIRST_RAMP_VEIN;
					adjrect = Union(adjrect, Union(adjacent.Overlay_Render_Rect(), adjacent.Overlay_Shadow_Render_Rect())) - TacticalRect.TopLeft;
					TacticalMap->Register_Dirty_Area(adjrect);
				}

				Map.Radar_Background(adjacent.CellID);
			}

			Trigger_Veins();
		} else {
			OverlayData = 2 * Ramp + (abs(Scen->RandomNumber()) & 1) + OVERLAYDATA_FIRST_RAMP_VEIN;
		}

		rect = Union(rect, Union(Overlay_Render_Rect(), Overlay_Shadow_Render_Rect())) - TacticalRect.TopLeft;
		TacticalMap->Register_Dirty_Area(rect);
		Recalc_Attributes();
	}
}
