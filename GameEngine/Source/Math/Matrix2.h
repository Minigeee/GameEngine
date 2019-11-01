#ifndef MATRIX2_H
#define MATRIX2_H

#include <Math/Vector2.h>

///////////////////////////////////////////////////////////////////////////////

#define MATRIX2_SELF_OP(op) \
Matrix2<T>& CONCAT(operator, op)(const Matrix2<T>& val) \
{ x op val.x; y op val.y; return *this; } \
Matrix2<T>& CONCAT(operator, op)(T val) \
{ x op val; y op val; return *this; }

#define MATRIX2_COMPARE(op) \
Matrix2<bool> CONCAT(operator, op)(const Matrix2<T>& val) \
{ return Matrix2<bool>(x op val.x, y op val.y); } \
Matrix2<bool> CONCAT(operator, op)(T val) \
{ return Matrix2<bool>(x op val, y op val); }

#define MATRIX2_OP(op) \
template <typename T> inline Matrix2<T>& CONCAT(operator, op)(const Matrix2<T>& l, const Matrix2<T>& r) \
{ return Matrix2<T>(l.x op r.x, l.y op r.y); } \
template <typename T> inline Matrix2<T>& CONCAT(operator, op)(const Matrix2<T>& l, T r) \
{ return Matrix2<T>(l.x op r, l.y op r); } \
template <typename T> inline Matrix2<T>& CONCAT(operator, op)(T l, const Matrix2<T>& r) \
{ return Matrix2<T>(l op r.x, l op r.y); }

///////////////////////////////////////////////////////////////////////////////

template <typename T> struct Matrix2;
template <typename T> Matrix2<T> operator*(const Matrix2<T>&, const Matrix2<T>&);

/* Column-major 2x2 matrix */
template <typename T>
struct Matrix2
{
	Matrix2() = default;

	explicit Matrix2(T val) :
		x(val, 0),
		y(0, val)
	{ }

	explicit Matrix2(const Vector2<T>& x, const Vector2<T>& y) :
		x(x),
		y(y)
	{ }

	explicit Matrix2(T xx, T xy, T yx, T yy) :
		x(xx, xy),
		y(yx, yy)
	{ }

	MATRIX2_SELF_OP(+=);
	MATRIX2_SELF_OP(-=);
	MATRIX2_SELF_OP(/=);
	MATRIX2_SELF_OP(%=);

	/* Matrix scalar multiplication */
	Matrix2<T>& operator*=(T val)
	{
		x *= val; y *= val; return *this;
	}

	/* Matrix elementwise multiplication */
	Matrix2<T> Mul(const Matrix2<T>& val)
	{
		return Matrix2<T>(x * val.x, y * val.y);
	}

	/* Matrix-Matrix multiplication */
	Matrix2<T>& operator*=(const Matrix2<T>& val)
	{
		*this = *this * val; return *this;
	}


	MATRIX2_COMPARE(==);
	MATRIX2_COMPARE(!=);
	MATRIX2_COMPARE(>=);
	MATRIX2_COMPARE(<=);
	MATRIX2_COMPARE(>);
	MATRIX2_COMPARE(<);

	Vector2<T> x, y;
};

///////////////////////////////////////////////////////////////////////////////

MATRIX2_OP(+);
MATRIX2_OP(-);
MATRIX2_OP(/);
MATRIX2_OP(*);

template <typename T>
inline Matrix2<T> operator-(const Matrix2<T>& m)
{
	return Matrix2<T>(-m.x, -m.y);
}

template <typename T>
inline Matrix2<T> operator*(const Matrix2<T>& l, T r)
{
	return Matrix2<T>(l.x * r, l.y * r);
}

template <typename T>
inline Matrix2<T> operator*(T l, const Matrix2<T>& r)
{
	return Matrix2<T>(l * r.x, l * r.y);
}

/* Elementwise multiplication */
template <typename T>
inline Matrix2<T> Mul(const Matrix2<T>& l, const Matrix2<T>& r)
{
	return Matrix2<T>(l.x * r.x, l.y * r.y);
}

/* Matrix-Vector multiplication */
template <typename T>
inline Vector2<T> operator*(const Matrix2<T> & l, const Vector2<T> & r)
{
	return l.x * r.x + l.y * r.y;
}

/* Matrix-Matrix multiplication */
template <typename T>
inline Matrix2<T> operator*(const Matrix2<T>& l, const Matrix2<T>& r)
{
	return Matrix2<T>(
		l.x * r.x.x + l.y * r.x.y,
		l.x * r.y.x + l.y * r.y.y
		);
}

///////////////////////////////////////////////////////////////////////////////

/* Sum of matrix elements */
template <typename T>
inline T Sum(const Matrix2<T>& x)
{
	return Sum(x.x) + Sum(x.y);
}

///////////////////////////////////////////////////////////////////////////////

typedef Matrix2<float>	Matrix2f;
typedef Matrix2<double>	Matrix2d;
typedef Matrix2<Int32>	Matrix2i;
typedef Matrix2<Uint32>	Matrix2u;
typedef Matrix2<bool>	Matrix2b;

///////////////////////////////////////////////////////////////////////////////

#endif