// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).
#pragma once

#include "coord.h"
#include "matrix3d.h"
#include "noinit.h"
#include "quat.h"
#include "vector3.h"

enum BounceResultType {
	BOUNCE_MOVING,
	BOUNCE_IMPACT,
	BOUNCE_SETTLED,
};

class BounceClass
{
	public:
		BounceClass(void) :
			Elasticity(0),
			Gravity(0),
			MaxVelocity(0),
			MyCoord(),
			Velocity(),
			Rotation(0.0, 0.0, 0.0, 1.0),
			AngularVelocity(0.0, 0.0, 0.0, 1.0)
		{

		}

		BounceClass(NoInitClass const & x) : Rotation(x), AngularVelocity(x) {}

		void Init(Coord const & coord, double elasticity, double, double min_speed, double max_speed);
		void Init(Coord const & coord, double elasticity, double gravity, double max_velocity, Vector3 const & velocity, double rotation);

		Coord Get_Bounce_Coord(void) const;
		Matrix3D Get_Matrix(void) const;
		double Get_Remaining_Motion(void) const;
		BounceResultType AI(void);

	public:

		double Elasticity;

		double Gravity;

		double MaxVelocity;

		Vector3 MyCoord;

		Vector3 Velocity;

		Quaternion Rotation;

		Quaternion AngularVelocity;
};
