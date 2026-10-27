// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"
#include "geometry.h"
#include "modemhst.h"
#include "_rules.h"
#include "_timer.h"
#include "addon.h"
#include "ccrand.h"
#include "conquer.h"
#include "data.h"
#include "dbgprint.h"
#include "globals.h"
#include "goptions.h"
#include "houstype.h"
#include "init.h"
#include "language\language.h"
#include "lzopipe.h"
#include "mapgen.h"
#include "mplayer.h"
#include "msgbox.h"
#include "msgloop.h"
#include "netshare.h"
#include "nulldlg.h"
#include "nullmgr.h"
#include "ownrdraw.h"
#include "rules.h"
#include "scenario.h"
#include "sendfile.h"
#include "stimer.h"
#include "timer.h"
#include "trim.h"
#include "winstub.h"
#include "xpipe.h"

#undef LoadString
#include <stddef.h>
#include <wchar.h>
#include "stringtable.h"

// SYNTHETIC: GAMEMD 0x005B75D0 SYMBOL
// _$E54

char * __fastcall FUN_007350C0(wchar_t const * text);
wchar_t * __fastcall FUN_00735120(char const * narrow);
void __fastcall FUN_00694760(char * name, int type, bool a, bool b);
wchar_t * __fastcall wcstrim(wchar_t * buffer);

namespace OwnerDraw {
char * __fastcall WideToCharString(char * dest, wchar_t const * src, int count);
}

// Initialisers of geometry.h constants.
// SYNTHETIC: GAMEMD 0x005B75F0 SYMBOL
// _$E2
// SYNTHETIC: GAMEMD 0x005B7620 SYMBOL
// _$E5
// SYNTHETIC: GAMEMD 0x005B7640 SYMBOL
// _$E8
// SYNTHETIC: GAMEMD 0x005B7660 SYMBOL
// _$E11
// SYNTHETIC: GAMEMD 0x005B7680 SYMBOL
// _$E14
// SYNTHETIC: GAMEMD 0x005B76A0 SYMBOL
// _$E17
// SYNTHETIC: GAMEMD 0x005B76C0 SYMBOL
// _$E20
// SYNTHETIC: GAMEMD 0x005B7700 SYMBOL
// _$E23
// SYNTHETIC: GAMEMD 0x005B7730 SYMBOL
// _$E26
// SYNTHETIC: GAMEMD 0x005B7760 SYMBOL
// _$E29
// SYNTHETIC: GAMEMD 0x005B7790 SYMBOL
// _$E32
// SYNTHETIC: GAMEMD 0x005B77A0 SYMBOL
// _$E35
// SYNTHETIC: GAMEMD 0x005B77C0 SYMBOL
// _$E38

ModemHost * _ModemHost;

BOOL CALLBACK Modem_Host_Dialog_Proc(HWND window, UINT message, WPARAM wparam, LPARAM lparam);

// FUNCTION: GAMEMD 0x005B77E0
bool ModemHost::Dialog(void)
{
	bool started = false;

	DebugString("ModemHost.Dialog(enter)\n");

	RC = -1;
	_Dialog = OwnerDraw::Begin_Dialog(IDD_MODEM_HOST, Modem_Host_Dialog_Proc, 0);

	if (_Dialog != NULL) {
		SetWindowLong(_Dialog, DWL_USER, (LONG)this);

		InitDialog();
		OwnerDraw::Display_Dialog(_Dialog);

		Changed = false;
		Transmit = true;
		TransmitTime = TickCount;
		TheirResponseTime = 10000;
		LastMsgTime = TickCount;
		MsgTimeout = 20 * TIMER_SECOND;
		TimingTime = 0;
		GameOptions = false;
		TheirName[0] = '\0';
		TheirColor = -1;
		TheirHouse = HOUSE_NONE;
		_ModemHost = this;

		CheatCheck = RulesClass::Get_Rule_Unique_ID();
		RulesClass::Load_Art_INI();
		ArtCheatCheck = RulesClass::Get_Art_Unique_ID();
		AICheatCheck = RulesClass::Get_AI_Unique_ID();

		while (true) {
			RC = -1;
			while (RC < 0) {
				if (OwnerDraw::Dialog_Message_Handler() == true) {
					break;
				}
				Send(false);
				if (Receive(false) == true) {
					break;
				}
				Title_Screen_Restore();
			}

			if (RC != IDOK) {
				break;
			}
			ShowWindow(_Dialog, SW_HIDE);
			if (StartGame(false) == true) {
				started = true;
				break;
			}
			ShowWindow(_Dialog, SW_SHOW);
		}

		OwnerDraw::End_Dialog(_Dialog);
		_ModemHost = NULL;
		if (Changed == true) {
			Session.Write_MultiPlayer_Settings();
		}
	}

	if (RC == IDCANCEL) {
		SignOff();
	}

	DebugString("ModemHost.Dialog(exit %s)\n", started == true ? "true" : "false");
	return(started);
}

