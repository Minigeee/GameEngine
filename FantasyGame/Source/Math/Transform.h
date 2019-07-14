#ifndef TRANSFORM_FUNCS_H
#define TRANSFORM_FUNCS_H

#include <Math/Vector3.h>
#include <Math/Matrix4.h>
#include <Math/Quaternion.h>

///////////////////////////////////////////////////////////////////////////////

/* Create transformation matrix */
Matrix4f ToTransform(const Vector3f& t, const Quaternion& q, float k);
/* Create view matrix (FPS style) */
Matrix4f ToView(const Vector3f& pos, float pitch, float yaw);
/* Create perspective matrix (Takes FOV for x-axis instead of y-axis) */
Matrix4f ToPerspective(float fov, float ar, float near, float far);

///////////////////////////////////////////////////////////////////////////////

#endif