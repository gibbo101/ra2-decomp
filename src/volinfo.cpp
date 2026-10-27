// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"
#include <windows.h>
#include <string.h>

struct DiskInfo
{
	int DriveType;
	char VolumeName[64];
	DWORD Serial;
	char FileSystem[64];
	unsigned FreeBytes;
	unsigned TotalBytes;
};

void __cdecl FUN_004068E0(char const * format, ...);

// FUNCTION: GAMEMD 0x00751DD0
bool __fastcall FUN_00751DD0(char const * root, DiskInfo * info)
{
	DWORD sectorsPerCluster;
	DWORD bytesPerSector;
	DWORD freeClusters;
	DWORD totalClusters;
	DWORD flags;
	DWORD maxComponent;

	memset(info, 0, sizeof(DiskInfo));
	info->DriveType = GetDriveType(root);

	if (!GetDiskFreeSpace(root, &sectorsPerCluster, &bytesPerSector, &freeClusters, &totalClusters)) {
		FUN_004068E0("GetDiskFreeSpace() failed: %s\n", 0);
		return false;
	}
	info->FreeBytes = sectorsPerCluster * bytesPerSector * freeClusters;
	if (freeClusters) {
		return false;
	}
	info->TotalBytes = sectorsPerCluster * bytesPerSector * totalClusters;

	if (!GetVolumeInformation(root, info->VolumeName, sizeof(info->VolumeName), &info->Serial, &maxComponent, &flags, info->FileSystem, sizeof(info->FileSystem))) {
		FUN_004068E0("GetVolumeInformation() - failed: %s\n", 0);
		return false;
	}
	return true;
}

// FUNCTION: GAMEMD 0x00751EA0
void __fastcall FUN_00751EA0(char const * path, int * dirCount, int * fileCount, unsigned long * totalSize)
{
	WIN32_FIND_DATA ff;
	char buffer[260];

	HANDLE handle = FindFirstFile(path, &ff);
	if (handle != INVALID_HANDLE_VALUE) {
		do {
			if (ff.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
				if (strcmpi(ff.cFileName, ".") != 0 && strcmpi(ff.cFileName, "..") != 0) {
					(*dirCount)++;
					strcpy(buffer, path);
					buffer[strlen(buffer) - 1] = '\0';
					strcat(buffer, ff.cFileName);
					strcat(buffer, "\\*");
					FUN_00751EA0(buffer, dirCount, fileCount, totalSize);
				}
			} else {
				(*fileCount)++;
				*totalSize += ff.nFileSizeLow;
			}
		} while (FindNextFile(handle, &ff));
		FindClose(handle);
	}
}