// FUNCTION: GAMEMD 0x005B8CE0
bool ModemHost::ReadDialog(void)
{
	DebugString("ModemHost.ReadDialog(enter)\n");

	int waypoint_count = RandomMapWaypointCount(Session.Options.ScenarioIndex);

	int player_count = 2;
	HWND handle = GetDlgItem(_Dialog, IDC_AIPLAYERS);
	if (handle) {
		player_count += Slider_GetPos(handle);
	}

	if (waypoint_count < player_count) {
		wchar_t buffer[256];
		swprintf(buffer, StringTable::LoadString("TXT_SCENARIO_TOO_SMALL", NULL, "D:\\ra2mdpost\\ModemHst.cpp", 0x2f7), waypoint_count);
		WWMessageBox().Process(buffer, StringTable::LoadString("TXT_OK", NULL, "D:\\ra2mdpost\\ModemHst.cpp", 0x2f8), NULL, NULL);
		return(false);
	}

	wchar_t name[20];
	handle = GetDlgItem(_Dialog, IDC_NAME);
	if (handle) {
		SendMessage(handle, 0x4b3, 0x14, (LPARAM)name);
		OwnerDraw::WideToCharString(Session.Handle, name, -1);
	}
	DebugString("Name: %ls\n", Session.Handle);

	handle = GetDlgItem(_Dialog, IDC_MODEM_YOURCOLOR);
	if (handle) {
		Session.PrefColor = ComboBox_GetCurSel(handle);
	}
	DebugString("Color: %d\n", Session.PrefColor);

	handle = GetDlgItem(_Dialog, IDC_MODEM_YOURSIDE);
	if (handle) {
		int row = ComboBox_GetCurSel(handle);
		Session.House = ComboBox_GetItemData(handle, row);
	}

	handle = GetDlgItem(_Dialog, IDC_MODEM_UNITCOUNT);
	if (handle) {
		Session.Options.UnitCount = Slider_GetPos(handle);
	}
	DebugString("UnitCount: %d\n", Session.Options.UnitCount);

	handle = GetDlgItem(_Dialog, IDC_TECHLEVEL);
	if (handle) {
		BuildLevel = Slider_GetPos(handle);
	}
	DebugString("TechLevel: %d\n", BuildLevel);

	handle = GetDlgItem(_Dialog, IDC_CREDITS);
	if (handle) {
		Session.Options.Credits = Slider_GetPos(handle);
	}
	DebugString("Credits: %d\n", Session.Options.Credits);

	handle = GetDlgItem(_Dialog, IDC_AIPLAYERS);
	if (handle) {
		Session.Options.AIPlayers = Slider_GetPos(handle);
	}
	DebugString("AIPlayers: %d\n", Session.Options.AIPlayers);

	handle = GetDlgItem(_Dialog, IDC_GAME_SPEED_SLIDER);
	if (handle) {
		Session.Options.GameSpeed = 6 - Slider_GetPos(handle);
	}
	DebugString("Game Speed: %d\n", Session.Options.GameSpeed);

	handle = GetDlgItem(_Dialog, IDC_AILEVEL_SLIDER);
	if (handle) {
		Session.Options.AIDifficulty = (DiffType)Slider_GetPos(handle);
	}
	DebugString("AIDifficulty: %d\n", Session.Options.AIDifficulty);

	handle = GetDlgItem(_Dialog, IDC_MODEM_REDEPLOY_MCV);
	if (handle) {
		Session.Options.MCVRedeploy = Button_GetCheck(handle) == BST_CHECKED;
	}
	DebugString("MCVRedeploy: %s\n", Session.Options.MCVRedeploy == true ? "ON" : "OFF");

	handle = GetDlgItem(_Dialog, IDC_MODEM_BASES);
	if (handle) {
		Session.Options.Bases = Button_GetCheck(handle) == BST_CHECKED;
	}
	DebugString("Bases: %s\n", Session.Options.Bases == true ? "ON" : "OFF");

	handle = GetDlgItem(_Dialog, IDC_GOODIES);
	if (handle) {
		Session.Options.Goodies = Button_GetCheck(handle) == BST_CHECKED;
	}
	DebugString("Crates: %s\n", Session.Options.Goodies == true ? "ON" : "OFF");

	handle = GetDlgItem(_Dialog, IDC_MODEM_FOG);
	if (handle) {
		Session.Options.FogOfWar = Button_GetCheck(handle) == BST_CHECKED;
	}
	DebugString("FogOfWar: %s\n", Session.Options.FogOfWar == true ? "ON" : "OFF");

	handle = GetDlgItem(_Dialog, IDC_BRIDGE_DESTROY);
	if (handle) {
		Session.Options.BridgeDestruction = Button_GetCheck(handle) == BST_CHECKED;
	}
	DebugString("BridgeDestroy: %s\n", Session.Options.BridgeDestruction == true ? "ON" : "OFF");

	handle = GetDlgItem(_Dialog, IDC_HARVTRUCE);
	if (handle) {
		Session.Options.HarvTruce = Button_GetCheck(handle) == BST_CHECKED;
	}
	DebugString("Harvester Truce: %s\n", Session.Options.HarvTruce == true ? "ON" : "OFF");

	handle = GetDlgItem(_Dialog, IDC_SHORT_GAME);
	if (handle) {
		Session.Options.ShortGame = Button_GetCheck(handle) == BST_CHECKED;
	}
	DebugString("Short Game: %s\n", Session.Options.ShortGame == true ? "ON" : "OFF");

	handle = GetDlgItem(_Dialog, 0x69a);
	if (handle) {
		Session.Options.SWAllowed = Button_GetCheck(handle) == BST_CHECKED;
	}
	DebugString("Short Game: %s\n", Session.Options.ShortGame == true ? "ON" : "OFF");

	handle = GetDlgItem(_Dialog, 0x69d);
	if (handle) {
		Session.Options.BuildOffAlly = Button_GetCheck(handle) == BST_CHECKED;
	}
	DebugString("Short Game: %s\n", Session.Options.ShortGame == true ? "ON" : "OFF");

	handle = GetDlgItem(_Dialog, IDC_MULTI_ENGINEER);
	if (handle) {
		Session.Options.CrapEngineers = Button_GetCheck(handle) == BST_CHECKED;
	}
	DebugString("Crap Engineers: %s\n", Session.Options.CrapEngineers == true ? "ON" : "OFF");

	DebugString("ModemHost.ReadDialog(exit)\n");
	return(true);
}

