// 0x00789B60 61.9% raw / 98.3% normalised natural (2321/2312), n10 agent 23 Nov. Base: today's main wonline.cpp with (a) the stand-in trio FUN_007EF720/FUN_007EF738/FUN_007F7624_Class replaced by the SimpleWonlineDialogControl block below (ctor 0x0077D1D0, dtor 0x0077D1F0, F4 0x0077D200 defined at the top; the later class at ~4046 deleted; casts dropped), which resolves the IDCANCEL dtor call to 0x0077D1F0 but flips 0x00781480, 0x00783C80/0x007A3100, 0x00788180, 0x007A5890 by declaration weight (wol2's moves of WonlineString/CreateGame classes and the FUN_00779B50.. decl block to the top did not rebalance on today's main); (b) this function moved to its address position after 'extern int DAT_008316A4;' (no effect, all 470 others MATCH in that state). Remaining: ctor 0x007AC610 inlined (flips with 4 trivial inline sites anywhere after the control; not with in-class inline ctor, : Text()/base init forms, profile-> instead of profile.Get()->); lea ebx,[edx+edi] follows a FUNCTION-WIDE count of live getPointer/SR address loops (two extra real g_UserList getPointer loops ANYWHERE, even at the end, flip every such lea; dead getPointer loops, dc++ loops, SR loops with a constant base, w*3 SR loops, non-loop getPointer uses, if(0) loops, 4 extra string literals, 0..40 initialised locals do not; 54 locals do); with 4 end probes + 2 real loops both flip together.
//
// --- class block replacing the stand-in trio (top of wonline.cpp) ---
// class SimpleWonlineDialogControl : public OwnerDraw::SimpleDialogControl
// {
// 	public:
// 		SimpleWonlineDialogControl(void);
// 		virtual ~SimpleWonlineDialogControl(void);
// 		virtual bool F4(void);
// };
// 
// class WonlineStringDialogControl : public SimpleWonlineDialogControl
// {
// 	public:
// 		WonlineStringDialogControl(void);
// 		virtual ~WonlineStringDialogControl(void) {}
// 
// 		Wstring Text;
// };
// 
// class CreateGameDialogControl : public WonlineStringDialogControl
// {
// 	public:
// 		CreateGameDialogControl(bool limit_resolution, bool observer)
// 			: Flag10(false), Flag11(false), Flag12(limit_resolution), Flag13(observer) {}
// 		virtual ~CreateGameDialogControl(void) {}
// 
// 		bool Flag10;
// 		bool Flag11;
// 		bool Flag12;
// 		bool Flag13;
// };
// 
// Wstring __fastcall FUN_00779B50(void);
// Wstring __fastcall FUN_00779DD0(void);
// void __fastcall FUN_00779C90(Wstring const & value);
// bool FUN_00779730(void);
// int __fastcall FUN_00797640(int sku);
// int __fastcall StoreLogin(char const * login, char const * password, bool plain, int locale);
// void __fastcall FUN_0052FEC0(Surface * surface, bool flag);
// void Title_Screen_Restore(bool flag);
// void FUN_0072C600(void);
// void FUN_0072C650(void);

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
