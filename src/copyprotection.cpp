// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#define INCLUDE_COM
#include "always.h"
#include "globals.h"
#include "version.h"

#include <mmsystem.h>

void __cdecl FUN_004068E0(char const *fmt, ...);

namespace CopyProtection {

static char *LauncherMessage;
static const UINT LauncherMessageID = 0xBEEF;

bool __fastcall CheckVersion(void);

// FUNCTION: GAMEMD 0x0049F5C0
// CopyProtection::IsLauncherRunning
bool IsLauncherRunning(void)
{
	FUN_004068E0("COPYPROTECTION - Checking if launcher is running\n");
	HANDLE mutex = CreateMutex(NULL, FALSE, "48BC11BD-C4D7-466b-8A31-C6ABBAD47B3E");
	bool running = GetLastError() == ERROR_ALREADY_EXISTS;
	if (mutex) {
		CloseHandle(mutex);
	}
	FUN_004068E0("result was %d\n", running);
	return running;
}

// FUNCTION: GAMEMD 0x0049F620
// CopyProtection::NotifyLauncher
bool NotifyLauncher(void)
{
	MSG msg;
	FUN_004068E0("COPYPROTECTION - Notify launcher\n");
	PeekMessage(&msg, NULL, WM_USER, WM_USER, PM_NOREMOVE);
	DWORD deadline = timeGetTime() + 60000;
	HANDLE event = NULL;
	while (timeGetTime() < deadline) {
		event = OpenEvent(EVENT_MODIFY_STATE, TRUE, "D6E7FC97-64F9-4d28-B52C-754EDF721C6F");
		if (event) {
			break;
		}
		Sleep(0);
	}
	if (event) {
		SetEvent(event);
		CloseHandle(event);
		FUN_004068E0("Launcher notified.\n");
		FUN_004068E0("Waiting for message from launcher.\n");
		deadline = timeGetTime() + 10000;
		while (timeGetTime() <= deadline) {
			if (PeekMessage(&msg, NULL, LauncherMessageID, LauncherMessageID, PM_REMOVE) && msg.message == LauncherMessageID) {
				LauncherMessage = (char *)MapViewOfFileEx((HANDLE)msg.lParam, FILE_MAP_ALL_ACCESS, 0, 0, 0, NULL);
				if (LauncherMessage) {
					return true;
				}
				break;
			}
			Sleep(0);
		}
	}
	FUN_004068E0("***** Failed to notify launcher!\n");
	return false;
}

// FUNCTION: GAMEMD 0x0049F740
// CopyProtection::DispatchLauncherMessage
void __fastcall DispatchLauncherMessage(MSG *msg)
{
	if (msg->message == LauncherMessageID) {
		FUN_004068E0("COPYPROTECTION - Received message from launcher.\n");
		LauncherMessage = (char *)MapViewOfFileEx((HANDLE)msg->lParam, FILE_MAP_ALL_ACCESS, 0, 0, 0, NULL);
		if (!LauncherMessage) {
			FUN_004068E0("***** MapViewOfFileEx() Failed!");
		} else {
			FUN_004068E0("The message says: %s\n", LauncherMessage);
		}
	}
}

// FUNCTION: GAMEMD 0x0049F7A0
// CopyProtection::CheckProtectedData
bool CheckProtectedData(void)
{
	FUN_004068E0("COPYPROTECTION - Validating\n");
	FUN_004068E0("_mProtectedData = %d (%s)\n", LauncherMessage, LauncherMessage ? LauncherMessage : "EEK!");
	if (LauncherMessage) {
		return strcmp(LauncherMessage, "UIDATA,3DDATA,MAPS") == 0;
	}
	return CheckVersion();
}

// FUNCTION: GAMEMD 0x0049F8B0
// CopyProtection::ShutDown
void ShutDown(void)
{
	FUN_004068E0("COPYPROTECTION - Shutdown\n");
	if (LauncherMessage) {
		UnmapViewOfFile(LauncherMessage);
		LauncherMessage = NULL;
	}
	HKEY key;
	if (RegOpenKeyEx(HKEY_CLASSES_ROOT, "WChat\\SysID", 0, KEY_ALL_ACCESS, &key) == ERROR_SUCCESS) {
		RegDeleteValue(key, "ID");
		RegCloseKey(key);
	}
}

// FUNCTION: GAMEMD 0x0049F920
// CopyProtection::CheckVersion
bool __fastcall CheckVersion(void)
{
	HKEY key;
	DWORD size;
	DWORD type;
	DWORD value;

	if (RegOpenKeyEx(HKEY_CLASSES_ROOT, "WChat\\SysID", 0, KEY_READ, &key) != ERROR_SUCCESS) {
		return(false);
	}
	size = 4;
	LONG status = RegQueryValueEx(key, "ID", NULL, &type, (BYTE *)&value, &size);
	RegCloseKey(key);
	if (status != ERROR_SUCCESS || type != REG_DWORD) {
		return(false);
	}
	return(value == VerNum.Version_Number());
}

}
