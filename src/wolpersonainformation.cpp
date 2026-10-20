// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"
#include <math.h>
#include "cdfile.h"
#include "ini.h"
#include "playerprofile.h"
#include "rcptr.h"
#undef LoadString
#include "stringtable.h"
#include <time.h>
#include <stdlib.h>

namespace fastmath { double __cdecl sqrt(double x); double __cdecl tan(double x); double __cdecl atan(double x); }
extern const double DAT_007E1708;
extern const double DAT_007E1710;
extern const double DAT_007E1718;
extern const double DAT_007E1720;
extern const double DAT_007E1728;
extern const double DAT_007E1730;
extern const double DAT_007E1738;
extern const double DAT_007E1740;
extern const double DAT_007F74E0;
extern const double DAT_007F74E8;
wchar_t * __fastcall FUN_00735120(char const * text);
char * __fastcall FUN_007350C0(wchar_t const * text);
extern "C" wchar_t * __cdecl wcsstr(wchar_t const *, wchar_t const *);
void __fastcall FUN_00778F30(Wstring * from, WideWstring * to);

static inline int WOL_Int(INIClass & ini, Wstring const & section, char const * key, int def)
{
	return ini.Get_Int(section, key, def);
}


// FUNCTION: 0x007784F0 $E7
double DAT_00B73598 = fastmath::sqrt(pow(DAT_007E1710, DAT_007E1708) * 2);

// FUNCTION: 0x00778520 $E10
double DAT_00B735E8 = DAT_00B73598 / DAT_007F74E0;

// FUNCTION: 0x00778540 $E13
double DAT_00B735D0 = DAT_007E1718 / DAT_00B735E8;

// FUNCTION: 0x00778560 $E16
double DAT_00B735C8 = DAT_007E1720 * DAT_007F74E8;

// FUNCTION: 0x00778580 $E19
double DAT_00B735A0 = DAT_007E1728 * DAT_007F74E8;

// FUNCTION: 0x007785A0 $E22
double DAT_00B73578 = DAT_007E1730 * DAT_007F74E8;

// FUNCTION: 0x007785C0 $E25
int DAT_00B735F0 = (int)(fastmath::tan(DAT_00B73578 - DAT_00B735A0) * DAT_007E1738 * DAT_00B73598);

// FUNCTION: 0x00778600 $E28
double DAT_00B735A8 = fastmath::atan(DAT_00B735F0 * DAT_007E1740);

// FUNCTION: 0x00778630 $E31
double DAT_00B735C0 = fastmath::atan((DAT_00B735F0 * 2) / DAT_00B73598);

// FUNCTION: 0x00778660 $E34
int DAT_00B735E4 = (int)(DAT_00B735F0 * 4 + DAT_007E1738);

struct WOLShortPair { short a; short b; WOLShortPair(void) { a = 0; b = 0; } };
struct WOLIntTriple { int a; int b; int c; WOLIntTriple(void) { a = 0; b = 0; c = 0; } };

// FUNCTION: 0x00778690 $E37
WOLShortPair DAT_00B73594;

// FUNCTION: 0x007786A0 $E40
WOLIntTriple DAT_00B735B0;

struct WOLWindowSize { int W; int H; int Z; WOLWindowSize(void) { Z = 0; W = 0x80; H = 0x80; } };

// FUNCTION: 0x007786C0 $E43
WOLWindowSize DAT_00B735D8;

// FUNCTION: 0x007786E0 $E48
// FUNCTION: 0x00778700 $E46
Wstring DAT_00B73590("_Default_");

// FUNCTION: 0x00778710
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

// FUNCTION: 0x00778A20
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

// FUNCTION: 0x00778F30
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

// FUNCTION: 0x00779230
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

// FUNCTION: 0x00779650
char __fastcall FUN_00779650(Wstring const * from, WideWstring * to)
{
	return to->set(FUN_00735120(from->text()));
}

