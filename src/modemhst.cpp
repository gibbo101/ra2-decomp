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

ModemHost * _ModemHost;

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

// FUNCTION: 0x005B9A20
void ModemHost::SetCrapEngineers(void)
{
	HWND handle = GetDlgItem(_Dialog, IDC_MULTI_ENGINEER);
	if (handle) {
		Session.Options.CrapEngineers = Button_GetCheck(handle) == TRUE;
		Transmit = true;
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
