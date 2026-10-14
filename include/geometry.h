// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).
#pragma once

#include "coord.h"

#include <cmath>

// Defined here, so every module including this header holds its own copy.
const double ISO_TILE_SIZE = 60.0;
const int ISO_TILE_PIXEL_W = 60;
const int ISO_TILE_PIXEL_H = 30;
const double ONE_RAD = M_PI / 180.0;

const double CELL_LEPTON_DIAG = fastmath::sqrt(pow((double)CELL_LEPTON_H, 2) * 2);
const double LEPTONS_PER_PIXEL = CELL_LEPTON_DIAG / ISO_TILE_SIZE;
const double PIXELS_PER_LEPTON = 1.0 / LEPTONS_PER_PIXEL;

const double RAD_45 = ONE_RAD * 45;
const double RAD_60 = ONE_RAD * 60;
const double RAD_90 = ONE_RAD * 90;

const int LEVEL_LEPTON_H = (int)(fastmath::tan(RAD_90 - RAD_60) * CELL_LEPTON_DIAG * 0.5);
const double CELL_SLOPE_ANGLE = fastmath::atan(LEVEL_LEPTON_H * (1 / (double)CELL_LEPTON_H));
const double CELL_DIAG_SLOPE_ANGLE = fastmath::atan((LEVEL_LEPTON_H * 2) / CELL_LEPTON_DIAG);
const int BRIDGE_LEPTON_HEIGHT = (int)((LEVEL_LEPTON_H * BRIDGE_CELL_HEIGHT) + 0.5);

const Cell CELL_NONE(0, 0);
const Coord COORD_NONE(0, 0, 0);
const Coord CELL_CENTER_COORD(CELL_LEPTON_W / 2, CELL_LEPTON_H / 2, 0);
