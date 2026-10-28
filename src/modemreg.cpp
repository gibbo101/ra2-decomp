// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"
#include "modemreg.h"
#include "_winfix.h"
#include "dbgprint.h"
#include <cstdio>

extern HKEY Get_Registry_Sub_Key (HKEY base_key, char const *search_key, bool close);

// FUNCTION: GAMEMD 0x005BB580
HKEY Search_Registry_Key (HKEY key_in, char const *value_name, char const *search_string)
{
	int 	top_key_index = 0;
	int 	retval;
	HKEY	next_key;
	HKEY	next_search;

	char				*subkey_name = new char [256];
	DWORD	subkey_name_length = 256;
	FILETIME			filetime;
	DWORD	value_type;
	unsigned char	*key_value = new unsigned char [256];
	DWORD	key_value_length = 256;

	do {
		subkey_name_length = 256;

		retval = RegEnumKeyEx (key_in, top_key_index++, subkey_name, &subkey_name_length, NULL, NULL, NULL, &filetime);

		if ( retval == ERROR_SUCCESS ){
			next_key = Get_Registry_Sub_Key (key_in, subkey_name, FALSE);

			if (next_key){
				key_value_length = 256;

				if ( RegQueryValueEx (next_key, value_name, NULL, &value_type, key_value, &key_value_length) == ERROR_SUCCESS){
					if (value_type == REG_SZ && !strcmp ((char*)key_value, search_string)){
						delete [] subkey_name;
						delete [] key_value;
						return(next_key);
					}
				}

				next_search = Search_Registry_Key (next_key, value_name, search_string);
				RegCloseKey (next_key);

				if (next_search){
					delete [] subkey_name;
					delete [] key_value;
					return(next_search);
				}
			}
		}
	} while (retval == ERROR_SUCCESS);

	delete [] subkey_name;
	delete [] key_value;

	return(0);
}

