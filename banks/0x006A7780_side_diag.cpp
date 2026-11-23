// MATCH (1428/1428 bytes) as a diagnostic only, not landable: src/sidebar.cpp checks 115 of 115 with the function of 0x006A7780_side.cpp inserted after FUN_006A7760 and this one unused local in StripClass::Deactivate. It holds with src/sidebar.cpp otherwise unchanged and with 0x006A7780_side_sidebar.cpp.
// The local may sit in StripClass::Activate instead (before `int count`), and may read SidebarRect.Y or DAT_00B0B4FC instead: any scalar global the caller has not read before the inlined copy. In SetTab, RemoveButtons or AddButtons it breaks Recalc through the inline budget.
// What does not act: a load stored to another global (swept over 66 counts), Top_Left() (swept), and as written: a plain int local (dead or live), an inline argument, an array element, a Rect block copy, a folded `- Y + Y`, a discarded `Max_Visible();`. A dead `int rows = Map.Max_Visible();` in Deactivate acts too, but AddCameo then calls Max_Visible out of line where the exe has no call.
// FUNCTION: GAMEMD 0x006A83E0
void SidebarClass::StripClass::Deactivate(void)
{
	Point2D top(0, DAT_00B0B4F8);
	for (int index = 0; index < 60; index++) {
		Map.Remove_A_Button(SelectButton[ID * 60 + index]);
	}
}
