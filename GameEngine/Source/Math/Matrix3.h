#ifndef MATRIX3_H
#define MATRIX3_H

#include <Math/Vector3.h>
#include <Math/Matrix2.h>

///////////////////////////////////////////////////////////////////////////////

#define MATRIX3_SELF_OP(op) \
Matrix3<T>& CONCAT(operator, op)(const Matrix3<T>& val) \
{ x op val.x; y op val.y; z op val.z; return *this; } \
Matrix3<T>& CONCAT(operator, op)(T val) \
{ x op val; y op val; z op val; return *this; }

#define MATRIX3_COMPARE(op) \
Matrix3<bool> CONCAT(operator, op)(const Matrix3<T>& val) \
{ return Matrix3<bool>(x op val.x, y op val.y, z op val.z); } \
Matrix3<bool> CONCAT(operator, op)(T val) \
{ return Matrix3<bool>(x op val, y op val, z op val); }

#define MATRIX3_OP(op) \
template <typename T> inline Matrix3<T>& CONCAT(operator, op)(const Matrix3<T>& l, const Matrix3<T>& r) \
{ return Matrix3<T>(l.x op r.x, l.y op r.y, l.z op r.z); } \
template <typename T> inline Matrix3<T>& CONCAT(operator, op)(const Matrix3<T>& l, T r) \
{ return Matrix3<T>(l.x op r, l.y op r, l.z op r); } \
template <typename T> inline Matrix3<T>& CONCAT(operator, op)(T l, const Matrix3<T>& r) \
{ return Matrix3<T>(l op r.x, l op r.y, l op r.z); }

///////////////////////////////////////////////////////////////////////////////

template <typename T> struct Matrix3;
template <typename T> Matrix3<T> operator*(const Matrix3<T>&, const Matrix3<T>&);

/* Column-major 3x3 matrix */
template <typename T>
struct Matrix3
{
	Matrix3() = default;

	explicit Matrix3(T val) :
		x(val, 0, 0),
		y(0, val, 0),
		x(0, 0, val)
	{ }

	explicit Matrix3(const Vector3<T>& x, const Vector3<T>& y, const Vector3<T>& z) :
		x(x),
		y(y),
		z(z)
	{ }

	explicit Matrix3(T xx, T xy, T xz, T yx, T yy, T yz, T zx, T zy, T zz) :
		x(xx, xy, xz),
		y(yx, yy, yz),
		z(zx, zy, zz)
	{ }

	explicit Matrix3(const Matrix2<T>& m, T z) :
		x(m.x, 0),
		y(m.y, 0),
		z(0, 0, z)
	{ }

	MATRIX3_SELF_OP(+=);
	MATRIX3_SELF_OP(-=);
	MATRIX3_SELF_OP(/=);
	MATRIX3_SELF_OP(%=);

	/* Matrix scalar multiplication */
	Matrix3<T>& operator*=(T val)
	{
		x *= val; y *= val; z *= val; return *this;
	}

	/* Matrix elementwise multiplication */
	Matrix3<T> Mul(const Matrix3<T>& val)
	{
		return Matrix3<T>(x * val.x, y * val.y, z * val.z);
	}

	/* Matrix-Matrix multiplication */
	Matrix3<T>& operator*=(const Matrix3<T>& val)
	{
		*this = *this * val; return *this;
	}


	MATRIX3_COMPARE(==);
	MATRIX3_COMPARE(!=);
	MATRIX3_COMPARE(>=);
	MATRIX3_COMPARE(<=);
	MATRIX3_COMPARE(>);
	MATRIX3_COMPARE(<);

	Vector3<T> x, y, z;
};

///////////////////////////////////////////////////////////////////////////////

MATRIX3_OP(+);
MATRIX3_OP(-);
MATRIX3_OP(/);
MATRIX3_OP(%);

template <typename T>
inline Matrix3<T> operator-(const Matrix3<T>& m)
{
	return Matrix3<T>(-m.x, -m.y, -m.z);
}

template <typename T>
inline Matrix3<T> operator*(const Matrix3<T>& l, T r)
{
	return Matrix3<T>(l.x * r, l.y * r, l.z * r);
}

template <typename T>
inline Matrix3<T> operator*(T l, const Matrix3<T>& r)
{
	return Matrix3<T>(l * r.x, l * r.y, l * r.z);
}

/* Elementwise multiplication */
template <typename T>
inline Matrix3<T> Mul(const Matrix3<T>& l, const Matrix3<T>& r)
{
	return Matrix3<T>(l.x * r.x, l.y * r.y, l.z * r.z);
}

/* Matrix-Vector multiplication */
template <typename T>
inline Vector3<T> operator*(const Matrix3<T>& l, const Vector3<T>& r)
{
	return l.x* r.x + l.y * r.y + l.z * r.z;
}

/* Matrix-Matrix multiplication */
template <typename T>
inline Matrix3<T> operator*(const Matrix3<T>& l, const Matrix3<T>& r)
{
	return Matrix3<T>(
		l.x * r.x.x + l.y * r.x.y + l.z * r.x.z,
		l.x * r.y.x + l.y * r.y.y + l.z * r.y.z,
		l.x * r.z.x + l.y * r.z.y + l.z * r.z.z
		);
}

///////////////////////////////////////////////////////////////////////////////

/* Sum of matrix elements */
template <typename T>
inline T Sum(const Matrix3<T>& x)
{
	return Sum(x.x) + Sum(x.y) + Sum(x.z);
}

///////////////////////////////////////////////////////////////////////////////

typedef Matrix3<float>	Matrix3f;
typedef Matrix3<double>	Matrix3d;
typedef Matrix3<Int32>	Matrix3i;
typedef Matrix3<Uint32>	Matrix3u;
typedef Matrix3<bool>	Matrix3b;

///////////////////////////////////////////////////////////////////////////////

#endif