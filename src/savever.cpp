// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"
#include "savever.h"
#include "dbgprint.h"
#include "session.h"
#include <comdef.h>
#include "fastmath.h"

// FUNCTION: GAMEMD 0x00682150
HRESULT SaveVersionInfo::Load_String(IStorage *storage, int id, char *string)
{
	*string = '\0';

	HRESULT res;
	IStreamPtr stm;

	res = storage->OpenStream(Stream_Name_From_ID(id), NULL, STGM_SHARE_EXCLUSIVE, 0, &stm);
	if (FAILED(res)) {
		return(res);
	}

	int i = 0;
	while (true) {
		WCHAR buf[128];
		res = stm->Read(&buf[i], sizeof(buf[i]), NULL);
		if (FAILED(res)) {
			return(res);
		}
		if (buf[i] == '\0') {
			WideCharToMultiByte(CP_ACP, 0, buf, -1, string, ARRAY_SIZE(buf) - 1, 0, 0);
			return(res);
		}
		i++;
	}

	return(S_OK);
}

// FUNCTION: GAMEMD 0x00682240
HRESULT SaveVersionInfo::Load_String(IStorage *storage, int id, wchar_t *string)
{
	*string = '\0';

	HRESULT res;
	IStreamPtr stm;

	res = storage->OpenStream(Stream_Name_From_ID(id), NULL, STGM_SHARE_EXCLUSIVE, 0, &stm);
	if (FAILED(res)) {
		return(res);
	}

	while (true) {
		res = stm->Read(string, sizeof(*string), NULL);
		if (FAILED(res)) {
			return(res);
		}
		if (*string == '\0') {
			return(res);
		}
		string++;
	}
}

// FUNCTION: GAMEMD 0x006822F0
HRESULT SaveVersionInfo::Load_String_Set(IPropertySetStorage *storageset, int id, char *string)
{
	*string = '\0';

	HRESULT res;
	IPropertyStoragePtr storage;

	res = storageset->Open(FMTID_SummaryInformation, STGM_SHARE_EXCLUSIVE|STGM_READWRITE, &storage);
	if (FAILED(res)) {
		return(res);
	}

	PROPSPEC propsec;
	propsec.ulKind = PRSPEC_PROPID;
	propsec.propid = id;
	PROPVARIANT propvar;

	res = storage->ReadMultiple(1, &propsec, &propvar);
	if (FAILED(res)) {
		return(res);
	}

	if (propvar.vt == VT_LPWSTR) {
		WideCharToMultiByte(CP_ACP, 0, propvar.pwszVal, -1, string, 128, 0, 0);
	}

	return(res);
}

// FUNCTION: GAMEMD 0x006823D0
HRESULT SaveVersionInfo::Load_String_Set(IPropertySetStorage *storageset, int id, wchar_t *string)
{
	*string = '\0';

	HRESULT res;
	IPropertyStoragePtr storage;

	res = storageset->Open(FMTID_SummaryInformation, STGM_SHARE_EXCLUSIVE|STGM_READWRITE, &storage);
	if (FAILED(res)) {
		return(res);
	}

	PROPSPEC propsec;
	propsec.ulKind = PRSPEC_PROPID;
	propsec.propid = id;
	PROPVARIANT propvar;

	res = storage->ReadMultiple(1, &propsec, &propvar);
	if (FAILED(res)) {
		return(res);
	}

	if (propvar.vt == VT_LPWSTR) {
		wcsncpy(string, propvar.pwszVal, 128);
	}

	return(res);
}

// FUNCTION: GAMEMD 0x006824B0
HRESULT SaveVersionInfo::Load_Int(IStorage *storage, int id, int *integer)
{
	*integer = 0;

	HRESULT res;
	IStreamPtr stm;

	res = storage->OpenStream(Stream_Name_From_ID(id), NULL, STGM_SHARE_EXCLUSIVE, 0, &stm);
	if (FAILED(res)) {
		return(res);
	}

	res = stm->Read(integer, sizeof(*integer), NULL);
	if (FAILED(res)) {
		return(res);
	}

	return(res);
}

