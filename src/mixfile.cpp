// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"
#include "mixfile.h"
#include "bsearch.h"
#include "buff.h"
#include "ccfile.h"
#include "cd.h"
#include "conquer.h"
#include "crc.h"
#include "globals.h"
#include "pk.h"
#include "pkstraw.h"
#include "shastraw.h"
#include "xstraw.h"

// FUNCTION: 0x005B3C00
int __cdecl compfunc(void const * ptr1, void const * ptr2)
{
	if (*(int const *)ptr1 < *(int const *)ptr2) return(-1);
	if (*(int const *)ptr1 > *(int const *)ptr2) return(1);
	return(0);
}

// FUNCTION: 0x005B3E90
bool MixFileClass::Free(char const * filename)
{
	MixFileClass * ptr = Finder(filename);

	if (ptr) {
		ptr->Free();
		return(true);
	}
	return(false);
}

// FUNCTION: 0x005B3F50
MixFileClass::~MixFileClass(void)
{
	if (Filename) {
		free((char *)Filename);
	}
	if (Data != NULL && IsAllocated) {
		delete [] Data;
		IsAllocated = false;
	}
	Data = NULL;

	if (HeaderBuffer != NULL) {
		delete [] HeaderBuffer;
		HeaderBuffer = NULL;
	}

	Unlink();
}

// FUNCTION: 0x005B4350
MixFileClass * MixFileClass::Finder(char const * filename)
{
	MixFileClass * ptr = List.First();
	while (ptr->Is_Valid()) {
		char path[_MAX_PATH];
		char name[_MAX_FNAME];
		char ext[_MAX_EXT];

		_splitpath(ptr->Filename, NULL, NULL, name, ext);
		_makepath(path, NULL, NULL, name, ext);

		if (stricmp(path, filename) == 0) {
			return(ptr);
		}
		ptr = ptr->Next();
	}
	return(NULL);
}

// FUNCTION: 0x005B4400
void MixFileClass::Free(void)
{
	if (Data != NULL && IsAllocated) {
		delete [] Data;
	}
	Data = NULL;
	IsAllocated = false;
}