// FUNCTION: GAMEMD 0x005B9250
void ModemHost::SendCurrentMessage(void)
{
	wchar_t buf[256];

	memset(buf, 0, sizeof(buf));

	HWND handle = GetDlgItem(_Dialog, IDC_INPUT);
	if (handle) {
		SendMessage(handle, 0x4b3, 0x100, (LPARAM)buf);
		SendMessage(handle, 0x4b2, 0, (LPARAM)L"");
		if (wcslen(buf) && buf[0] != '\r') {
			wcstrim(buf);
			SendAMessage(buf);
		}
	}
}

// FUNCTION: GAMEMD 0x005B93F0
void ModemHost::SetHouse(void)
{
	HWND handle = GetDlgItem(_Dialog, IDC_MODEM_YOURSIDE);
	if (handle) {
		int i = ComboBox_GetCurSel(handle);
		if (i != CB_ERR) {
			i = ComboBox_GetItemData(handle,i);
			if (i != CB_ERR) {
				Session.House = i;
				Transmit = true;
				Changed = true;
			}
		}
	}
}

// FUNCTION: GAMEMD 0x005B9450
void ModemHost::SetColor(void)
{
	HWND handle = GetDlgItem(_Dialog, IDC_MODEM_YOURCOLOR);
	if (handle) {
		int color = ComboBox_GetCurSel(handle);
		if (color != CB_ERR) {
			Session.PrefColor = color;
			Transmit = true;
			Changed = true;
			SendMessage(handle, OD_SETCOLOR, 0, (color >= 0 && color < MAX_PLAYERS ? PlayerColorTable[color] : ColorMe));
		}
	}
}

// FUNCTION: GAMEMD 0x005B94C0
void ModemHost::SetName(bool update)
{
	wchar_t namebuf[20];
	HWND handle = GetDlgItem(_Dialog, IDC_NAME);
	if (handle) {
		SendMessage(handle, 0x4b3, 0x14, (LPARAM)namebuf);
		wcstrim(namebuf);
		if (!update) {
			SendMessage(handle, 0x4b2, 0, (LPARAM)namebuf);
		}
		strcpy(Session.Handle, FUN_007350C0(namebuf));
		Transmit = true;
		Changed = true;
	}
}

// FUNCTION: GAMEMD 0x005B9550
void ModemHost::SetOther(const wchar_t * name, int color, int house)
{
	if (wcsicmp(TheirName, name) != 0 || TheirColor != color || TheirHouse != house) {
		wchar_t buf[80];

		wcscpy(TheirName, name);
		TheirColor = color;
		TheirHouse = house;

		HWND handle = GetDlgItem(_Dialog, IDC_MODEM_OPPONENT);
		if (handle) {
			swprintf(buf, L"%s [%s]", name, HouseTypes[house]->UIName);
			SendMessage(handle, 0x4b2, 0, (LPARAM)buf);
			SendMessage(handle, OD_SETCOLOR, 0, (color >= 0 && color < MAX_PLAYERS ? PlayerColorTable[color] : ColorMe));
		}

		VocClass::PlayGlobal(Rule->OptionsChanged, 1.0f, 0x2000);
	}

	Transmit = true;
}

// FUNCTION: GAMEMD 0x005B9650
void ModemHost::SetUnitCount(void)
{
	HWND handle = GetDlgItem(_Dialog, IDC_MODEM_UNITCOUNT);
	if (handle) {
		Session.Options.UnitCount = Slider_GetPos(handle);
		Transmit = true;
	}
}

// FUNCTION: GAMEMD 0x005B9680
void ModemHost::SetBuildLevel(void)
{
	HWND handle = GetDlgItem(_Dialog, IDC_TECHLEVEL);
	if (handle) {
		BuildLevel = Slider_GetPos(handle);
		Transmit = true;
	}
}

// FUNCTION: GAMEMD 0x005B96B0
void ModemHost::SetCredits(void)
{
	HWND handle = GetDlgItem(_Dialog, IDC_CREDITS);
	if (handle) {
		Session.Options.Credits = Slider_GetPos(handle);
		Transmit = true;
	}
}

// FUNCTION: GAMEMD 0x005B96E0
void ModemHost::SetAIPlayers(void)
{
	HWND handle = GetDlgItem(_Dialog, IDC_AIPLAYERS);
	if (handle) {
		Session.Options.AIPlayers = Slider_GetPos(handle);
		Transmit = true;
	}
}

// FUNCTION: GAMEMD 0x005B9710
void ModemHost::SetGameSpeed(void)
{
	HWND handle = GetDlgItem(_Dialog, IDC_GAME_SPEED_SLIDER);
	if (handle) {
		Session.Options.GameSpeed = 6 - Slider_GetPos(handle);
		Transmit = true;
	}
}

// FUNCTION: GAMEMD 0x005B9750
void ModemHost::SetAIDifficulty(void)
{
	HWND handle = GetDlgItem(_Dialog, IDC_AILEVEL_SLIDER);
	if (handle) {
		Session.Options.AIDifficulty = (DiffType)Slider_GetPos(handle);
		Transmit = true;
	}
}

