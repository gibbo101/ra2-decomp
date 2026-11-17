// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"
#include <math.h>
#include "cdfile.h"
#include "ini.h"
#include "geometry.h"
#include "rect.h"
#include "playerprofile.h"
#include "wolapi/wolapi.h"
#include "rcptr.h"
#include "wolopt.h"
#undef LoadString
#include "stringtable.h"
#include <time.h>
#include <stdlib.h>

// SYNTHETIC: GAMEMD 0x007784D0 SYMBOL
// _$E46

namespace fastmath { double __cdecl sqrt(double x); double __cdecl tan(double x); double __cdecl atan(double x); }
wchar_t * __fastcall FUN_00735120(char const * text);
char * __fastcall FUN_007350C0(wchar_t const * text);
extern "C" wchar_t * __cdecl wcsstr(wchar_t const *, wchar_t const *);
void __fastcall FUN_00778F30(Wstring * from, WideWstring * to);

static inline int WOL_Int(INIClass & ini, Wstring const & section, char const * key, int def)
{
	return ini.Get_Int(section, key, def);
}
// SYNTHETIC: GAMEMD 0x007784F0 SYMBOL
// _$E7
// SYNTHETIC: GAMEMD 0x00778520 SYMBOL
// _$E10
// SYNTHETIC: GAMEMD 0x00778540 SYMBOL
// _$E13
// SYNTHETIC: GAMEMD 0x00778560 SYMBOL
// _$E16
// SYNTHETIC: GAMEMD 0x00778580 SYMBOL
// _$E19
// SYNTHETIC: GAMEMD 0x007785A0 SYMBOL
// _$E22
// SYNTHETIC: GAMEMD 0x007785C0 SYMBOL
// _$E25
// SYNTHETIC: GAMEMD 0x00778600 SYMBOL
// _$E28
// SYNTHETIC: GAMEMD 0x00778630 SYMBOL
// _$E31
// SYNTHETIC: GAMEMD 0x00778660 SYMBOL
// _$E34
// SYNTHETIC: GAMEMD 0x00778690 SYMBOL
// _$E37
// SYNTHETIC: GAMEMD 0x007786A0 SYMBOL
// _$E40
// SYNTHETIC: GAMEMD 0x007786C0 SYMBOL
// _$E43

// SYNTHETIC: GAMEMD 0x007786E0 SYMBOL
// _$E51
// SYNTHETIC: GAMEMD 0x00778700 SYMBOL
// _$E49
Wstring DAT_00B73590("_Default_");

// FUNCTION: GAMEMD 0x00778710
rc_ptr<PlayerProfile> __fastcall FUN_00778710(void)
{
	Wstring server;
	int locale = 0;
	int country = -2;
	int color = -2;
	Wstring text;
	WideWstring timeText;
	WideWstring dateText;
	CDFileClass file("wolinfoMD.ini");
	if (!file.Is_Available()) {
		file.Create();
		file.Close();
		file.Open("wolinfoMD.ini", 1);
	}
	if (file.Is_Available()) {
		INIClass ini;
		ini.Load(file, false);
		file.Close();
		if (ini.GetSection(DAT_00B73590.text())) {
			ini.Get_String(DAT_00B73590, "Server", server, server);
			locale = WOL_Int(ini, DAT_00B73590, "Locale", 0);
			country = ini.Get_Int(DAT_00B73590, "Country", -2);
			color = ini.Get_Int(DAT_00B73590, "Color", -2);
			ini.Get_String(DAT_00B73590, "UpdateTime", text, text);
			FUN_00778F30(&text, &timeText);
			text.clear();
			ini.Get_String(DAT_00B73590, "UpdateDate", text, text);
			dateText.set(FUN_00735120(text.text()));
		}
	}
	PlayerProfile * profile = new PlayerProfile(server, locale, 0, 0, country, color, -1, -1, -1, -1, -1, 1, dateText, timeText);
	return rc_ptr<PlayerProfile>(profile);
}