// FUNCTION: 0x00779670
char __fastcall FUN_00779670(WideWstring const * from, Wstring * to)
{
	return to->set(FUN_007350C0(from->text()));
}

// FUNCTION: 0x00779690
PlayerProfile::PlayerProfile(Wstring const & name, int a, bool b, int c, int d, int e, int f, int g, int h, int i, int j, bool k, WideWstring const & l, WideWstring const & m) :
	Field8(0),
	Name(name),
	Field10(a),
	Field14(b),
	Field18(c),
	Field1C(d),
	Field20(e),
	Field24(f),
	Field28(g),
	Field2C(h),
	Field30(i),
	Field34(j),
	Field38(k),
	Wide3C(m),
	Wide40(l)
{
}

// FUNCTION: 0x00779730
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

// FUNCTION: 0x00779830
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

// FUNCTION: 0x00779940
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

// FUNCTION: 0x00779A40
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

// FUNCTION: 0x00779B50
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

// FUNCTION: 0x00779C90
void __fastcall FUN_00779C90(Wstring const * value)
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
			ini.Put_String(section, key, *value);
		}
		ini.Save(file);
	}
}

// FUNCTION: 0x00779DD0
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

// FUNCTION: 0x00779F10
void __fastcall FUN_00779F10(Wstring const * value)
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
			ini.Put_String(section, key, *value);
		}
		ini.Save(file);
	}
}

// FUNCTION: 0x0077A050
PlayerProfile::~PlayerProfile(void)
{
}

struct WOLOptions
{
	int Flags;
	char Loaded;
	WOLOptions(void) { Flags = 0; Loaded = 0; FUN_005175F0(); }
	void FUN_005175F0(void);
	unsigned char FUN_005176B0(void);
};

static inline void WOL_PutInt(INIClass & ini, Wstring const & section, char const * key, int value)
{
	Wstring k(key);
	ini.Put_Int(section, k, value, 0);
}

