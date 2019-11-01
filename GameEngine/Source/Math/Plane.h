#ifndef PLANE_H
#define PLANE_H

#include <Math/Vector3.h>

///////////////////////////////////////////////////////////////////////////////

class Plane
{
public:
	Plane() = default;
	Plane(const Vector3f& n, float d);
	Plane(float a, float b, float c, float d);

	/* Get signed distance to point */
	float Dist(const Vector3f& p) const;

	Vector3f n;
	float d;
};

///////////////////////////////////////////////////////////////////////////////

#endif