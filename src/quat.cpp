// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"
#include "quat.h"
#include "fastmath.h"
#include "matrix3d.h"

#define SLERP_EPSILON 1e-5
#define SLERP_HALF_PI 1.570796326794895

// FUNCTION: GAMEMD 0x00645C30
Quaternion::Quaternion(float x, float y, float z, float w)
{
	X=x;
	Y=y;
	Z=z;
	W=w;
}

// FUNCTION: GAMEMD 0x00645C50
void Quaternion::Set(float x, float y, float z, float w)
{
	X=x;
	Y=y;
	Z=z;
	W=w;
}

// FUNCTION: GAMEMD 0x00645C70
void Quaternion::Normalize(void)
{
	float len2 = X * X + Y * Y + Z * Z + W * W;

	if (0.0f != len2) {
		X /= len2;
		Y /= len2;
		Z /= len2;
		W /= len2;
	}
}

// FUNCTION: GAMEMD 0x00645CD0
void Quaternion::Scale(float s)
{
	X = (float)(s * X);
	Y = (float)(s * Y);
	Z = (float)(s * Z);
	W = (float)(s * W);
}

// FUNCTION: GAMEMD 0x00645D00
float & Quaternion::operator [](int i)
{
	return (&X)[i];
}

// FUNCTION: GAMEMD 0x00645D10
const float Quaternion::operator [](int i) const
{
	return (&X)[i];
}

// FUNCTION: GAMEMD 0x00645D20
Quaternion Quaternion::operator=(const Quaternion & q)
{
	X = q[0];
	Y = q[1];
	Z = q[2];
	W = q[3];

	return(*this);
}

// FUNCTION: GAMEMD 0x00645ED0
Quaternion operator * (const Quaternion & a,const Quaternion & b)
{
	Quaternion t1;
	Quaternion t2;
	Quaternion t3;
	Quaternion q;

	t1 = a;
	t2 = b;

	t1.Scale(b.W);
	t2.Scale(a.W);

	t3[0] = a.Y * b.Z - a.Z * b.Y;
	t3[1] = a.Z * b.X - a.X * b.Z;
	t3[2] = a.X * b.Y - a.Y * b.X;

	q.X = t1.X + t2.X + t3.X;
	q[1] = t1[1] + t2[1] + t3.Y;
	q[2] = t1[2] + t2[2] + t3.Z;
	q.W = a.W * b.W - (a.X * b.X + (a.Y * b.Y + a[2] * b[2]));

	q.Normalize();
	return(q);
}

// FUNCTION: GAMEMD 0x00646040
Quaternion operator / (const Quaternion & a,const Quaternion & b)
{
	return(a * Inverse(b));
}

// FUNCTION: GAMEMD 0x006460C0
Quaternion Inverse(const Quaternion & q)
{
	return(Quaternion(-q[0],-q[1],-q[2],q[3]));
}

// FUNCTION: GAMEMD 0x00646110
Quaternion Conjugate(const Quaternion & q)
{
	return(Quaternion(-q.X,-q.Y,-q.Z,q.W));
}

inline float Trackball_Length(Vector3 const &vector)
{
	double y = vector.Y;
	double z = vector.Z;
	double x = vector.X;
	return fastmath::sqrt(x * x + z * z + y * y);
}

static float project_to_sphere(float radius, float x, float y)
{
	const float sqrt_two = 1.41421356f;
	float distance = fastmath::sqrt(x * x + y * y);
	float z;
	if (distance < radius * (sqrt_two / 2.0f)) {
		z = fastmath::sqrt(radius * radius - distance * distance);
	} else {
		float tangent = radius / sqrt_two;
		z = tangent * tangent / distance;
	}
	return z;
}

// FUNCTION: GAMEMD 0x00646160
Quaternion Trackball(float x0, float y0, float x1, float y1, float radius)
{
	Vector3 axis;
	float angle;
	Vector3 first;
	Vector3 second;
	Vector3 delta;
	float distance;
	if (x0 == x1 && y0 == y1) {
		return Quaternion(0.0f, 0.0f, 0.0f, 1.0f);
	}
	first.X = x0;
	first.Y = y0;
	first.Z = project_to_sphere(radius, x0, y0);
	second.X = x1;
	second.Y = y1;
	second.Z = project_to_sphere(radius, x1, y1);
	axis = Vector3::Cross_Product(second, first);
	delta = first - second;
	distance = Trackball_Length(delta) / (2.0 * radius);
	if (distance > 1.0f) {
		distance = 1.0f;
	} else if (distance < -1.0f) {
		distance = -1.0f;
	}
	angle = 2.0 * fastmath::asin(distance);
	return Axis_To_Quat(axis, angle);
}

inline Vector3 Quat_Normalize(Vector3 const &vector)
{
	float length = fastmath::sqrt(vector.X * vector.X + vector.Y * vector.Y + vector.Z * vector.Z);
	if (length != 0.0) {
		double divisor = length;
		return Vector3(
			(float)(vector.X / divisor),
			(float)(vector.Y / divisor),
			(float)(vector.Z / divisor));
	}
	return vector;
}

