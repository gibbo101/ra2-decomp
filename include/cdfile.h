// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).
#pragma once

#include "bfiofile.h"

class CDFileClass : public BufferIOFileClass
{
		typedef BufferIOFileClass BASECLASS;

	public:
		CDFileClass(char const *filename);
		CDFileClass(void);
		virtual ~CDFileClass(void) override {};

		virtual char const * Set_Name(char const *filename) override;
		virtual int Open(char const *filename, int rights=READ) override;
		virtual int Open(int rights=READ) override;

		void Searching(int on) {IsDisabled = !on;};

		static bool Is_There_Search_Drives(void) {return(First != NULL);};
		static int Set_Search_Drives(char * pathlist);
		static void Add_Search_Drive(char *path);
		static void Clear_Search_Drives(void);
		static void Refresh_Search_Drives(void);
		static void Set_CD_Drive(int drive);
		static int  Get_CD_Drive(void) {return(CurrentCDDrive);};
		static int  Get_Last_CD_Drive(void) {return(LastCDDrive);};

		static bool Find_First_File(char *buffer);
		static bool Find_Next_File(char *buffer);
		static void Find_Close(void);

	private:

		bool IsDisabled;

		struct SearchDriveType {
			void * Next;
			char const * Path;
		};

		static SearchDriveType * First;

		static char RawPath[512];

		static int CurrentCDDrive;

		static int LastCDDrive;
};