// FUNCTION: GAMEMD 0x00778A20
rc_ptr<PlayerProfile> __fastcall FUN_00778A20(Wstring const * name)
{
	CDFileClass file("wolinfoMD.ini");
	if (!file.Is_Available()) {
		file.Create();
		file.Close();
		file.Open("wolinfoMD.ini", 1);
	}
	if (!file.Is_Available()) {
		return FUN_00778710();
	}
	INIClass ini;
	ini.Load(file, false);
	file.Close();
	Wstring section(*name);
	section.toLower();
	if (!ini.GetSection(section.text())) {
		return FUN_00778710();
	}
	Wstring server;
	ini.Get_String(section, "Server", server, server);
	int locale = WOL_Int(ini, section, "Locale", 0);
	bool localeVerified = ini.Get_Bool(section.get(), "LocaleVerified", false);
	int country = ini.Get_Int(section, "Country", -2);
	int color = ini.Get_Int(section, "Color", -2);
	int rank = ini.Get_Int(section, "Rank", -1);
	int points = ini.Get_Int(section, "Points", -1);
	int wins = ini.Get_Int(section, "Wins", -1);
	int losses = ini.Get_Int(section, "Losses", -1);
	int disconnects = ini.Get_Int(section, "Disconnects", -1);
	bool matchByRes = ini.Get_Bool(section.get(), "MatchByRes", true);
	Wstring text;
	WideWstring timeText;
	ini.Get_String(section, "UpdateTime", text, text);
	FUN_00778F30(&text, &timeText);
	text.clear();
	WideWstring dateText;
	ini.Get_String(section, "UpdateDate", text, text);
	dateText.set(FUN_00735120(text.text()));
	PlayerProfile * profile = new PlayerProfile(server, locale, localeVerified, localeVerified ? 1 : 3, country, color, rank, points, wins, losses, disconnects, matchByRes, dateText, timeText);
	return rc_ptr<PlayerProfile>(profile);
}

// FUNCTION: GAMEMD 0x00778F30
void __fastcall FUN_00778F30(Wstring * from, WideWstring * to)
{
	to->set(FUN_00735120(from->text()));
	if (wcsstr(to->get(), L"Jan")) {
		to->replace(L"Jan", StringTable::LoadString("TXT_JANUARY", 0, "D:\\ra2mdpost\\WOLPersonaInformation.cpp", 127));
		return;
	}
	if (wcsstr(to->get(), L"Feb")) {
		to->replace(L"Feb", StringTable::LoadString("TXT_FEBRUARY", 0, "D:\\ra2mdpost\\WOLPersonaInformation.cpp", 132));
		return;
	}
	if (wcsstr(to->get(), L"Mar")) {
		to->replace(L"Mar", StringTable::LoadString("TXT_MARCH", 0, "D:\\ra2mdpost\\WOLPersonaInformation.cpp", 137));
		return;
	}
	if (wcsstr(to->get(), L"Apr")) {
		to->replace(L"Apr", StringTable::LoadString("TXT_APRIL", 0, "D:\\ra2mdpost\\WOLPersonaInformation.cpp", 142));
		return;
	}
	if (wcsstr(to->get(), L"May")) {
		to->replace(L"May", StringTable::LoadString("TXT_MAY", 0, "D:\\ra2mdpost\\WOLPersonaInformation.cpp", 147));
		return;
	}
	if (wcsstr(to->get(), L"Jun")) {
		to->replace(L"Jun", StringTable::LoadString("TXT_JUNE", 0, "D:\\ra2mdpost\\WOLPersonaInformation.cpp", 152));
		return;
	}
	if (wcsstr(to->get(), L"Jul")) {
		to->replace(L"Jul", StringTable::LoadString("TXT_JULY", 0, "D:\\ra2mdpost\\WOLPersonaInformation.cpp", 157));
		return;
	}
	if (wcsstr(to->get(), L"Aug")) {
		to->replace(L"Aug", StringTable::LoadString("TXT_AUGUST", 0, "D:\\ra2mdpost\\WOLPersonaInformation.cpp", 162));
		return;
	}
	if (wcsstr(to->get(), L"Sep")) {
		to->replace(L"Sep", StringTable::LoadString("TXT_SEPTEMBER", 0, "D:\\ra2mdpost\\WOLPersonaInformation.cpp", 167));
		return;
	}
	if (wcsstr(to->get(), L"Oct")) {
		to->replace(L"Oct", StringTable::LoadString("TXT_OCTOBER", 0, "D:\\ra2mdpost\\WOLPersonaInformation.cpp", 172));
		return;
	}
	if (wcsstr(to->get(), L"Nov")) {
		to->replace(L"Nov", StringTable::LoadString("TXT_NOVEMBER", 0, "D:\\ra2mdpost\\WOLPersonaInformation.cpp", 177));
		return;
	}
	if (wcsstr(to->get(), L"Dec")) {
		to->replace(L"Dec", StringTable::LoadString("TXT_DECEMBER", 0, "D:\\ra2mdpost\\WOLPersonaInformation.cpp", 182));
		return;
	}
}

