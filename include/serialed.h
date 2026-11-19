// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).
#pragma once

#include "session.h"

class SerialEdit
{
		friend BOOL CALLBACK Com_Settings_Dialog_Proc(HWND window, UINT message, WPARAM wparam, LPARAM lparam);
	public:
		bool Dialog(SerialSettingsType *settings);

	private:
		static __forceinline BOOL Handle_Message(HWND window, UINT message, WPARAM wparam, LPARAM lparam);
		bool Save(void);
		void Init(SerialSettingsType *settings);
		void Build_Port_Listbox(void);
		void Build_Baud_Rate_Listbox(void);
		void Build_Call_Wait_Listbox(void);
		void Build_Init_String_Listbox(void);
		void Set_Dial_Method(void);
		void Set_String(int id);
		void Set_Port(void);
		void Set_Init_String(void);
		void Set_Custom_Port(int id, int port, char *str);
		void Add_Init_String(void);
		void Delete_Init_String(void);

	private:

		HWND _Dialog;

		int RC;

		char TempBuf[128];

		SerialSettingsType TempSettings;

		int ModemsFound;
		char ModemNames[10][MODEM_NAME_MAX];
};