// FUNCTION: GAMEMD 0x005B9780
void ModemHost::SetMCVRedeploy(void)
{
	HWND handle = GetDlgItem(_Dialog, IDC_MODEM_REDEPLOY_MCV);
	if (handle) {
		Session.Options.MCVRedeploy = Button_GetCheck(handle) == TRUE;
		Transmit = true;
	}
}

// FUNCTION: GAMEMD 0x005B97C0
void ModemHost::SetBases(void)
{
	HWND handle = GetDlgItem(_Dialog, IDC_MODEM_BASES);
	if (handle) {
		Session.Options.Bases = Button_GetCheck(handle) == TRUE;
		if (!Session.Options.Bases) {
			HWND handle = GetDlgItem(_Dialog, IDC_SHORT_GAME);
			if (handle) {
				Session.Options.ShortGame = false;
				SendDlgItemMessage(_Dialog, IDC_SHORT_GAME, BM_SETCHECK, FALSE, 0);
			}
		}
		Transmit = true;
	}
}

// FUNCTION: GAMEMD 0x005B9830
void ModemHost::SetGoodies(void)
{
	HWND handle = GetDlgItem(_Dialog, IDC_GOODIES);
	if (handle) {
		Session.Options.Goodies = Button_GetCheck(handle) == TRUE;
		Transmit = true;
	}
}

// FUNCTION: GAMEMD 0x005B9870
void ModemHost::SetFogOfWar(void)
{
	HWND handle = GetDlgItem(_Dialog, IDC_MODEM_FOG);
	if (handle) {
		Session.Options.FogOfWar = Button_GetCheck(handle) == TRUE;
		Transmit = true;
	}
}

// FUNCTION: GAMEMD 0x005B98B0
void ModemHost::SetBridgeDestruction(void)
{
	HWND handle = GetDlgItem(_Dialog, IDC_BRIDGE_DESTROY);
	if (handle) {
		Session.Options.BridgeDestruction = Button_GetCheck(handle) == TRUE;
		Transmit = true;
	}
}

// FUNCTION: GAMEMD 0x005B98F0
void ModemHost::SetHarvTruce(void)
{
	HWND handle = GetDlgItem(_Dialog, IDC_HARVTRUCE);
	if (handle) {
		Session.Options.HarvTruce = Button_GetCheck(handle) == TRUE;
		Transmit = true;
	}
}

// FUNCTION: GAMEMD 0x005B9930
void ModemHost::SetShortGame(void)
{
	HWND handle = GetDlgItem(_Dialog, IDC_SHORT_GAME);
	if (handle) {
		Session.Options.ShortGame = Button_GetCheck(handle) == TRUE;
		Transmit = true;
		if (Session.Options.ShortGame) {
			HWND handle = GetDlgItem(_Dialog, IDC_MODEM_BASES);
			if (handle) {
				Session.Options.Bases = true;
				SendDlgItemMessage(_Dialog, IDC_MODEM_BASES, BM_SETCHECK, TRUE, 0);
			}
		}
	}
}

// FUNCTION: GAMEMD 0x005B99A0
void ModemHost::SetSWAllowed(void)
{
	HWND handle = GetDlgItem(_Dialog, 0x69a);
	if (handle) {
		Session.Options.SWAllowed = Button_GetCheck(handle) == TRUE;
		Transmit = true;
	}
}

// FUNCTION: GAMEMD 0x005B99E0
void ModemHost::SetBuildOffAlly(void)
{
	HWND handle = GetDlgItem(_Dialog, 0x69d);
	if (handle) {
		Session.Options.ShortGame = Button_GetCheck(handle) == TRUE;
		Transmit = true;
	}
}

// FUNCTION: GAMEMD 0x005B9A20
void ModemHost::SetCrapEngineers(void)
{
	HWND handle = GetDlgItem(_Dialog, IDC_MULTI_ENGINEER);
	if (handle) {
		Session.Options.CrapEngineers = Button_GetCheck(handle) == TRUE;
		Transmit = true;
	}
}

static inline LRESULT _Send_Dlg_Item_Message(HWND dialog, int id, UINT msg, WPARAM wparam, LPARAM lparam)
{
	HWND handle = GetDlgItem(dialog, id);
	return SendMessage(handle, msg, wparam, lparam);
}

// FUNCTION: GAMEMD 0x005B9A60
void ModemHost::SelectMultiMap(void)
{
	DebugString("ModemHost.SelectMultiMap(enter)\n");

	int old_scenario_index = Session.Options.ScenarioIndex;

	char old_scenario_desc[132];
	strcpy(old_scenario_desc, Session.ScenarioFileName);

	wchar_t old_scenario_wdesc[130];
	wcscpy(old_scenario_wdesc, Session.Options.ScenarioDescription);

	ShowWindow(_Dialog, SW_HIDE);

	if (Scenario_Dialog(MainWindow) == IDCANCEL) {
		Session.Options.ScenarioIndex = old_scenario_index;
		Set_Scenario_Info_From_Index(old_scenario_index);
		Update_Network_Dialog_Preview(_Dialog);
		ShowWindow(_Dialog, SW_SHOW);

		if (stricmp(Session.Scenarios[Session.Options.ScenarioIndex]->Get_Filename(), "RandMap.Sed") == 0) {
			delete MultiplayerMapPreview;
			MultiplayerMapPreview = new MapPreviewClass;
			MultiplayerMapPreview->Read_PCX_Preview("RandMap.img");
			if (MultiplayerMapPreview->Get_Preview_Surface() == NULL) {
				Update_Network_Dialog_Preview(_Dialog);
			}
			InvalidateRect(_Dialog, NULL, FALSE);
		} else {
			Update_Network_Dialog_Preview(_Dialog);
		}
		InvalidateRect(_Dialog, NULL, FALSE);
	} else {
		ShowWindow(_Dialog, SW_SHOW);
		if (Set_Scenario_Info_From_Index(Session.Options.ScenarioIndex) == true) {
			_Send_Dlg_Item_Message(_Dialog, 0x5a8, 0x4b2, 0, (LPARAM)Session.Options.ScenarioDescription);

			if (stricmp(Session.Scenarios[Session.Options.ScenarioIndex]->Get_Filename(), "RandMap.Sed") == 0) {
				if (MultiplayerMapPreview != NULL) {
					delete MultiplayerMapPreview;
					MultiplayerMapPreview = new MapPreviewClass;
					MultiplayerMapPreview->Read_PCX_Preview("RandMap.img");
				}
				if (MultiplayerMapPreview->Get_Preview_Surface() == NULL) {
					Update_Network_Dialog_Preview(_Dialog);
				}
				InvalidateRect(_Dialog, NULL, FALSE);
			} else {
				Update_Network_Dialog_Preview(_Dialog);
			}
		} else {
			Session.Options.ScenarioIndex = old_scenario_index;
		}
	}

	Transmit = true;
	DebugString("ModemHost.SelectMultiMap(exit)\n");
}

