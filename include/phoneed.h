// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).
#pragma once

#include "phone.h"

class PhoneEdit
{
		friend BOOL CALLBACK Phone_Dialog_Proc(HWND window, UINT message, WPARAM wparam, LPARAM lparam);
	public:
		PhoneEdit(void) {}
		bool Dialog(void);

	private:
		bool Dial(void);
		void Build_Phone_Listbox(void);
		void Select(void);
		void Add(void);
		void Edit(void);
		void Delete(void);

	private:

		HWND _Dialog;

		int RC;

		bool Changed;

		char Number[PhoneEntryClass::PHONE_MAX_NUM];
};
