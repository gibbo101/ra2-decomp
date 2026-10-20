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
