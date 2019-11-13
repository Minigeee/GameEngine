#ifndef VECTOR4_H
#define VECTOR4_H

#include <Math/Vector3.h>

///////////////////////////////////////////////////////////////////////////////

#define VECTOR4_SELF_OP(op) \
Vector4<T>& CONCAT(operator, op)(const Vector4<T>& val) \
{ x op val.x; y op val.y; z op val.z; w op val.w; return *this; } \
Vector4<T>& CONCAT(operator, op)(T val) \
{ x op val; y op val; z op val; w op val; return *this; }

#define VECTOR4_OP(op) \
template <typename T> inline Vector4<T> CONCAT(operator, op)(const Vector4<T>& l, const Vector4<T>& r) \
{ return Vector4<T>(l.x op r.x, l.y op r.y, l.z op r.z, l.w op r.w); } \
template <typename T> inline Vector4<T> CONCAT(operator, op)(const Vector4<T>& l, T r) \
{ return Vector4<T>(l.x op r, l.y op r, l.z op r, l.w op r); } \
template <typename T> inline Vector4<T> CONCAT(operator, op)(T l, const Vector4<T>& r) \
{ return Vector4<T>(l op r.x, l op r.y, l op r.z, l op r.w); }

#define VECTOR4_COMPARE(op) \
Vector4<bool> CONCAT(operator, op)(const Vector4<T>& a) \
{ return Vector4<bool>(x op a.x, y op a.y, z op a.z, w op a.w); } \
Vector4<bool> CONCAT(operator, op)(T a) \
{ return Vector4<bool>(x op a, y op a, z op a, w op a); }

///////////////////////////////////////////////////////////////////////////////

/* 4D vector */
template <typename T>
struct Vector4
{
	Vector4() = default;

	explicit Vector4(T val) :
		x(val),
		y(val),
		z(val),
		w(val)
	{ }

	explicit Vector4(T x, T y, T z, T w) :
		x(x),
		y(y),
		z(z),
		w(w)
	{ }

	explicit Vector4(const Vector3<T>& v, T w) :
		x(v.x),
		y(v.y),
		z(v.z),
		w(w)
	{ }

	explicit Vector4(const Vector2<T>& v, T z, T w) :
		x(v.x),
		y(v.y),
		z(z),
		w(w)
	{ }

	template <typename U>
	operator Vector4<U>() const
	{
		return Vector4<U>((U)x, (U)y, (U)z, (U)w);
	}

	VECTOR4_SELF_OP(+=);
	VECTOR4_SELF_OP(-=);
	VECTOR4_SELF_OP(*=);
	VECTOR4_SELF_OP(/=);
	VECTOR4_SELF_OP(%=);

	VECTOR4_COMPARE(==);
	VECTOR4_COMPARE(!=);
	VECTOR4_COMPARE(>=);
	VECTOR4_COMPARE(<=);
	VECTOR4_COMPARE(>);
	VECTOR4_COMPARE(<);

	T x, y, z, w;
};

///////////////////////////////////////////////////////////////////////////////

VECTOR4_OP(+);
VECTOR4_OP(-);
VECTOR4_OP(*);
VECTOR4_OP(/);
VECTOR4_OP(%);

template <typename T>
inline Vector4<T> operator-(const Vector4<T>& x)
{
	return Vector4<T>(-x.x, -x.y, -x.z, -x.w);
}

///////////////////////////////////////////////////////////////////////////////

/* Get length of vector from origin */
template <typename T>
inline T Length(const Vector4<T>& x)
{
	return (T)sqrt(x.x * x.x + x.y * x.y + x.z * x.z + x.w * x.w);
}

/* Get distance between 2 vectors */
template <typename T>
inline T Distance(const Vector4<T> & a, const Vector4<T> & b)
{
	return Length(a - b);
}

/* Get squared distance between 2 vectors */
template <typename T>
inline T DistanceSquared(const Vector4<T> & a, const Vector4<T> & b)
{
	Vector4<T> d = a - b;
	return d.x* d.x + d.y * d.y + d.z * d.z + d.w * d.w;
}

/* Get normalized (unit) vector */
template <typename T>
inline Vector4<T> Normalize(const Vector4<T> & x)
{
	T d = Length(x);
	return x / d;
}

/* Get reflection of incident vector w.r.t. normal vector */
template <typename T>
inline Vector4<T> Reflect(const Vector4<T> & incident, const Vector4<T> & normal)
{
	const Vector4<T>& d = incident;
	Vector4<T> n = Normalize(normal);
	return d - 2 * Dot(d, n) * n;
}

/* Sum of vector elements */
template <typename T>
inline T Sum(const Vector4<T> & x)
{
	return x.x + x.y + x.z + x.w;
}

/* Dot product of 2 vectors */
template <typename T>
inline T Dot(const Vector4<T> & a, const Vector4<T> & b)
{
	return Sum(a * b);
}

///////////////////////////////////////////////////////////////////////////////

/* Floor */
template <typename T>
inline Vector4<double> Floor(const Vector4<T>& x)
{
	return Vector4<double>(floor(x.x), floor(x.y), floor(x.z), floor(x.w));
}

/* Ceil */
template <typename T>
inline Vector4<double> Ceil(const Vector4<T>& x)
{
	return Vector4<double>(ceil(x.x), ceil(x.y), ceil(x.z), ceil(x.w));
}

///////////////////////////////////////////////////////////////////////////////

typedef Vector4<float>	Vector4f;
typedef Vector4<double>	Vector4d;
typedef Vector4<Int32>	Vector4i;
typedef Vector4<Uint32>	Vector4u;
typedef Vector4<bool>	Vector4b;

///////////////////////////////////////////////////////////////////////////////

#endif