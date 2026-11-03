// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
#pragma once

#include "widewstring.h"
#include "win.h"

struct TooltipRequest
{
	HWND Window;
	int Index;
	WideWstring Text;
};
