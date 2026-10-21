// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"
#include "matrix3d.h"
#include "fastmath.h"
#include <cstring>

inline void Matrix3D::Multiply(const Matrix3D & A, const Matrix3D & B, Matrix3D * set_result)
{
	float a00 = A.Row[0].X;
	float a01 = A.Row[0].Y;
	float a02 = A.Row[0].Z;
	float a10 = A.Row[1].X;
	float a11 = A.Row[1].Y;
	float a12 = A.Row[1].Z;
	float a20 = A.Row[2].X;
	float a21 = A.Row[2].Y;
	float a22 = A.Row[2].Z;

	float b00 = B.Row[0].X;
	float b01 = B.Row[0].Y;
	float b02 = B.Row[0].Z;
	float b03 = B.Row[0].W;
	float b10 = B.Row[1].X;
	float b11 = B.Row[1].Y;
	float b12 = B.Row[1].Z;
	float b13 = B.Row[1].W;
	float b20 = B.Row[2].X;
	float b21 = B.Row[2].Y;
	float b22 = B.Row[2].Z;
	float b23 = B.Row[2].W;

	set_result->Row[0].X = a00 * b00 + a01 * b10 + a02 * b20;
	set_result->Row[1].X = a10 * b00 + a11 * b10 + a12 * b20;
	set_result->Row[2].X = a20 * b00 + a21 * b10 + a22 * b20;

	set_result->Row[0].Y = a00 * b01 + a01 * b11 + a02 * b21;
	set_result->Row[1].Y = a10 * b01 + a11 * b11 + a12 * b21;
	set_result->Row[2].Y = a20 * b01 + a21 * b11 + a22 * b21;

	set_result->Row[0].Z = a00 * b02 + a01 * b12 + a02 * b22;
	set_result->Row[1].Z = a10 * b02 + a11 * b12 + a12 * b22;
	set_result->Row[2].Z = a20 * b02 + a21 * b12 + a22 * b22;

	set_result->Row[0].W = a00 * b03 + a01 * b13 + a02 * b23 + A.Row[0].W;
	set_result->Row[1].W = a10 * b03 + a11 * b13 + a12 * b23 + A.Row[1].W;
	set_result->Row[2].W = a20 * b03 + a21 * b13 + a22 * b23 + A.Row[2].W;
}

// FUNCTION: GAMEMD 0x005AE5E0
Matrix3D::Matrix3D(float m[12])
{
	for (int i = 0; i < 12; i++) {
		((float *)this)[i] = m[i];
	}
}

// FUNCTION: GAMEMD 0x005AE610
Matrix3D::Matrix3D(const Matrix3DStruct & m)
{
	Matrix3DStruct_To_Matrix3D(*this, m);
}

// FUNCTION: GAMEMD 0x005AE630
Matrix3D::Matrix3D
(
	float m11,float m12,float m13,float m14,
	float m21,float m22,float m23,float m24,
	float m31,float m32,float m33,float m34
)
{
	Row[0].Set(m11,m12,m13,m14);
	Row[1].Set(m21,m22,m23,m24);
	Row[2].Set(m31,m32,m33,m34);
}

// FUNCTION: GAMEMD 0x005AE690
Matrix3D::Matrix3D
(
	const Vector3	&x,
	const Vector3	&y,
	const Vector3	&z,
	const Vector3	&pos
)
{
	Row[0].X = x.X;
	Row[0].Y = x.Y;
	Row[0].Z = x.Z;
	Row[0].W = pos.X;

	Row[1].X = y.X;
	Row[1].Y = y.Y;
	Row[1].Z = y.Z;
	Row[1].W = pos.Y;

	Row[2].X = z.X;
	Row[2].Y = z.Y;
	Row[2].Z = z.Z;
	Row[2].W = pos.Z;
}

