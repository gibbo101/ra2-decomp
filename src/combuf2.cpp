// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).


#include "always.h"
#include <stddef.h>
#include "_command.h"
#include "command.h"
#include "index.h"
#include "keyboard.h"

bool __fastcall FUN_0048BB40(CommandClass const * command, int key);
bool __fastcall FUN_0048BB60(int key);

// FUNCTION: GAMEMD 0x0048BB40
bool __fastcall FUN_0048BB40(CommandClass const * command, int key)
{
	key &= ~KN_RLSE_BIT;
	int plain = key & ~(WWKEY_SHIFT_BIT|WWKEY_ALT_BIT|WWKEY_CTRL_BIT|WWKEY_VK_BIT);
	if (key != plain) {
		return !command->Unknown14(key);
	}
	return true;
}

// FUNCTION: GAMEMD 0x0048BB60
bool __fastcall FUN_0048BB60(int input)
{
	int key = input & ~KN_RLSE_BIT;
	int plain = key & ~(WWKEY_SHIFT_BIT|WWKEY_ALT_BIT|WWKEY_CTRL_BIT|WWKEY_VK_BIT);
	CommandClass const * command = HotkeyCommands[plain];
	if (command == NULL) {
		return true;
	}
	return !command->Unknown14(key);
}

// TEMPLATE: GAMEMD 0x0048BC20
// ?search_compfunc@?$IndexClass@HPBVCommandClass@@@@CAHPBX0@Z
template class IndexClass<int, CommandClass const *>;