// FUNCTION: GAMEMD 0x005B9C90
void ModemHost::SelectRandomMap(void)
{
	DebugString("ModemHost.SelectRandomMap(enter)\n");
	IsRandomMap = false;

	if (Do_Random_Map_Dialog(ModemHostCallback) == IDOK) {
		IsRandomMap = true;
		RandomMapGen.SeedData.Save("RandMap.Sed");

		if (MultiplayerMapPreview != NULL) {
			delete MultiplayerMapPreview;
			MultiplayerMapPreview = NULL;
		}

		_Send_Dlg_Item_Message(_Dialog, 0x5a8, 0x4b2, 0, (LPARAM)RandomMapGen.SeedData.MapDescription);
		bool found = false;

		int index = 0;
		for (; index < Session.Scenarios.Count(); index++) {
			if (stricmp(Session.Scenarios[index]->Get_Filename(), "RandMap.Sed") == 0) {
				found = true;
				break;
			}
		}

		char * digest = CalcRandomMapDigest();

		if (!found) {
			Session.Scenarios.Add(new MultiMission("RandMap.Sed", RandomMapGen.SeedData.MapDescription, digest, true, NULL, 2, 4));
		} else {
			Session.Scenarios[index]->Set_Digest(digest);
			Session.Scenarios[index]->Set_Description(RandomMapGen.SeedData.MapDescription);
		}

		delete digest;

		if (Set_Scenario_Info_From_Index(index) == true) {
			Session.Options.ScenarioIndex = index;
			strcpy(Session.ScenarioFileName, "RandMap.Sed");
			strcpy(Scen->ScenarioName, Session.ScenarioFileName);
			if (CCFileClass(Session.ScenarioFileName).Is_Available()) {
				MultiplayerMapPreview = new MapPreviewClass;
				MultiplayerMapPreview->Read_PCX_Preview("RandMap.img");
				PostMessage(_Dialog, WM_PAINT, 0, 0);
			}
		}

		Transmit = true;
	}

	IsRandomMap = true;
	DebugString("ModemHost.SelectMultiMap(exit)\n");
}

// FUNCTION: GAMEMD 0x005B9F30
void ModemHost::Send(bool force)
{
	NullModem.Service();

	if ((Transmit == 1 && (TickCount - TransmitTime) > PACKET_RETRANS_TIME) || force == true) {
		SendGameOptions();
		TransmitTime = TickCount;
		Transmit = false;
	}

	if ((TickCount - TimingTime) > PACKET_TIMING_TIMEOUT) {
		SerialPacketType packet;
		memset(&packet, 0, 250);
		packet.Command = SERIAL_TIMING;
		packet.ScenarioInfo.ResponseTime = NullModem.Response_Time();
		packet.ID = Session.ModemType;

		NullModem.Send_Message(&packet, 250, 0);
		TimingTime = TickCount;
	}
}