// FUNCTION: GAMEMD 0x005AE6F0
Matrix3D::Matrix3D(float zrot, float xrot)
{
	float(&m)[3][4] = (float(&)[3][4])*this;

	m[0][0] = 1.0f;
	m[0][1] = 0.0f;
	m[0][2] = 0.0f;
	m[0][3] = 0.0f;

	m[1][0] = 0.0f;
	m[1][1] = 1.0f;
	m[1][2] = 0.0f;
	m[1][3] = 0.0f;

	m[2][0] = 0.0f;
	m[2][1] = 0.0f;
	m[2][2] = 1.0f;
	m[2][3] = 0.0f;

	Rotate_Z(zrot);
	Rotate_X(xrot);
	Rotate_Z(-zrot);
}

// FUNCTION: GAMEMD 0x005AE750
Matrix3D::Matrix3D(const Vector3 & axis, float angle)
{
	Set(axis, angle);
}

// FUNCTION: GAMEMD 0x005AE860
void Matrix3D::Make_Identity(void)
{
	Row[0].Set(1,0,0,0);
	Row[1].Set(0,1,0,0);
	Row[2].Set(0,0,1,0);
}

// FUNCTION: GAMEMD 0x005AE890
void Matrix3D::Translate(float x, float y, float z)
{
	Row[0].W += (float)(Row[0].X*x + Row[0].Y*y + Row[0].Z*z);
	Row[1].W += (float)(Row[1].X*x + Row[1].Y*y + Row[1].Z*z);
	Row[2].W += (float)(Row[2].X*x + Row[2].Y*y + Row[2].Z*z);
}

// FUNCTION: GAMEMD 0x005AE8F0
void Matrix3D::Translate(Vector3 const & t)
{
	Translate_X(t[0]);
	Translate_Y(t[1]);
	Translate_Z(t[2]);
}

// FUNCTION: GAMEMD 0x005AE980
void Matrix3D::Translate_X(float x)
{
	Row[0].W += (float)(Row[0].X*x);
	Row[1].W += (float)(Row[1].X*x);
	Row[2].W += (float)(Row[2].X*x);
}

// FUNCTION: GAMEMD 0x005AE9B0
void Matrix3D::Translate_Y(float y)
{
	Row[0].W += (float)(Row[0].Y*y);
	Row[1].W += (float)(Row[1].Y*y);
	Row[2].W += (float)(Row[2].Y*y);
}

// FUNCTION: GAMEMD 0x005AE9E0
void Matrix3D::Translate_Z(float z)
{
	Row[0].W += (float)(Row[0].Z*z);
	Row[1].W += (float)(Row[1].Z*z);
	Row[2].W += (float)(Row[2].Z*z);
}

// FUNCTION: GAMEMD 0x005AEA10
void Matrix3D::Scale(float scale)
{
	Row[0].X *= scale;
	Row[1].X *= scale;
	Row[2].X *= scale;
	Row[0].Y *= scale;
	Row[1].Y *= scale;
	Row[2].Y *= scale;
	Row[0].Z *= scale;
	Row[1].Z *= scale;
	Row[2].Z *= scale;
}

// FUNCTION: GAMEMD 0x005AEA70
void Matrix3D::Scale(float x, float y, float z)
{
	Row[0].X *= x;
	Row[1].X *= x;
	Row[2].X *= x;
	Row[0].Y *= y;
	Row[1].Y *= y;
	Row[2].Y *= y;
	Row[0].Z *= z;
	Row[1].Z *= z;
	Row[2].Z *= z;
}

// FUNCTION: GAMEMD 0x005AEAD0
void Matrix3D::Scale_X(float scale)
{
	Row[0].X *= scale;
	Row[1].X *= scale;
	Row[2].X *= scale;
}

// FUNCTION: GAMEMD 0x005AEAF0
void Matrix3D::Scale_Y(float scale)
{
	Row[0].Y *= scale;
	Row[1].Y *= scale;
	Row[2].Y *= scale;
}

// FUNCTION: GAMEMD 0x005AEB20
void Matrix3D::Scale_Z(float scale)
{
	Row[0].Z *= scale;
	Row[1].Z *= scale;
	Row[2].Z *= scale;
}

// FUNCTION: GAMEMD 0x005AEB50
void Matrix3D::Shear_YZ(float y, float z)
{
	Row[0].X += y * Row[0].Y + z * Row[0].Z;
	Row[1].X += y * Row[1].Y + z * Row[1].Z;
	Row[2].X += y * Row[2].Y + z * Row[2].Z;
}

