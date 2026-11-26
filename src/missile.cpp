// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"
#include "geometry.h"
#include "rect.h"
#include <math.h>
#include "velocity.h"

#include "_map.h"
#include "cell.h"
#include "missile.h"

template class TVelocity3D<double>;

// SYNTHETIC: GAMEMD 0x005B1EE0 SYMBOL
// _$E41
// SYNTHETIC: GAMEMD 0x005B1F00 SYMBOL
// _$E2
// SYNTHETIC: GAMEMD 0x005B1F30 SYMBOL
// _$E5
// SYNTHETIC: GAMEMD 0x005B1F50 SYMBOL
// _$E8
// SYNTHETIC: GAMEMD 0x005B1F70 SYMBOL
// _$E11
// SYNTHETIC: GAMEMD 0x005B1F90 SYMBOL
// _$E14
// SYNTHETIC: GAMEMD 0x005B1FB0 SYMBOL
// _$E17
// SYNTHETIC: GAMEMD 0x005B1FD0 SYMBOL
// _$E20
// SYNTHETIC: GAMEMD 0x005B2010 SYMBOL
// _$E23
// SYNTHETIC: GAMEMD 0x005B2040 SYMBOL
// _$E26
// SYNTHETIC: GAMEMD 0x005B2070 SYMBOL
// _$E29
// SYNTHETIC: GAMEMD 0x005B20A0 SYMBOL
// _$E32
// SYNTHETIC: GAMEMD 0x005B20B0 SYMBOL
// _$E35

// SYNTHETIC: GAMEMD 0x005B20D0 SYMBOL
// _$E38

// TEMPLATE: GAMEMD 0x005B2A30
// ?Set_Pitch@?$TVelocity3D@N@@QAEXABVDirType@@@Z

// FUNCTION: GAMEMD 0x005B20F0
int Projectile_Motion(Coord & coord, TVelocity3D<double> & velocity, Coord const & target_coord, DirType const & rot, bool aircraft, bool airburst, bool veryhigh, bool level)
{
	if (target_coord == COORD_NONE) {
		DirType pitch = velocity.Get_Pitch();
		pitch.Turn(DirType(DIR_NE), rot);
		velocity.Set_Pitch(pitch);
		TVelocity3D<double> frame_velocity(velocity);
		coord = coord + Point3D(frame_velocity.X, frame_velocity.Y, frame_velocity.Z);
		return(velocity.Magnitude());
	} else {
		TVelocity3D<double> frame_velocity(velocity, velocity.Z);
		coord = coord + Point3D(frame_velocity.X, frame_velocity.Y, frame_velocity.Z);
		int target_distance = coord.Distance_To(target_coord);
		int horizontal_distance = Point2D(coord).Distance_To(Point2D(target_coord));

		Coord delta = target_coord - coord;

		TVelocity3D<double> target_direction;
		TVelocity3D<double> dir = TVelocity3D<double>(delta.X, delta.Y, delta.Z);
		target_direction = dir;

		DirType desired_yaw = velocity.Get_Yaw();
		desired_yaw.Turn(target_direction.Get_Yaw(), rot);
		velocity.Set_Yaw(desired_yaw);

		DirType desired_pitch = velocity.Get_Pitch();

		if (!aircraft && (airburst || horizontal_distance > ((veryhigh ? 6 : 3) * CELL_LEPTON)) && rot.As_Dir256() > 1) {
			TVelocity3D<double> lookahead_velocity(velocity);
			Coord lookahead_coord = coord + Point3D(lookahead_velocity.X, lookahead_velocity.Y, lookahead_velocity.Z) * 6;

			int terrain_height = Map.Get_Height_GL(lookahead_coord);
			if (Map[lookahead_coord].IsUnderBridge) {
				terrain_height += BRIDGE_LEPTON_HEIGHT;
			}

			int cell_distance = target_distance / CELL_LEPTON;
			cell_distance = (airburst || veryhigh) ? 10 : min(cell_distance, 5);

			if (!level) {
				int height_delta = coord.Z - LEVEL_LEPTON_H * cell_distance - terrain_height;
				if (height_delta < -20) {
					coord.Z += 18;
				} else if (height_delta > 20) {
					coord.Z -= 18;
				}

				if (height_delta < -(LEVEL_LEPTON_H / 2)) {
					desired_pitch.Turn(DirType(DIR_NE), rot / 2);
				} else if (height_delta > LEVEL_LEPTON_H / 2) {
					desired_pitch.Turn(DirType(Dir256(DIR_E + DIR_STEP_32)), rot / 2);
				} else {
					desired_pitch.Turn(DIR_E, rot / 2);
				}
			}
		} else if (!level) {
			desired_pitch.Turn(target_direction.Get_Pitch(), (rot + 256) / 2);
		}

		velocity.Set_Pitch(desired_pitch);

		delta.Z = airburst ? 0 : delta.Z / 4;

		return(delta.Length());
	}
}

// FUNCTION: GAMEMD 0x005B2930
// ??HDirType@@QBE?AV0@ABV0@@Z

// FUNCTION: GAMEMD 0x005B2950
// ??GDirType@@QBE?AV0@ABV0@@Z

// FUNCTION: GAMEMD 0x005B2970
// ??KDirType@@QBE?AV0@H@Z

// FUNCTION: GAMEMD 0x005B2990
// ?Is_Complete_Turn@DirType@@QAE_NABV1@0@Z

// FUNCTION: GAMEMD 0x005B29C0
// ?Turn@DirType@@QAE_NABV1@0@Z