// FUNCTION: GAMEMD 0x005BA060
bool ModemHost::Receive(bool silent)
{
	SerialPacketType receivepacket;
	unsigned int version;
	unsigned int starttime;
	int packetlen;
	wchar_t * name;

	if (NullModem.Get_Message(&receivepacket, &packetlen) > 0) {

		LastMsgTime = TickCount;
		MsgTimeout = 20 * TIMER_SECOND;

		if (receivepacket.Command >= SERIAL_CONNECT &&
			receivepacket.Command < SERIAL_LAST_COMMAND &&
			receivepacket.Command != SERIAL_MESSAGE &&
			receivepacket.ID == Session.ModemType) {

			DebugString("Received our own packet back\n");

			if (!silent) {
				WWMessageBox().Process(StringTable::LoadString("TXT_SYSTEM_NOT_RESPONDING", NULL, "D:\\ra2mdpost\\ModemHst.cpp", 0x7e2), StringTable::LoadString("TXT_OK", NULL, "D:\\ra2mdpost\\ModemHst.cpp", 0x7e2), NULL, NULL);
			}

			return(true);
		}

		switch (receivepacket.Command) {
			case (SERIAL_SIGN_OFF):
				DebugString("Received SignOff!\n");
				starttime = TickCount;
				while (TickCount - starttime < TIMER_SECOND) {
					Call_Back();
					NullModem.Service();
				}

				if (!silent) {
					WWMessageBox().Process(StringTable::LoadString("TXT_USER_SIGNED_OFF", NULL, "D:\\ra2mdpost\\ModemHst.cpp", 0x7f6), StringTable::LoadString("TXT_OK", NULL, "D:\\ra2mdpost\\ModemHst.cpp", 0x7f6), NULL, NULL);
				}

				LastMsgTime = TickCount;

				Shutdown_Modem();
				return(true);

			case (SERIAL_GAME_OPTIONS):
				DebugString("Received game options\n");

				version = VerNum.Clip_Version(receivepacket.ScenarioInfo.MinVersion,
					receivepacket.ScenarioInfo.MaxVersion);
				if (version == 0) {
					if (!silent) {
						WWMessageBox().Process(StringTable::LoadString("TXT_DESTGAME_OUTDATED", NULL, "D:\\ra2mdpost\\ModemHst.cpp", 0x80b), StringTable::LoadString("TXT_OK", NULL, "D:\\ra2mdpost\\ModemHst.cpp", 0x80b), NULL, NULL);
					}

					LastMsgTime = TickCount;

					return(true);
				} else if (version == 0xffffffff) {
					if (!silent) {
						WWMessageBox().Process(StringTable::LoadString("TXT_YOURGAME_OUTDATED", NULL, "D:\\ra2mdpost\\ModemHst.cpp", 0x814), StringTable::LoadString("TXT_OK", NULL, "D:\\ra2mdpost\\ModemHst.cpp", 0x814), NULL, NULL);
					}

					LastMsgTime = TickCount;

					return(true);
				} else {

					bool ok = true;

					if (receivepacket.ScenarioInfo.CheatCheck != CheatCheck) {
						ok = false;
					}
					if (receivepacket.ScenarioInfo.AICheatCheck != AICheatCheck) {
						ok = false;
					}
					if (receivepacket.ScenarioInfo.ArtCheatCheck != ArtCheatCheck) {
						ok = false;
					}

					if (!ok) {

						if (!silent) {
							WWMessageBox().Process(StringTable::LoadString("TXT_MISMATCH", NULL, "D:\\ra2mdpost\\ModemHst.cpp", 0x851), StringTable::LoadString("TXT_OK", NULL, "D:\\ra2mdpost\\ModemHst.cpp", 0x851), NULL, NULL);
						}

						LastMsgTime = TickCount;

						return(true);
					}

					Session.CommProtocol = VerNum.Version_Protocol(version);

					name = FUN_00735120(receivepacket.Name);
					SetOther(name, receivepacket.ScenarioInfo.Color, receivepacket.ScenarioInfo.House);
					GameOptions = true;
				}
				break;

			case (SERIAL_ACCEPT_OPTIONS):
				DebugString("Received Accept Options\n");
				EnableWindow(GetDlgItem(_Dialog, 1), TRUE);
				break;

			case (SERIAL_MESSAGE):
				DebugString("Received serial message\n");
				name = FUN_00735120(receivepacket.Name);
				DrawMessage(name, receivepacket.ID, receivepacket.Message.Message);

				if (!silent) {
					VocClass::PlayGlobal(Rule->IncomingMessage, 1.0f, 0x2000);
				}
				break;

			case (SERIAL_TIMING):
				DebugString("Received timing packet\n");
				TheirResponseTime = receivepacket.ScenarioInfo.ResponseTime;

				if (!GameOptions) {
					Transmit = true;
				}
				break;

			case (SERIAL_SCORE_SCREEN):
				DebugString("Received score screen packet\n");
				break;

			case (SERIAL_REQ_PREVIEW):
				SendPreview();
				LastMsgTime = TickCount;
				break;

			default:
				DebugString("Received packet\n");
				break;
		}
	}

	unsigned int time = (TickCount - LastMsgTime);
	if (time > MsgTimeout) {
		DebugString("Timeout! %d ticks since last incoming message\n", time);

		if (!silent) {
			WWMessageBox().Process(StringTable::LoadString("TXT_SYSTEM_NOT_RESPONDING", NULL, "D:\\ra2mdpost\\ModemHst.cpp", 0x895), StringTable::LoadString("TXT_OK", NULL, "D:\\ra2mdpost\\ModemHst.cpp", 0x895), NULL, NULL);
		}

		return(true);
	}
	return(false);
}

