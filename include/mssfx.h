// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).
#pragma once

class MSSfx
{
	public:
		MSSfx(char const * name, char const * file_name);
		~MSSfx(void);
		char const * Get_Name(void) {return(Name);}
		void Play(int volume);

	private:

		char * Name;

		void * Sample;

		bool Loaded;
};