// FUNCTION: GAMEMD 0x005AEBA0
void Matrix3D::Shear_XY(float x, float y)
{
	Row[0].Z += x * Row[0].X + y * Row[0].Y;
	Row[1].Z += x * Row[1].X + y * Row[1].Y;
	Row[2].Z += x * Row[2].X + y * Row[2].Y;
}

// FUNCTION: GAMEMD 0x005AEBF0
void Matrix3D::Shear_XZ(float x, float z)
{
	Row[0].Y += x * Row[0].X + z * Row[0].Z;
	Row[1].Y += x * Row[1].X + z * Row[1].Z;
	Row[2].Y += x * Row[2].X + z * Row[2].Z;
}

// FUNCTION: GAMEMD 0x005AEC40
void Matrix3D::Pre_Rotate_X(float theta)
{
	float s = fastmath::sin(theta);
	float c = fastmath::cos(theta);
	float ns = -s;

	float m10 = Row[1].X;
	float m11 = Row[1].Y;
	float m12 = Row[1].Z;
	float m13 = Row[1].W;

	float m20 = Row[2].X;
	float m21 = Row[2].Y;
	float m22 = Row[2].Z;
	float m23 = Row[2].W;

	Row[1].X = m20 * ns + m10 * c;
	Row[1].Y = m21 * ns + m11 * c;
	Row[1].Z = m22 * ns + m12 * c;
	Row[1].W = m23 * ns + m13 * c;

	Row[2].X = m20 * c + m10 * s;
	Row[2].Y = m21 * c + m11 * s;
	Row[2].Z = m22 * c + m12 * s;
	Row[2].W = m23 * c + m13 * s;
}

// FUNCTION: GAMEMD 0x005AEE50
void Matrix3D::Pre_Rotate_Z(float theta)
{
	float s = fastmath::sin(theta);
	float c = fastmath::cos(theta);
	float ns = -s;

	float m00 = Row[0].X;
	float m01 = Row[0].Y;
	float m02 = Row[0].Z;
	float m03 = Row[0].W;

	float m10 = Row[1].X;
	float m11 = Row[1].Y;
	float m12 = Row[1].Z;
	float m13 = Row[1].W;

	Row[0].X = m10 * ns + m00 * c;
	Row[0].Y = m11 * ns + m01 * c;
	Row[0].Z = m12 * ns + m02 * c;
	Row[0].W = m13 * ns + m03 * c;

	Row[1].X = m10 * c + m00 * s;
	Row[1].Y = m11 * c + m01 * s;
	Row[1].Z = m12 * c + m02 * s;
	Row[1].W = m13 * c + m03 * s;
}

// FUNCTION: GAMEMD 0x005AEF60
void Matrix3D::Rotate_X(float theta)
{
	float tmp1,tmp2;
	float s,c;

	s = (float)fastmath::sin(theta);
	c = (float)fastmath::cos(theta);

	tmp1 = Row[0].Y; tmp2 = Row[0].Z;
	Row[0].Y = (float)( c*tmp1 + s*tmp2);
	Row[0].Z = (float)(-s*tmp1 + c*tmp2);

	tmp1 = Row[1].Y; tmp2 = Row[1].Z;
	Row[1].Y = (float)( c*tmp1 + s*tmp2);
	Row[1].Z = (float)(-s*tmp1 + c*tmp2);

	tmp1 = Row[2].Y; tmp2 = Row[2].Z;
	Row[2].Y = (float)( c*tmp1 + s*tmp2);
	Row[2].Z = (float)(-s*tmp1 + c*tmp2);
}

// FUNCTION: GAMEMD 0x005AF000
void Matrix3D::Rotate_X(float s,float c)
{
	float tmp1,tmp2;

	tmp1 = Row[0].Y; tmp2 = Row[0].Z;
	Row[0].Y = (float)( c*tmp1 + s*tmp2);
	Row[0].Z = (float)(-s*tmp1 + c*tmp2);

	tmp1 = Row[1].Y; tmp2 = Row[1].Z;
	Row[1].Y = (float)( c*tmp1 + s*tmp2);
	Row[1].Z = (float)(-s*tmp1 + c*tmp2);

	tmp1 = Row[2].Y; tmp2 = Row[2].Z;
	Row[2].Y = (float)( c*tmp1 + s*tmp2);
	Row[2].Z = (float)(-s*tmp1 + c*tmp2);
}

