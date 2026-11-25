// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).
#pragma once

#include "vector3.h"
#include "coord.h"
#include "map.h"
#include "cell.h"
#include "building.h"
#include "builtype.h"
#include "tactical.h"
#include "_tactica.h"
#include "_voxel.h"

inline bool Ballistic_Move(Vector3 & position, Vector3 & velocity, double gravity, double elasticity, double max_velocity)
{
	velocity.Z = velocity.Z - gravity;

	if (max_velocity > 0.0) {
		double speed = velocity.Length();
		if (speed > max_velocity) {
			velocity *= (float)(speed / speed);
		}
	}

	Coord previous_coord(position[0], position[1], position[2]);
	position += velocity;
	Coord current_coord(position[0], position[1], position[2]);

	int ground_height = Map.Get_Height_GL(current_coord);
	int bridge_height = BRIDGE_LEPTON_HEIGHT + ground_height;
	CellClass * cellptr = &Map[current_coord];

	bool hit_bridge_top = false;
	bool hit_bridge_bottom = false;
	if (cellptr->IsUnderBridge || Map[previous_coord].IsUnderBridge) {
		if (current_coord.Z >= bridge_height) {
			if (previous_coord.Z < bridge_height) {
				hit_bridge_bottom = true;
			}
		} else if (previous_coord.Z >= bridge_height) {
			hit_bridge_top = true;
		}
	}

	bool hit_obstacle = false;
	if (!hit_bridge_top && !hit_bridge_bottom) {
		if (position[2] >= ground_height && position[2] - 150.0f < ground_height) {
			BuildingClass * building = cellptr->Cell_Building();
			if (building == NULL && !cellptr->Has_Wall_Or_Gate(OVERLAY_NONE, FACING_NONE)) {
				hit_obstacle = false;
			} else {
				hit_obstacle = true;
				if (building != NULL && building->Class->IsLaserFence && building->LaserFenceFrame >= 8) {
					hit_obstacle = false;
				}
				if (building != NULL && building->IsStrange()) {
					hit_obstacle = false;
				}
			}

		}
	}

	float ground_z = (float)ground_height;
	if (position[2] >= ground_z && !hit_bridge_top && !hit_bridge_bottom && !hit_obstacle) {
		return false;
	}
	{
		if (hit_bridge_top) {
			position[2] = (float)bridge_height;
		} else if (hit_bridge_bottom) {
			position[2] = (float)(bridge_height - 20);
		} else {
			if ((ground_height - 100) < position[2]) {
				position[2] = ground_z;
			}
		}

		{
			Matrix3D slope_matrix = Get_Slope_Matrix(TacticalMap->Get_Cell_Ramp(current_coord));
			Matrix3D inverse_slope_matrix = Matrix3D::Orthogonal_Inverse(slope_matrix);
			Vector3 bounce_velocity(velocity.X, -velocity.Y, velocity.Z);
			bounce_velocity = inverse_slope_matrix.Rotate_Vector(bounce_velocity);
			bounce_velocity *= (float)elasticity;
			bounce_velocity.Z = -bounce_velocity.Z;
			bounce_velocity = slope_matrix.Rotate_Vector(bounce_velocity);
			velocity.X = bounce_velocity.X;
			velocity.Y = -bounce_velocity.Y;
			velocity.Z = bounce_velocity.Z;
		}

	}
	return true;
}
