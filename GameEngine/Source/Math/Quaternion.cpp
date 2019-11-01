#include <Math/Quaternion.h>
#include <Math/Math.h>

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

Quaternion::Quaternion() :
	x(0.0f),
	y(0.0f),
	z(0.0f),
	w(1.0f)
{

}

Quaternion::Quaternion(const Vector3f& a, float r) :
	x(0.0f),
	y(0.0f),
	z(0.0f),
	w(1.0f)
{
	float angle = ToRadians(r);
	float result = sin(angle * 0.5f);
	w = cos(angle * 0.5f);

	x = a.x * result;
	y = a.y * result;
	z = a.z * result;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

Quaternion& Quaternion::operator*=(const Quaternion& x)
{
	*this = *this * x;

	return *this;
}

///////////////////////////////////////////////////////////////////////////////

Quaternion Quaternion::operator*(const Quaternion& q) const
{
	Quaternion r;

	r.w = w * q.w - x * q.x - y * q.y - z * q.z;
	r.x = w * q.x + x * q.w + y * q.z - z * q.y;
	r.y = w * q.y + y * q.w + z * q.x - x * q.z;
	r.z = w * q.z + z * q.w + x * q.y - y * q.x;

	return r;
}

///////////////////////////////////////////////////////////////////////////////

Matrix4f ToMatrix(const Quaternion& q)
{
	return Matrix4f(
		// First column
		1.0f - 2.0f * (q.y * q.y + q.z * q.z),
		2.0f * (q.x * q.y + q.w * q.z),
		2.0f * (q.x * q.z - q.w * q.y),
		0.0f,

		// Second column
		2.0f * (q.x * q.y - q.w * q.z),
		1.0f - 2.0f * (q.x * q.x + q.z * q.z),
		2.0f * (q.y * q.z + q.w * q.x),
		0.0f,

		// Third column
		2.0f * (q.x * q.z + q.w * q.y),
		2.0f * (q.y * q.z - q.w * q.x),
		1.0f - 2.0f * (q.x * q.x + q.y * q.y),
		0.0f,

		// Fourth column
		0.0f, 0.0f, 0.0f, 1.0f
	);
}

///////////////////////////////////////////////////////////////////////////////