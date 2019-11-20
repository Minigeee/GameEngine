#ifndef VECTOR3_H
#define VECTOR3_H

#include <Math/Vector2.h>

///////////////////////////////////////////////////////////////////////////////

#define VECTOR3_SELF_OP(op) \
Vector3<T>& CONCAT(operator, op)(const Vector3<T>& val) \
{ x op val.x; y op val.y; z op val.z; return *this; } \
Vector3<T>& CONCAT(operator, op)(T val) \
{ x op val; y op val; z op val; return *this; }

#define VECTOR3_OP(op) \
template <typename T> inline Vector3<T> CONCAT(operator, op)(const Vector3<T>& l, const Vector3<T>& r) \
{ return Vector3<T>(l.x op r.x, l.y op r.y, l.z op r.z); } \
template <typename T> inline Vector3<T> CONCAT(operator, op)(const Vector3<T>& l, T r) \
{ return Vector3<T>(l.x op r, l.y op r, l.z op r); } \
template <typename T> inline Vector3<T> CONCAT(operator, op)(T l, const Vector3<T>& r) \
{ return Vector3<T>(l op r.x, l op r.y, l op r.z); }

#define VECTOR3_COMPARE(op) \
Vector3<bool> CONCAT(operator, op)(const Vector3<T>& a) \
{ return Vector3<bool>(x op a.x, y op a.y, z op a.z); } \
Vector3<bool> CONCAT(operator, op)(T a) \
{ return Vector3<bool>(x op a, y op a, z op a); }

///////////////////////////////////////////////////////////////////////////////

/* 3D vector */
template <typename T>
struct Vector3
{
	Vector3() = default;

	explicit Vector3(T val) :
		x(val),
		y(val),
		z(val)
	{ }

	explicit Vector3(T x, T y, T z) :
		x(x),
		y(y),
		z(z)
	{ }

	explicit Vector3(const Vector2<T>& v, T z) :
		x(v.x),
		y(v.y),
		z(z)
	{ }

	template <typename U>
	operator Vector3<U>() const
	{
		return Vector3<U>((U)x, (U)y, (U)z);
	}

	VECTOR3_SELF_OP(+=);
	VECTOR3_SELF_OP(-=);
	VECTOR3_SELF_OP(*=);
	VECTOR3_SELF_OP(/=);
	VECTOR3_SELF_OP(%=);

	VECTOR3_COMPARE(==);
	VECTOR3_COMPARE(!=);
	VECTOR3_COMPARE(>=);
	VECTOR3_COMPARE(<=);
	VECTOR3_COMPARE(>);
	VECTOR3_COMPARE(<);

	T x, y, z;
};

///////////////////////////////////////////////////////////////////////////////

VECTOR3_OP(+);
VECTOR3_OP(-);
VECTOR3_OP(*);
VECTOR3_OP(/);
VECTOR3_OP(%);

template <typename T>
inline Vector3<T> operator-(const Vector3<T>& x)
{
	return Vector3<T>(-x.x, -x.y, -x.z);
}

///////////////////////////////////////////////////////////////////////////////

/* Get length of vector from origin */
template <typename T>
inline T Length(const Vector3<T>& x)
{
	return (T)sqrt(x.x * x.x + x.y * x.y + x.z * x.z);
}

/* Get length squared of vector from origin */
template <typename T>
inline T LengthSquared(const Vector3<T>& x)
{
	return x.x * x.x + x.y * x.y + x.z * x.z;
}

/* Get distance between 2 vectors */
template <typename T>
inline T Distance(const Vector3<T> & a, const Vector3<T> & b)
{
	return Length(a - b);
}

/* Get squared distance between 2 vectors */
template <typename T>
inline T DistanceSquared(const Vector3<T> & a, const Vector3<T> & b)
{
	Vector3<T> d = a - b;
	return d.x* d.x + d.y * d.y + d.z * d.z;
}

/* Get normalized (unit) vector */
template <typename T>
inline Vector3<T> Normalize(const Vector3<T> & x)
{
	T d = Length(x);
	return x / d;
}

/* Get reflection of incident vector w.r.t. normal vector */
template <typename T>
inline Vector3<T> Reflect(const Vector3<T> & incident, const Vector3<T> & normal)
{
	const Vector3<T>& d = incident;
	Vector3<T> n = Normalize(normal);
	return d - 2 * Dot(d, n) * n;
}

/* Sum of vector elements */
template <typename T>
inline T Sum(const Vector3<T> & x)
{
	return x.x + x.y + x.z;
}

/* Dot product of 2 vectors */
template <typename T>
inline T Dot(const Vector3<T> & a, const Vector3<T> & b)
{
	return Sum(a * b);
}

/* Cross product of 2 vectors */
template <typename T>
inline Vector3<T> Cross(const Vector3<T>& a, const Vector3<T>& b)
{
	return Vector3<T>(
		a.y * b.z - a.z * b.y,
		a.z * b.x - a.x * b.z,
		a.x * b.y - a.y * b.x
	);
}

///////////////////////////////////////////////////////////////////////////////

/* Floor */
template <typename T>
inline Vector3<double> Floor(const Vector3<T>& x)
{
	return Vector3<double>(floor(x.x), floor(x.y), floor(x.z));
}

/* Ceil */
template <typename T>
inline Vector3<double> Ceil(const Vector3<T>& x)
{
	return Vector3<double>(ceil(x.x), ceil(x.y), ceil(x.z));
}

///////////////////////////////////////////////////////////////////////////////

typedef Vector3<float>	Vector3f;
typedef Vector3<double>	Vector3d;
typedef Vector3<Int32>	Vector3i;
typedef Vector3<Uint32>	Vector3u;
typedef Vector3<bool>	Vector3b;

///////////////////////////////////////////////////////////////////////////////

#endif