// FUNCTION: GAMEMD 0x00682570
HRESULT SaveVersionInfo::Load_Int_Set(IPropertySetStorage *storageset, int id, int *integer)
{
	*integer = 0;

	HRESULT res;
	IPropertyStoragePtr storage;

	res = storageset->Open(FMTID_SummaryInformation, STGM_SHARE_EXCLUSIVE|STGM_READWRITE, &storage);
	if (FAILED(res)) {
		return(res);
	}

	PROPSPEC propsec;
	propsec.ulKind = PRSPEC_PROPID;
	propsec.propid = id;
	PROPVARIANT propvar;

	res = storage->ReadMultiple(1, &propsec, &propvar);
	if (FAILED(res)) {
		return(res);
	}

	if (propvar.vt == VT_I4) {
		*integer = propvar.lVal;
	}

	return(res);
}

// FUNCTION: GAMEMD 0x00682640
HRESULT SaveVersionInfo::Save_String(IStorage *storage, int id, char *string)
{
	WCHAR buf[128];

	MultiByteToWideChar(CP_ACP, 0, string, -1, buf, ARRAY_SIZE(buf));

	IStreamPtr stm(NULL);

	HRESULT res = storage->CreateStream(Stream_Name_From_ID(id), STGM_SHARE_EXCLUSIVE|STGM_READWRITE, 0, 0, &stm);
	if (FAILED(res)) {
		return(res);
	}

	res = stm->Write(buf, sizeof(WCHAR) * wcslen(buf) + 2, NULL);
	if (FAILED(res)) {
		return(res);
	}
	res = stm->Commit(0);
	if (FAILED(res)) {
		return(res);
	}

	return(res);
}

// FUNCTION: GAMEMD 0x00682780
HRESULT SaveVersionInfo::Save_String(IStorage *storage, int id, const wchar_t *string)
{
	IStreamPtr stm(NULL);

	HRESULT res = storage->CreateStream(Stream_Name_From_ID(id), STGM_SHARE_EXCLUSIVE|STGM_READWRITE, 0, 0, &stm);
	if (FAILED(res)) {
		return(res);
	}

	res = stm->Write(string, sizeof(WCHAR) * wcslen(string) + 2, NULL);
	if (FAILED(res)) {
		return(res);
	}
	res = stm->Commit(0);
	if (FAILED(res)) {
		return(res);
	}

	return(res);
}

// FUNCTION: GAMEMD 0x00682880
HRESULT SaveVersionInfo::Save_String_Set(IPropertySetStorage *storageset, int id, const char *string)
{
	WCHAR buf[128];

	MultiByteToWideChar(CP_ACP, 0, string, -1, buf, ARRAY_SIZE(buf));

	HRESULT res;
	IPropertyStoragePtr storage;

	res = storageset->Open(FMTID_SummaryInformation, STGM_SHARE_EXCLUSIVE|STGM_READWRITE, &storage);
	if (FAILED(res)) {
		res = storageset->Create(FMTID_SummaryInformation, NULL, PROPSETFLAG_DEFAULT, STGM_SHARE_EXCLUSIVE|STGM_READWRITE|STGM_CREATE, &storage);
		if (FAILED(res)) {
			return(res);
		}
	}

	PROPSPEC propsec;
	propsec.ulKind = PRSPEC_PROPID;
	propsec.propid = id;
	PROPVARIANT propvar;

	propvar.vt = VT_LPWSTR;
	propvar.pwszVal = buf;

	res = storage->WriteMultiple(1, &propsec, &propvar, PID_FIRST_USABLE);
	if (FAILED(res)) {
		return(res);
	}

	return(res);
}

