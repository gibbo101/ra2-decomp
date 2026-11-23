// best 98.5% raw, 98.7% normalised (1428/1428 bytes), no overlay; insert after FUN_006A7760 in src/sidebar.cpp, which is otherwise unchanged; two of the four inlined row counts still subtract DAT_00B0B4F8 before the reserved rows
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
