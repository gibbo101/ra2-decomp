// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"

#include "ccfile.h"

#include "_mixfile.h"
#include "assert.h"
#include "cd.h"
#include "mixfile.h"
#include "wstring.h"

#include <new>
#include <math.h>

// FUNCTION: 0x004739F0
CCFileClass::CCFileClass(char const * filename) :
	Position(0)
{
	Availability = 0;
	CDFileClass::Set_Name(filename);
}

// FUNCTION: 0x00473A30
CCFileClass::CCFileClass(Wstring const & filename) :
	Position(0)
{
	Availability = 0;
	char const * name = filename.text();
	Availability = 0;
	CDFileClass::Set_Name(name);
}

// FUNCTION: 0x00473A80
CCFileClass::CCFileClass(void) :
	Position(0)
{
	Availability = 0;
}

// FUNCTION: 0x00473AB0
void CCFileClass::Error(int, int, char const *)
{
	if (!SkipCDCheck) {
		CD().Force_Available(CD::Get_Required_Disk());
	}
}

// FUNCTION: 0x00473AE0
int CCFileClass::Write(void const * buffer, int size)
{
	if (Is_Resident()) {
		Error(13, false, File_Name());
	}
	return(BASECLASS::Write(buffer, size));
}

// FUNCTION: 0x00473B10
int CCFileClass::Read(void * buffer, int size)
{
	bool opened = false;

	if (!Is_Open()) {
		if (Open()) {
			opened = true;
		}
	}

	if (Is_Resident()) {
		int maximum = Data.Get_Size() - Position;
		size = maximum < size ? maximum : size;
		if (size) {
			memmove(buffer, (char *)Data + Position, size);
			Position += size;
		}
		if (opened) Close();
		return(size);
	}

	int s = BASECLASS::Read(buffer, size);
	if (opened) Close();
	return(s);
}

// FUNCTION: 0x00473BA0
int CCFileClass::Seek(int pos, int dir)
{
	if (Is_Resident()) {
		switch (dir) {
			case SEEK_END:
				Position = Data.Get_Size();
				break;
			case SEEK_SET:
				Position = 0;
				break;
			case SEEK_CUR:
			default:
				break;
		}
		Position += pos;
		Position = Position < 0 ? 0 : Position;
		Position = Position > Data.Get_Size() ? Data.Get_Size() : Position;
		return(Position);
	}
	return(BASECLASS::Seek(pos, dir));
}

// FUNCTION: 0x00473C00
int CCFileClass::Size(void)
{
	if (Is_Resident()) return(Data.Get_Size());

	if (!BASECLASS::Is_Available()) {
		int length = 0;
		MFCD::Offset(File_Name(), NULL, NULL, NULL, &length);
		return(length);
	}
	return(BASECLASS::Size());
}

// FUNCTION: 0x00473C50
bool CCFileClass::Is_Available(int)
{
	if (Availability == 1) return(true);
	if (Is_Open()) {
		Availability = 1;
		return(true);
	}
	if (MFCD::Offset(File_Name())) {
		Availability = 1;
		return(true);
	}
	if (BASECLASS::Is_Available()) {
		Availability = 1;
		return(true);
	}
	Availability = 2;
	return(false);
}

// FUNCTION: 0x00473CD0
bool CCFileClass::Is_Open(void) const
{
	if (Is_Resident()) return(true);
	return(BASECLASS::Is_Open());
}

// FUNCTION: 0x00473CE0
void CCFileClass::Close(void)
{
	new(&Data) ::Buffer;
	Position = 0;
	BASECLASS::Close();
}

// FUNCTION: 0x00473D10
int CCFileClass::Open(int rights)
{
	Close();

	if ((rights & WRITE) || BASECLASS::Is_Available()) {
		return(BASECLASS::Open(rights));
	}

	MFCD * mixfile = NULL;
	void * pointer = NULL;
	int length = 0;
	int start = 0;
	if (MFCD::Offset(File_Name(), &pointer, &mixfile, &start, &length)) {
		if (pointer == NULL && mixfile != NULL) {
			if (Open(mixfile->Filename, READ)) {
				char * dupfile = strdup(File_Name());
				Searching(false);
				Set_Name(dupfile);
				Searching(true);
				Bias(0);
				Bias(start, length);
				Seek(0, SEEK_SET);
				free(dupfile);
				return(true);
			}
			return(false);
		} else {
			new (&Data) ::Buffer(pointer, length);
			Position = 0;
		}
	} else {
		return(BASECLASS::Open(rights));
	}
	return(true);
}

// FUNCTION: 0x00473E50
unsigned int CCFileClass::Get_Date_Time(void)
{
	unsigned int datetime;
	MFCD * mixfile = NULL;

	datetime = BASECLASS::Get_Date_Time();

	if (!datetime) {
		if (MFCD::Offset(File_Name(), NULL, &mixfile, NULL, NULL)) {
			return(CCFileClass(mixfile->Filename).Get_Date_Time());
		}
	}

	return(datetime);
}

// FUNCTION: 0x00473F00
bool CCFileClass::Set_Date_Time(unsigned int datetime)
{
	bool status;
	MFCD * mixfile = NULL;

	status = BASECLASS::Set_Date_Time(datetime);

	if (!status) {
		if (MFCD::Offset(File_Name(), NULL, &mixfile, NULL, NULL)) {
			return(CCFileClass(mixfile->Filename).Set_Date_Time(datetime));
		}
	}

	return(status);
}

// FUNCTION: 0x00473FC0
char const * CCFileClass::Set_Name(char const * filename)
{
	Availability = 0;
	return(CDFileClass::Set_Name(filename));
}

namespace fastmath { double __cdecl sqrt(double x); double __cdecl tan(double x); double __cdecl atan(double x); }
extern const double DAT_007E1708;
extern const double DAT_007E1710;
extern const double DAT_007E1718;
extern const double DAT_007E1720;
extern const double DAT_007E1728;
extern const double DAT_007E1730;
extern const double DAT_007E1738;
extern const double DAT_007E1740;
extern const double DAT_007E4C40;
extern const double DAT_007E4C48;

// FUNCTION: 0x00473FE0 $E9
double DAT_0089E2B0 = fastmath::sqrt(pow(DAT_007E1710, DAT_007E1708) * 2);

// FUNCTION: 0x00474010 $E12
double DAT_0089E300 = DAT_0089E2B0 / DAT_007E4C40;

// FUNCTION: 0x00474030 $E15
double DAT_0089E2E8 = DAT_007E1718 / DAT_0089E300;

// FUNCTION: 0x00474050 $E18
double DAT_0089E2E0 = DAT_007E1720 * DAT_007E4C48;

// FUNCTION: 0x00474070 $E21
double DAT_0089E2B8 = DAT_007E1728 * DAT_007E4C48;

// FUNCTION: 0x00474090 $E24
double DAT_0089E290 = DAT_007E1730 * DAT_007E4C48;

// FUNCTION: 0x004740B0 $E27
int DAT_0089E308 = (int)(fastmath::tan(DAT_0089E290 - DAT_0089E2B8) * DAT_007E1738 * DAT_0089E2B0);

// FUNCTION: 0x004740F0 $E30
double DAT_0089E2C0 = fastmath::atan(DAT_0089E308 * DAT_007E1740);

// FUNCTION: 0x00474120 $E33
double DAT_0089E2D8 = fastmath::atan((DAT_0089E308 * 2) / DAT_0089E2B0);
