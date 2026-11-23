// best natural 72.2% raw, 82.6% normalised (2185/2191 B), no overlay; insert after 0x00485590 (Redraw_Veins) in src/cell.cpp. The 6 missing bytes are a zero that ours keeps in EBX through the first adjacent block (cmp [mem],ebx twice) where the exe has xor eax,eax and mov/test; the this/dir EBX/EBP swap follows from it. A diagnostic store flips it only between the OverlayData assignment and Recalc_Attributes (any count >= 1), never elsewhere in the loop; dead locals, dead expressions and trivial inline calls there do not, nor do eight respellings of the OverlayData statement or seven of the second Union statement
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
						adjacent.Recalc_Attributes();
						Point2D origin = TacticalRect.TopLeft;
						adjrect = Union(adjrect, Union(adjacent.Overlay_Render_Rect(), adjacent.Overlay_Shadow_Render_Rect())) - origin;
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
