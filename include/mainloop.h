// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).
#pragma once

#include "keyboard.h"

bool Main_Loop(void);
void Keyboard_Process(KeyNumType & input);
void Message_Input(KeyNumType & input);
void Sync_Delay(void);
void Motion_Capture(void);
void Multiplayer_Debug_Print(void);
