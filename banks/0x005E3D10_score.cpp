// best 77.3% raw / 97.0% normalised (8723/8702 B), no overlay; insert before 0x005E6230. Every inlining decision now equals the exe's (GenericNode::Unlink is called): the two AI-slot string loops are one inline helper used twice, and the observer test calls FUN_005E93F0. Left: frame 0x3d8 vs 0x3d0 and its slot order, the W-branch store order, one Add operand swap.
extern int DAT_008316A4;
extern bool DAT_00B77DC1;
extern int DAT_00B77E28;
extern wchar_t const DAT_00887734[];
extern NodeNameType * DAT_00AC119C;
extern NodeNameType * DAT_00AC11A0;
extern NodeNameType * DAT_00AC11A4;
extern NodeNameType * DAT_00AC11A8;
extern NodeNameType * DAT_00AC11AC;
extern NodeNameType * DAT_00AC11B0;
extern NodeNameType * DAT_00AC11B4;
bool FUN_007930A0(void);
void __fastcall FUN_00793A10(char const * name, unsigned short port);
void __fastcall FUN_00793930(char const * from, char const * to, int state);
void __fastcall FUN_007A0EA0(char const * name, int ping);
bool FUN_0077CF40(void);
int __fastcall FUN_0077ED00(Wstring const & who, int country, int color, int start, int team);
void __fastcall FUN_00791AA0(HWND window);
int __fastcall Net2SetHouseAndColor(char * who, int house, int color, int start, int team);
int Game_Mode_Total(void);
void __fastcall FUN_004E4FC0(int count);
void __fastcall FUN_004E5310(HWND dialog);
void __fastcall FUN_004E5D60(HWND dialog);
void __fastcall FUN_004E3CE0(HWND dialog, int side);
void __fastcall FUN_004E48E0(HWND dialog, int side);
void __fastcall FUN_004E53D0(HWND dialog, int slot);
void __fastcall FUN_004E5E20(HWND dialog, int slot);
void __fastcall FUN_004E5CB0(HWND dialog, int id);
void __fastcall FUN_004E5260(HWND dialog, int id);
void __fastcall FUN_004E50C0(HWND dialog, int id);
void __fastcall FUN_004E5B60(HWND dialog, int id);
int __fastcall FUN_004E37D0(int side);
int __fastcall FUN_004E41D0(int side);
int __fastcall FUN_004E5940(int id);
int __fastcall FUN_004E4E60(int id);
void __fastcall FUN_004E3A00(HWND dialog, int id);
void __fastcall FUN_004E3B00(HWND dialog, int id);
void __fastcall FUN_004E45A0(HWND dialog, int id);
void __fastcall FUN_004E4770(HWND dialog, int id);
void __fastcall FUN_004E49A0(HWND dialog, int id, int flag);
NodeNameType * __fastcall FUN_005EDFC0(WideWstring * name);
NodeNameType * __fastcall FUN_005E8BF0(wchar_t const * name);
void __fastcall FUN_005E8EA0(wchar_t * list);
void __fastcall FUN_005EBA00(HWND dialog);
void __fastcall FUN_005EBDD0(HWND dialog, int start, int end, MultiplayerGameMode * mode);
void __fastcall FUN_005E9E70(HWND dialog, int slot, bool enable);
void __fastcall FUN_005E74E0(HWND dialog);
void FUN_005EE3A0(void);
void __fastcall FUN_005EE510(int arg);
void FUN_00785360(void);
void FUN_005DAFD0(void);
WideWstring __fastcall FUN_007B7ED0(Wstring const & text);
WideWstring __fastcall FUN_007B83E0(Wstring const & text);
int FUN_005E8D40(void);
int __fastcall FUN_005E98F0(int kind);
bool __fastcall FUN_005E93F0(NodeNameType * slot);

static inline void AI_Slot_String(char * buffer)
{
	buffer[0] = 0;
	for (int s = 0; s < 8; s++) {
		char text[32];
		sprintf(text, "%d,%d,%d,", Session.Options.AISlots.Countries[s], Session.Options.AISlots.Starts[s], Session.Options.AISlots.Allies[s]);
		strcat(buffer, text);
	}
}

