#ifndef BOUNDING_SPHERE_H
#define BOUNDING_SPHERE_H

#include <Math/Vector3.h>

///////////////////////////////////////////////////////////////////////////////

class BoundingSphere
{
public:
	BoundingSphere() = default;
	BoundingSphere(const Vector3f& p, float r);

	/* Position of sphere */
	Vector3f p;
	/* Radius of sphere */
	float r;
};

///////////////////////////////////////////////////////////////////////////////

#endif