// FUNCTION: GAMEMD 0x006829A0
HRESULT SaveVersionInfo::Save_String_Set(IPropertySetStorage *storageset, int id, const wchar_t *string)
{
	HRESULT res;
	IPropertyStoragePtr storage;

	res = storageset->Open(FMTID_SummaryInformation, STGM_SHARE_EXCLUSIVE|STGM_READWRITE, &storage);
	if (FAILED(res)) {
		res = storageset->Create(FMTID_SummaryInformation, NULL, PROPSETFLAG_DEFAULT, STGM_SHARE_EXCLUSIVE|STGM_READWRITE|STGM_CREATE, &storage);
		if (FAILED(res)) {
			return(res);
		}
	}

	PROPSPEC propsec;
	propsec.ulKind = PRSPEC_PROPID;
	propsec.propid = id;
	PROPVARIANT propvar;

	propvar.vt = VT_LPWSTR;
	propvar.pwszVal = (wchar_t *)string;

	res = storage->WriteMultiple(1, &propsec, &propvar, PID_FIRST_USABLE);
	if (FAILED(res)) {
		return(res);
	}

	return(res);
}

// FUNCTION: GAMEMD 0x00682A90
HRESULT SaveVersionInfo::Save_Int(IStorage *storage, int id, int integer)
{
	IStreamPtr stm(NULL);

	HRESULT res = storage->CreateStream(Stream_Name_From_ID(id), STGM_SHARE_EXCLUSIVE|STGM_READWRITE, 0, 0, &stm);
	if (FAILED(res)) {
		return(res);
	}

	res = stm->Write(&integer, sizeof(integer), NULL);
	if (FAILED(res)) {
		return(res);
	}
	res = stm->Commit(STGM_READ);
	if (FAILED(res)) {
		return(res);
	}

	return(res);
}

// FUNCTION: GAMEMD 0x00682B80
HRESULT SaveVersionInfo::Save_Int_Set(IPropertySetStorage *storageset, int id, int integer)
{
	HRESULT res;
	IPropertyStoragePtr storage;

	res = storageset->Open(FMTID_SummaryInformation, STGM_SHARE_EXCLUSIVE|STGM_READWRITE, &storage);
	if (FAILED(res)) {
		res = storageset->Create(FMTID_SummaryInformation, NULL, PROPSETFLAG_DEFAULT, STGM_SHARE_EXCLUSIVE|STGM_READWRITE|STGM_CREATE, &storage);
		if (FAILED(res)) {
			return(res);
		}
	}

	PROPSPEC propsec;
	propsec.ulKind = PRSPEC_PROPID;
	propsec.propid = id;
	PROPVARIANT propvar;

	propvar.vt = VT_I4;
	propvar.lVal = integer;

	res = storage->WriteMultiple(1, &propsec, &propvar, PID_FIRST_USABLE);
	if (FAILED(res)) {
		return(res);
	}

	return(res);
}

// FUNCTION: GAMEMD 0x00682C70
HRESULT SaveVersionInfo::Load_Time(IStorage *storage, int id, FILETIME *time)
{
	time->dwLowDateTime = 0;
	time->dwHighDateTime = 0;

	HRESULT res;
	IStreamPtr stm;

	res = storage->OpenStream(Stream_Name_From_ID(id), NULL, STGM_SHARE_EXCLUSIVE, 0, &stm);
	if (FAILED(res)) {
		return(res);
	}

	res = stm->Read(time, sizeof(*time), NULL);
	if (FAILED(res)) {
		return(res);
	}

	return(res);
}

// FUNCTION: GAMEMD 0x00682D30
HRESULT SaveVersionInfo::Load_Time_Set(IPropertySetStorage *storageset, int id, FILETIME *time)
{
	time->dwLowDateTime = 0;
	time->dwHighDateTime = 0;

	HRESULT res;
	IPropertyStoragePtr storage;

	res = storageset->Open(FMTID_SummaryInformation, STGM_SHARE_EXCLUSIVE|STGM_READWRITE, &storage);
	if (FAILED(res)) {
		return(res);
	}

	PROPSPEC propsec;
	propsec.ulKind = PRSPEC_PROPID;
	propsec.propid = id;
	PROPVARIANT propvar;

	res = storage->ReadMultiple(1, &propsec, &propvar);
	if (FAILED(res)) {
		return(res);
	}

	if (propvar.vt == VT_FILETIME) {
		*time = propvar.filetime;
	}

	return(res);
}

