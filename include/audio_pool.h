// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).
#pragma once

struct SoundNode;
struct SoundPool
{
    void FUN_00407510(void);
    SoundNode * FUN_00407520(void);
    void __fastcall FUN_00407530(SoundNode * node);
};
SoundPool * __fastcall FUN_004074D0(int count, int size);
