// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "vqaplay.h"

static VQAConfig _defaultconfig = {
	0,

	0,

	0,

	0,

	0,

	0,

	0,

	NULL,

	-1, -1,

	-1, -1,

	-1,

	-1,

	0,

	0,

	0,

	VQAOPTF_AUDIO,

	6,

	3,

	0,

	NULL,

	-1,

	2048,

	-1,

	0x00FF00FF,

	0,

	NULL,

	-1,

	0,

	0,

	0,

	0,

	0
};

// FUNCTION: 0x007BE640
void VQA_DefaultConfig(VQAConfig *config)
{
	memcpy(config, &_defaultconfig, sizeof(VQAConfig));
}
