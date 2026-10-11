// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"
#include "cdfile.h"
#include "diskswap.h"

HANDLE FindFileHandle = INVALID_HANDLE_VALUE;

// FUNCTION: 0x0047AA50
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

// FUNCTION: 0x0047AAB0
int CDFileClass::Open(int rights)
{
	return(BASECLASS::Open(rights));
}

// FUNCTION: 0x0047AAC0
void CDFileClass::Refresh_Search_Drives (void)
{
	Clear_Search_Drives();
	Set_Search_Drives(RawPath);
}

// FUNCTION: 0x0047AD50
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

// FUNCTION: 0x0047ADA0
void CDFileClass::Set_CD_Drive (int drive)
{
	LastCDDrive = CurrentCDDrive;
	CurrentCDDrive = drive;
}

// FUNCTION: 0x0047ADC0
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

// FUNCTION: 0x0047AF70
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

// FUNCTION: 0x0047B0C0
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

// FUNCTION: 0x0047B130
void CDFileClass::Find_Close(void)
{
	if (FindFileHandle != INVALID_HANDLE_VALUE) {
		FindClose(FindFileHandle);
		FindFileHandle = INVALID_HANDLE_VALUE;
	}
}