// FUNCTION: 0x0077A080
void PlayerProfile::Save(Wstring const * name)
{
	WOLOptions options;
	if (Field8) {
		Wstring section(*name);
		if (!name->length()) {
			section = DAT_00B73590;
		} else {
			section.toLower();
			if (!options.FUN_005176B0()) {
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
			{ Wstring k("Locale"); ini.Put_Int(section, k, Field10, 0); }
			{ Wstring k("LocaleVerified"); ini.Put_Bool(section, k, Field14); }
			{ Wstring k("Country"); ini.Put_Int(section, k, Field1C, 0); }
			{ Wstring k("Color"); ini.Put_Int(section, k, Field20, 0); }
			{ Wstring k("Rank"); ini.Put_Int(section, k, Field24, 0); }
			{ Wstring k("Points"); ini.Put_Int(section, k, Field28, 0); }
			{ Wstring k("Wins"); ini.Put_Int(section, k, Field2C, 0); }
			{ Wstring k("Losses"); ini.Put_Int(section, k, Field30, 0); }
			{ Wstring k("Disconnects"); ini.Put_Int(section, k, Field34, 0); }
			{ Wstring k("MatchByRes"); ini.Put_Bool(section, k, Field38); }
			Wstring text;
			FUN_00779230(&Wide3C, &text);
			{ Wstring k("UpdateTime"); ini.Put_String(section, k, text); }
			text.clear();
			text.set(FUN_007350C0(Wide40.text()));
			{ Wstring k("UpdateDate"); ini.Put_String(section, k, text); }
			ini.Save(file);
			file.Close();
			Field8 = 0;
			char const * unused0 = Name.c_str();
			char const * unused1 = Name.c_str();
			char const * unused2 = Name.c_str();
		}
	}
}

// FUNCTION: 0x0077A4A0
void PlayerProfile::SetName(Wstring const * name)
{
	if (!(*name == Name)) {
		Name = *name;
		Field8 = 1;
	}
}

// FUNCTION: 0x0077A4D0
void PlayerProfile::SetUpdateStamp(void)
{
	time_t now = time(0);
	struct tm * local = localtime(&now);
	Wide40.clear();
	char buf[16];
	strftime(buf, 16, "%m", local);
	int month = atoi(buf);
	strftime(buf, 16, "%d", local);
	if (month == 1) {
		Wide40.set(StringTable::LoadString("TXT_JANUARY", 0, "D:\\ra2mdpost\\WOLPersonaInformation.cpp", 0x261));
	} else if (month == 2) {
		Wide40.set(StringTable::LoadString("TXT_FEBRUARY", 0, "D:\\ra2mdpost\\WOLPersonaInformation.cpp", 0x263));
	} else if (month == 3) {
		Wide40.set(StringTable::LoadString("TXT_MARCH", 0, "D:\\ra2mdpost\\WOLPersonaInformation.cpp", 0x265));
	} else if (month == 4) {
		Wide40.set(StringTable::LoadString("TXT_APRIL", 0, "D:\\ra2mdpost\\WOLPersonaInformation.cpp", 0x267));
	} else if (month == 5) {
		Wide40.set(StringTable::LoadString("TXT_MAY", 0, "D:\\ra2mdpost\\WOLPersonaInformation.cpp", 0x269));
	} else if (month == 6) {
		Wide40.set(StringTable::LoadString("TXT_JUNE", 0, "D:\\ra2mdpost\\WOLPersonaInformation.cpp", 0x26B));
	} else if (month == 7) {
		Wide40.set(StringTable::LoadString("TXT_JULY", 0, "D:\\ra2mdpost\\WOLPersonaInformation.cpp", 0x26D));
	} else if (month == 8) {
		Wide40.set(StringTable::LoadString("TXT_AUGUST", 0, "D:\\ra2mdpost\\WOLPersonaInformation.cpp", 0x26F));
	} else if (month == 9) {
		Wide40.set(StringTable::LoadString("TXT_SEPTEMBER", 0, "D:\\ra2mdpost\\WOLPersonaInformation.cpp", 0x271));
	} else if (month == 10) {
		Wide40.set(StringTable::LoadString("TXT_OCTOBER", 0, "D:\\ra2mdpost\\WOLPersonaInformation.cpp", 0x273));
	} else if (month == 11) {
		Wide40.set(StringTable::LoadString("TXT_NOVEMBER", 0, "D:\\ra2mdpost\\WOLPersonaInformation.cpp", 0x275));
	} else if (month == 12) {
		Wide40.set(StringTable::LoadString("TXT_DECEMBER", 0, "D:\\ra2mdpost\\WOLPersonaInformation.cpp", 0x277));
	} else {
		Wide40.set(StringTable::LoadString("TXT_JANUARY", 0, "D:\\ra2mdpost\\WOLPersonaInformation.cpp", 0x279));
	}
	Wide40.cat(L" ");
	Wide40.cat(FUN_00735120(buf));
	Wide40.cat(L" ");
	strftime(buf, 16, "%Y", local);
	Wide40.cat(FUN_00735120(buf));
	char timeBuf[64];
	timeBuf[0] = 0;
	strftime(timeBuf, 64, "%H:%M:%S", local);
	Wide3C.set(FUN_00735120(timeBuf));
	Field8 = 1;
}

// FUNCTION: 0x0077A700
void PlayerProfile::SetLocale(int locale, int state)
{
	if (locale == Field10) {
		if (state == 2 && !Field14) {
			Field14 = 1;
			Field8 = 1;
		}
		return;
	}
	bool newer = state >= Field18;
	if (Field10 == 0) {
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
	Field10 = locale;
	Field18 = state;
	Field8 = 1;
}

// FUNCTION: 0x0077A760
void PlayerProfile::SetMatchByRes(bool value)
{
	if (value != Field38) {
		Field38 = value;
		Field8 = 1;
	}
}

// FUNCTION: 0x0077A780
void PlayerProfile::SetCountry(int country)
{
	if (country != Field1C) {
		Field1C = country;
		Field8 = 1;
	}
}