// FUNCTION: GAMEMD 0x00682E10
HRESULT SaveVersionInfo::Save_Time(IStorage *storage, int id, FILETIME *time)
{
	IStreamPtr stm(NULL);

	HRESULT res = storage->CreateStream(Stream_Name_From_ID(id), STGM_SHARE_EXCLUSIVE|STGM_READWRITE, 0, 0, &stm);
	if (FAILED(res)) {
		return(res);
	}

	res = stm->Write(time, sizeof(*time), NULL);
	if (FAILED(res)) {
		return(res);
	}
	res = stm->Commit(STGM_READ);
	if (FAILED(res)) {
		return(res);
	}

	return(res);
}

// FUNCTION: GAMEMD 0x00682F00
HRESULT SaveVersionInfo::Save_Time_Set(IPropertySetStorage *storageset, int id, FILETIME *time)
{
	HRESULT res;
	IPropertyStoragePtr storage;

	res = storageset->Open(FMTID_SummaryInformation, STGM_SHARE_EXCLUSIVE|STGM_READWRITE, &storage);
	if (FAILED(res)) {
		res = storageset->Create(FMTID_SummaryInformation, NULL, PROPSETFLAG_DEFAULT, STGM_SHARE_EXCLUSIVE|STGM_READWRITE|STGM_CREATE, &storage);
		if (FAILED(res)) {
			return(res);
		}
	}

	PROPSPEC propsec;
	propsec.ulKind = PRSPEC_PROPID;
	propsec.propid = id;
	PROPVARIANT propvar;

	propvar.vt = VT_FILETIME;
	propvar.filetime = *time;

	res = storage->WriteMultiple(1, &propsec, &propvar, PID_FIRST_USABLE);
	if (FAILED(res)) {
		return(res);
	}

	return(res);
}

// FUNCTION: GAMEMD 0x00683000
const WCHAR *Stream_Name_From_ID(int id)
{
	static struct pidsiStruct {
		int ID;
		WCHAR const *Name;
	} _ids[] = {
		{PIDSI_SCEN_DESCRIP,	L"Scenario Description"},
		{PIDSI_PLAYER_HOUSE,	L"Player House"},
		{PIDSI_G_VERSION,	L"Version"},
		{PIDSI_INTERNAL_VER,	L"Internal Version"},
		{PIDSI_G_START_TIME,	L"Start Time"},
		{PIDSI_LAST_SAVE_TIME,	L"Last Save Time"},
		{PIDSI_G_PLAY_TIME,		L"Play Time"},
		{PIDSI_EXEC_NAME,		L"Executable Name"},
		{PIDSI_PLAYER_NAME1,	L"Player Name"},
		{PIDSI_PLAYER_NAME2,	L"Player Name2"},
		{PIDSI_SCENARIO_NUM,	L"Scenario Number"},
		{PIDSI_CAMPAIGN_NUM,	L"Campaign"},
		{PIDSI_GAME_TYPE,		L"GameType"},
	};

	for (int i = 0; i < ARRAY_SIZE(_ids); i++) {
		if (_ids[i].ID == id) {
			return(_ids[i].Name);
		}
	}

	return(NULL);
}

// TEMPLATE: GAMEMD 0x00683030
// ??C?$_com_ptr_t@V?$_com_IIID@UIStream@@$1?_GUID_0000000c_0000_0000_c000_000000000046@@3U__s_GUID@@A@@@@QBEPAUIStream@@XZ
// TEMPLATE: GAMEMD 0x00683050
// ?_Release@?$_com_ptr_t@V?$_com_IIID@UIStream@@$1?_GUID_0000000c_0000_0000_c000_000000000046@@3U__s_GUID@@A@@@@AAEXXZ
template class _com_ptr_t<_com_IIID<IStream, &__uuidof(IStream)> >;

// TEMPLATE: GAMEMD 0x00683060
// ?_Release@?$_com_ptr_t@V?$_com_IIID@UIPropertyStorage@@$1?_GUID_00000138_0000_0000_c000_000000000046@@3U__s_GUID@@A@@@@AAEXXZ
template class _com_ptr_t<_com_IIID<IPropertyStorage, &__uuidof(IPropertyStorage)> >;
