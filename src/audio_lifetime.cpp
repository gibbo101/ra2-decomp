// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
#include "always.h"
extern int DAT_0087E730;
void FUN_0040A940(void);
void FUN_0040A950(void);
// FUNCTION: GAMEMD 0x00407020
void FUN_00407020(void)
{
    if (!DAT_0087E730) FUN_0040A940();
    ++DAT_0087E730;
}
// FUNCTION: GAMEMD 0x00407040
void FUN_00407040(void)
{
    if (DAT_0087E730 && --DAT_0087E730 == 0) FUN_0040A950();
}
