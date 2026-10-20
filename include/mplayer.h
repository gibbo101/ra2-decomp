// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).
#pragma once

#include "session.h"

template<class T> class DynamicVectorClass;

GameType Select_MPlayer_Game (void);

void Clear_Vector(DynamicVectorClass <NodeNameType *> *vector);

int Surrender_Dialog(wchar_t const * text);
int Abort_Dialog(void);
