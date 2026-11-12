// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#pragma once

#include "face.h"
#include "fastmath.h"

template <typename T>
class TVelocity2D
{
	public:
		TVelocity2D(void) {}
		TVelocity2D(T x, T y)
		{
			X = x;
			Y = y;
		}

		T Speed(void) const
		{
			return(fastmath::sqrt(X * X + Y * Y));
		}

		void Set_Yaw(DirType const & yaw)
		{
			Nonzero_Check();
			T horizontal_speed = Speed();
			X = horizontal_speed * fastmath::cos(yaw.As_Radian());
			Y = -horizontal_speed * fastmath::sin(yaw.As_Radian());
		}

		void Yaw(TVelocity2D const & towards, DirType const & rate)
		{
			DirType current_yaw = Get_Yaw();
			DirType desired_yaw = towards.Get_Yaw();
			current_yaw.Turn(desired_yaw, rate);
			Set_Yaw(current_yaw);
		}

		DirType Get_Yaw(void) const
		{
			return(fastmath::atan2((double)-Y, (double)X));
		}

		void Nonzero_Check(void)
		{
			if (X == 0 && Y == 0) {
				X = 100;
			}
		}

	public:
		T X;
		T Y;
};

template <typename T>
class TVelocity3D : public TVelocity2D<T>
{
		typedef TVelocity2D<T> BASECLASS;

	public:
		using BASECLASS::X;
		using BASECLASS::Y;

		TVelocity3D(void) {}
		TVelocity3D(const TVelocity3D & velocity) : BASECLASS(velocity), Z(velocity.Z) {}
		TVelocity3D(Dir256 pitch, Dir256 yaw, T magnitude);
		TVelocity3D(DirType const & yaw, DirType const & pitch, T magnitude);
		TVelocity3D(double angle, T magnitude);
		TVelocity3D(T x, T y, T z) : BASECLASS(x, y), Z(z) {}

		void Set(T x, T y, T z)
		{
			X = x;
			Y = y;
			Z = z;
		}

		T Horizontal_Speed(void) const { return(fastmath::sqrt(X * X + Y * Y)); }

		T Speed(void) const { return(fastmath::sqrt(X * X + Y * Y + Z * Z)); }

		void Set_Pitch(DirType const & pitch)
		{
			double current_pitch = Get_Pitch().As_Radian();
			T speed = Speed();
			if (current_pitch != 0) {
				X /= fastmath::cos(current_pitch);
				Y /= fastmath::cos(current_pitch);
			}
			X *= fastmath::cos(pitch.As_Radian());
			Y *= fastmath::cos(pitch.As_Radian());
			Z = speed * fastmath::sin(pitch.As_Radian());
		}

		void Pitch(DirType const & towards, DirType const & rate)
		{
			DirType current_pitch = Get_Pitch();
			current_pitch.Turn(towards, rate);
			Set_Pitch(current_pitch);
		}

		void Set_Speed(T speed)
		{
			Nonzero_Check();
			*this *= speed / Speed();
		}

		DirType Get_Pitch(void) const
		{
			return(fastmath::atan2((double)Z, Horizontal_Speed()));
		}

		void Nonzero_Check(void)
		{
			if (X == 0 && Y == 0 && Z == 0) {
				X = 100;
			}
		}

		TVelocity3D & operator += (const TVelocity3D & velocity)
		{
			X += velocity.X;
			Y += velocity.Y;
			Z += velocity.Z;
			return(*this);
		}

		TVelocity3D & operator *= (double scale)
		{
			X *= scale;
			Y *= scale;
			Z *= scale;
			return(*this);
		}

	public:
		T Z;
};

template <typename T>
TVelocity3D<T>::TVelocity3D(Dir256 pitch, Dir256 yaw, T magnitude)
	: BASECLASS(
		TVelocity2D<T>(
			fastmath::cos(DirType(pitch).As_Radian()) * fastmath::cos(DirType(yaw).As_Radian()) * magnitude,
			fastmath::cos(DirType(pitch).As_Radian()) * fastmath::sin(DirType(yaw).As_Radian()) * magnitude)),
		Z(fastmath::sin(DirType(pitch).As_Radian()) * magnitude)
{
}

template <typename T>
TVelocity3D<T>::TVelocity3D(DirType const & yaw, DirType const & pitch, T magnitude)
	: BASECLASS(
		TVelocity2D<T>(
			fastmath::cos(yaw.As_Radian()) * fastmath::cos(pitch.As_Radian()) * magnitude,
			fastmath::sin(yaw.As_Radian()) * fastmath::cos(pitch.As_Radian()) * magnitude)),
		Z(fastmath::sin(pitch.As_Radian()) * magnitude)
{
}

template <typename T>
TVelocity3D<T>::TVelocity3D(double angle, T magnitude)
{
	X = magnitude * fastmath::cos(angle);
	Y = -(magnitude * fastmath::sin(angle));
	Z = 0;
}
