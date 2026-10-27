// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).
#pragma once

class Coord;
template <typename T>
class TVelocity3D;
class DirType;

int Projectile_Motion(Coord & coord, TVelocity3D<double> & velocity, Coord const & target_coord, DirType const & dir, bool aircraft, bool airburst, bool veryhigh, bool level);
