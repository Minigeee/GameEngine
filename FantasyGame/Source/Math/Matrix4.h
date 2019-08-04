#ifndef MATRIX4_H
#define MATRIX4_H

#include <Math/Vector4.h>
#include <Math/Matrix3.h>

///////////////////////////////////////////////////////////////////////////////

#define MATRIX4_SELF_OP(op) \
Matrix4<T>& CONCAT(operator, op)(const Matrix4<T>& val) \
{ x op val.x; y op val.y; z op val.z; w op val.w; return *this; } \
Matrix4<T>& CONCAT(operator, op)(T val) \
{ x op val; y op val; z op val; w op val; return *this; }

#define MATRIX4_COMPARE(op) \
Matrix4<bool> CONCAT(operator, op)(const Matrix4<T>& val) \
{ return Matrix4<bool>(x op val.x, y op val.y, z op val.z, w op val.w); } \
Matrix4<bool> CONCAT(operator, op)(T val) \
{ return Matrix4<bool>(x op val, y op val, z op val, w op val); }

#define MATRIX4_OP(op) \
template <typename T> inline Matrix4<T>& CONCAT(operator, op)(const Matrix4<T>& l, const Matrix4<T>& r) \
{ return Matrix4<T>(l.x op r.x, l.y op r.y, l.z op r.z, l.w op r.w); } \
template <typename T> inline Matrix4<T>& CONCAT(operator, op)(const Matrix4<T>& l, T r) \
{ return Matrix4<T>(l.x op r, l.y op r, l.z op r, l.w op r); } \
template <typename T> inline Matrix4<T>& CONCAT(operator, op)(T l, const Matrix4<T>& r) \
{ return Matrix4<T>(l op r.x, l op r.y, l op r.z, l op r.w); }

///////////////////////////////////////////////////////////////////////////////

template <typename T> struct Matrix4;
template <typename T> Matrix4<T> operator*(const Matrix4<T>&, const Matrix4<T>&);

/* Column-major 4x4 matrix */
template <typename T>
struct Matrix4
{
	Matrix4() = default;

	explicit Matrix4(T val) :
		x(val, 0, 0, 0),
		y(0, val, 0, 0),
		z(0, 0, val, 0),
		w(0, 0, 0, val)
	{ }

	explicit Matrix4(const Vector4<T>& x, const Vector4<T>& y, const Vector4<T>& z, const Vector4<T>& w) :
		x(x),
		y(y),
		z(z),
		w(w)
	{ }

	explicit Matrix4(T xx, T xy, T xz, T xw, T yx, T yy, T yz, T yw, T zx, T zy, T zz, T zw, T wx, T wy, T wz, T ww) :
		x(xx, xy, xz, xw),
		y(yx, yy, yz, yw),
		z(zx, zy, zz, zw),
		w(wx, wy, wz, ww)
	{ }

	explicit Matrix4(const Matrix2<T>& m, T z, T w) :
		x(m.x, 0, 0),
		y(m.y, 0, 0),
		z(0, 0, z, 0),
		w(0, 0, 0, w)
	{ }

	explicit Matrix4(const Matrix3<T>& m, T w) :
		x(m.x, 0),
		y(m.y, 0),
		z(m.z, 0),
		w(0, 0, 0, w)
	{ }

	MATRIX4_SELF_OP(+=);
	MATRIX4_SELF_OP(-=);
	MATRIX4_SELF_OP(/=);
	MATRIX4_SELF_OP(%=);

	/* Matrix scalar multiplication */
	Matrix4<T>& operator*=(T val)
	{
		x *= val; y *= val; z *= val; w *= val; return *this;
	}

	/* Matrix elementwise multiplication */
	Matrix4<T> Mul(const Matrix4<T>& val)
	{
		return Matrix4<T>(x * val.x, y * val.y, z * val.z, w * val.w);
	}

	/* Matrix-Matrix multiplication */
	Matrix4<T>& operator*=(const Matrix4<T>& val)
	{
		*this = *this * val; return *this;
	}


	MATRIX4_COMPARE(==);
	MATRIX4_COMPARE(!=);
	MATRIX4_COMPARE(>=);
	MATRIX4_COMPARE(<=);
	MATRIX4_COMPARE(>);
	MATRIX4_COMPARE(<);

	Vector4<T> x, y, z, w;
};

///////////////////////////////////////////////////////////////////////////////

MATRIX4_OP(+);
MATRIX4_OP(-);
MATRIX4_OP(/);
MATRIX4_OP(%);

template <typename T>
inline Matrix4<T> operator-(const Matrix4<T>& m)
{
	return Matrix4<T>(-m.x, -m.y, -m.z, -m.w);
}

template <typename T>
inline Matrix4<T> operator*(const Matrix4<T> & l, T r)
{
	return Matrix4<T>(l.x * r, l.y * r, l.z * r, l.w * r);
}

template <typename T>
inline Matrix4<T> operator*(T l, const Matrix4<T> & r)
{
	return Matrix4<T>(l * r.x, l * r.y, l * r.z, l * r.w);
}

/* Elementwise multiplication */
template <typename T>
inline Matrix4<T> Mul(const Matrix4<T> & l, const Matrix4<T> & r)
{
	return Matrix4<T>(l.x * r.x, l.y * r.y, l.z * r.z, l.w * r.w);
}

/* Matrix-Vector multiplication */
template <typename T>
inline Vector4<T> operator*(const Matrix4<T> & l, const Vector4<T> & r)
{
	return l.x* r.x + l.y * r.y + l.z * r.z + l.w * r.w;
}

/* Matrix-Matrix multiplication */
template <typename T>
inline Matrix4<T> operator*(const Matrix4<T> & l, const Matrix4<T> & r)
{
	return Matrix4<T>(
		l.x * r.x.x + l.y * r.x.y + l.z * r.x.z + l.w * r.x.w,
		l.x * r.y.x + l.y * r.y.y + l.z * r.y.z + l.w * r.y.w,
		l.x * r.z.x + l.y * r.z.y + l.z * r.z.z + l.w * r.z.w,
		l.x * r.w.x + l.y * r.w.y + l.z * r.w.z + l.w * r.w.w
		);
}

///////////////////////////////////////////////////////////////////////////////

/* Sum of matrix elements */
template <typename T>
inline T Sum(const Matrix4<T>& x)
{
	return Sum(x.x) + Sum(x.y) + Sum(x.z) + Sum(x.w);
}

///////////////////////////////////////////////////////////////////////////////

typedef Matrix4<float>	Matrix4f;
typedef Matrix4<double>	Matrix4d;
typedef Matrix4<Int32>	Matrix4i;
typedef Matrix4<Uint32>	Matrix4u;
typedef Matrix4<bool>	Matrix4b;

///////////////////////////////////////////////////////////////////////////////

#endif