// FUNCTION: GAMEMD 0x00779230
void __fastcall FUN_00779230(WideWstring const * from, Wstring * to)
{
	WideWstring text;
	text.set(from->text());
	if (wcsstr(text.get(), StringTable::LoadString("TXT_JANUARY", 0, "D:\\ra2mdpost\\WOLPersonaInformation.cpp", 191))) {
		text.replace(StringTable::LoadString("TXT_JANUARY", 0, "D:\\ra2mdpost\\WOLPersonaInformation.cpp", 192), L"Jan");
	}
	if (wcsstr(text.get(), StringTable::LoadString("TXT_FEBRUARY", 0, "D:\\ra2mdpost\\WOLPersonaInformation.cpp", 194))) {
		text.replace(StringTable::LoadString("TXT_FEBRUARY", 0, "D:\\ra2mdpost\\WOLPersonaInformation.cpp", 195), L"Feb");
	}
	if (wcsstr(text.get(), StringTable::LoadString("TXT_MARCH", 0, "D:\\ra2mdpost\\WOLPersonaInformation.cpp", 197))) {
		text.replace(StringTable::LoadString("TXT_MARCH", 0, "D:\\ra2mdpost\\WOLPersonaInformation.cpp", 198), L"Mar");
	}
	if (wcsstr(text.get(), StringTable::LoadString("TXT_APRIL", 0, "D:\\ra2mdpost\\WOLPersonaInformation.cpp", 200))) {
		text.replace(StringTable::LoadString("TXT_APRIL", 0, "D:\\ra2mdpost\\WOLPersonaInformation.cpp", 201), L"Apr");
	}
	if (wcsstr(text.get(), StringTable::LoadString("TXT_MAY", 0, "D:\\ra2mdpost\\WOLPersonaInformation.cpp", 203))) {
		text.replace(StringTable::LoadString("TXT_MAY", 0, "D:\\ra2mdpost\\WOLPersonaInformation.cpp", 204), L"May");
	}
	if (wcsstr(text.get(), StringTable::LoadString("TXT_JUNE", 0, "D:\\ra2mdpost\\WOLPersonaInformation.cpp", 206))) {
		text.replace(StringTable::LoadString("TXT_JUNE", 0, "D:\\ra2mdpost\\WOLPersonaInformation.cpp", 207), L"Jun");
	}
	if (wcsstr(text.get(), StringTable::LoadString("TXT_JULY", 0, "D:\\ra2mdpost\\WOLPersonaInformation.cpp", 209))) {
		text.replace(StringTable::LoadString("TXT_JULY", 0, "D:\\ra2mdpost\\WOLPersonaInformation.cpp", 210), L"Jul");
	}
	if (wcsstr(text.get(), StringTable::LoadString("TXT_AUGUST", 0, "D:\\ra2mdpost\\WOLPersonaInformation.cpp", 212))) {
		text.replace(StringTable::LoadString("TXT_AUGUST", 0, "D:\\ra2mdpost\\WOLPersonaInformation.cpp", 213), L"Aug");
	}
	if (wcsstr(text.get(), StringTable::LoadString("TXT_SEPTEMBER", 0, "D:\\ra2mdpost\\WOLPersonaInformation.cpp", 215))) {
		text.replace(StringTable::LoadString("TXT_SEPTEMBER", 0, "D:\\ra2mdpost\\WOLPersonaInformation.cpp", 216), L"Sep");
	}
	if (wcsstr(text.get(), StringTable::LoadString("TXT_OCTOBER", 0, "D:\\ra2mdpost\\WOLPersonaInformation.cpp", 218))) {
		text.replace(StringTable::LoadString("TXT_OCTOBER", 0, "D:\\ra2mdpost\\WOLPersonaInformation.cpp", 219), L"Oct");
	}
	if (wcsstr(text.get(), StringTable::LoadString("TXT_NOVEMBER", 0, "D:\\ra2mdpost\\WOLPersonaInformation.cpp", 221))) {
		text.replace(StringTable::LoadString("TXT_NOVEMBER", 0, "D:\\ra2mdpost\\WOLPersonaInformation.cpp", 222), L"Nov");
	}
	if (wcsstr(text.get(), StringTable::LoadString("TXT_DECEMBER", 0, "D:\\ra2mdpost\\WOLPersonaInformation.cpp", 224))) {
		text.replace(StringTable::LoadString("TXT_DECEMBER", 0, "D:\\ra2mdpost\\WOLPersonaInformation.cpp", 225), L"Dec");
	}
	to->set(FUN_007350C0(text.get()));
}

