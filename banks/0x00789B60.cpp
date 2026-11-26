// 0x00789B60 99.8% raw (2312/2312 bytes, ctor 0x007AC610 now called as the exe does; only lea ebx,[edx+edi] left), agent-789b60 24 Nov. Base: today's main wonline.cpp with this function at its address position (after 'extern int DAT_008316A4;', unmarked end copy removed, as the fablestop/wolcls banks).
// File changes this form needs (full diff against that base is the block at the end of this comment):
//  (1) Build_ExInfo, a static inline factored from the 4 identical memset/Encode_Channel_ExInfo/[40]=0 sequences, defined after Encode_Channel_ExInfo; WOL_Wait_Callback, FUN_0078F000 and FUN_007A5690 call it and stay MATCH.
//  (2) Clear_Channel (existing helper, real callers at ~13339) made visible above this function (forward declaration, or move its definition up; same result).
//  (3) profile-> (rc_ptr::operator->) for the two field reads, in place of profile.Get()->.
// With (1)-(3): 469/470 other wonline functions MATCH; CChatEventSink::OnChannelJoin 0x007A5890 flips to 99.9% (heap-sensitive; not by decl weight before it: declprobe var 1..12 does not restore it; profile-> alone keeps it MATCH, so it is (1) or (2)).
// Inline budget, measured: each of (1),(2),(3) cuts the "4 trivial inline sites after the control" deficit (Clear_Channel 2, profile-> 1, together with Build_ExInfo 0). Begin_Transition / Is_WDT_Session helpers, a TibSun-form Player_Name_To_Index, named ctor-arg locals, decls at the top, body-assigned ctor: no help or worse.
// lea operand order: NOT heap/decl weight (declprobe 1..24 flat) and NOT plain locals (70 uninitialised dead locals flat). It follows the function's expression mass: one dead local `int p = Inline_Probe_(0);` whose inline body has K terms gives MATCH for K in ~31..~125 with this form (the same window on the base, and with a repeated constant), anywhere in the body; K<=29 or K>=130 keeps [edi+edx]. With K>=44 placed BEFORE the control it fixes the ctor too (full MATCH on the untouched base). So the original carries roughly 120-500 more expression nodes than ours that compile to nothing (an inline expansion or verbose form we do not have); not found.
//
// 970a971,977
// > static inline void Build_ExInfo(char * info)
// > {
// > 	memset(info, 0, 80);
// > 	Encode_Channel_ExInfo(info);
// > 	info[40] = 0;
// > }
// > 
// 1444,1446c1451
// < 			memset(info, 0, sizeof(info));
// < 			Encode_Channel_ExInfo(info);
// < 			info[40] = 0;
// ---
// > 			Build_ExInfo(info);
// 5154a5160,5161
// > static inline void Clear_Channel(Channel & channel);
// > 
// 5172c5179
// < 	memset(&channel, 0, sizeof(channel));
// ---
// > 	Clear_Channel(channel);
// 5180,5182c5187
// < 	memset(user_options, 0, sizeof(user_options));
// < 	Encode_Channel_ExInfo(user_options);
// < 	user_options[40] = 0;
// ---
// > 	Build_ExInfo(user_options);
// 5216,5217c5221,5222
// < 		Session.FUN_0069B760(profile.Get()->Country, false);
// < 		Session.FUN_0069B7E0(profile.Get()->Field20, false);
// ---
// > 		Session.FUN_0069B760(profile->Country, false);
// > 		Session.FUN_0069B7E0(profile->Field20, false);
// 6366,6368c6371
// < 		memset(info, 0, sizeof(info));
// < 		Encode_Channel_ExInfo(info);
// < 		info[40] = 0;
// ---
// > 		Build_ExInfo(info);
// 12258,12260c12261
// < 		memset(info, 0, sizeof(info));
// < 		Encode_Channel_ExInfo(info);
// < 		info[40] = 0;
// ---
// > 		Build_ExInfo(info);

static inline void Clear_Channel(Channel & channel);

