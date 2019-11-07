#ifndef BOUNDING_BOX_H
#define BOUNDING_BOX_H

#include <Math/Vector3.h>

///////////////////////////////////////////////////////////////////////////////

class BoundingBox
{
public:
	BoundingBox();
	BoundingBox(const Vector3f& min, const Vector3f& max);

	/* Get center of box */
	Vector3f GetPosition() const;

	Vector3f mMin, mMax;
};

///////////////////////////////////////////////////////////////////////////////

#endif