// FUNCTION: GAMEMD 0x00779650
char __fastcall FUN_00779650(Wstring const * from, WideWstring * to)
{
	return to->set(FUN_00735120(from->text()));
}

// FUNCTION: GAMEMD 0x00779670
char __fastcall FUN_00779670(WideWstring const * from, Wstring * to)
{
	return to->set(FUN_007350C0(from->text()));
}

// FUNCTION: GAMEMD 0x00779690
PlayerProfile::PlayerProfile(Wstring const & name, int a, bool b, int c, int d, int e, int f, int g, int h, int i, int j, bool k, WideWstring const & l, WideWstring const & m) :
	IsChanged(0),
	Name(name),
	Locale(a),
	Field14(b),
	Field18(c),
	Country(d),
	Field20(e),
	Rung(f),
	Points(g),
	Wins(h),
	Losses(i),
	Disconnects(j),
	MatchByRes(k),
	UpdateTime(m),
	UpdateDate(l)
{
}

// FUNCTION: GAMEMD 0x00779730
bool FUN_00779730(void)
{
	CDFileClass file("wolinfoMD.ini");
	if (!file.Is_Available()) {
		return false;
	}
	INIClass ini;
	ini.Load(file, false);
	file.Close();
	return ini.Get_Bool("Ask", "AutoLogin", true);
}

// FUNCTION: GAMEMD 0x00779830
void __fastcall FUN_00779830(bool value)
{
	CDFileClass file("wolinfoMD.ini");
	if (!file.Is_Available()) {
		file.Create();
		file.Close();
		file.Open("wolinfoMD.ini", 1);
	}
	INIClass ini;
	if (file.Is_Available()) {
		ini.Load(file, false);
		ini.Put_Bool("Ask", "AutoLogin", value);
		ini.Save(file);
	}
}

// FUNCTION: GAMEMD 0x00779940
bool FUN_00779940(void)
{
	CDFileClass file("wolinfoMD.ini");
	if (!file.Is_Available()) {
		return false;
	}
	INIClass ini;
	ini.Load(file, false);
	file.Close();
	return ini.Get_Bool("Ask", "AddMatchBuddies", true);
}

// FUNCTION: GAMEMD 0x00779A40
void __fastcall FUN_00779A40(bool value)
{
	CDFileClass file("wolinfoMD.ini");
	if (!file.Is_Available()) {
		file.Create();
		file.Close();
		file.Open("wolinfoMD.ini", 1);
	}
	INIClass ini;
	if (file.Is_Available()) {
		ini.Load(file, false);
		ini.Put_Bool("Ask", "AddMatchBuddies", value);
		ini.Save(file);
	}
}

// FUNCTION: GAMEMD 0x00779B50
Wstring __fastcall FUN_00779B50(void)
{
	CDFileClass file("wolinfoMD.ini");
	if (!file.Is_Available()) {
		return Wstring((char const *)0);
	}
	INIClass ini;
	ini.Load(file, false);
	file.Close();
	Wstring result;
	{
		Wstring key("AutoLogin");
		Wstring section("DefaultPersona");
		ini.Get_String(section, key, result, result);
	}
	return result;
}

// FUNCTION: GAMEMD 0x00779C90
void __fastcall FUN_00779C90(Wstring const & value)
{
	CDFileClass file("wolinfoMD.ini");
	if (!file.Is_Available()) {
		file.Create();
		file.Close();
		file.Open("wolinfoMD.ini", 1);
	}
	INIClass ini;
	if (file.Is_Available()) {
		ini.Load(file, false);
		{
			Wstring key("AutoLogin");
			Wstring section("DefaultPersona");
			ini.Put_String(section, key, value);
		}
		ini.Save(file);
	}
}

