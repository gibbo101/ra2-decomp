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

char * __fastcall FUN_007350C0(wchar_t const * text);
wchar_t * __fastcall FUN_00735120(char const * narrow);
wchar_t * __fastcall wcstrim(wchar_t * buffer);

namespace OwnerDraw {
char * __fastcall WideToCharString(char * dest, wchar_t const * src, int count);
}

// Initialisers of geometry.h constants.
// FUNCTION: 0x005B75F0 $E2
// FUNCTION: 0x005B7620 $E5
// FUNCTION: 0x005B7640 $E8
// FUNCTION: 0x005B7660 $E11
// FUNCTION: 0x005B7680 $E14
// FUNCTION: 0x005B76A0 $E17
// FUNCTION: 0x005B76C0 $E20
// FUNCTION: 0x005B7700 $E23
// FUNCTION: 0x005B7730 $E26
// FUNCTION: 0x005B7760 $E29
// FUNCTION: 0x005B7790 $E32
// FUNCTION: 0x005B77A0 $E35
// FUNCTION: 0x005B77C0 $E38

ModemHost * _ModemHost;

BOOL CALLBACK Modem_Host_Dialog_Proc(HWND window, UINT message, WPARAM wparam, LPARAM lparam);

// FUNCTION: 0x005B77E0
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

// FUNCTION: 0x005B8CE0
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

// FUNCTION: 0x005B9250
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

// FUNCTION: 0x005B93F0
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

// FUNCTION: 0x005B9450
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

// FUNCTION: 0x005B94C0
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

// FUNCTION: 0x005B9550
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

// FUNCTION: 0x005B9650
void ModemHost::SetUnitCount(void)
{
	HWND handle = GetDlgItem(_Dialog, IDC_MODEM_UNITCOUNT);
	if (handle) {
		Session.Options.UnitCount = Slider_GetPos(handle);
		Transmit = true;
	}
}

// FUNCTION: 0x005B9680
void ModemHost::SetBuildLevel(void)
{
	HWND handle = GetDlgItem(_Dialog, IDC_TECHLEVEL);
	if (handle) {
		BuildLevel = Slider_GetPos(handle);
		Transmit = true;
	}
}

// FUNCTION: 0x005B96B0
void ModemHost::SetCredits(void)
{
	HWND handle = GetDlgItem(_Dialog, IDC_CREDITS);
	if (handle) {
		Session.Options.Credits = Slider_GetPos(handle);
		Transmit = true;
	}
}

// FUNCTION: 0x005B96E0
void ModemHost::SetAIPlayers(void)
{
	HWND handle = GetDlgItem(_Dialog, IDC_AIPLAYERS);
	if (handle) {
		Session.Options.AIPlayers = Slider_GetPos(handle);
		Transmit = true;
	}
}

// FUNCTION: 0x005B9710
void ModemHost::SetGameSpeed(void)
{
	HWND handle = GetDlgItem(_Dialog, IDC_GAME_SPEED_SLIDER);
	if (handle) {
		Session.Options.GameSpeed = 6 - Slider_GetPos(handle);
		Transmit = true;
	}
}

// FUNCTION: 0x005B9750
void ModemHost::SetAIDifficulty(void)
{
	HWND handle = GetDlgItem(_Dialog, IDC_AILEVEL_SLIDER);
	if (handle) {
		Session.Options.AIDifficulty = (DiffType)Slider_GetPos(handle);
		Transmit = true;
	}
}

// FUNCTION: 0x005B9780
void ModemHost::SetMCVRedeploy(void)
{
	HWND handle = GetDlgItem(_Dialog, IDC_MODEM_REDEPLOY_MCV);
	if (handle) {
		Session.Options.MCVRedeploy = Button_GetCheck(handle) == TRUE;
		Transmit = true;
	}
}

// FUNCTION: 0x005B97C0
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

// FUNCTION: 0x005B9830
void ModemHost::SetGoodies(void)
{
	HWND handle = GetDlgItem(_Dialog, IDC_GOODIES);
	if (handle) {
		Session.Options.Goodies = Button_GetCheck(handle) == TRUE;
		Transmit = true;
	}
}

// FUNCTION: 0x005B9870
void ModemHost::SetFogOfWar(void)
{
	HWND handle = GetDlgItem(_Dialog, IDC_MODEM_FOG);
	if (handle) {
		Session.Options.FogOfWar = Button_GetCheck(handle) == TRUE;
		Transmit = true;
	}
}

// FUNCTION: 0x005B98B0
void ModemHost::SetBridgeDestruction(void)
{
	HWND handle = GetDlgItem(_Dialog, IDC_BRIDGE_DESTROY);
	if (handle) {
		Session.Options.BridgeDestruction = Button_GetCheck(handle) == TRUE;
		Transmit = true;
	}
}

// FUNCTION: 0x005B98F0
void ModemHost::SetHarvTruce(void)
{
	HWND handle = GetDlgItem(_Dialog, IDC_HARVTRUCE);
	if (handle) {
		Session.Options.HarvTruce = Button_GetCheck(handle) == TRUE;
		Transmit = true;
	}
}

// FUNCTION: 0x005B9930
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

// FUNCTION: 0x005B99A0
void ModemHost::SetSWAllowed(void)
{
	HWND handle = GetDlgItem(_Dialog, 0x69a);
	if (handle) {
		Session.Options.SWAllowed = Button_GetCheck(handle) == TRUE;
		Transmit = true;
	}
}

// FUNCTION: 0x005B99E0
void ModemHost::SetBuildOffAlly(void)
{
	HWND handle = GetDlgItem(_Dialog, 0x69d);
	if (handle) {
		Session.Options.ShortGame = Button_GetCheck(handle) == TRUE;
		Transmit = true;
	}
}

// FUNCTION: 0x005B9A20
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

// FUNCTION: 0x005B9A60
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

// FUNCTION: 0x005B9C90
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

// FUNCTION: 0x005B9F30
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

// FUNCTION: 0x005BA970
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

// FUNCTION: 0x005BAA80
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

// FUNCTION: 0x005BB260
bool ModemHostCallback(void)
{
	if (_ModemHost != NULL) {
		_ModemHost->Send(false);
		_ModemHost->Receive(true);
	}
	Call_Back();
	return(false);
}
