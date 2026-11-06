// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"
#include "volinfo.h"
#include "cd.h"
#include <windows.h>
#include <string.h>
#include <stdio.h>

void __cdecl FUN_004068E0(char const * format, ...);

// FUNCTION: GAMEMD 0x00751C30
bool VolumeCheck(void)
{
	char buffer[260];
	VolInfo info;
	if (!GetLogicalDriveStrings(sizeof(buffer), buffer)) {
		return (false);
	}
	const char * drive_root_name = buffer;
	while (strlen(drive_root_name) != 0) {
		if (GetDriveType(drive_root_name) == DRIVE_CDROM) {
			if (!GetVolInfo(drive_root_name, &info)) {
				FUN_004068E0("VolumeCheck() - GetVolInfo failed\n");
				drive_root_name += strlen(drive_root_name) + 1;
				continue;
			} else {
				char name[32];
				for (int i = 0; i < 8; i++) {
					sprintf(name, "RA%d", i + 1);
					if (!stricmp(name, info.VolumeName)) {
						return (true);
					}
				}
			}
		}
		drive_root_name += strlen(drive_root_name) + 1;
	}
	if (CD().Force_Available()) {
		return (true);
	}
	return (false);
}

// FUNCTION: GAMEMD 0x00751DD0
inline bool GetVolInfo(char const * root, VolInfo * info)
{
	DWORD sectorsPerCluster;
	DWORD bytesPerSector;
	DWORD freeClusters;
	DWORD totalClusters;
	DWORD flags;
	DWORD maxComponent;

	memset(info, 0, sizeof(VolInfo));
	info->DriveType = GetDriveType(root);

	if (!GetDiskFreeSpace(root, &sectorsPerCluster, &bytesPerSector, &freeClusters, &totalClusters)) {
		FUN_004068E0("GetDiskFreeSpace() failed: %s\n", 0);
		return false;
	}
	info->FreeSpace = sectorsPerCluster * bytesPerSector * freeClusters;
	if (freeClusters) {
		return false;
	}
	info->TotalSpace = sectorsPerCluster * bytesPerSector * totalClusters;

	if (!GetVolumeInformation(root, info->VolumeName, sizeof(info->VolumeName), (DWORD *)&info->VolumeSerialNumber, &maxComponent, &flags, info->FileSystemName, sizeof(info->FileSystemName))) {
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
