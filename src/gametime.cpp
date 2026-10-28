// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"
#include "gametime.h"
#include "win.h"

// SYNTHETIC: GAMEMD 0x004E2440 SYMBOL
// _$E2
GameTimeClass Game_Time;

// FUNCTION: GAMEMD 0x004E2450
GameTimeClass::GameTimeClass( void )
{
	game_start_time = timeGetTime();
}

// FUNCTION: GAMEMD 0x004E2460
unsigned int GameTimeClass::Get_Time( void )
{
	unsigned int curr_windows_time;
	unsigned int game_time;

	curr_windows_time = timeGetTime();
	if ( curr_windows_time <= game_start_time ) {
		game_time = MAX_ULONG - game_start_time + curr_windows_time;
	}
	else {
		game_time = curr_windows_time - game_start_time;
	}
	return( game_time );
}

// FUNCTION: GAMEMD 0x004E2480
unsigned int Get_Game_Time( void )
{
	return( Game_Time.Get_Time() );
}

// FUNCTION: GAMEMD 0x004E24A0
unsigned int Get_Game_Time_50( void )
{
	return( 3 * Game_Time.Get_Time() / 50 );
}
