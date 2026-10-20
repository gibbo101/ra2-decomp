// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"
#include "modemgst.h"
#include "_rules.h"
#include "_timer.h"
#include "addon.h"
#include "conquer.h"
#include "data.h"
#include "dbgprint.h"
#include "globals.h"
#include "goptions.h"
#include "houstype.h"
#include "init.h"
#include "language\language.h"
#include "lzostraw.h"
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
#include "xstraw.h"

#include <math.h>

namespace OwnerDraw {
char * __fastcall WideToCharString(char * dest, wchar_t const * src, int count);
}

extern double __cdecl FUN_004CAC40(double);
extern double __cdecl FUN_004CAD50(double);
extern double __cdecl FUN_004CADE0(double);
extern const double DAT_007E1708;
extern const double DAT_007E1710;
extern const double DAT_007E1718;
extern const double DAT_007E1720;
extern const double DAT_007E1728;
extern const double DAT_007E1730;
extern const double DAT_007E1738;
extern const double DAT_007E1740;
extern const double DAT_007EDF58;
extern const double DAT_007EDF60;

struct ShortPair {
	unsigned short A, B;
	ShortPair() { A = 0; B = 0; }
};

struct IntTriple {
	int A, B, C;
	IntTriple() { A = 0; B = 0; C = 0; }
};

// FUNCTION: 0x005B47C0 $E43
double DAT_00ABF030 = FUN_004CAC40(pow(DAT_007E1710, DAT_007E1708) * 2);

// FUNCTION: 0x005B47F0 $E46
double DAT_00ABF080 = DAT_00ABF030 / DAT_007EDF58;

// FUNCTION: 0x005B4810 $E49
double DAT_00ABF068 = DAT_007E1718 / DAT_00ABF080;

// FUNCTION: 0x005B4830 $E52
double DAT_00ABF060 = DAT_007E1720 * DAT_007EDF60;

// FUNCTION: 0x005B4850 $E55
double DAT_00ABF038 = DAT_007E1728 * DAT_007EDF60;

// FUNCTION: 0x005B4870 $E58
double DAT_00ABF010 = DAT_007E1730 * DAT_007EDF60;

// FUNCTION: 0x005B4890 $E61
int DAT_00ABF088 = (int)(DAT_007E1738 * (FUN_004CAD50(DAT_00ABF010 - DAT_00ABF038) * DAT_00ABF030));

// FUNCTION: 0x005B48D0 $E64
double DAT_00ABF040 = FUN_004CADE0(DAT_00ABF088 * DAT_007E1740);

// FUNCTION: 0x005B4900 $E67
double DAT_00ABF058 = FUN_004CADE0((DAT_00ABF088 * 2) / DAT_00ABF030);

// FUNCTION: 0x005B4930 $E70
int DAT_00ABF07C = (int)(DAT_00ABF088 * 4 + DAT_007E1738);

// FUNCTION: 0x005B4960 $E73
ShortPair DAT_00ABF028;

// FUNCTION: 0x005B4970 $E76
IntTriple DAT_00ABF048;

wchar_t * __fastcall FUN_00735120(char const * narrow);
wchar_t * __fastcall wcstrim(wchar_t * buffer);

int DAT_00ABF070;
int DAT_00ABF074;
int DAT_00ABF078;

// FUNCTION: 0x005B4990
void FUN_005B4990(void)
{
	DAT_00ABF078 = 0;
	DAT_00ABF070 = 128;
	DAT_00ABF074 = 128;
}

BOOL CALLBACK Modem_Guest_Dialog_Proc(HWND window, UINT message, WPARAM wparam, LPARAM lparam);

