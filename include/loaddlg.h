// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).
#pragma once

#include "session.h"

#include "house.hh"

template<class T> class DynamicVectorClass;

class FileEntryClass {
	public:
		char Descr[128];
		char Filename[32];
		unsigned Scenario;
		HousesType House;
		char PlayerName[64];
		int Num;
		FILETIME DateTime;
		bool Valid;
		bool Old;
		GameType Type;

		FileEntryClass(void) :
			Scenario(0),
			House(HOUSE_NONE),
			Num(-1),
			Valid(true),
			Old(false),
			Type(GAME_NORMAL)
		{
			Descr[0] = '\0';
			Filename[0] = '\0';
			PlayerName[0] = '\0';
			DateTime.dwHighDateTime = 0;
			DateTime.dwLowDateTime = 0;
		}
};

class LoadOptionsClass
{
	public:

		enum LoadStyleType {
			NONE = 0,
			LOAD,
			SAVE,
			WWDELETE
		};

		enum {
			GAMEVER_TS = 114465,
			GAMEVER_FS = 114849
		};

		LoadOptionsClass (void);
		virtual ~LoadOptionsClass (void);

		bool Load(void);
		bool Save(char * description);
		bool Delete(void);

		void Pick_Filename(char * file_name);
		bool Files_Present(void);

		virtual bool Load_File(const char * file_name);
		virtual bool Save_File(const char * file_name, const char * descr);
		virtual bool Delete_File(const char * file_name);
		virtual bool Read_File(FileEntryClass * entry, WIN32_FIND_DATAA * ff);
		virtual wchar_t const * GetUIString_Load(void) const;
		virtual wchar_t const * GetUIString_Save(void) const;
		virtual wchar_t const * GetUIString_Delete(void) const;
		virtual wchar_t const * GetUIString_GameSaved(void) const;

	protected:

		void Clear_List (void);
		void Fill_List (HWND window);
		int Num_From_Ext (char *fname);
		static int __cdecl Compare(const void *p1, const void *p2);

		bool Dialog(void);

		static void Load_Dialog_On_WM_COMMAND(HWND window, WPARAM wparam, LPARAM lparam, int id);
		static void Save_Dialog_On_WM_COMMAND(HWND window, WPARAM wparam, LPARAM lparam, int id);
		static void Delete_Dialog_On_WM_COMMAND(HWND window, WPARAM wparam, LPARAM lparam, int id);

		static LRESULT CALLBACK Load_Dialog_Proc(HWND window, UINT message, WPARAM wparam, LPARAM lparam);
		static LRESULT CALLBACK Save_Dialog_Proc(HWND window, UINT message, WPARAM wparam, LPARAM lparam);
		static LRESULT CALLBACK Delete_Dialog_Proc(HWND window, UINT message, WPARAM wparam, LPARAM lparam);

		LoadStyleType Style;

		char const * Extension;

		wchar_t * Description;

		unsigned int MinSpaceRequired;

	public:

		bool (*Callback)();

		enum LoadDialogState {
			STATE_PENDING	= -1,
			STATE_OK		= IDOK,
			STATE_CLOSE		= IDCANCEL
		};
		LoadDialogState State;

		unsigned int : 32;

		DynamicVectorClass<FileEntryClass *> Files;
};
