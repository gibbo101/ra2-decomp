// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"
#include "netdlg.h"
#include "_map.h"
#include "_rules.h"
#include "data.h"
#include "dbgprint.h"
#include "event.h"
#include "globals.h"
#include "house.h"
#include "houstype.h"
#include "ipxmgr.h"
#include "language\language.h"
#include "list.h"
#include "msglist.h"
#include "nulldlg.h"
#include "queue.h"
#include "rules.h"
#include "session.h"
#include "stringtable.h"
#include "wonline.h"
#include "wsproto.h"
#include <ctime>
#include <math.h>

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
extern const double DAT_007EEEF8;
extern const double DAT_007EEF00;

struct ShortPair {
	unsigned short A, B;
	ShortPair() { A = 0; B = 0; }
};

struct IntTriple {
	int A, B, C;
	IntTriple() { A = 0; B = 0; C = 0; }
};

// SYNTHETIC: GAMEMD 0x005DA4D0 SYMBOL
// _$E80
double DAT_00ABFF10 = FUN_004CAC40(pow(DAT_007E1710, DAT_007E1708) * 2);

// SYNTHETIC: GAMEMD 0x005DA500 SYMBOL
// _$E83
double DAT_00ABFF60 = DAT_00ABFF10 / DAT_007EEEF8;

// SYNTHETIC: GAMEMD 0x005DA520 SYMBOL
// _$E86
double DAT_00ABFF48 = DAT_007E1718 / DAT_00ABFF60;

// SYNTHETIC: GAMEMD 0x005DA540 SYMBOL
// _$E89
double DAT_00ABFF40 = DAT_007E1720 * DAT_007EEF00;

// SYNTHETIC: GAMEMD 0x005DA560 SYMBOL
// _$E92
double DAT_00ABFF18 = DAT_007E1728 * DAT_007EEF00;

// SYNTHETIC: GAMEMD 0x005DA580 SYMBOL
// _$E95
double DAT_00ABFEF0 = DAT_007E1730 * DAT_007EEF00;

// SYNTHETIC: GAMEMD 0x005DA5A0 SYMBOL
// _$E98
int DAT_00ABFF68 = (int)(DAT_007E1738 * (FUN_004CAD50(DAT_00ABFEF0 - DAT_00ABFF18) * DAT_00ABFF10));

// SYNTHETIC: GAMEMD 0x005DA5E0 SYMBOL
// _$E101
double DAT_00ABFF20 = FUN_004CADE0(DAT_00ABFF68 * DAT_007E1740);

// SYNTHETIC: GAMEMD 0x005DA610 SYMBOL
// _$E104
double DAT_00ABFF38 = FUN_004CADE0((DAT_00ABFF68 * 2) / DAT_00ABFF10);

// SYNTHETIC: GAMEMD 0x005DA640 SYMBOL
// _$E107
int DAT_00ABFF5C = (int)(DAT_00ABFF68 * 4 + DAT_007E1738);

// SYNTHETIC: GAMEMD 0x005DA670 SYMBOL
// _$E110
ShortPair DAT_00ABFF08;

// SYNTHETIC: GAMEMD 0x005DA680 SYMBOL
// _$E113
IntTriple DAT_00ABFF28;

extern int DAT_00ABFF50, DAT_00ABFF54, DAT_00ABFF58;
extern int DAT_00AC01A8, DAT_00AC01AC, DAT_00AC01B0, DAT_00AC01B4;

// FUNCTION: GAMEMD 0x005DA6A0
void Init_Net_Pad(void)
{
	DAT_00ABFF58 = 0;
	DAT_00ABFF50 = 0x80;
	DAT_00ABFF54 = 0x80;
}

// FUNCTION: GAMEMD 0x005DA6C0
bool Init_Network(void)
{
	NetNumType net;
	NetNodeType node;

	if (!Ipx.Init()) {
		return false;
	}
	if (Session.Type != GAME_INTERNET) {
		if (Session.IsBridge) {
			Session.BridgeNet.Get_Address(net, node);
			Ipx.Set_Bridge(net);
		}
	}
	return true;
}

// FUNCTION: GAMEMD 0x005DA720
void Shutdown_Network(void)
{
	Session.GameName[0] = 0;
	if (PacketTransport) {
		delete PacketTransport;
	}
	PacketTransport = NULL;
}

namespace Unsorted { extern int CurrentFrame; }
namespace Game { extern int PlayerCount; namespace Network { extern int Tournament; } }
extern void __cdecl FUN_004068E0(char const *, ...);
extern NodeNameType * DAT_00AC119C;

struct Unknown_00B052D0 {
	void FUN_006646C0(int);
	int FUN_00664440(void);
	bool FUN_006639D0(void);
};
extern Unknown_00B052D0 DAT_00B052D0;

