// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
#pragma once

struct MovieUnlockableInfo {
	char const * Filename;
	char const * Description;
	int DiskRequired;

	static MovieUnlockableInfo Common[];
	static MovieUnlockableInfo Allied[];
	static MovieUnlockableInfo Soviet[];
};