// FUNCTION: GAMEMD 0x005AF080
void Matrix3D::Rotate_Y(float theta)
{
	float tmp1,tmp2;
	float s,c;

	s = (float)fastmath::sin(theta);
	c = (float)fastmath::cos(theta);

	tmp1 = Row[0].X; tmp2 = Row[0].Z;
	Row[0].X = (float)(c*tmp1 - s*tmp2);
	Row[0].Z = (float)(s*tmp1 + c*tmp2);

	tmp1 = Row[1].X; tmp2 = Row[1].Z;
	Row[1].X = (float)(c*tmp1 - s*tmp2);
	Row[1].Z = (float)(s*tmp1 + c*tmp2);

	tmp1 = Row[2].X; tmp2 = Row[2].Z;
	Row[2].X = (float)(c*tmp1 - s*tmp2);
	Row[2].Z = (float)(s*tmp1 + c*tmp2);
}

// FUNCTION: GAMEMD 0x005AF120
void Matrix3D::Rotate_Y(float s,float c)
{
	float tmp1,tmp2;

	tmp1 = Row[0].X; tmp2 = Row[0].Z;
	Row[0].X = (float)(c*tmp1 - s*tmp2);
	Row[0].Z = (float)(s*tmp1 + c*tmp2);

	tmp1 = Row[1].X; tmp2 = Row[1].Z;
	Row[1].X = (float)(c*tmp1 - s*tmp2);
	Row[1].Z = (float)(s*tmp1 + c*tmp2);

	tmp1 = Row[2].X; tmp2 = Row[2].Z;
	Row[2].X = (float)(c*tmp1 - s*tmp2);
	Row[2].Z = (float)(s*tmp1 + c*tmp2);
}

// FUNCTION: GAMEMD 0x005AF1A0
void Matrix3D::Rotate_Z(float theta)
{
	float tmp1,tmp2;
	float s, c;

	c = (float)fastmath::cos(theta);
	s = (float)fastmath::sin(theta);

	tmp1 = Row[0].X; tmp2 = Row[0].Y;
	Row[0].X = (float)( c*tmp1 + s*tmp2);
	Row[0].Y = (float)(-s*tmp1 + c*tmp2);

	tmp1 = Row[1].X; tmp2 = Row[1].Y;
	Row[1].X = (float)( c*tmp1 + s*tmp2);
	Row[1].Y = (float)(-s*tmp1 + c*tmp2);

	tmp1 = Row[2].X; tmp2 = Row[2].Y;
	Row[2].X = (float)( c*tmp1 + s*tmp2);
	Row[2].Y = (float)(-s*tmp1 + c*tmp2);
}

// FUNCTION: GAMEMD 0x005AF240
void Matrix3D::Rotate_Z(float s,float c)
{
	float tmp1,tmp2;

	tmp1 = Row[0].X; tmp2 = Row[0].Y;
	Row[0].X = (float)( c*tmp1 + s*tmp2);
	Row[0].Y = (float)(-s*tmp1 + c*tmp2);

	tmp1 = Row[1].X; tmp2 = Row[1].Y;
	Row[1].X = (float)( c*tmp1 + s*tmp2);
	Row[1].Y = (float)(-s*tmp1 + c*tmp2);

	tmp1 = Row[2].X; tmp2 = Row[2].Y;
	Row[2].X = (float)( c*tmp1 + s*tmp2);
	Row[2].Y = (float)(-s*tmp1 + c*tmp2);
}

// FUNCTION: GAMEMD 0x005AF2C0
float Matrix3D::Get_X_Val(void)
{
	Vector3 v = (*this) * Vector3(0, 0, 0);
	return(v.X);
}

// FUNCTION: GAMEMD 0x005AF310
float Matrix3D::Get_Y_Val(void)
{
	Vector3 v = (*this) * Vector3(0, 0, 0);
	return(v.Y);
}