// FUNCTION: GAMEMD 0x005BB6E0
ModemRegistryEntryClass::ModemRegistryEntryClass (int modem_number)
{
	HKEY			key;
	unsigned char	return_buf[256];
	DWORD			retbuf_size = sizeof(return_buf);

	int				pnp = 0;

	ModemName = NULL;
	ModemDeviceName = NULL;
	ErrorCorrectionEnable = NULL;
	ErrorCorrectionDisable = NULL;
	CompressionEnable = NULL;
	CompressionDisable = NULL;
	HardwareFlowControl = NULL;

	char which_modem[5];
	wsprintf (which_modem, "%04d", modem_number);

	if (WinVersion.Is_Win9x()) {
		if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, "System\\CurrentControlSet\\Services\\Class\\Modem", 0, KEY_READ, &key) != ERROR_SUCCESS) {
			return;
		}
		if (RegOpenKeyEx(key, which_modem, REG_OPTION_NON_VOLATILE, KEY_READ, &key) != ERROR_SUCCESS) {
			DebugString("Unable to find System\\CurrentControlSet\\Services\\Class\\Modem in registry\n");
			return;
		}
	} else {
		if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, "System\\CurrentControlSet\\Control\\Class", 0, KEY_READ, &key) != ERROR_SUCCESS) {
			DebugString("Unable to find System\\CurrentControlSet\\Control\\Class in registry\n");
			return;
		}

		char registry_path[256];
		char key_name_buf[256];
		char key_class_buf[256];

		int key_index = 0;
		int key_name_buf_size = sizeof (key_name_buf);
		int key_class_buf_size = sizeof (key_class_buf);

		HKEY modem_key;
		FILETIME key_time;
		bool found = false;

		while (RegEnumKeyEx(key, key_index, key_name_buf, (LPDWORD)&key_name_buf_size, 0, key_class_buf, (LPDWORD)&key_class_buf_size, &key_time) == ERROR_SUCCESS) {
			strcpy(registry_path, "System\\CurrentControlSet\\Control\\Class\\");
			strcat(registry_path, key_name_buf);
			if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, registry_path, 0, KEY_READ, &modem_key) == ERROR_SUCCESS) {
				retbuf_size = sizeof (return_buf);
				if (RegQueryValueEx(modem_key, "Class", 0, 0, return_buf, &retbuf_size) == ERROR_SUCCESS && stricmp((char *)return_buf, "Modem") == 0) {
					found = true;
					break;
				}
			}
			key_index++;
			key_name_buf_size = sizeof (key_name_buf);
			key_class_buf_size = sizeof (key_class_buf);
		}

		if (!found) return;

		strcpy(registry_path, "System\\CurrentControlSet\\Control\\Class\\");
		strcat(registry_path, key_name_buf);
		strcat(registry_path, "\\");
		strcat(registry_path, which_modem);
		if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, registry_path, 0, KEY_READ, &key) != ERROR_SUCCESS) {
			return;
		}
	}

	retbuf_size = sizeof (return_buf);
	if (RegQueryValueEx(key, "Model", NULL, NULL, return_buf, &retbuf_size) != ERROR_SUCCESS){
		RegCloseKey (key);
		return;
	}
	ModemName = new char [retbuf_size+1];
	memcpy (ModemName, return_buf, retbuf_size);
	DebugString("Modem name is %s\n", ModemName);

	retbuf_size = sizeof (return_buf);
	bool attached_pnp = false;
	if (RegQueryValueEx(key, "AttachedTo", NULL, NULL, return_buf, &retbuf_size) != ERROR_SUCCESS) {
		retbuf_size = sizeof (return_buf);
		if (WinVersion.Is_Win9x() || RegQueryValueEx(key, "PnPAttachedTo", NULL, NULL, return_buf, &retbuf_size) != ERROR_SUCCESS) {
			attached_pnp = true;
		}
	}
	if (attached_pnp) {
		pnp = 1;
		ModemDeviceName = new char [strlen (ModemName)+1];
		strcpy (ModemDeviceName, ModemName);
	} else {
		ModemDeviceName = new char [retbuf_size+1];
		memcpy (ModemDeviceName, return_buf, retbuf_size);
	}
	DebugString("Modem attached to %s\n", ModemDeviceName);

	key = Get_Registry_Sub_Key (key, "Settings", TRUE);
	if (!key) return;

	retbuf_size = sizeof (return_buf);
	if (RegQueryValueEx(key, "ErrorControl_On", NULL, NULL, return_buf, &retbuf_size) == ERROR_SUCCESS){
		ErrorCorrectionEnable = new char [retbuf_size+1];
		memcpy (ErrorCorrectionEnable, return_buf, retbuf_size);
		DebugString("Modem error control enable string: %s\n", ErrorCorrectionEnable);
	}

	retbuf_size = sizeof (return_buf);
	if (RegQueryValueEx(key, "ErrorControl_Off", NULL, NULL, return_buf, &retbuf_size) == ERROR_SUCCESS){
		ErrorCorrectionDisable = new char [retbuf_size+1];
		memcpy (ErrorCorrectionDisable, return_buf, retbuf_size);
		DebugString("Modem error control disable string: %s\n", ErrorCorrectionDisable);
	}

	retbuf_size = sizeof (return_buf);
	if (RegQueryValueEx(key, "Compression_On", NULL, NULL, return_buf, &retbuf_size) == ERROR_SUCCESS){
		CompressionEnable = new char [retbuf_size+1];
		memcpy (CompressionEnable, return_buf, retbuf_size);
		DebugString("Modem compression enable string: %s\n", CompressionEnable);
	}

	retbuf_size = sizeof (return_buf);
	if (RegQueryValueEx(key, "Compression_Off", NULL, NULL, return_buf, &retbuf_size) == ERROR_SUCCESS){
		CompressionDisable = new char [retbuf_size+1];
		memcpy (CompressionDisable, return_buf, retbuf_size);
		DebugString("Modem compression disable string: %s\n", CompressionDisable);
	}

	retbuf_size = sizeof (return_buf);
	if (RegQueryValueEx(key, "FlowControl_Hard", NULL, NULL, return_buf, &retbuf_size) == ERROR_SUCCESS){
		HardwareFlowControl = new char [retbuf_size+1];
		memcpy (HardwareFlowControl, return_buf, retbuf_size);
		DebugString("Modem hardware flow control enable string: %s\n", HardwareFlowControl);
	}

	RegCloseKey (key);

	if (pnp){
		char search_string [256] = {"Modem\\"};
		strcat (search_string, which_modem);

		key = Get_Registry_Sub_Key (HKEY_LOCAL_MACHINE, "Enum", FALSE);
		if (!key) return;

		HKEY newkey = Search_Registry_Key ( key, "Driver", search_string );

		if (newkey){
			retbuf_size = sizeof (return_buf);

			retbuf_size = sizeof (return_buf);
			if (RegQueryValueEx(newkey, "PORTNAME", NULL, NULL, return_buf, &retbuf_size) == ERROR_SUCCESS){
				if (ModemDeviceName) delete [] ModemDeviceName;

				ModemDeviceName = new char [retbuf_size+1];
				memcpy (ModemDeviceName, return_buf, retbuf_size);
				DebugString("PNP Modem attached to %s\n", ModemName);
			}
		} else {
			if (ModemDeviceName != NULL && ModemName != NULL) {
				if (!strcmp (ModemName, ModemDeviceName)) {
					DebugString("Failed to find port for device %s\n", ModemName);
					delete [] ModemName;
					ModemName = NULL;
					delete [] ModemDeviceName;
					ModemDeviceName = NULL;
				}
			}
		}
		RegCloseKey (key);
	}
}

// FUNCTION: GAMEMD 0x005BBED0
ModemRegistryEntryClass::~ModemRegistryEntryClass (void)
{
	Clear();
}

// FUNCTION: GAMEMD 0x005BBEE0
void ModemRegistryEntryClass::Clear (void)
{
	if (ModemName) {
		delete [] ModemName;
		ModemName = NULL;
	}
	if (ModemDeviceName) {
		delete [] ModemDeviceName;
		ModemDeviceName = NULL;
	}

	if (ErrorCorrectionEnable) {
		delete [] ErrorCorrectionEnable;
		ErrorCorrectionEnable = NULL;
	}
	if (ErrorCorrectionDisable) {
		delete [] ErrorCorrectionDisable;
		ErrorCorrectionDisable = NULL;
	}

	if (CompressionEnable) {
		delete [] CompressionEnable;
		CompressionEnable = NULL;
	}
	if (CompressionDisable) {
		delete [] CompressionDisable;
		CompressionDisable = NULL;
	}

	if (HardwareFlowControl) {
		delete [] HardwareFlowControl;
		HardwareFlowControl = NULL;
	}
}
