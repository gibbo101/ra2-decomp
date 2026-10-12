// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).
#pragma once

#include "audioctl.h"
#include "coord.h"

#include "voc.hh"

class CCINIClass;
class VocClass;

void Init_Vocs(CCINIClass const &ini);
void Free_Vocs(void);

int Sound_Effect(VocType voc, float volume=1, int=0);
int Voice_Sound_Effect(VocType voc, float volume=1);
int Sound_Effect(VocType voc, Coord const & coord);
VocClass * VocClass_From_Name(char const * name);
char const * Voc_Name(VocType voc);

enum ContextType {
	IN_NOVAR,
	IN_VAR
};

class VocClass
{
	public:
		VocClass(const char *filename);
		~VocClass(void);

		bool Fill_In(CCINIClass const &ini);

		bool Can_Play(void);
		int Play(float vol, int var);
		int Play(float vol);

		VocType Voc_Type(void);

		static void * PlayAt(int voc, Coord const & coord, AudioController * controller = NULL);
		static void * PlayGlobal(int voc, float volume, int panning, AudioController * controller = NULL);
		static VocType From_Name(char const * name);
		friend VocClass *VocClass_From_Name(char const * name);
		friend char const * Voc_Name(VocType voc);

	private:
		char Name[256];
		int Priority;
		float Volume;
		const void * FilePtr;
};