// FUNCTION: GAMEMD 0x005AF360
float Matrix3D::Get_Z_Val(void)
{
	Vector3 v = (*this) * Vector3(0, 0, 0);
	return(v.Z);
}

// FUNCTION: GAMEMD 0x005AF3B0
float Matrix3D::Get_X_Rotation(void)
{
	Vector3 v = (*this) * Vector3(0, 1, 0);
	return(fastmath::atan2(v.Z, v.Y));
}

// FUNCTION: GAMEMD 0x005AF410
float Matrix3D::Get_Y_Rotation(void)
{
	Vector3 v = (*this) * Vector3(0, 0, 1);
	return(fastmath::atan2(v.X, v.Z));
}

// FUNCTION: GAMEMD 0x005AF470
float Matrix3D::Get_Z_Rotation(void)
{
	Vector3 v = (*this) * Vector3(1, 0, 0);
	return(fastmath::atan2(v.Y, v.X));
}

// FUNCTION: GAMEMD 0x005AF4D0
Vector3 Matrix3D::Rotate_Vector(Vector3 const & vect)
{
	return(Vector3(
		(Row[0].X * vect.X + Row[0].Y * vect.Y + Row[0].Z * vect.Z),
		(Row[1].X * vect.X + Row[1].Y * vect.Y + Row[1].Z * vect.Z),
		(Row[2].X * vect.X + Row[2].Y * vect.Y + Row[2].Z * vect.Z))
	);
}

// FUNCTION: GAMEMD 0x005AF710
void Matrix3D::Obj_Look_At(const Vector3 &p, const Vector3 &t, float roll)
{
	float sinp,cosp;
	float siny,cosy;
	float sinr,cosr;
	float yaw;

	Vector3 s;
	Vector3 r;
	Vector3 d = (t - p);

	if (d.Y == 0.0f && d.X == 0.0f) {
		yaw = 0.0;
	}
	else {
		yaw = fastmath::atan2(d.Y, d.X);
	}

	float len = fastmath::sqrt(d[1] * d[1] + d[0] * d[0]);

	float pitch = fastmath::atan2(-d.Z, len);

	cosy = fastmath::cos(yaw);
	siny = fastmath::sin(yaw);

	cosp = fastmath::cos(pitch);
	sinp = fastmath::sin(pitch);

	cosr = fastmath::cos(roll);
	sinr = fastmath::sin(roll);

	s.X = cosr * sinp * cosy + sinr * siny;
	s.Y = cosr * sinp * siny - sinr * cosy;
	s.Z = cosr * cosp;

	d = Normalize(d);

	Vector3::Cross_Product(d, s, &r);

	Row[0].X = r.X;
	Row[0].Y = s.X;
	Row[0].Z = d.X;
	Row[0].W = p.X;

	Row[1].X = r.Y;
	Row[1].Y = s.Y;
	Row[1].Z = d.Y;
	Row[1].W = p.Y;

	Row[2].X = r.Z;
	Row[2].Y = s.Z;
	Row[2].Z = d.Z;
	Row[2].W = p.Z;
}

// FUNCTION: GAMEMD 0x005AF980
Matrix3D operator * (const Matrix3D & a, const Matrix3D & b)
{
	Matrix3D c;
	Matrix3D::Multiply(a, b, &c);
	return(c);
}

// FUNCTION: GAMEMD 0x005AFC00
Matrix3D & Matrix3D::Matrix3DStruct_To_Matrix3D(Matrix3D & d, const Matrix3DStruct &s)
{
	memcpy(&d, &s, sizeof(Matrix3D));
	return d;
}

