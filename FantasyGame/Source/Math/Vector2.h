#ifndef VECTOR2_H
#define VECTOR2_H

#include <Core/DataTypes.h>
#include <Core/Macros.h>

#include <cmath>

///////////////////////////////////////////////////////////////////////////////

#define VECTOR2_SELF_OP(op) \
Vector2<T>& CONCAT(operator, op)(const Vector2<T>& val) \
{ x op val.x; y op val.y; return *this; } \
Vector2<T>& CONCAT(operator, op)(T val) \
{ x op val; y op val; return *this; }

#define VECTOR2_OP(op) \
template <typename T> inline Vector2<T> CONCAT(operator, op)(const Vector2<T>& l, const Vector2<T>& r) \
{ return Vector2<T>(l.x op r.x, l.y op r.y); } \
template <typename T> inline Vector2<T> CONCAT(operator, op)(const Vector2<T>& l, T r) \
{ return Vector2<T>(l.x op r, l.y op r); } \
template <typename T> inline Vector2<T> CONCAT(operator, op)(T l, const Vector2<T>& r) \
{ return Vector2<T>(l op r.x, l op r.y); }

#define VECTOR2_COMPARE(op) \
Vector2<bool> CONCAT(operator, op)(const Vector2<T>& a) \
{ return Vector2<bool>(x op a.x, y op a.y); } \
Vector2<bool> CONCAT(operator, op)(T a) \
{ return Vector2<bool>(x op a, y op a); }

///////////////////////////////////////////////////////////////////////////////

/* 2D vector */
template <typename T>
struct Vector2
{
public:
	Vector2() = default;

	explicit Vector2(T val) :
		x(val),
		y(val)
	{ }

	explicit Vector2(T x, T y) :
		x(x),
		y(y)
	{ }

	template <typename U>
	operator Vector2<U>() const
	{
		return Vector2<U>((U)x, (U)y);
	}

	VECTOR2_SELF_OP(+=);
	VECTOR2_SELF_OP(-=);
	VECTOR2_SELF_OP(*=);
	VECTOR2_SELF_OP(/=);
	VECTOR2_SELF_OP(%=);

	VECTOR2_COMPARE(==);
	VECTOR2_COMPARE(!=);
	VECTOR2_COMPARE(>=);
	VECTOR2_COMPARE(<=);
	VECTOR2_COMPARE(>);
	VECTOR2_COMPARE(<);

	T x, y;
};

///////////////////////////////////////////////////////////////////////////////

VECTOR2_OP(+);
VECTOR2_OP(-);
VECTOR2_OP(*);
VECTOR2_OP(/);
VECTOR2_OP(%);

template <typename T>
inline Vector2<T> operator-(const Vector2<T>& x)
{
	return Vector2<T>(-x.x, -x.y);
}

///////////////////////////////////////////////////////////////////////////////

/* Get length of vector from origin */
template <typename T>
inline T Length(const Vector2<T>& x)
{
	return (T)sqrt(x.x * x.x + x.y * x.y);
}

/* Get distance between 2 vectors */
template <typename T>
inline T Distance(const Vector2<T>& a, const Vector2<T>& b)
{
	return Length(a - b);
}

/* Get squared distance between 2 vectors */
template <typename T>
inline T DistanceSquared(const Vector2<T>& a, const Vector2<T>& b)
{
	Vector2<T> d = a - b;
	return d.x * d.x + d.y * d.y;
}

/* Get normalized (unit) vector */
template <typename T>
inline Vector2<T> Normalize(const Vector2<T>& x)
{
	T d = Length(x);
	return x / d;
}

/* Get reflection of incident vector w.r.t. normal vector */
template <typename T>
inline Vector2<T> Reflect(const Vector2<T>& incident, const Vector2<T>& normal)
{
	const Vector2<T>& d = incident;
	Vector2<T> n = Normalize(normal);
	return d - 2 * Dot(d, n) * n;
}

/* Sum of vector elements */
template <typename T>
inline T Sum(const Vector2<T>& x)
{
	return x.x + x.y;
}

/* Dot product of 2 vectors */
template <typename T>
inline T Dot(const Vector2<T>& a, const Vector2<T>& b)
{
	return Sum(a * b);
}

///////////////////////////////////////////////////////////////////////////////

typedef Vector2<float>	Vector2f;
typedef Vector2<double>	Vector2d;
typedef Vector2<Int32>	Vector2i;
typedef Vector2<Uint32>	Vector2u;
typedef Vector2<bool>	Vector2b;

///////////////////////////////////////////////////////////////////////////////

#endif