// FUNCTION: GAMEMD 0x00779DD0
Wstring __fastcall FUN_00779DD0(void)
{
	CDFileClass file("wolinfoMD.ini");
	if (!file.Is_Available()) {
		return Wstring((char const *)0);
	}
	INIClass ini;
	ini.Load(file, false);
	file.Close();
	Wstring result;
	{
		Wstring key("LastValid");
		Wstring section("DefaultPersona");
		ini.Get_String(section, key, result, result);
	}
	return result;
}

// FUNCTION: GAMEMD 0x00779F10
void __fastcall FUN_00779F10(Wstring const & value)
{
	CDFileClass file("wolinfoMD.ini");
	if (!file.Is_Available()) {
		file.Create();
		file.Close();
		file.Open("wolinfoMD.ini", 1);
	}
	INIClass ini;
	if (file.Is_Available()) {
		ini.Load(file, false);
		{
			Wstring key("LastValid");
			Wstring section("DefaultPersona");
			ini.Put_String(section, key, value);
		}
		ini.Save(file);
	}
}

// FUNCTION: GAMEMD 0x0077A050
PlayerProfile::~PlayerProfile(void)
{
}

static inline void WOL_PutInt(INIClass & ini, Wstring const & section, char const * key, int value)
{
	Wstring k(key);
	ini.Put_Int(section, k, value, 0);
}

// FUNCTION: GAMEMD 0x0077A080
void PlayerProfile::Save(Wstring const * name)
{
	WOLOptionsClass options;
	if (IsChanged) {
		Wstring section(*name);
		if (!name->length()) {
			section = DAT_00B73590;
		} else {
			section.toLower();
			if (!options.Is_Bit1_Clear()) {
				return;
			}
		}
		CDFileClass file("wolinfoMD.ini");
		if (!file.Is_Available()) {
			file.Create();
			file.Close();
			file.Open("wolinfoMD.ini", 1);
		}
		if (file.Is_Available()) {
			INIClass ini;
			ini.Load(file, false);
			{ Wstring k("Server"); ini.Put_String(section, k, Name); }
			{ Wstring k("Locale"); ini.Put_Int(section, k, Locale, 0); }
			{ Wstring k("LocaleVerified"); ini.Put_Bool(section, k, Field14); }
			{ Wstring k("Country"); ini.Put_Int(section, k, Country, 0); }
			{ Wstring k("Color"); ini.Put_Int(section, k, Field20, 0); }
			{ Wstring k("Rank"); ini.Put_Int(section, k, Rung, 0); }
			{ Wstring k("Points"); ini.Put_Int(section, k, Points, 0); }
			{ Wstring k("Wins"); ini.Put_Int(section, k, Wins, 0); }
			{ Wstring k("Losses"); ini.Put_Int(section, k, Losses, 0); }
			{ Wstring k("Disconnects"); ini.Put_Int(section, k, Disconnects, 0); }
			{ Wstring k("MatchByRes"); ini.Put_Bool(section, k, MatchByRes); }
			Wstring text;
			FUN_00779230(&UpdateTime, &text);
			{ Wstring k("UpdateTime"); ini.Put_String(section, k, text); }
			text.clear();
			text.set(FUN_007350C0(UpdateDate.text()));
			{ Wstring k("UpdateDate"); ini.Put_String(section, k, text); }
			ini.Save(file);
			file.Close();
			IsChanged = 0;
			char const * unused0 = Name.c_str();
			char const * unused1 = Name.c_str();
			char const * unused2 = Name.c_str();
		}
	}
}

// FUNCTION: GAMEMD 0x0077A4A0
void PlayerProfile::SetName(Wstring const * name)
{
	if (!(*name == Name)) {
		Name = *name;
		IsChanged = 1;
	}
}

