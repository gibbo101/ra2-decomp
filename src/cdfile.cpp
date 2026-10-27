// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"
#include "cdfile.h"
#include "geometry.h"
#include "rect.h"
#include "diskswap.h"
#include <math.h>


HANDLE FindFileHandle = INVALID_HANDLE_VALUE;

// FUNCTION: GAMEMD 0x0047A9D0
CDFileClass::CDFileClass(char const * filename) :
	IsDisabled(false)
{
	CDFileClass::Set_Name(filename);
}

// FUNCTION: GAMEMD 0x0047AA30
CDFileClass::CDFileClass(void) :
	IsDisabled(false)
{
}

// FUNCTION: GAMEMD 0x0047AA50
int Is_Disk_Inserted(int disk)
{
	WIN32_FIND_DATA fb;
	HANDLE file_handle;

	char	scan[] = "?:\\*.*";

	scan[0] = (char)('A' + disk);

	file_handle = FindFirstFile(scan, &fb);
	if (file_handle == INVALID_HANDLE_VALUE) {
		return(false);
	}
	FindClose(file_handle);
	return(true);
}

// FUNCTION: GAMEMD 0x0047AAB0
int CDFileClass::Open(int rights)
{
	return(BASECLASS::Open(rights));
}

// FUNCTION: GAMEMD 0x0047AAC0
void CDFileClass::Refresh_Search_Drives (void)
{
	Clear_Search_Drives();
	Set_Search_Drives(RawPath);
}

struct CDDriveManagerClass {
	static int __fastcall GetCDNumber(int drive, int seconds);
};

// FUNCTION: GAMEMD 0x0047AB10
int CDFileClass::Set_Search_Drives(char * pathlist)
{
	bool found = false;

	if (!pathlist) return(0);

	char *copy_pathlist = strdup(pathlist);

	if (copy_pathlist != RawPath) {
		strcat (RawPath, ";");
		strcat (RawPath, copy_pathlist);
	}

	char const * ptr = strtok(copy_pathlist, ";");
	while (ptr != NULL) {
		if (strlen(ptr) > 0) {
			char path[MAX_PATH];

			strcpy(path, ptr);
			switch (path[strlen(path)-1]) {
				case ':':
				case '\\':
					break;

				default:
					strcat(path, "\\");
					break;
			}

			if (strncmp(path, "?:", 2) == 0) {
				if (CurrentCDDrive) {
					found = true;
					if (CDDriveManagerClass::GetCDNumber(CurrentCDDrive, 2*60) > -1) {
						path[0] = (char)(CurrentCDDrive + 'A');
						Add_Search_Drive(path);
					}
				}
				ptr = strtok(NULL, ";");
				continue;
			}

			int drive = path[0] - 'A';
			if (drive > 0 && CDDriveManagerClass::GetCDNumber(drive, 2*60) == 2) {
				found = true;
				Add_Search_Drive(path);
			}
		}
		ptr = strtok(NULL, ";");
	}

	free(copy_pathlist);

	if (!found) return(1);
	return(0);
}

// FUNCTION: GAMEMD 0x0047AD50
void CDFileClass::Add_Search_Drive(char *path)
{
	SearchDriveType *srch;
	srch	= new SearchDriveType;

	srch->Path = strdup(path);
	srch->Next = NULL;

	if (!First) {
		First = srch;
	} else {
		SearchDriveType * chain = First;

		while (chain->Next) {
			chain = (SearchDriveType *)chain->Next;
		}
		chain->Next = srch;
	}
}

// FUNCTION: GAMEMD 0x0047ADA0
void CDFileClass::Set_CD_Drive (int drive)
{
	LastCDDrive = CurrentCDDrive;
	CurrentCDDrive = drive;
}

// FUNCTION: GAMEMD 0x0047ADC0
void CDFileClass::Clear_Search_Drives(void)
{
	SearchDriveType	* chain;

	chain = First;
	while (chain) {
		SearchDriveType	*next;

		next = (SearchDriveType *)chain->Next;
		if (chain->Path) {
			free((char *)chain->Path);
		}
		delete chain;

		chain = next;
	}
	First = 0;
}

// FUNCTION: GAMEMD 0x0047AE10
char const * CDFileClass::Set_Name(char const *filename)
{
	BASECLASS::Set_Name(filename);
	if (IsDisabled || !First || BASECLASS::Is_Available()) return(File_Name());
	SearchDriveType * srch = First;

	while (srch) {
		char path[_MAX_PATH];
		strcpy(path, srch->Path);
		strcat(path, filename);
		BASECLASS::Set_Name(path);
		if (BASECLASS::Is_Available()) {
			return(File_Name());
		}
		srch = (SearchDriveType *)srch->Next;
	}
	BASECLASS::Set_Name(filename);
	return(File_Name());
}

