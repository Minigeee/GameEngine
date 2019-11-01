#ifndef QUATERNION_H
#define QUATERNION_H

#include <Math/Vector3.h>
#include <Math/Matrix4.h>

///////////////////////////////////////////////////////////////////////////////

class Quaternion
{
public:
	Quaternion();
	Quaternion(const Vector3f& axis, float rotation);

	Quaternion& operator*=(const Quaternion& x);
	Quaternion operator*(const Quaternion& x) const;

	float x, y, z, w;
};

///////////////////////////////////////////////////////////////////////////////

/* Create matrix from quaternion */
Matrix4f ToMatrix(const Quaternion& q);

///////////////////////////////////////////////////////////////////////////////

#endif