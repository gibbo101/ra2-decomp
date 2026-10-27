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

// FUNCTION: GAMEMD 0x00646480
Quaternion Axis_To_Quat(const Vector3 &a, float phi)
{
	Quaternion q;
	Vector3 tmp = a;

	tmp = Normalize(tmp);
	q.X = tmp[0];
	q.Y = tmp[1];
	q.Z = tmp[2];

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