// FUNCTION: GAMEMD 0x0047AF10
int CDFileClass::Open(char const *filename, int rights)
{
	CDFileClass::Close();
	if (!filename) {
		Error(ENOENT, false);
	}
	if (IsDisabled || rights == WRITE) {

		BASECLASS::Set_Name( filename );
		return( BASECLASS::Open( rights ) );
	}
	Set_Name(filename);
	return(BASECLASS::Open(rights));
}

// FUNCTION: GAMEMD 0x0047AF70
bool CDFileClass::Find_First_File(char *fname)
{
	WIN32_FIND_DATAA fb;
	char scan_path[MAX_PATH];
	SearchDriveType *entry;

	if (fname) {
		Find_Close();

		strcpy(scan_path, fname);

		HANDLE file_handle = ::FindFirstFile(scan_path, &fb);
		if (file_handle != INVALID_HANDLE_VALUE && !(fb.dwFileAttributes & (FILE_ATTRIBUTE_TEMPORARY|FILE_ATTRIBUTE_DIRECTORY|FILE_ATTRIBUTE_SYSTEM|FILE_ATTRIBUTE_HIDDEN))) {
			strcpy(fname, fb.cFileName);
			FindFileHandle = file_handle;

			return(true);
		}

		entry = First;

		if (entry != NULL) {
			while (true) {
				strcpy(scan_path, entry->Path);
				strcat(scan_path, fname);

				file_handle = ::FindFirstFile(scan_path, &fb);
				if (file_handle != INVALID_HANDLE_VALUE && !(fb.dwFileAttributes & (FILE_ATTRIBUTE_TEMPORARY|FILE_ATTRIBUTE_DIRECTORY|FILE_ATTRIBUTE_SYSTEM|FILE_ATTRIBUTE_HIDDEN))) {
					break;
				}

				entry = (SearchDriveType *)entry->Next;
				if (entry == NULL) {
					return(false);
				}
			}

			strcpy(fname, fb.cFileName);
			FindFileHandle = file_handle;

			return(true);
		}
	}
	return(false);
}

// FUNCTION: GAMEMD 0x0047B0C0
bool CDFileClass::Find_Next_File(char *buffer)
{
	WIN32_FIND_DATAA fb;

	if (buffer) {
		if (FindFileHandle != INVALID_HANDLE_VALUE && ::FindNextFile(FindFileHandle, &fb) == TRUE) {
			strcpy(buffer, fb.cFileName);
			return(true);
		}

		buffer[0] = '\0';
	}
	return(false);
}

// FUNCTION: GAMEMD 0x0047B130
void CDFileClass::Find_Close(void)
{
	if (FindFileHandle != INVALID_HANDLE_VALUE) {
		FindClose(FindFileHandle);
		FindFileHandle = INVALID_HANDLE_VALUE;
	}
}
// SYNTHETIC: GAMEMD 0x0047B150 SYMBOL
// _$E6
// SYNTHETIC: GAMEMD 0x0047B180 SYMBOL
// _$E9
// SYNTHETIC: GAMEMD 0x0047B1A0 SYMBOL
// _$E12
// SYNTHETIC: GAMEMD 0x0047B1C0 SYMBOL
// _$E15
// SYNTHETIC: GAMEMD 0x0047B1E0 SYMBOL
// _$E18
// SYNTHETIC: GAMEMD 0x0047B200 SYMBOL
// _$E21
// SYNTHETIC: GAMEMD 0x0047B220 SYMBOL
// _$E24
// SYNTHETIC: GAMEMD 0x0047B260 SYMBOL
// _$E27
// SYNTHETIC: GAMEMD 0x0047B290 SYMBOL
// _$E30
// SYNTHETIC: GAMEMD 0x0047B2C0 SYMBOL
// _$E33
// SYNTHETIC: GAMEMD 0x0047B2F0 SYMBOL
// _$E36
// SYNTHETIC: GAMEMD 0x0047B300 SYMBOL
// _$E39
// SYNTHETIC: GAMEMD 0x0047B320 SYMBOL
// _$E42
// SYNTHETIC: GAMEMD 0x0047B340 SYMBOL
// _$E45

