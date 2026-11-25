// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
#pragma once

// Scenario INI strength uses 256 for full health.
inline double Scenario_Health_Ratio(float strength)
{
	return strength / 256.0;
}