// FUNCTION: GAMEMD 0x0077A4D0
void PlayerProfile::SetUpdateStamp(void)
{
	time_t now = time(0);
	struct tm * local = localtime(&now);
	UpdateDate.clear();
	char buf[16];
	strftime(buf, 16, "%m", local);
	int month = atoi(buf);
	strftime(buf, 16, "%d", local);
	if (month == 1) {
		UpdateDate.set(StringTable::LoadString("TXT_JANUARY", 0, "D:\\ra2mdpost\\WOLPersonaInformation.cpp", 0x261));
	} else if (month == 2) {
		UpdateDate.set(StringTable::LoadString("TXT_FEBRUARY", 0, "D:\\ra2mdpost\\WOLPersonaInformation.cpp", 0x263));
	} else if (month == 3) {
		UpdateDate.set(StringTable::LoadString("TXT_MARCH", 0, "D:\\ra2mdpost\\WOLPersonaInformation.cpp", 0x265));
	} else if (month == 4) {
		UpdateDate.set(StringTable::LoadString("TXT_APRIL", 0, "D:\\ra2mdpost\\WOLPersonaInformation.cpp", 0x267));
	} else if (month == 5) {
		UpdateDate.set(StringTable::LoadString("TXT_MAY", 0, "D:\\ra2mdpost\\WOLPersonaInformation.cpp", 0x269));
	} else if (month == 6) {
		UpdateDate.set(StringTable::LoadString("TXT_JUNE", 0, "D:\\ra2mdpost\\WOLPersonaInformation.cpp", 0x26B));
	} else if (month == 7) {
		UpdateDate.set(StringTable::LoadString("TXT_JULY", 0, "D:\\ra2mdpost\\WOLPersonaInformation.cpp", 0x26D));
	} else if (month == 8) {
		UpdateDate.set(StringTable::LoadString("TXT_AUGUST", 0, "D:\\ra2mdpost\\WOLPersonaInformation.cpp", 0x26F));
	} else if (month == 9) {
		UpdateDate.set(StringTable::LoadString("TXT_SEPTEMBER", 0, "D:\\ra2mdpost\\WOLPersonaInformation.cpp", 0x271));
	} else if (month == 10) {
		UpdateDate.set(StringTable::LoadString("TXT_OCTOBER", 0, "D:\\ra2mdpost\\WOLPersonaInformation.cpp", 0x273));
	} else if (month == 11) {
		UpdateDate.set(StringTable::LoadString("TXT_NOVEMBER", 0, "D:\\ra2mdpost\\WOLPersonaInformation.cpp", 0x275));
	} else if (month == 12) {
		UpdateDate.set(StringTable::LoadString("TXT_DECEMBER", 0, "D:\\ra2mdpost\\WOLPersonaInformation.cpp", 0x277));
	} else {
		UpdateDate.set(StringTable::LoadString("TXT_JANUARY", 0, "D:\\ra2mdpost\\WOLPersonaInformation.cpp", 0x279));
	}
	UpdateDate.cat(L" ");
	UpdateDate.cat(FUN_00735120(buf));
	UpdateDate.cat(L" ");
	strftime(buf, 16, "%Y", local);
	UpdateDate.cat(FUN_00735120(buf));
	char timeBuf[64];
	timeBuf[0] = 0;
	strftime(timeBuf, 64, "%H:%M:%S", local);
	UpdateTime.set(FUN_00735120(timeBuf));
	IsChanged = 1;
}

// FUNCTION: GAMEMD 0x0077A700
void PlayerProfile::SetLocale(int locale, int state)
{
	if (locale == Locale) {
		if (state == 2 && !Field14) {
			Field14 = 1;
			IsChanged = 1;
		}
		return;
	}
	bool newer = state >= Field18;
	if (Locale == 0) {
		newer = true;
	} else if (!newer) {
		return;
	}
	if (locale == 0 || !newer) {
		return;
	}
	if (state == 3) {
		Field14 = 0;
	}
	Locale = locale;
	Field18 = state;
	IsChanged = 1;
}

// FUNCTION: GAMEMD 0x0077A760
void PlayerProfile::SetMatchByRes(bool value)
{
	if (value != MatchByRes) {
		MatchByRes = value;
		IsChanged = 1;
	}
}

// FUNCTION: GAMEMD 0x0077A780
void PlayerProfile::SetCountry(int country)
{
	if (country != Country) {
		Country = country;
		IsChanged = 1;
	}
}

// FUNCTION: GAMEMD 0x0077A7A0
void PlayerProfile::SetField20(int value)
{
	if (value != Field20) {
		Field20 = value;
		IsChanged = 1;
	}
}

// FUNCTION: GAMEMD 0x0077A7C0
void PlayerProfile::UpdateStats(Ladder const * record)
{
	char changed = 0;
	if (record->rung != Rung) {
		Rung = record->rung;
		IsChanged = 1;
		changed = 1;
	}
	if (record->points != Points) {
		Points = record->points;
		IsChanged = 1;
		changed = 1;
	}
	if (record->wins != Wins) {
		Wins = record->wins;
		IsChanged = 1;
		changed = 1;
	}
	if (record->losses != Losses) {
		Losses = record->losses;
		IsChanged = 1;
		changed = 1;
	}
	if (record->disconnects != Disconnects) {
		Disconnects = record->disconnects;
		IsChanged = 1;
		SetUpdateStamp();
		return;
	}
	if (changed) {
		SetUpdateStamp();
	}
}