// FUNCTION: 0x005B49B0
bool ModemGuest::Dialog(void)
{
	bool started = false;

	DebugString("ModemGuest.Dialog(enter)\n");

	RC = -1;
	_Dialog = OwnerDraw::Begin_Dialog(IDD_MODEM_GUEST, Modem_Guest_Dialog_Proc, 0);

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
		TheirName[0] = '\0';
		TheirColor = -1;
		TheirHouse = HOUSE_NONE;

		CheatCheck = RulesClass::Get_Rule_Unique_ID();
		RulesClass::Load_Art_INI();
		ArtCheatCheck = RulesClass::Get_Art_Unique_ID();
		AICheatCheck = RulesClass::Get_AI_Unique_ID();

		while (RC < 0) {
			if (OwnerDraw::Dialog_Message_Handler() == true) {
				break;
			}
			Send(false);
			if (Receive() == true) {
				break;
			}
			Title_Screen_Restore();
		}

		OwnerDraw::End_Dialog(_Dialog);
		if (Changed == true) {
			Session.Write_MultiPlayer_Settings();
		}

		if (RC == 1) {
			started = true;
		} else if (RC == IDCANCEL) {
			started = false;
			SignOff();
		}
	}

	DebugString("ModemGuest.Dialog(exit %s)\n", started == true ? "true" : "false");
	return(started);
}

// FUNCTION: 0x005B4B10
BOOL CALLBACK Modem_Guest_Dialog_Proc(HWND window, UINT message, WPARAM wparam, LPARAM lparam)
{
	int rc = OwnerDraw::Default_Dialog_Proc(window, message, wparam, lparam);

	if (rc == 0) {
		ModemGuest * _this = (ModemGuest *)GetWindowLong(window, DWL_USER);

		switch (message) {
			case WM_COMMAND: {
				switch (LOWORD(wparam)) {
					case IDC_MODEM_YOURSIDE:
						if (HIWORD(wparam) == CBN_SELCHANGE) {
							_this->SetHouse();
						}
						break;

					case IDC_NAME:
						if (HIWORD(wparam) == EN_MAXTEXT || HIWORD(wparam) == EN_CHANGE) {
							_this->SetName(HIWORD(wparam) == EN_CHANGE);
						}
						break;

					case IDC_INPUT:
						if (HIWORD(wparam) == EN_MAXTEXT) {
							_this->SendCurrentMessage();
						}
						break;

					case IDC_MODEM_YOURCOLOR:
						if (HIWORD(wparam) == CBN_SELCHANGE) {
							_this->SetColor();
						}
						break;

					case IDOK:
						if (HIWORD(wparam) == BN_CLICKED) {
							_this->AcceptOptions();
						}
						break;

					case IDCANCEL:
						if (HIWORD(wparam) == BN_CLICKED) {
							_this->ReadDialog();
							_this->RC = IDCANCEL;
						}
						break;
				}
			}
			break;

			case WM_DESTROY:
				if (MultiplayerMapPreview != NULL) {
					delete MultiplayerMapPreview;
					MultiplayerMapPreview = NULL;
				}
				break;

			case WM_PAINT:
				if (MultiplayerMapPreview != NULL) {
					MultiplayerMapPreview->Blit_Preview(window);
				}
				ValidateRect(window, NULL);
				break;
		}
		rc = 0;
	}

	return(rc);
}

// FUNCTION: 0x005B56F0
void ModemGuest::ReadDialog(void)
{
	DebugString("ModemGuest.ReadDialog(enter)\n");

	wchar_t name[20];
	HWND handle = GetDlgItem(_Dialog, IDC_NAME);
	if (handle) {
		SendMessage(handle, 0x4b3, 0x14, (LPARAM)name);
		OwnerDraw::WideToCharString(Session.Handle, name, -1);
	}
	DebugString("Name: %s\n", Session.Handle);

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

	DebugString("ModemGuest.ReadDialog(exit)\n");
}

