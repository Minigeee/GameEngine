#ifndef FRUSTUM_H
#define FRUSTUM_H

#include <Math/Plane.h>
#include <Math/BoundingBox.h>

///////////////////////////////////////////////////////////////////////////////

class Frustum
{
public:
	enum Side
	{
		Left = 0,
		Right,
		Bottom,
		Top,
		Near,
		Far
	};

public:
	Frustum();

	/* Set plane in frustum */
	void SetPlane(const Plane& plane, Side side);

	/* Returns true if frustum partially or fully contains a box */
	bool Contains(const BoundingBox& box) const;

private:
	Plane mPlanes[6];
};

///////////////////////////////////////////////////////////////////////////////

#endif