// FUNCTION: GAMEMD 0x00789B60
bool __fastcall FUN_00789B60(void)
{
	Wstring password;
	if (Options.ScreenWidth >= 1024) Session.WOLLimitResolution = false;
	CreateGameDialogControl control(Session.WOLLimitResolution, Game::ObserverMode == 1);
	g_GameStartRequested = false;
	if (Is_Internal_Session()) {
		g_MaxPlayers = 2;
	} else {
		FUN_0052FEC0(NULL, true);
		if (Run_Dialog_Control(0xC3, WOL_New_Game_Dialog_Proc, &control) == IDCANCEL) return false;
		password = control.Text;
		password.replace(" ", "_");
	}
	g_pChat->RequestChannelLeave();
	Channel channel;
	Clear_Channel(channel);
	sprintf((char *)channel.name, "%s's_game", g_NickName.c_str());
	channel.type = 41;
	channel.minUsers = 1;
	if (g_MaxPlayers < 2) g_MaxPlayers = 6;
	strcpy((char *)channel.key, password.get());
	channel.maxUsers = g_MaxPlayers;
	char user_options[80];
	Build_ExInfo(user_options);
	strcpy((char *)channel.exInfo, user_options);
	if (Session.Type == GAME_INTERNET && Session.WOLWelcomeChoice == 3) {
		channel.tournament = 1;
		channel.reserved |= 0x1000;
	} else if (Is_Internal_Session()) {
		if (Session.WOLWelcomeChoice == 2) channel.tournament = 0;
		else channel.tournament = 1;
		channel.reserved |= 0x800;
	} else {
		channel.tournament = control.Flag10;
		if (control.Flag11) channel.reserved |= 0x100;
		Session.WOLLimitResolution = control.Flag12;
		if (control.Flag12) channel.reserved |= Options.ScreenWidth == 800 ? 0x400 : 0x200;
	}
	channel.reserved += g_CurrentLobby;
	Leave_Channel(0xC0, false);
	MultiplayerGameMode * mode = First_Game_Mode();
	Session.MPGameMode = mode;
	Session.Options.MPModeIndex = mode->MPModeIndex;
	mode->vt_entry_20();
	Set_Scenario_Info_From_Index(0);
	FUN_005C6120(&Session.Players);
	FUN_005E8B40();
	FUN_005E8B60();
	NodeNameType * who = new NodeNameType;
	memset(who, 0, sizeof(NodeNameType));
	WideWstring name = FUN_007B7ED0(g_NickName);
	wcscpy(who->Name, name.get());
	WOLOptionsClass options;
	rc_ptr<PlayerProfile> profile;
	if (options.Is_Bit1_Clear()) profile = FUN_00778A20(&g_NickName);
	else profile = DAT_00B75E48;
	if (profile.Get()) {
		Session.FUN_0069B760(profile->Country, false);
		Session.FUN_0069B7E0(profile->Field20, false);
	}
	Session.Start = -1;
	Session.InitialStart = -2;
	who->Set_Country(Session.FUN_0069B700());
	who->Set_Color(Session.FUN_0069B720());
	who->Set_Start(-2);
	who->Set_Team(-2);
	who->Slot.Unknown77 = 1;
	who->Slot.SpectatorFlag = 0;
	DAT_008316A4 = 7;
	Game::ObserverMode = control.Flag13;
	FUN_005EE350(0);
	if (Game::ObserverMode) {
		DAT_008316A4 = 0;
		who->Slot.SpectatorFlag = -1;
		who->Set_Country(-3);
	}
	Session.Players.Add(who);
	FUN_005E8DA0(0);
	wcscpy(Session.GameName, name.get());
	HRESULT result = g_pChat->RequestChannelCreate(&channel);
	if (result == 0x800401F8) {
		ODMessageBox(StringTable::LoadString("TXT_BAD_CHANKEY", NULL, "D:\\ra2mdpost\\wonline.cpp", 0x2E0D), 0, FUN_0077D710, false);
		FUN_00792EC0(0x80);
		return false;
	}
	if (result != 0) {
		ODMessageBox(StringTable::LoadString("TXT_CANTCREATE_CHAN", NULL, "D:\\ra2mdpost\\wonline.cpp", 0x2E13), 0, FUN_0077D710, false);
		FUN_00792EC0(0x80);
		return false;
	}
	WideWstring title(StringTable::LoadString("GUI:CreatingChannel", NULL, "D:\\ra2mdpost\\wonline.cpp", 0x2E1B));
	Show_Wait_Window(4, title, false);
	FUN_00792EC0(0x80);
	Show_Wait_Window(4, title, true);
	int index = Player_Name_To_Index(g_NickName);
	if (index < 0) return false;
	User * user = NULL;
	g_UserList.getPointer(&user, index);
	who->Address = FUN_0077D720(user);
	if (!Is_Internal_Session()) {
		HWND window = WS_Top_Window();
		if (window) {
			OwnerDraw::DrawCampaignMenuTransition(window, false);
			FUN_0052FEC0(NULL, true);
		}
		FUN_0052FEC0(NULL, true);
		FUN_0072CF40();
		HWND dialog = UI::BeginDialog(0xC2, WOL_Game_Options_Proc, 0);
		Session.MPGameMode->vt_entry_9C();
		Session.MPGameMode->vt_entry_20();
		Session.MPGameMode->vt_entry_18();
		Session.MPGameMode->UnfixAlliances();
		ShowWindow(dialog, SW_SHOWNORMAL);
		if (window) ShowWindow(window, SW_HIDE);
	}
	return true;
}