// FUNCTION: GAMEMD 0x005BA660
void ModemHost::SendGameOptions(void)
{
	SerialPacketType sendpacket;

	DebugString("Sending game options packet\n");
	EnableWindow(GetDlgItem(_Dialog, 1), FALSE);

	memset(&sendpacket, 0, sizeof(SerialPacketType));
	sendpacket.Command = SERIAL_GAME_OPTIONS;
	sendpacket.ScenarioInfo.CheatCheck = CheatCheck;
	sendpacket.ScenarioInfo.ArtCheatCheck = ArtCheatCheck;
	sendpacket.ScenarioInfo.AICheatCheck = AICheatCheck;
	sendpacket.ScenarioInfo.BuildNumber = Build_Number();
	sendpacket.ScenarioInfo.MinVersion = VerNum.Min_Version();
	sendpacket.ScenarioInfo.MaxVersion = VerNum.Max_Version();
	sendpacket.ScenarioInfo.Seed = Seed;
	sendpacket.ScenarioInfo.Special = Special;
	sendpacket.ScenarioInfo.GameSpeed = Session.Options.GameSpeed;
	sendpacket.ScenarioInfo.IsCrapEngineers = Session.Options.CrapEngineers;
	sendpacket.ScenarioInfo.IsBuildOffAlly = Session.Options.BuildOffAlly;
	sendpacket.ScenarioInfo.IsSWAllowed = Session.Options.SWAllowed;
	sendpacket.ScenarioInfo.IsShortGame = Session.Options.ShortGame;
	sendpacket.ID = Session.ModemType;
	strcpy(sendpacket.Name, Session.Handle);
	sendpacket.ScenarioInfo.House = (HousesType)Session.House;
	sendpacket.ScenarioInfo.Color = Session.PrefColor;
	sendpacket.ScenarioInfo.Credits = Session.Options.Credits;
	sendpacket.ScenarioInfo.IsBases = Session.Options.Bases;
	sendpacket.ScenarioInfo.IsMCVRedeploy = Session.Options.MCVRedeploy;
	sendpacket.ScenarioInfo.IsBridgeDestruction = Session.Options.BridgeDestruction;
	sendpacket.ScenarioInfo.IsGoodies = Session.Options.Goodies;
	sendpacket.ScenarioInfo.IsHarvTruce = Session.Options.HarvTruce;
	sendpacket.ScenarioInfo.IsFogOfWar = Session.Options.FogOfWar;
	sendpacket.ScenarioInfo.AIPlayers = Session.Options.AIPlayers;
	sendpacket.ScenarioInfo.AIDifficulty = Session.Options.AIDifficulty;
	sendpacket.ScenarioInfo.BuildLevel = BuildLevel;
	sendpacket.ScenarioInfo.UnitCount = Session.Options.UnitCount;

	wcscpy(sendpacket.ScenarioInfo.Scenario, Session.Scenarios[Session.Options.ScenarioIndex]->Description());
	CCFileClass file(Session.Scenarios[Session.Options.ScenarioIndex]->Get_Filename());

	sendpacket.ScenarioInfo.FileLength = file.Size();

	strncpy(sendpacket.ScenarioInfo.ShortFileName, Session.Scenarios[Session.Options.ScenarioIndex]->Get_Filename(), sizeof(sendpacket.ScenarioInfo.ShortFileName));
	strncpy((char *)sendpacket.ScenarioInfo.FileDigest, Session.Scenarios[Session.Options.ScenarioIndex]->Get_Digest(), sizeof sendpacket.ScenarioInfo.FileDigest);
	sendpacket.ScenarioInfo.OfficialScenario = Session.Scenarios[Session.Options.ScenarioIndex]->Get_Official();
	NullModem.Send_Message(&sendpacket, sizeof(sendpacket), 1);
}

// FUNCTION: GAMEMD 0x005BA970
void ModemHost::SendAMessage(const wchar_t * message)
{
	if (message != NULL) {
		SerialPacketType packet;
		memset(&packet, 0, 250);
		packet.Command = SERIAL_MESSAGE;
		strcpy(packet.Name, Session.Handle);
		packet.ID = Session.PrefColor;
		wcscpy(packet.Message.Message, message);

		NullModem.Send_Message(&packet, 250, 1);
		NullModem.Service();

		wchar_t * name = FUN_00735120(Session.Handle);
		DrawMessage(name, Session.PrefColor, message);
	}
}

// FUNCTION: GAMEMD 0x005BAA80
void ModemHost::DrawMessage(const wchar_t * name, int color, const wchar_t * message)
{
	wchar_t buf[256];

	if (message != NULL) {
		HWND handle = GetDlgItem(_Dialog, 0x53f);
		if (handle) {
			COLORREF c = (color >= 0 && color < MAX_PLAYERS ? PlayerColorTable[color] : ColorMe);
			swprintf(buf, L"[%s] %s", name, message);
			_DrawMessage(c, buf, handle);
		}
	}
}

// FUNCTION: GAMEMD 0x005BAB00
void ModemHost::SignOff(void)
{
	unsigned int starttime;
	int packetlen;
	SerialPacketType sendpacket;
	SerialPacketType receivepacket;

	memset(&sendpacket, 0, sizeof(SerialPacketType));
	sendpacket.Command = SERIAL_SIGN_OFF;
	sendpacket.ScenarioInfo.Color = Session.PrefColor;
	sendpacket.ID = Session.ModemType;
	NullModem.Send_Message(&sendpacket, sizeof(sendpacket), 1);

	HWND dialog = OwnerDraw::Custom_Message_Box(StringTable::LoadString("TXT_SIGNING_OFF", NULL, "D:\\ra2mdpost\\ModemHst.cpp", 0x95e), NULL, NULL);

	if (dialog) {
		OwnerDraw::Display_Dialog(dialog);
	}

	starttime = TickCount;
	while (NullModem.Num_Send() && ((TickCount - starttime) < SIGNOFF_PACKET_CANCEL_TIMEOUT)) {
		if (NullModem.Get_Message(&receivepacket, &packetlen) > 0) {
			if (receivepacket.Command == SERIAL_SIGN_OFF && receivepacket.ID == Session.ModemType) {
				break;
			}
		}

		Call_Back();
		Windows_Message_Handler();
		NullModem.Service();
	}
	Shutdown_Modem();
	if (dialog) {
		OwnerDraw::End_Dialog(dialog);
	}
}