// FUNCTION: GAMEMD 0x005DA750
void __fastcall Destroy_Connection(int id, int error)
{
	int i;
	HouseClass *housep;
	wchar_t txt[80];

	housep = Houses[id];

	if (Debug_Print_Events) {
		FUN_004068E0("Destroying connection for house %d (%ls)\n", id, housep->UIName);
	}

	if (!housep || !housep->IsHuman) {
		return;
	}

	txt[0] = 0;
	if (error == 1) {
		swprintf(txt, StringTable::LoadString("TXT_CONNECTION_LOST", NULL, "D:\\ra2mdpost\\NetDlg.CPP", 0x1d0), housep->UIName);
	} else if (error == 0) {
		swprintf(txt, StringTable::LoadString("TXT_LEFT_GAME", NULL, "D:\\ra2mdpost\\NetDlg.CPP", 0x1d2), housep->UIName);
	}

	if (wcslen(txt)) {
		Session.Messages.Add_Message(NULL, 0, txt, housep->Class->Scheme, TextPrintType(0x4046), int(Rule->MessageDelay * TICKS_PER_MINUTE), false);
		Map.Flag_To_Redraw();
	}

	Ipx.Delete_Connection(id);

	if (error) {
		if (Unsorted::CurrentFrame == 0) {
			housep->Flag_To_Die();
		} else {
			OutList.Add(EventClass(PlayerPtr->HeapID, (unsigned char)EventClass::REMOVEPLAYER, id));
		}
	} else if (Session.Type == GAME_INTERNET) {
		if (Game::Network::Tournament) {
			housep->Flag_To_Die();
		}
		if (Session.Type == GAME_INTERNET && Session.ViaPacketRouter) {
			DAT_00B052D0.FUN_006646C0(id);
		}
	}

	if (Session.Type == GAME_INTERNET && Session.ViaPacketRouter && Session.Am_I_Master(housep)) {
		*(bool *)&Session.unknown_3098 = true;
		int newid = DAT_00B052D0.FUN_00664440();
		if (newid != -1) {
			HouseClass *newmaster = Houses[newid];
			if (newmaster) {
				int count = Session.Players.Count();
				if (count > 2) {
					IPXAddressClass *addr = NULL;
					for (i = 0; i < count; i++) {
						if (Session.Players[i]->Player.ID == newid) {
							addr = &Session.Players[i]->Address;
							*(unsigned short *)Session.unknown_2984 = Session.PlayerPorts[i];
							break;
						}
					}
					NetNumType net;
					NetNodeType node;
					addr->Get_Address(net, node);
					Session.unknown_296C.Set_Address(net, node, 0);
					FUN_004068E0("Session.PacketRouterAddress = %s\n", Session.unknown_296C.As_String(0));
					FUN_004068E0("Session.PacketRouterPort = %d\n", *(unsigned short *)Session.unknown_2984);
					Ipx.FUN_00543050(&Session.unknown_296C);
				}
				wcscpy(Session.MasterPlayerName, newmaster->UIName);
				FUN_004068E0("New game host is %ls\n", Session.MasterPlayerName);
				Session.MasterPlayerID = newid;
				if (Session.Am_I_Master(NULL)) {
					DAT_00B052D0.FUN_006639D0();
				}
			}
		}
	}

	for (i = 0; i < Session.Players.Count(); i++) {
		if (Session.Players[i]->Player.ID == id) {
			for (int j = 0; j < 8; j++) {
				if (Session.Slots[j] == Session.Players[i]) {
					Session.Slots[j] = DAT_00AC119C;
					break;
				}
			}
			delete Session.Players[i];
			Session.Players.Delete(Session.Players[i]);
			Session.FUN_0069BBF0(i);
			if (Session.Type == GAME_INTERNET && Session.ViaPacketRouter) {
				DAT_00B052D0.FUN_006646C0(id);
			}
			break;
		}
	}
	Game::PlayerCount--;
}



// FUNCTION: GAMEMD 0x005DAB80
unsigned int Compute_Name_CRC(char *name)
{
	char buf[80];
	unsigned int crc = 0L;
	int i;

	strcpy (buf, name);
	_strupr (buf);

	for (i = 0; (unsigned)i < strlen(buf); i++) {
		Add_CRC (&crc, (unsigned int)buf[i]);
	}

	return(crc);
}

// FUNCTION: GAMEMD 0x005DAC00
unsigned int Compute_Name_CRC_W(wchar_t *name)
{
	wchar_t buf[80];
	unsigned int crc = 0L;
	unsigned int i;

	wcscpy(buf, name);
	_wcsupr(buf);

	for (i = 0; i < wcslen(buf); i++) {
		Add_CRC(&crc, (unsigned int)buf[i]);
	}
	return crc;
}

// FUNCTION: GAMEMD 0x005DAC80
void Clear_Listbox(ListClass *list)
{
	wchar_t *item;

	while (list->Count()) {
		item = (wchar_t *)list->Get_Item(0);
		list->Remove_Item(item);
		delete item;
	}
	list->Flag_To_Redraw();
}

// FUNCTION: GAMEMD 0x005DACD0
void Clear_Net_Counters(void)
{
	DAT_00AC01A8 = 0;
	DAT_00AC01AC = 0;
	DAT_00AC01B0 = 0;
	DAT_00AC01B4 = 0;
}
