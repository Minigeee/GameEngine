#ifndef TRANSFORM_FUNCS_H
#define TRANSFORM_FUNCS_H

#include <Math/Vector3.h>
#include <Math/Matrix4.h>
#include <Math/Quaternion.h>

///////////////////////////////////////////////////////////////////////////////

/* Create transformation matrix */
Matrix4f ToTransform(const Vector3f& t, const Quaternion& q, float k);
/* Create transformation matrix */
Matrix4f ToTransform(const Vector3f& t, const Vector3f& r, float k);
/* Create view matrix (Make sure front and right vectors are normalized) */
Matrix4f ToView(const Vector3f& p, const Vector3f& f, const Vector3f& r);
/* Create perspective matrix (Takes FOV for x-axis instead of y-axis) */
Matrix4f ToPerspective(float fov, float ar, float near, float far);

///////////////////////////////////////////////////////////////////////////////

#endif