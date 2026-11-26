// 0x00789B60 61.9% natural (2321/2312), unchanged from 0x00789B60_order.cpp; splice as that bank says. Two independent blockers. (1) inlinebudget: exe calls WonlineStringDialogControl ctor 0x007AC610, ours inlines it: flips with 4 more inline sites after the control construction or 32..40 units spent before it. (2) lea ebx,[edx+edi] in the inlined Player_Name_To_Index loop follows N10 of the outside rules (a count of strength-reduced index loops before the site): measured 23 Nov, one dead probe loop `for (pw...) { getPointer(&pu, pw); FUN_0077D210(pu->name); }` just before the site leaves [edi+edx], two give the exe order; dead string-literal locals (1, 3), an initialised int and a dead getPointer loop at the top do nothing. The exe has no such loop before the site, so the original had two index loops that fold away (e.g. in an inlined helper); not found.
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
	memset(&channel, 0, sizeof(channel));
	sprintf((char *)channel.name, "%s's_game", g_NickName.c_str());
	channel.type = 41;
	channel.minUsers = 1;
	if (g_MaxPlayers < 2) g_MaxPlayers = 6;
	strcpy((char *)channel.key, password.get());
	channel.maxUsers = g_MaxPlayers;
	char user_options[80];
	memset(user_options, 0, sizeof(user_options));
	Encode_Channel_ExInfo(user_options);
	user_options[40] = 0;
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
		Session.FUN_0069B760(profile.Get()->Country, false);
		Session.FUN_0069B7E0(profile.Get()->Field20, false);
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
