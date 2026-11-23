// best 59.2% raw / 95.0% normalised (1259/1277 B), no overlay; insert before 0x005EA8E0. Inlining decisions equal the exe's. Left: in the full path the exe gives the two country loops' temporaries EDI and reloads dialog from its stack home; ours keeps dialog in EDI and uses EBX. One more real use of item in Update_Country_Flags flips both loops and the code after them to the exe's registers (96.3% normalised with the probe), leaving node and surface swapped in both inlined FUN_005EA2A0 copies.
static inline void Update_Country_Flags(HWND dialog)
{
	for (int i = 0; i < 8; i++) {
		HWND item = GetDlgItem(dialog, FUN_004E3320(i));
		FUN_00603D30(item, FUN_004E3560(FUN_005EA920(i)));
	}
}

static inline void Update_Country_Combos(HWND dialog)
{
	for (int i = 0; i < 8; i++) {
		int id = FUN_004E37D0(i);
		FUN_004E3F70(dialog, id, FUN_005EA920(i));
	}
}

static inline void Build_Country_String(char * buffer)
{
	buffer[0] = '\0';
	for (int i = 0; i < 8; i++) {
		char text[4];
		sprintf(text, "%d,", FUN_005EA920(i));
		strcat(buffer, text);
	}
}

// FUNCTION: GAMEMD 0x005EA3E0
void __fastcall FUN_005EA3E0(HWND dialog, bool full)
{

	if (full) {
		FUN_005EB060(dialog);
		Update_Country_Flags(dialog);
		Update_Country_Combos(dialog);
		FUN_005EAEE0(dialog);
		FUN_005EADA0(dialog);
		FUN_005EAE40(dialog);
		FUN_005EA2A0(dialog);
		if (Session.Type == GAME_INTERNET) {
			FUN_007A0E50();
		}
		if (Session.MPGameMode) {
			Session.MPGameMode->vt_entry_78(dialog);
		}
		return;
	}
	if (Session.MPGameMode) {
		Session.MPGameMode->vt_entry_78(dialog);
	}
	FUN_005EA2A0(dialog);
	wcscpy(DAT_00AC0D44, DAT_00AC0B94);
	FUN_005E8DC0(DAT_00AC0B94);
	if (wcscmp(DAT_00AC0D44, DAT_00AC0B94) != 0) {
		FUN_005EB060(dialog);
		if (Session.Type == GAME_INTERNET) {
			FUN_007A0E50();
		}
	}
	Update_Country_Flags(dialog);
	strcpy(DAT_00AC0EA4, DAT_00AC0B10);
	Build_Country_String(DAT_00AC0B10);
	if (strcmp(DAT_00AC0EA4, DAT_00AC0B10) != 0) {
		Update_Country_Combos(dialog);
	}
	strcpy(DAT_00AC0AC8, DAT_00AC0AEC);
	FUN_005EAB30(DAT_00AC0AEC);
	if (strcmp(DAT_00AC0AC8, DAT_00AC0AEC) != 0) {
		FUN_005EAEE0(dialog);
	}
	strcpy(DAT_00AC0A9C, DAT_00AC0B6C);
	FUN_005EAC00(DAT_00AC0B6C);
	if (strcmp(DAT_00AC0A9C, DAT_00AC0B6C) != 0) {
		FUN_005EADA0(dialog);
	}
	strcpy(DAT_00AC0278, DAT_00AC0B48);
	FUN_005EACD0(DAT_00AC0B48);
	if (strcmp(DAT_00AC0278, DAT_00AC0B48) != 0) {
		FUN_005EAE40(dialog);
	}
}