// FUNCTION: 0x005B57D0
void ModemGuest::SendCurrentMessage(void)
{
	wchar_t buf[256];

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

// FUNCTION: 0x005B5960
void ModemGuest::SetHouse(void)
{
	HWND handle = GetDlgItem(_Dialog, IDC_MODEM_YOURSIDE);
	if (handle) {
		int i = ComboBox_GetCurSel(handle);
		if (i != CB_ERR) {
			Session.House = ComboBox_GetItemData(handle,i);
			Transmit = true;
			Changed = true;
		}
	}
}

// FUNCTION: 0x005B59B0
void ModemGuest::SetColor(void)
{
	HWND handle = GetDlgItem(_Dialog, IDC_MODEM_YOURCOLOR);
	if (handle) {
		int color = ComboBox_GetCurSel(handle);
		if (color != CB_ERR) {
			if (color == TheirColor) {
				color = TheirColor + 1;
				if (color > MAX_PLAYERS - 1) {
					color = 0;
				}
				ComboBox_SetCurSel(handle, color);
			}
			Session.PrefColor = color;
			Transmit = true;
			Changed = true;
			SendMessage(handle, OD_SETCOLOR, 0, (color >= 0 && color < MAX_PLAYERS ? PlayerColorTable[color] : ColorMe));
		}
	}
}

// FUNCTION: 0x005B5A50
void ModemGuest::SetName(bool update)
{
	wchar_t namebuf[21];
	HWND handle = GetDlgItem(_Dialog, IDC_NAME);
	if (handle) {
		SendMessage(handle, 0x4b3, 0x15, (LPARAM)namebuf);
		wcstrim(namebuf);
		if (!update) {
			SendMessage(handle, 0x4b2, 0, (LPARAM)namebuf);
		}
		OwnerDraw::WideToCharString(Session.Handle, namebuf, -1);
		Transmit = true;
		Changed = true;
	}
}

// FUNCTION: 0x005B5AD0
void ModemGuest::SetOther(const wchar_t * name, int color, int house)
{
	wchar_t buf[80];

	wcscpy(TheirName, name);
	TheirColor = color;

	HWND handle = GetDlgItem(_Dialog, IDC_MODEM_YOURCOLOR);
	if (handle) {
		int i = ComboBox_GetCurSel(handle);
		if (i != CB_ERR) {
			if (i == TheirColor) {
				SetColor();
			}
		}
	}

	TheirHouse = house;

	handle = GetDlgItem(_Dialog, IDC_MODEM_OPPONENT);
	if (handle) {
		swprintf(buf, L"%s [%s]", name, HouseTypes[house]->UIName);
		SendMessage(handle, 0x4b2, 0, (LPARAM)buf);
		SendMessage(handle, OD_SETCOLOR, 0, (color >= 0 && color < MAX_PLAYERS ? PlayerColorTable[color] : ColorMe));
	}
}

// FUNCTION: 0x005B5BC0
void ModemGuest::SetSettings(void)
{
	DebugString("Settings change\n");

	HWND handle = GetDlgItem(_Dialog, IDC_MODEM_UNITCOUNT);
	if (handle) {
		Slider_SetPos(handle, Session.Options.UnitCount);
	}

	handle = GetDlgItem(_Dialog, IDC_TECHLEVEL);
	if (handle) {
		Slider_SetPos(handle, BuildLevel);
	}

	handle = GetDlgItem(_Dialog, IDC_CREDITS);
	if (handle) {
		Slider_SetPos(handle, Session.Options.Credits);
	}

	handle = GetDlgItem(_Dialog, IDC_AIPLAYERS);
	if (handle) {
		Slider_SetPos(handle, Session.Options.AIPlayers);
	}

	handle = GetDlgItem(_Dialog, IDC_GAME_SPEED_SLIDER);
	if (handle) {
		Slider_SetPos(handle, (6 - Session.Options.GameSpeed));
	}

	handle = GetDlgItem(_Dialog, IDC_AILEVEL_SLIDER);
	if (handle) {
		Slider_SetPos(handle, Session.Options.AIDifficulty);
	}

	handle = GetDlgItem(_Dialog, IDC_MODEM_REDEPLOY_MCV);
	if (handle) {
		Button_SetCheck(handle, Session.Options.MCVRedeploy == true);
	}

	handle = GetDlgItem(_Dialog, IDC_MODEM_BASES);
	if (handle) {
		Button_SetCheck(handle, Session.Options.Bases == true);
	}

	handle = GetDlgItem(_Dialog, IDC_MULTI_ENGINEER);
	if (handle) {
		Button_SetCheck(handle, Session.Options.CrapEngineers == true);
	}

	handle = GetDlgItem(_Dialog, IDC_SHORT_GAME);
	if (handle) {
		Button_SetCheck(handle, Session.Options.ShortGame == true);
	}

	handle = GetDlgItem(_Dialog, 0x69a);
	if (handle) {
		Button_SetCheck(handle, Session.Options.SWAllowed == true);
	}

	handle = GetDlgItem(_Dialog, 0x69d);
	if (handle) {
		Button_SetCheck(handle, Session.Options.BuildOffAlly == true);
	}

	handle = GetDlgItem(_Dialog, IDC_GOODIES);
	if (handle) {
		Button_SetCheck(handle, Session.Options.Goodies == true);
	}

	handle = GetDlgItem(_Dialog, IDC_MODEM_FOG);
	if (handle) {
		Button_SetCheck(handle, Session.Options.FogOfWar == true);
	}

	handle = GetDlgItem(_Dialog, IDC_BRIDGE_DESTROY);
	if (handle) {
		Button_SetCheck(handle, Session.Options.BridgeDestruction == true);
	}

	handle = GetDlgItem(_Dialog, IDC_HARVTRUCE);
	if (handle) {
		Button_SetCheck(handle, Session.Options.HarvTruce == true);
	}

	handle = GetDlgItem(_Dialog, 0x5a8);
	if (handle) {
		SendMessage(handle, 0x4b2, 0, (LPARAM)Session.Options.ScenarioDescription);
	}

	if (MultiplayerMapPreview == NULL || MultiplayerMapPreview->Get_Preview_Surface() == NULL) {
		Update_Network_Dialog_Preview(_Dialog);
	}

	handle = GetDlgItem(_Dialog, IDOK);
	if (handle) {
		Button_Enable(handle, TRUE);
	}

	VocClass::PlayGlobal(Rule->OptionsChanged, 1.0f, 0x2000);
}

// FUNCTION: 0x005B5E90
void ModemGuest::AcceptOptions(void)
{
	HWND handle = GetDlgItem(_Dialog, IDOK);
	if (handle) {
		Button_Enable(handle, FALSE);
	}

	SerialPacketType packet;
	memset((void *)&packet, 0, 250);
	packet.Command = SERIAL_ACCEPT_OPTIONS;
	NullModem.Send_Message(&packet, 250, 1);
}

// FUNCTION: 0x005B5EF0
void ModemGuest::Send(bool force)
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

// FUNCTION: 0x005B67F0
bool ModemGuest::SetGameOptions(SerialPacketType * packet)
{
	if (Session.Options.Bases == (bool)packet->ScenarioInfo.IsBases &&
		Session.Options.MCVRedeploy == (bool)packet->ScenarioInfo.IsMCVRedeploy &&
		Session.Options.HarvTruce == (bool)packet->ScenarioInfo.IsHarvTruce &&
		Session.Options.BridgeDestruction == (bool)packet->ScenarioInfo.IsBridgeDestruction &&
		Session.Options.Goodies == (bool)packet->ScenarioInfo.IsGoodies &&
		Session.Options.FogOfWar == (bool)packet->ScenarioInfo.IsFogOfWar &&
		Session.Options.AIPlayers == packet->ScenarioInfo.AIPlayers &&
		Session.Options.AIDifficulty == packet->ScenarioInfo.AIDifficulty &&
		BuildLevel == packet->ScenarioInfo.BuildLevel &&
		Session.Options.Credits == (int)packet->ScenarioInfo.Credits &&
		Session.Options.UnitCount == packet->ScenarioInfo.UnitCount &&
		Seed == packet->ScenarioInfo.Seed &&
		Special == packet->ScenarioInfo.Special &&
		Session.Options.GameSpeed == packet->ScenarioInfo.GameSpeed &&
		Session.Options.ShortGame == (bool)packet->ScenarioInfo.IsShortGame &&
		Session.Options.SWAllowed == (bool)packet->ScenarioInfo.IsSWAllowed &&
		Session.Options.BuildOffAlly == (bool)packet->ScenarioInfo.IsBuildOffAlly &&
		Session.Options.CrapEngineers == (bool)packet->ScenarioInfo.IsCrapEngineers &&
		stricmp(Session.ScenarioFileName, packet->ScenarioInfo.ShortFileName) == 0 &&
		strcmp(Session.ScenarioDigest, (char const *)packet->ScenarioInfo.FileDigest) == 0 &&
		Session.ScenarioFileLength == packet->ScenarioInfo.FileLength) {

		return(false);
	}

	DebugString("Decoding game options\n");

	Session.Options.Bases = packet->ScenarioInfo.IsBases;
	Session.Options.MCVRedeploy = packet->ScenarioInfo.IsMCVRedeploy;
	Session.Options.BridgeDestruction = packet->ScenarioInfo.IsBridgeDestruction;
	Special.IsDestroyBridges = packet->ScenarioInfo.IsBridgeDestruction;
	Session.Options.Goodies = packet->ScenarioInfo.IsGoodies;
	Session.Options.FogOfWar = packet->ScenarioInfo.IsFogOfWar;
	Session.Options.HarvTruce = packet->ScenarioInfo.IsHarvTruce;
	Session.Options.AIPlayers = packet->ScenarioInfo.AIPlayers;
	Session.Options.AIDifficulty = (DiffType)packet->ScenarioInfo.AIDifficulty;
	BuildLevel = packet->ScenarioInfo.BuildLevel;
	Session.Options.Credits = packet->ScenarioInfo.Credits;
	Session.Options.UnitCount = packet->ScenarioInfo.UnitCount;
	Seed = packet->ScenarioInfo.Seed;
	Special = packet->ScenarioInfo.Special;
	Options.GameSpeed = Session.Options.GameSpeed = packet->ScenarioInfo.GameSpeed;
	Session.Options.ShortGame = packet->ScenarioInfo.IsShortGame;
	Session.Options.SWAllowed = packet->ScenarioInfo.IsSWAllowed;
	Session.Options.BuildOffAlly = packet->ScenarioInfo.IsBuildOffAlly;
	Session.Options.CrapEngineers = packet->ScenarioInfo.IsCrapEngineers;
	Special.IsTGrowth = true;
	Special.IsTSpread = true;
	Scen->Special.IsAllianceFixed = false;
	Session.Options.AlliesAllowed = true;
	Special.Apply_To_Game();

	if (stricmp(Session.ScenarioFileName, packet->ScenarioInfo.ShortFileName) != 0 ||
		stricmp(Session.ScenarioDigest, (char const *)(packet->ScenarioInfo.FileDigest)) != 0 ||
		Session.ScenarioIsOfficial != (bool)packet->ScenarioInfo.OfficialScenario ||
		Session.ScenarioFileLength != packet->ScenarioInfo.FileLength ||
		Session.Options.ScenarioDescription[0] == '\0') {

		DebugString("Scenario has changed\n");

		strcpy(Session.ScenarioFileName, packet->ScenarioInfo.ShortFileName);
		strcpy(Session.ScenarioDigest, (char const *)packet->ScenarioInfo.FileDigest);
		Session.ScenarioIsOfficial = packet->ScenarioInfo.OfficialScenario;
		Session.ScenarioFileLength = packet->ScenarioInfo.FileLength;

		bool found = false;
		if (strcmp(Session.ScenarioFileName, RANDOM_MAP_FILE_NAME) != 0 || !Session.ScenarioIsOfficial) {
			for (int i = 0; i < Session.Scenarios.Count(); i++) {
				MultiMission * mm = Session.Scenarios[i];
				if (stricmp(Session.ScenarioFileName, mm->Get_Filename()) == 0) {
					wcscpy(Session.Options.ScenarioDescription, mm->Description());
					found = true;
					break;
				}
			}
		}

		if (!found) {
			wcscpy(Session.Options.ScenarioDescription, packet->ScenarioInfo.Scenario);
		}

		if (MultiplayerMapPreview != NULL) {
			delete MultiplayerMapPreview;
			MultiplayerMapPreview = NULL;
		}
	}

	return(true);
}

// FUNCTION: 0x005B6DA0
void ModemGuest::SendAMessage(const wchar_t * message)
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

// FUNCTION: 0x005B6EB0
void ModemGuest::DrawMessage(const wchar_t * name, int color, const wchar_t * message)
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