// FUNCTION: GAMEMD 0x005BAC60
bool ModemHost::StartGame(bool load_game)
{
	SerialPacketType sendpacket;
	SerialPacketType receivepacket;
	unsigned int starttime;
	int packetlen;
	NodeNameType * who;

	DebugString("ModemHost.StartGame(enter)\n");
	DebugString("LoadGame = %s\n", load_game == true ? "true" : "false");

	if (load_game == true) {
		return(false);
	}

	Scen->Scenario = Session.Options.ScenarioIndex;
	strcpy(Scen->ScenarioName, Session.Scenarios[Session.Options.ScenarioIndex]->Get_Filename());

	Scen->CDifficulty = DIFF_NORMAL;
	Scen->Difficulty = DIFF_NORMAL;

	Session.NumPlayers = 0;

	who = new NodeNameType;
	if (who != NULL) {
		wcscpy(who->Name, FUN_00735120(Session.Handle));
		who->Player.House = Session.House;
		who->Player.Color = Session.PrefColor;
		who->Player.ProcessTime = -1;
		Session.Players.Add(who);
	}

	who = new NodeNameType;
	if (who != NULL) {
		wcscpy(who->Name, TheirName);
		who->Player.House = TheirHouse;
		who->Player.Color = TheirColor;
		who->Player.ProcessTime = -1;
		Session.Players.Add(who);
	}

	if (Session.Players.Count() != 2) {
		return(false);
	}

	memset(&sendpacket, 0, sizeof(SerialPacketType));
	sendpacket.Command = SERIAL_GO;

	sendpacket.ScenarioInfo.ResponseTime = NullModem.Response_Time();
	if (TheirResponseTime == 10000) {
		;
	} else if (sendpacket.ScenarioInfo.ResponseTime < TheirResponseTime) {
		sendpacket.ScenarioInfo.ResponseTime = TheirResponseTime;
	}

	Options.GameSpeed = Session.Options.GameSpeed;
	Session.FrameSendRate = 5;
	Session.LatencyFudge = 0;
	Session.PrecalcMaxAhead = 0;
	Session.PrecalcDesiredFrameRate = 0;

	if (Session.CommProtocol == COMM_PROTOCOL_MULTI_E_COMP) {
		Session.MaxAhead = MAX(((sendpacket.ScenarioInfo.ResponseTime / 8 + 4) / 5) * 5, 10);
	} else {
		Session.MaxAhead = sendpacket.ScenarioInfo.ResponseTime / 8;
		Session.MaxAhead = MAX((int)(Session.MaxAhead), MODEM_MIN_MAX_AHEAD);
	}
	sendpacket.ID = Session.ModemType;

	NullModem.Send_Message(&sendpacket, sizeof(sendpacket), 1);
	HWND msgbox = OwnerDraw::Custom_Message_Box(StringTable::LoadString("TXT_SENDING_GO", NULL, "D:\\ra2mdpost\\ModemHst.cpp", 0x9ec));
	if (msgbox) {
		OwnerDraw::Display_Dialog(msgbox);
	}
	starttime = TickCount;
	while ((NullModem.Num_Send() && ((TickCount - starttime) < PACKET_SENDING_TIMEOUT)) || ((TickCount - starttime) < TIMER_SECOND)) {
		Call_Back();
		Windows_Message_Handler();
		NullModem.Service();
	}
	if (msgbox) {
		OwnerDraw::Set_Custom_Message_Box_Text(msgbox, StringTable::LoadString("TXT_WAITING_GO", NULL, "D:\\ra2mdpost\\ModemHst.cpp", 0x9fd));
	}

	DebugString("Waiting for 'GO' response.\n");
	starttime = TickCount;

	do {
		Call_Back();
		Windows_Message_Handler();
		NullModem.Service();

		if (NullModem.Get_Message(&receivepacket, &packetlen) > 0) {
			DebugString("Received message\n");

			if (receivepacket.Command == SERIAL_READY_TO_GO) {
				DebugString("Received GO message\n");
				break;
			}

			if (receivepacket.Command == SERIAL_NO_SCENARIO) {
				WWMessageBox().Process(StringTable::LoadString("TXT_NO_EXPANSION_SCENARIO", NULL, "D:\\ra2mdpost\\ModemHst.cpp", 0xa16), StringTable::LoadString("TXT_CANCEL", NULL, "D:\\ra2mdpost\\ModemHst.cpp", 0xa16), NULL, NULL);
				LastMsgTime = TickCount;
				DebugString("***Guest doesn't have scenario!\n");
				return(false);
			}

			if (receivepacket.Command == SERIAL_REQ_SCENARIO) {
				DebugString("About to call 'Send_Remote_File'.\n");
				FUN_00694760(Scen->ScenarioName, GAME_MODEM, false, false);
				break;
			}
		}

	} while (TickCount - starttime < 20 * TIMER_SECOND);

	if (msgbox) {
		OwnerDraw::End_Dialog(msgbox);
	}

	NullModem.Init_Send_Queue();

	Session.NetOpen = false;
	Scen->Scenario = Session.Options.ScenarioIndex;
	Session.NumPlayers = Session.Players.Count();
	Special.IsHarvesterImmune = Session.Options.HarvTruce;
	Special.IsDestroyBridges = Session.Options.BridgeDestruction;
	Frame = 0;
	Session.CommProtocol = (CommProtocolType)2;
	Special.IsTGrowth = true;
	Special.IsTSpread = true;
	Scen->Special.IsAllianceFixed = false;
	Session.Options.AlliesAllowed = true;
	Special.Apply_To_Game();
	DebugString("ModemHost.StartGame(exit)\n");
	return(true);
}

// FUNCTION: GAMEMD 0x005BB260
bool ModemHostCallback(void)
{
	if (_ModemHost != NULL) {
		_ModemHost->Send(false);
		_ModemHost->Receive(true);
	}
	Call_Back();
	return(false);
}