// FUNCTION: GAMEMD 0x0077A840
bool PlayerProfile::StatsDiffer(Ladder const * record) const
{
	if (record->rung != Rung) {
		return true;
	} else {
		if (record->points != Points) {
			return true;
		} else {
			return false;
		}
	}
}

// FUNCTION: GAMEMD 0x0077A870
void PlayerProfile::AddWins(int delta)
{
	if (Wins != -1) {
		IsChanged = 1;
		Wins += delta;
	}
}

// FUNCTION: GAMEMD 0x0077A890
void PlayerProfile::AddLosses(int delta)
{
	if (Losses != -1) {
		IsChanged = 1;
		Losses += delta;
	}
}

// FUNCTION: GAMEMD 0x0077A8B0
void PlayerProfile::AddDisconnects(int delta)
{
	if (Disconnects != -1) {
		IsChanged = 1;
		Disconnects += delta;
	}
}

extern char DAT_00889F64[];
extern wchar_t DAT_00887734[];

// FUNCTION: GAMEMD 0x0077A8D0
void __fastcall FUN_0077A8D0(HWND dialog, bool clearBadge)
{
	HWND item;
	if (clearBadge) {
		HWND badge = GetDlgItem(dialog, 0x628);
		SendMessageA(badge, 0x4b4, 0, (LPARAM)DAT_00889F64);
		InvalidateRect(badge, NULL, 0);
	}
	item = GetDlgItem(dialog, 0x6ef);
	SendMessageA(item, 0x4b2, 0, (LPARAM)DAT_00887734);
	WideWstring unknown(StringTable::LoadString("GUI:UnknownStats", NULL, "D:\\ra2mdpost\\WOLPersonaInformation.cpp", 0x37c));
	item = GetDlgItem(dialog, 0x6f0);
	SendMessageA(item, 0x4b2, 0, (LPARAM)unknown.text());
	item = GetDlgItem(dialog, 0x6f1);
	SendMessageA(item, 0x4b2, 0, (LPARAM)unknown.text());
	item = GetDlgItem(dialog, 0x701);
	SendMessageA(item, 0x4b2, 0, (LPARAM)unknown.text());
	item = GetDlgItem(dialog, 0x6f4);
	SendMessageA(item, 0x4b2, 0, (LPARAM)unknown.text());
	item = GetDlgItem(dialog, 0x6f5);
	SendMessageA(item, 0x4b2, 0, (LPARAM)unknown.text());
}

WideWstring __fastcall FUN_007B7ED0(Wstring const & text);
WideWstring __fastcall FUN_007B7DD0(int value);

static inline void Profile_Text(HWND dialog, int id, WideWstring const & text)
{
	HWND item = GetDlgItem(dialog, id);
	SendMessageA(item, 0x4B2, 0, (LPARAM)text.text());
}