// FUNCTION: GAMEMD 0x005AFC20
Matrix3D Matrix3D::Orthogonal_Inverse(Matrix3D const & src)
{
	Matrix3D inv;

	inv.Row[0].X = src.Row[0].X;
	inv.Row[0].Y = src.Row[1].X;
	inv.Row[0].Z = src.Row[2].X;

	inv.Row[1].X = src.Row[0].Y;
	inv.Row[1].Y = src.Row[1].Y;
	inv.Row[1].Z = src.Row[2].Y;

	inv.Row[2].X = src.Row[0].Z;
	inv.Row[2].Y = src.Row[1].Z;
	inv.Row[2].Z = src.Row[2].Z;

	inv.Row[0].W = -(inv.Row[0].X * src.Row[0].W + inv.Row[0].Z * src.Row[2].W + inv.Row[0].Y * src.Row[1].W);
	inv.Row[1].W = -(inv.Row[1].X * src.Row[0].W + inv.Row[1].Z * src.Row[2].W + inv.Row[1].Y * src.Row[1].W);
	inv.Row[2].W = -(inv.Row[2].X * src.Row[0].W + inv.Row[2].Z * src.Row[2].W + inv.Row[2].Y * src.Row[1].W);

	return(inv);
}

extern const double DAT_007EDB58;
extern const double DAT_007EDB60;
extern const double DAT_007E1708;
extern const double DAT_007E1710;
extern const double DAT_007E1718;
extern const double DAT_007E1720;
extern const double DAT_007E1728;
extern const double DAT_007E1730;
extern const double DAT_007E1738;
extern const double DAT_007E1740;

// SYNTHETIC: GAMEMD 0x005AFCE0 SYMBOL
// _$E2
double DAT_00ABEDF8 = fastmath::sqrt(pow(DAT_007E1710, DAT_007E1708) * 2);

// SYNTHETIC: GAMEMD 0x005AFD10 SYMBOL
// _$E5
double DAT_00ABEE48 = DAT_00ABEDF8 / DAT_007EDB58;

// SYNTHETIC: GAMEMD 0x005AFD30 SYMBOL
// _$E8
double DAT_00ABEE30 = DAT_007E1718 / DAT_00ABEE48;

// SYNTHETIC: GAMEMD 0x005AFD50 SYMBOL
// _$E11
double DAT_00ABEE28 = DAT_007E1720 * DAT_007EDB60;

// SYNTHETIC: GAMEMD 0x005AFD70 SYMBOL
// _$E14
double DAT_00ABEE00 = DAT_007E1728 * DAT_007EDB60;

// SYNTHETIC: GAMEMD 0x005AFD90 SYMBOL
// _$E17
double DAT_00ABEDD8 = DAT_007E1730 * DAT_007EDB60;

// SYNTHETIC: GAMEMD 0x005AFDB0 SYMBOL
// _$E20
int DAT_00ABEE50 = (int)(fastmath::tan(DAT_00ABEDD8 - DAT_00ABEE00) * DAT_007E1738 * DAT_00ABEDF8);

// SYNTHETIC: GAMEMD 0x005AFDF0 SYMBOL
// _$E23
double DAT_00ABEE08 = fastmath::atan(DAT_00ABEE50 * DAT_007E1740);

// SYNTHETIC: GAMEMD 0x005AFE20 SYMBOL
// _$E26
double DAT_00ABEE20 = fastmath::atan((DAT_00ABEE50 * 2) / DAT_00ABEDF8);

// SYNTHETIC: GAMEMD 0x005AFE50 SYMBOL
// _$E29
int DAT_00ABEE44 = (int)(DAT_00ABEE50 * 4 + DAT_007E1738);

struct ShortPair {
	unsigned short A, B;
	ShortPair() { A = 0; B = 0; }
};

struct IntTriple {
	int A, B, C;
	IntTriple() { A = 0; B = 0; C = 0; }
};

struct IntPairZ {
	int A, B, C;
	IntPairZ() { C = 0; A = 128; B = 128; }
};

struct IntQuad {
	int A, B, C, D;
	IntQuad() { A = 0; B = 0; C = 0; D = 0; }
};

// SYNTHETIC: GAMEMD 0x005AFE80 SYMBOL
// _$E32
ShortPair DAT_00ABEDF0;

// SYNTHETIC: GAMEMD 0x005AFE90 SYMBOL
// _$E35
IntTriple DAT_00ABEE10;

// SYNTHETIC: GAMEMD 0x005AFEB0 SYMBOL
// _$E38
IntPairZ DAT_00ABEE38;

// SYNTHETIC: GAMEMD 0x005AFED0 SYMBOL
// _$E41
IntQuad DAT_00ABEDE0;