// FUNCTION: GAMEMD 0x00646480
Quaternion Axis_To_Quat(const Vector3 &a, float phi)
{
	Quaternion q;
	Vector3 tmp = a;

	tmp = Quat_Normalize(tmp);
	q.X = tmp.X;
	q.Y = tmp.Y;
	q.Z = tmp.Z;

	q.Scale(fastmath::sin(phi / 2.0f));
	q.W = fastmath::cos(phi / 2.0f);

	return(q);
}

// FUNCTION: GAMEMD 0x00646590
Quaternion Slerp(const Quaternion &a, const Quaternion &b, float t)
{
	Quaternion res;
	double alpha;				/// interpolation parameter (0 to 1)
	double beta;				// complementary interploation parameter
	double theta;				// angle between p and q
	double sin_t, cos_t; 		// sine, cosine of theta

	cos_t = a.X * b.X + a.Y * b.Y + a.Z * b.Z + a.W * b.W;

	if ((cos_t + 1.0) > SLERP_EPSILON) {
		
		if ((1.0 - cos_t) > SLERP_EPSILON) {
			theta = fastmath::acos(cos_t);
			sin_t = fastmath::sin(theta);
			beta = fastmath::sin((1.0f - t) * theta) / sin_t;
			alpha = fastmath::sin(t * theta) / sin_t;
		} else {
			beta = 1.0f - t;
			alpha = t;

		}
		for (int i = 0; i < 4; i++) {
			res[i] = (beta * a[i]) + (alpha * b[i]);
		}
	} else {
		beta = fastmath::sin((1.0f - t) * SLERP_HALF_PI);
		alpha = fastmath::sin(t * SLERP_HALF_PI);
		for (int i = 0; i < 4; i++) {
			res[i] = (beta * a[i]) + (alpha * b[i]);
		}
	}
	return(res);
}

// FUNCTION: GAMEMD 0x00646730
Quaternion Build_Quaternion(const Matrix3D& mat)
{
	const float * m = (float const *)&mat;

	double s, tr;
	int i, j, k;
	Quaternion q;

	int tab1[3] = { (4 * 0) + 0, (4 * 1) + 1, (4 * 2) + 2 };

	int tab2[3][4] = {
		{ (4 * 0) + 0, (4 * 0) + 1, (4 * 0) + 2, (4 * 0) + 3 },
		{ (4 * 1) + 0, (4 * 1) + 1, (4 * 1) + 2, (4 * 1) + 3 },
		{ (4 * 2) + 0, (4 * 2) + 1, (4 * 2) + 2, (4 * 2) + 3 },
	};


	tr = m[tab1[0]] + m[tab1[1]] + m[tab1[2]];

	if (tr == 0.0) {
		return(q);
	}

	if (tr > 0.0) {

		s = fastmath::sqrt(tr + 1.0);
		q[3] = (s * 0.5);
		s = 0.5 / s;

		q[0] = (m[tab2[2][1]] - m[tab2[1][2]]) * s;
		q[1] = (m[tab2[0][2]] - m[tab2[2][0]]) * s;
		q[2] = (m[tab2[1][0]] - m[tab2[0][1]]) * s;

	} else {

		i = 0;
		if ((double)m[tab1[1]] > (double)m[tab1[i]]) i = 1;
		if ((double)m[tab1[2]] > (double)m[tab1[i]]) i = 2;

		switch (i) {
			case 1:
				j = 2;
				k = 0;
				break;
			case 2:
				j = 0;
				k = 1;
				break;
			default:
				j = 1;
				k = 2;
				break;
		}

		double di = m[tab1[i]];
		double dj = m[tab1[j]];
		double dk = m[tab1[k]];

		s = fastmath::sqrt(di - (dj + dk) + 1.0);

		q[i] = (s * 0.5);
		s = 0.5 / s;

		q[3] = (m[tab2[k][j]] - m[tab2[j][k]]) * s;
		q[j] = (m[tab2[j][i]] + m[tab2[i][j]]) * s;
		q[k] = (m[tab2[k][i]] + m[tab2[i][k]]) * s;
	}

	return(q);
}

// FUNCTION: GAMEMD 0x00646980
Matrix3D Build_Matrix3D(const Quaternion &q)
{
	Matrix3D m;

	m[0].X = 1.0f - 2.0f * (q[1] * q[1] + q[2] * q[2]);
	m[0].Y = 2.0f * (q[0] * q[1] - q[2] * q[3]);
	m[0].Z = 2.0f * (q[2] * q[0] + q[1] * q[3]);

	m[1].X = 2.0f * (q[0] * q[1] + q[2] * q[3]);
	m[1].Y = 1.0f - 2.0f * (q[2] * q[2] + q[0] * q[0]);
	m[1].Z = 2.0f * (q[1] * q[2] - q[0] * q[3]);

	m[2].X = 2.0f * (q[2] * q[0] - q[1] * q[3]);
	m[2].Y = 2.0f * (q[1] * q[2] + q[0] * q[3]);
	m[2].Z = 1.0f - 2.0f * (q[1] * q[1] + q[0] * q[0]);

	m[0].W = m[1].W = m[2].W = 0.0f;

	return(m);
}