static inline void Profile_Text(HWND dialog, int id, wchar_t const * text)
{
	HWND item = GetDlgItem(dialog, id);
	SendMessageA(item, 0x4B2, 0, (LPARAM)text);
}
// FUNCTION: GAMEMD 0x0077A9F0
void PlayerProfile::FUN_0077A9F0(HWND dialog, Wstring const * name, bool update)
{
	if (update) {
		HWND item = GetDlgItem(dialog, 0x628);
		bool wide = SendMessageA(item, 0x4CE, 0, 0) != 0;
		if (name->length() != 0 && !wide) {
			int selected = SendMessageA(item, 0x4B9, -1, (LPARAM)name->text());
			SendMessageA(item, 0x14E, selected, 0);
			if (selected == -1) {
				SendMessageA(item, 0x4B4, 0, (LPARAM)name->text());
			}
		} else {
			WideWstring nickname = FUN_007B7ED0(*name);
			int selected = -1;
			if (name->length() > 0) {
				selected = SendMessageA(item, 0x4BF, -1, (LPARAM)nickname.text());
			}
			if (selected == -1) {
				nickname = StringTable::LoadString(
					"GUI:Default", NULL,
					"D:\\ra2mdpost\\WOLPersonaInformation.cpp", 0x399);
				selected = SendMessageA(item, 0x4BF, -1, (LPARAM)nickname.text());
			}
			SendMessageA(item, 0x14E, selected, 0);
			if (selected == -1) {
				SendMessageA(item, 0x4B2, 0, (LPARAM)nickname.text());
			}
		}
	}

	HWND item = GetDlgItem(dialog, 0x629);
	int selected = SendMessageA(item, 0x4B9, -1, (LPARAM)Name.text());
	SendMessageA(item, 0x14E, selected, 0);

	item = GetDlgItem(dialog, 0x62A);
	if (item) {
		int count = SendMessageA(item, 0x146, 0, 0);
		for (int i = 0; i < count; i++) {
			LRESULT data = SendMessageA(item, 0x150, i, 0);
			if (data == Locale) {
				SendMessageA(item, 0x14E, i, 0);
				break;
			}
		}
	}

	item = GetDlgItem(dialog, 0x6A1);
	if (item) {
		int count = SendMessageA(item, 0x146, 0, 0);
		for (int i = 0; i < count; i++) {
			LRESULT data = SendMessageA(item, 0x150, i, 0);
			if (data == Country) {
				SendMessageA(item, 0x14E, i, 0);
				break;
			}
		}
	}

	item = GetDlgItem(dialog, 0x6A2);
	if (item) {
		int count = SendMessageA(item, 0x146, 0, 0);
		for (int i = 0; i < count; i++) {
			LRESULT data = SendMessageA(item, 0x150, i, 0);
			if (data == Field20) {
				SendMessageA(item, 0x14E, i, 0);
				break;
			}
		}
	}

	item = GetDlgItem(dialog, 0x773);
	if (item) {
		SendMessageA(item, 0xF1, MatchByRes, 0);
	}

	WideWstring unknown(StringTable::LoadString(
		"GUI:UnknownStats", NULL,
		"D:\\ra2mdpost\\WOLPersonaInformation.cpp", 0x3DB));

	Profile_Text(dialog, 0x6F4,
		Rung == -1 ? unknown : FUN_007B7DD0(Rung));
	Profile_Text(dialog, 0x6F5,
		Points == -1 ? unknown : FUN_007B7DD0(Points));
	Profile_Text(dialog, 0x6F0,
		Wins == -1 ? unknown : FUN_007B7DD0(Wins));
	Profile_Text(dialog, 0x6F1,
		Losses == -1 ? unknown : FUN_007B7DD0(Losses));
	Profile_Text(dialog, 0x701,
		Disconnects == -1 ? unknown : FUN_007B7DD0(Disconnects));

	if (name->length() > 0) {
		WideWstring label(StringTable::LoadString(
			"GUI:UserProfileLabel", NULL,
			"D:\\ra2mdpost\\WOLPersonaInformation.cpp", 0x3E3));
		wchar_t * title = new wchar_t[label.length() + name->length() + 1];
		swprintf(title, label.get(), name->text());
		Profile_Text(dialog, 0x6EF, title);
		delete [] title;
	} else {
		Profile_Text(dialog, 0x6EF, DAT_00887734);
	}

	wchar_t * text = new wchar_t[
		UpdateDate.length() + UpdateTime.length() +
		wcslen(StringTable::LoadString(
			"GUI:WelcomeUpdate", NULL,
			"D:\\ra2mdpost\\WOLPersonaInformation.cpp", 0x3ED))];

	if (text && UpdateDate.length() > 0 && UpdateTime.length() > 0) {
		swprintf(text, StringTable::LoadString(
			"GUI:WelcomeUpdate", NULL,
			"D:\\ra2mdpost\\WOLPersonaInformation.cpp", 0x3EF),
			UpdateDate.str, UpdateTime.str);
		item = GetDlgItem(dialog, 0x797);
		SendMessageA(item, 0x4B2, 0, (LPARAM)text);
		goto update_done;
	}

	{
		wchar_t const * blank = StringTable::LoadString(
			"GUI:Blank", NULL,
			"D:\\ra2mdpost\\WOLPersonaInformation.cpp", 0x3F2);
		item = GetDlgItem(dialog, 0x797);
		SendMessageA(item, 0x4B2, 0, (LPARAM)blank);
	}

update_done:
	if (text) {
		delete [] text;
	}
}

// SYNTHETIC: GAMEMD 0x0077B030
// ??_GPlayerProfile@@UAEPAXI@Z


