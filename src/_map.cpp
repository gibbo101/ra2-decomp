// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"
#include "rect.h"
#include "geometry.h"
#include "scroll.h"
#include "mouse.h"
#include "imap.h"

// SYNTHETIC: GAMEMD 0x0040CF80 SYMBOL
// _$E2
// SYNTHETIC: GAMEMD 0x0040CFA0 SYMBOL
// _$E5
// SYNTHETIC: GAMEMD 0x0040CFD0 SYMBOL
// _$E8
// SYNTHETIC: GAMEMD 0x0040CFF0 SYMBOL
// _$E11
// SYNTHETIC: GAMEMD 0x0040D010 SYMBOL
// _$E14
// SYNTHETIC: GAMEMD 0x0040D030 SYMBOL
// _$E17
// SYNTHETIC: GAMEMD 0x0040D050 SYMBOL
// _$E20
// SYNTHETIC: GAMEMD 0x0040D070 SYMBOL
// _$E23
// SYNTHETIC: GAMEMD 0x0040D0B0 SYMBOL
// _$E26
// SYNTHETIC: GAMEMD 0x0040D0E0 SYMBOL
// _$E29
// SYNTHETIC: GAMEMD 0x0040D110 SYMBOL
// _$E32
// SYNTHETIC: GAMEMD 0x0040D140 SYMBOL
// _$E35
// SYNTHETIC: GAMEMD 0x0040D150 SYMBOL
// _$E38
// SYNTHETIC: GAMEMD 0x0040D170 SYMBOL
// _$E41
// SYNTHETIC: GAMEMD 0x0040D190 SYMBOL
// _$E91
// SYNTHETIC: GAMEMD 0x0040D1B0 SYMBOL
// _$E89
// SYNTHETIC: GAMEMD 0x0040D200 SYMBOL
// _$E96
// SYNTHETIC: GAMEMD 0x0040D220 SYMBOL
// _$E94
// FUNCTION: GAMEMD 0x0040D230
// GScreenClass::AddRef
// FUNCTION: GAMEMD 0x0040D240
// GScreenClass::Release
// FUNCTION: GAMEMD 0x0040D250
// DisplayClass::Put_Place_Back
// FUNCTION: GAMEMD 0x0040D260
// SidebarClass::StripClass::BuildType::~BuildType
// FUNCTION: GAMEMD 0x0040D270
// ScrollClass::Init_IO
// FUNCTION: GAMEMD 0x0040D280
// MouseClass::Get_Mouse_Shape
// FUNCTION: GAMEMD 0x0040D290
// MouseClass::~MouseClass
// TEMPLATE: GAMEMD 0x0040D300
// ??1?$BasicTimerClass@VFrameTimerClass@@@@QAE@XZ

MouseClass Map;

IGameMapPtr GameMap;
