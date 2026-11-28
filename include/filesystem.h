// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
#pragma once

struct FileSystem {
	static void const * LoadFile(char const * name, bool as_shape);
	static void * LoadWholeFileEx(char const * name, bool & allocated);
};
