// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"
#include "nulldlg.h"
#include "_map.h"
#include "_rules.h"
#include "_timer.h"
#include "data.h"
#include "dbgprint.h"
#include "globals.h"
#include "house.h"
#include "houstype.h"
#include "init.h"
#include "modemgst.h"
#include "modemhst.h"
#include "modemreg.h"
#include "msgbox.h"
#include "nullmgr.h"
#include "ownrdraw.h"
#include "phone.h"
#include "phoneed.h"
#include "rules.h"
#include "serialed.h"
#include "stimer.h"
#include "wincomm.h"
#include <ctime>

void Select_Serial_On_INITDIALOG(HWND window);

// FUNCTION: 0x005EFF70
void Select_Serial_On_INITDIALOG(HWND window)
{
	HWND handle;

	bool enabled = true;
	if (Session.SerialDefaults.Port == 0) {
		enabled = false;
	} else if (Session.SerialDefaults.IRQ == -1) {
		enabled = false;
	} else if (Session.SerialDefaults.Baud == -1 || NullModem.Detect_Port(&Session.SerialDefaults)) {
		enabled = false;
	}

	handle = GetDlgItem(window, IDC_SERIAL_DIAL);
	if (handle) {
		EnableWindow(handle, enabled);
	}

	handle = GetDlgItem(window, IDC_SERIAL_ANSWER);
	if (handle) {
		EnableWindow(handle, enabled);
	}

	handle = GetDlgItem(window, IDC_SERIAL_NULLMODEM);
	if (handle) {
		EnableWindow(handle, enabled);
	}
}

// FUNCTION: 0x005F0010
bool Init_Null_Modem( SerialSettingsType *settings )
{
	if ( NullModem.Init( settings->Port, settings->IRQ,
								settings->ModemName,
								settings->Baud, 0,  8, 1 ) ) {
		return(true);
	} else {
		return(false);
	}
}

// FUNCTION: 0x005F0040
void Shutdown_Modem( void )
{
	if (!Session.Play) {
		if (Session.Type == GAME_MODEM) {
			NullModem.Hangup_Modem();
		}
	}

	NullModem.Shutdown();
}
