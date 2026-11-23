// best natural 99.0% raw, 99.2% normalised (1428/1428 bytes), no overlay; insert after FUN_006A7760 in src/sidebar.cpp WITH StripClass::Page (0x006A89E0) calling Map.Max_Visible() in place of Strip_Rows(). Not a match: one of the four inlined row counts (SetTab's, through AddButtons and Activate) still subtracts DAT_00B0B4F8 before the reserved rows.
// With src/sidebar.cpp unchanged this same text scores 98.5% (two copies wrong). 0x006A7780_side_sidebar.cpp beside this file is src/sidebar.cpp with the two invented helpers Strip_Rows and Visible_Cameos removed and seven sites calling the real members (Max_Visible, GetVisibleCameoCount), as TibSun's sidebar.cpp does: all 114 marked functions still MATCH (check.py --summary), and this function scores 99.0% in it.
// SidebarClass::Draw_It (0x006A6C30) is the one site that needs the static copy (Max_Visible_Cameos): with the member it scores 99.4% at every local count.
// Diagnostic only, not source: one unused class-type local set from a global the function has not read yet, in StripClass::Deactivate or StripClass::Activate (`Point2D top(0, DAT_00B0B4F8);`), gives MATCH here with all 114 other functions still MATCH (0x006A7780_side_diag.cpp).
// FUNCTION: GAMEMD 0x006A7780
void SidebarClass::AI(KeyNumType & input, Point2D const & xy)
{
	Point2D xy_rel = xy - Point2D(SidebarRect.X, 0);

	if (!Debug_Map) {
		Activate(1);
		for (int index = 0; index < STRIPS; index++) {
			Column[index].AI(input, xy_rel);
		}
	}

	if (IsSidebarActive) {
		if (unknown_5398 == 1) {
			unknown_5394++;
			if (unknown_5394 <= ((short *)DAT_00B0B478)[3]) {
				goto animated;
			}
			goto reset;
		} else if (unknown_5398 == -1) {
			unknown_5394--;
			if (unknown_5394 < 0) {
reset:
				unknown_5398 = 0;
				unknown_5394 = 0;
			}
		}
animated:

		FUN_006A6290(PlayerPtr->CurBuildings > 0);

		if (input == 0x8066) {
			VocClass::PlayGlobal(Rule->GUIMainButtonSound, 1.0f, 0x2000);
			Sell_Mode_Control(-1);
		} else if (input == 0x8065) {
			VocClass::PlayGlobal(Rule->GUIMainButtonSound, 1.0f, 0x2000);
			Repair_Mode_Control(-1);
		} else if (input >= 0x80cb && input < 0x80cf) {
			SetTab(input - 0x80cb);
			VocClass::PlayGlobal(Rule->GUITabSound, 1.0f, 0x2000);
		} else {
			int button = input & ~0x4000;
			if (button == (Upgrade.ID | 0x8000)) {
				Upgrade.IsPressed = false;
				if (!Column[ActiveTabIndex].Page(false)) {
					VocClass::PlayGlobal(Rule->ScoldSound, 1.0f, 0x2000);
				} else {
					VocClass::PlayGlobal(Rule->GUITabSound, 1.0f, 0x2000);
				}
			} else if (button == (Waypoint.ID | 0x8000)) {
				Waypoint.IsPressed = false;
				if (!Column[ActiveTabIndex].Page(true)) {
					VocClass::PlayGlobal(Rule->ScoldSound, 1.0f, 0x2000);
				} else {
					VocClass::PlayGlobal(Rule->GUITabSound, 1.0f, 0x2000);
				}
			}
		}

		bool changed = false;
		for (int index = 0; index < STRIPS; index++) {
			if (TabButtons[index].AI()) {
				changed = true;
			}
		}
		if (changed) {
			IsToRedraw = true;
			IsToBlitSidebar = true;
			Map.IsForceCompleteRedraw = true;
			Flag_To_Redraw();
		}
	}

	if (!IsSellMode && Power.IsOn) {
		Power.Turn_Off();
	}
	if (!IsRepairMode && Repair.IsOn) {
		Repair.Turn_Off();
	}

	BASECLASS::AI(input, xy);
}