// FUNCTION: GAMEMD 0x005E3D10
bool DecodePubGameopt(char * options, char * name)
{
	static int _last_game_speed = -1;
	static int _last_credits = -1;
	static int _last_unit_count = -1;
	static int _last_difficulty = 1;
	static bool _last_short_game;
	static bool _last_sw_allowed;
	static bool _last_build_off_ally;
	static bool _last_mcv_redeploy;
	static bool _last_crates;

	bool do_decode = false;

	char * string = strdup(options);
	char * token = string;

	if (*token == 'Z') {
		FUN_004068E0("Decoding public OPTION_NAT\n");
		switch (token[1]) {
			case '1': {
				for (int i = 0; i < 8; i++) {
					if (stricmp(name, FUN_007350C0(Session.Slots[i]->Name)) == 0) {
						char buffer[128];
						strcpy(buffer, &token[3]);
						FUN_00793930(name, buffer, token[2] - '0');
					}
				}
				break;
			}

			case '2':
				FUN_00793A10(name, atol(&token[2]));
				break;

			case '3':
				if (!FUN_007930A0()) {
					HWND dialog = WS_Find_Dialog(IDD_WOL_GUEST);
					HWND item = GetDlgItem(dialog, 0x71f);
					if (item) {
						ShowWindow(item, SW_SHOWNORMAL);
						ShowWindow(GetDlgItem(dialog, 0x468), SW_HIDE);
#line 1326 "D:\\ra2mdpost\\netshare.cpp"
						SendMessage(item, 0x4b2, 0, (LPARAM)StringTable::LoadString("TXT_NAT_NO_PREVIEW", NULL, __FILE__, __LINE__));
					}
				}
				break;
		}
		free(string);
		return(false);
	} else if (Session.Type == GAME_INTERNET && (DAT_00B77DC1 || DAT_00B77E28 != 0)) {
	} else if (*token == 'W') {
		FUN_004068E0("Saw Waypoint info [%s]\n", token);
		token++;
		int width = 0;
		int height = 0;
		int startx = 0;
		int starty = 0;
		int count = 0;
		int x[8];
		int y[8];
		int cellx[8];
		int celly[8];

		token = strtok(token, ",");
		if (token) width = atol(token);
		token = strtok(NULL, ",");
		if (token) height = atol(token);
		token = strtok(NULL, ",");
		if (token) startx = atol(token);
		token = strtok(NULL, ",");
		if (token) starty = atol(token);
		token = strtok(NULL, ",");
		if (token) count = atol(token);

		for (int i = 0; i < 8; i++) {
			token = strtok(NULL, ",");
			if (token) x[i] = atol(token);
			token = strtok(NULL, ",");
			if (token) y[i] = atol(token);
			token = strtok(NULL, ",");
			if (token) cellx[i] = atol(token);
			token = strtok(NULL, ",");
			if (token) celly[i] = atol(token);
		}

		if (token && Scen) {
			Scen->Width = width;
			Scen->Height = height;
			Scen->StartX = startx;
			Scen->StartY = starty;
			Scen->NumberStartingPoints = count;
			for (int j = 0; j < 8; j++) {
				Scen->StartingPoints[j] = Point2D(x[j], y[j]);
				Scen->HouseHomeCells[j] = Cell(cellx[j], celly[j]);
			}
			HWND dialog = WS_Find_Dialog(IDD_WOL_GUEST);
			if (dialog || (dialog = WS_Find_Dialog(IDD_MPLAYER_GUEST)) != NULL) {
				InvalidateRect(dialog, NULL, FALSE);
			}
		}
		free(string);
		return(false);
	} else if (*token == 'O') {
		HWND dialog = WS_Find_Dialog(IDD_WOL_GUEST);
		if (dialog == NULL) {
			dialog = WS_Find_Dialog(IDD_MPLAYER_GUEST);
			if (dialog == NULL) {
				{ free(string); return(false); }
			}
		}
		int old = DAT_008316A4;
		DAT_008316A4 = (token[1] == '8') ? -1 : token[1] - '0';
		if (DAT_008316A4 != old) {
			FUN_005EBA00(dialog);
			wchar_t wide[256];
			wcscpy(wide, FUN_00735120(&token[2]));
			if (wide[0] != 0) {
				NodeNameType * node;
				{
					WideWstring text(wide);
					node = FUN_005EDFC0(&text);
				}
				if (node == NULL) {
					node = FUN_005E8BF0(wide);
					if (node == NULL) {
						{ free(string); return(false); }
					}
				}
				if (node == DAT_00AC11B4 || (FUN_005E93F0(node) && node->Slot.SpectatorFlag == -1)) {
					Session.Slots[DAT_008316A4] = node;
					FUN_004E3B00(dialog, FUN_004E37D0(DAT_008316A4));
					SendMessage(GetDlgItem(dialog, FUN_005E98F0(DAT_008316A4)), 0x4f1, 0, 1);
					int combo = FUN_004E41D0(DAT_008316A4);
					FUN_004E4770(dialog, combo);
					FUN_004E49A0(dialog, combo, 8);
				} else {
					FUN_004E3A00(dialog, FUN_004E37D0(DAT_008316A4));
					SendMessage(GetDlgItem(dialog, FUN_005E98F0(DAT_008316A4)), 0x4f1, 0, 0);
					int combo = FUN_004E41D0(DAT_008316A4);
					FUN_004E45A0(dialog, combo);
					FUN_004E49A0(dialog, combo, -2);
				}
				Session.Slots[DAT_008316A4] = node;
			}
		}
		if (DAT_008316A4 >= 0 && DAT_008316A4 != Session.LocalSlot) {
			FUN_005E9E70(dialog, DAT_008316A4, false);
		}
	} else if (*token == 'A') {
		int status = atol(&token[1]);
		if (g_pChat != NULL) {
			SetPlayerAccepted(name, status);
		} else {
			FUN_005DBE20(name, status);
		}
		free(string);
		return(false);
	} else if (*token == 'C') {
		int length = strlen(options);
		if (length < 2) {
			{ free(string); return(false); }
		}
		if (string[1] == '0') {
			FUN_004068E0("@@@@ Decoding cooperative campaign game options %s\n", options);
			int index = -1;
			DynamicVectorClass<int> picks;
			int a = 0;
			int b = 0;
			MPlayerScoreType stats[2];

			token = strtok(string + 2, ",");
			if (token) index = atol(token);
			token = strtok(NULL, ",");
			if (token) a = atol(token);
			token = strtok(NULL, ",");
			if (token) b = atol(token);
			token = strtok(NULL, ",");
			if (token) stats[1].Kills[0] = atol(token);
			token = strtok(NULL, ",");
			if (token) stats[1].Built[0] = atol(token);
			token = strtok(NULL, ",");
			if (token) stats[1].Lost[0] = atol(token);
			token = strtok(NULL, ",");
			if (token) stats[1].Score[0] = atol(token);
			token = strtok(NULL, ",");
			if (token) stats[0].Kills[0] = atol(token);
			token = strtok(NULL, ",");
			if (token) stats[0].Built[0] = atol(token);
			token = strtok(NULL, ",");
			if (token) stats[0].Lost[0] = atol(token);
			token = strtok(NULL, ",");
			if (token) stats[0].Score[0] = atol(token);

			for (int i = 0; i < 10; i++) {
				token = strtok(NULL, ",");
				if (token == NULL) break;
				int pick = atol(token);
				if (pick < 0) break;
				picks.Add(pick);
			}

			char * map = strtok(NULL, ",");
			char * mapname = strtok(NULL, ",");
			if (map == NULL || mapname == NULL) {
				free(string);
				return(false);
			}
			for (int m = 0; m <= Game_Mode_Total(); m++) {
				if (MultiplayerGameMode::Get(m) != NULL) {
					FUN_004068E0("Setting Campaign info for Mode %d: %s\n", m, FUN_007350C0(MultiplayerGameMode::Get(m)->UIName.text()));
					if (MultiplayerGameMode::Get(m)->vt_entry_58(index, picks, map, mapname)) {
						MultiplayerGameMode::Get(m)->vt_entry_5C(a, b, (unsigned)stats);
					}
				}
			}
		} else {
			FUN_004068E0("@@@@ Decoding cooperative campaign game options %s\n", options);
			for (int m = 0; m <= Game_Mode_Total(); m++) {
				if (MultiplayerGameMode::Get(m) != NULL) {
					FUN_004068E0("Handling Campaign info for Mode %d: %s\n", m, FUN_007350C0(MultiplayerGameMode::Get(m)->UIName.text()));
					if (length < 3) {
						{ free(string); return(false); }
					}
					MultiplayerGameMode::Get(m)->vt_entry_74(string[1] - '0', (unsigned)&string[2]);
				}
			}
		}
		free(string);
		return(false);
	} else if (*token == 'L') {
		WideWstring list = FUN_007B7ED0(options + 1);
		FUN_005E8EA0(list.get());
		HWND dialog = WS_Find_Dialog(IDD_WOL_GUEST);
		if (dialog) {
			int slot = FUN_005E8D40();
			if (slot == -1) {
				FUN_00791AA0(dialog);
			} else if (slot != Session.LocalSlot) {
				int id = FUN_004E37D0(slot);
				if (Session.Slots[slot]->Slot.SpectatorFlag == -1) {
					FUN_004E3B00(dialog, id);
					SendMessage(GetDlgItem(dialog, FUN_005E98F0(slot)), 0x4f1, 0, 1);
					int combo = FUN_004E41D0(slot);
					FUN_004E4770(dialog, combo);
					FUN_004E49A0(dialog, combo, 8);
					FUN_004E5260(dialog, FUN_004E4E60(slot));
					FUN_004E5CB0(dialog, FUN_004E5940(slot));
				} else {
					id = FUN_004E37D0(slot);
					if (Session.MPGameMode != NULL && !Session.MPGameMode->vt_entry_04()) {
						FUN_004E3A00(dialog, id);
					}
					SendMessage(GetDlgItem(dialog, FUN_005E98F0(slot)), 0x4f1, 0, 0);
					int combo = FUN_004E41D0(slot);
					FUN_004E45A0(dialog, combo);
					FUN_004E49A0(dialog, combo, -2);
					FUN_004E50C0(dialog, FUN_004E4E60(slot));
					FUN_004E5B60(dialog, FUN_004E5940(slot));
				}
				int me = FUN_005E8D40();
				Session.LocalSlot = me;
				for (int i = 0; i < 8; i++) {
					HWND item = GetDlgItem(dialog, FUN_005E98F0(i));
					EnableWindow(item, FALSE);
					FUN_005E9E70(dialog, i, false);
				}
				HWND item = GetDlgItem(dialog, FUN_005E98F0(me));
				EnableWindow(item, TRUE);
				FUN_005E9E70(dialog, me, true);
				FUN_004E3CE0(dialog, Session.LocalSlot);
				FUN_004E48E0(dialog, Session.LocalSlot);
				FUN_004E53D0(dialog, Session.LocalSlot);
				FUN_004E5E20(dialog, Session.LocalSlot);
			}
		}
		free(string);
		return(false);
	} else if (*token == 'G') {
		if (WS_Find_Dialog(IDD_WOL_GUEST)) {
			FUN_005E36E0();
		}
		free(string);
		return(false);
	} else if (*token == 'K') {
		if (WS_Find_Dialog(IDD_WOL_GAMEOPT) || WS_Find_Dialog(IDD_MPLAYER_HOST)) {
			switch (token[1]) {
				case '1':
					FUN_005E39C0(name, 1);
					break;
				case '2':
					FUN_005E39C0(name, 2);
					break;
				default:
					FUN_005E39C0(name, 0);
					break;
			}
		}
		free(string);
		return(false);
	} else if (*token == 'P') {
		FUN_004068E0("Got Ping game options string. %s\n", token);
		char * p = &token[3];
		if (Session.Type == GAME_INTERNET) {
			while (*p) {
				char player[32];
				char number[32];
				int i = 0;
				while (*p && *p != ',') {
					char c = *p++;
					player[i++] = c;
				}
				player[i] = 0;
				i = 0;
				if (*p) p++;
				while (*p && *p != ',') {
					char c = *p++;
					number[i++] = c;
				}
				number[i] = 0;
				int ping = atol(number);
				if (*p) p++;
				FUN_007A0EA0(player, ping);
			}
		}
		free(string);
		return(false);
	} else {

	int topid = FUN_00775B20();
	if (topid != IDD_MPLAYER_HOST && topid != IDD_MPLAYER_GUEST) {
		SendDlgItemMessage(GameoptWindow(), IDC_USERS, OD_DISABLEPAINT, 0, 1);
	}
	FUN_004068E0("Decoding game options %s\n", options);

	token = strtok(token, ",");
	if (token) Seed = atol(token);
	token = strtok(NULL, ",");
	if (token) Options.GameSpeed = atol(token);
	token = strtok(NULL, ",");
	if (token) Session.Options.GameSpeed = atol(token);
	token = strtok(NULL, ",");
	if (token) Session.Options.Credits = atol(token);
	token = strtok(NULL, ",");
	if (token) Session.Options.UnitCount = atol(token);
	token = strtok(NULL, ",");
	if (token) Session.Options.ShortGame = atol(token) != 0;
	token = strtok(NULL, ",");
	if (token) Session.Options.SWAllowed = atol(token) != 0;
	token = strtok(NULL, ",");
	if (token) Session.Options.BuildOffAlly = atol(token) != 0;
	token = strtok(NULL, ",");
	if (token) Session.Options.MCVRedeploy = atoi(token) != 0;
	token = strtok(NULL, ",");
	if (token) Session.Options.Goodies = atoi(token) != 0;
	token = strtok(NULL, ",");
	if (token) Session.Options.MPModeIndex = atol(token);
	token = strtok(NULL, ",");
	if (token != NULL) {
	Options.CampDifficulty = atol(token);

	bool same_scenario = true;
	token = strtok(NULL, ",");
	WideWstring description = (Session.Type == GAME_INTERNET && FUN_0077CF40()) ? WideWstring(DAT_00887734) : FUN_007B83E0(token);

	token = strtok(NULL, ",");
	int old_max = Session.MaxPlayers;
	if (token) {
		Session.MaxPlayers = atoi(token);
		if (Session.MaxPlayers != old_max) same_scenario = false;
	}

	token = strtok(NULL, ",");
	bool old_official = Session.ScenarioIsOfficial;
	if (token) {
		Session.ScenarioIsOfficial = atoi(token) != 0;
		if (Session.ScenarioIsOfficial != old_official) same_scenario = false;
	}

	token = strtok(NULL, ",");
	unsigned int old_length = Session.ScenarioFileLength;
	if (token) {
		Session.ScenarioFileLength = atoi(token);
		if (Session.ScenarioFileLength != old_length) same_scenario = false;
	}

	token = strtok(NULL, ",");
	if (token) {
		if (stricmp(Session.ScenarioFileName, token) != 0) {
			same_scenario = false;
		}
		strncpy(Session.ScenarioFileName, token, sizeof(Session.ScenarioFileName));
		strcpy(Scen->ScenarioName, Session.ScenarioFileName);
	}

	char * digest = strtok(NULL, ":");
	if (digest) {
		if (strcmp(Session.ScenarioDigest, digest) != 0) {
			same_scenario = false;
		}
		strncpy(Session.ScenarioDigest, digest, sizeof(Session.ScenarioDigest) - 1);
	}

	MultiplayerGameMode * mode = MultiplayerGameMode::Get(Session.Options.MPModeIndex);
	if (mode != Session.MPGameMode) {
		same_scenario = false;
	}
	if (topid == IDD_MPLAYER_GUEST || topid == IDD_WOL_GUEST) {
		FUN_005EBDD0(GameoptWindow(), old_max, Session.MaxPlayers, mode);
	}

	bool found = true;
	if (!same_scenario || wcslen(Session.Options.ScenarioDescription) == 0) {
		found = false;
		same_scenario = false;
		for (int i = 0; i < Session.Scenarios.Count(); i++) {
			MultiMission * scenario = Session.Scenarios[i];
			if (stricmp(Session.ScenarioFileName, scenario->Get_Filename()) == 0) {
				CCFileClass file(scenario->Get_Filename());
				if (file.Is_Available() && file.Size() == Session.ScenarioFileLength) {
					INIClass ini;
					ini.Load(file);
					char scenario_digest[32];
					ini.Get_String("Digest", "1", "No digest here mate. Nope.", scenario_digest, sizeof(scenario_digest));
					if (Session.ScenarioDigest[0] == 0 || strcmp(Session.ScenarioDigest, scenario_digest) == 0) {
						wcscpy(Session.Options.ScenarioDescription, scenario->Description());
						found = true;
					}
				}
			}
		}
		if (!found && description.length() > 0) {
			wcscpy(Session.Options.ScenarioDescription, description.get());
		}
		if (Session.MPGameMode != NULL) {
			Session.MPGameMode->vt_entry_24();
		}
	}

	mode = MultiplayerGameMode::Get(Session.Options.MPModeIndex);
	if (mode != Session.MPGameMode) {
		if (Session.MPGameMode != NULL) {
			Session.MPGameMode->vt_entry_9C();
		}
		if (mode != NULL) {
			mode->vt_entry_20();
		}
	}
	Session.MPGameMode = mode;
	if (mode != NULL && !same_scenario) {
		mode->UnfixAlliances();
		FUN_005EE3A0();
	}

	HWND dialog = GameoptWindow();
	FUN_005E2EF0(dialog, mode);
	FUN_005E2F60(dialog);

	Scen->Scenario = -1;
	Frame = 0;
	Session.CommProtocol = COMM_PROTOCOL_MULTI_E_COMP;

	if (old_official != Session.ScenarioIsOfficial || old_length != Session.ScenarioFileLength) {
		same_scenario = false;
	}

	if (!same_scenario) {
		FUN_004068E0("Not same scenario...\n");
		if (Session.ScenarioIsOfficial && !found && stricmp(Session.ScenarioFileName, "RandMap.Sed") != 0) {
			if (GameoptWindow()) {
#line 2068 "D:\\ra2mdpost\\netshare.cpp"
				PMessagePrintf(-1, StringTable::LoadString("GUI:JoinerNoMap", NULL, __FILE__, __LINE__), Session.Options.ScenarioDescription);
			}
			DAT_00AC11C2 = false;
			EnableWindow(GetDlgItem(GameoptWindow(), 0x59f), FALSE);
			InvalidateRect(GetDlgItem(GameoptWindow(), 0x59f), NULL, FALSE);
			SendPublicGameopts("K0");
		} else {
			DAT_00AC11C2 = true;
			EnableWindow(GetDlgItem(GameoptWindow(), 0x59f), TRUE);
			InvalidateRect(GetDlgItem(GameoptWindow(), 0x59f), NULL, FALSE);
			if (!Session.ScenarioIsOfficial && !found && stricmp(Session.ScenarioFileName, "RandMap.Sed") != 0) {
				if (GameoptWindow()) {
#line 2084 "D:\\ra2mdpost\\netshare.cpp"
					PMessagePrintf(-1, StringTable::LoadString("GUI:JoinerMapTransfer", NULL, __FILE__, __LINE__), Session.Options.ScenarioDescription);
				}
				SendPublicGameopts("K2");
			} else {
				SendPublicGameopts("K1");
			}
		}
		FUN_005E74E0(GameoptWindow());
		FUN_004E4FC0((Session.MPGameMode != NULL && Session.MPGameMode->vt_entry_04() && Scen->NumCoopHumanStartSpots <= Session.MaxPlayers) ? Scen->NumCoopHumanStartSpots : Session.MaxPlayers);
		if (topid == IDD_MPLAYER_GUEST || topid == IDD_WOL_GUEST) {
			FUN_004E5310(FUN_00775B10());
			FUN_004E5D60(FUN_00775B10());
			FUN_005EE510(-1);
		}
	}

	if (digest == NULL) {
		PMessagePrintf(-1, L"Gameopt parse failed");
	}

	unsigned int address = 0;
	token = strtok(NULL, ",:");
	while (token != NULL) {
		char * handle = token;
		if (strcmp(handle, "@") == 0) break;

		token = strtok(NULL, ",:");
		if (token == NULL) break;
		int house = atol(token);

		token = strtok(NULL, ",:");
		if (token == NULL) break;
		int color = atol(token);

		token = strtok(NULL, ",:");
		if (token == NULL) break;
		int start = atol(token);

		token = strtok(NULL, ",:");
		if (token == NULL) break;
		int team = atol(token);

		token = strtok(NULL, ",:");
		if (token == NULL) break;
		sscanf(token, "%X", &address);

		if (address != 0 && Session.Type == GAME_INTERNET) {
			int i;
			for (i = 0; i < 8; i++) {
				if (stricmp(handle, FUN_007350C0(Session.Slots[i]->Name)) == 0) {
					unsigned long net = 0;
					unsigned char node[6];
					memset(node, 0, 6);
					*(unsigned long *)node = address;
					Session.PlayerLocalAddress[i].Set_Address((unsigned char *)&net, node, 0);
					token = strtok(NULL, ",:");
					if (token) {
						unsigned int value;
						sscanf(token, "%X", &value);
						Session.PlayerFirewall[i] = value;
						token = strtok(NULL, ",:");
						if (token) {
							unsigned int port;
							sscanf(token, "%X", &port);
							Session.PlayerPortBase[i] = port;
						}
					}
					break;
				}
			}
			if (i == 8) {
				strtok(NULL, ",:");
				strtok(NULL, ",:");
			}
		} else {
			strtok(NULL, ",:");
			strtok(NULL, ",:");
		}

		IsColorChangePending = true;
		if (stricmp(handle, Session.Handle) == 0 && color != Session.PrefColor) {
			if (!IsColorChangePending) {
#line 2230 "D:\\ra2mdpost\\netshare.cpp"
				PMessagePrintf(-1, StringTable::LoadString("TXT_COLOR_IN_USE", NULL, __FILE__, __LINE__));
			}
			Session.PrefColor = color;
		}
		if (stricmp(handle, Session.Handle) == 0) {
			IsColorChangePending = false;
		}

		if (g_pChat != NULL && FUN_0077ED00(handle, house, color, start, team) ||
			g_pChat == NULL && Net2SetHouseAndColor(handle, house, color, start, team)) {
			do_decode = true;
		}
		token = strtok(NULL, ",:");
	}

	char old_ai[84];
	AI_Slot_String(old_ai);
	int ai_players = 0;
	for (int k = 0; k < 8; k++) {
		token = strtok(NULL, ",:");
		if (token == NULL) break;
		Session.Options.AISlots.Difficulties[k] = atol(token);
		token = strtok(NULL, ",:");
		if (token == NULL) break;
		Session.Options.AISlots.Countries[k] = atol(token);
		token = strtok(NULL, ",:");
		if (token == NULL) break;
		Session.Options.AISlots.Colors[k] = atol(token);
		if (Session.Options.AISlots.Countries[k] != -1) ai_players++;
		token = strtok(NULL, ",:");
		if (token == NULL) break;
		Session.Options.AISlots.Starts[k] = atol(token);
		token = strtok(NULL, ",:");
		if (token == NULL) break;
		Session.Options.AISlots.Allies[k] = atol(token);
	}
	Session.Options.AIPlayers = ai_players;
	char new_ai[84];
	AI_Slot_String(new_ai);
	if (strcmp(old_ai, new_ai) != 0) {
		do_decode = true;
	}

	free(string);

	if (!(Session.Type == GAME_INTERNET && FUN_0077CF40())) {
		DisplayGameopts(GameoptWindow(), FALSE);
	}

	if (_last_game_speed != Session.Options.GameSpeed) do_decode = true;
	if (_last_credits != Session.Options.Credits) do_decode = true;
	if (_last_unit_count != Session.Options.UnitCount) do_decode = true;
	if (_last_short_game != Session.Options.ShortGame) do_decode = true;
	if (_last_sw_allowed != Session.Options.SWAllowed) do_decode = true;
	if (_last_build_off_ally != Session.Options.BuildOffAlly) do_decode = true;
	if (_last_mcv_redeploy != Session.Options.MCVRedeploy) do_decode = true;
	if (_last_crates != Session.Options.Goodies) do_decode = true;
	if (_last_difficulty != Options.CampDifficulty) do_decode = true;

	if (!same_scenario || do_decode) {
		FUN_005E3AB0();
	}

	if (topid != IDD_MPLAYER_HOST && topid != IDD_MPLAYER_GUEST) {
		SendDlgItemMessage(GameoptWindow(), IDC_USERS, OD_DISABLEPAINT, 0, 0);
	}

	if (g_pChat != NULL) {
		FUN_00785360();
	} else {
		FUN_005DAFD0();
	}

	_last_game_speed = Session.Options.GameSpeed;
	_last_credits = Session.Options.Credits;
	_last_unit_count = Session.Options.UnitCount;
	_last_short_game = Session.Options.ShortGame;
	_last_sw_allowed = Session.Options.SWAllowed;
	_last_build_off_ally = Session.Options.BuildOffAlly;
	_last_mcv_redeploy = Session.Options.MCVRedeploy;
	_last_crates = Session.Options.Goodies;
	_last_difficulty = Options.CampDifficulty;

	return(!same_scenario);
	}
	}

	free(string);
	return(false);
}
