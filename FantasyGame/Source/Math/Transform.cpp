#include <Math/Transform.h>

///////////////////////////////////////////////////////////////////////////////

Matrix4f ToTransform(const Vector3f& t, const Quaternion& q, float k)
{
	return Matrix4f(
		// First column
		k * (1.0f - 2.0f * (q.y * q.y + q.z * q.z)),
		k * (2.0f * (q.x * q.y + q.w * q.z)),
		k * (2.0f * (q.x * q.z - q.w * q.y)),
		0.0f,

		// Second column
		k * (2.0f * (q.x * q.y - q.w * q.z)),
		k * (1.0f - 2.0f * (q.x * q.x + q.z * q.z)),
		k * (2.0f * (q.y * q.z + q.w * q.x)),
		0.0f,

		// Third column
		k * (2.0f * (q.x * q.z + q.w * q.y)),
		k * (2.0f * (q.y * q.z - q.w * q.x)),
		k * (1.0f - 2.0f * (q.x * q.x + q.y * q.y)),
		0.0f,

		// Fourth column
		t.x, t.y, t.z, 1.0f
	);
}

///////////////////////////////////////////////////////////////////////////////

Matrix4f ToView(const Vector3f& pos, float pitch, float yaw)
{
	float cx = cos(pitch);
	float sx = sin(pitch);
	float cy = cos(yaw);
	float sy = sin(yaw);

	Vector3f x(cy, 0.0f, -sy);
	Vector3f y(sy * sx, cx, cy * sx);
	Vector3f z(sy * cx, -sx, cx * cy);

	return Matrix4f(
		x.x, y.x, z.x, 0.0f,
		x.y, y.y, z.y, 0.0f,
		x.z, y.z, z.z, 0.0f,
		-Dot(x, pos), -Dot(y, pos), -Dot(z, pos), 1.0f
	);
}

///////////////////////////////////////////////////////////////////////////////

Matrix4f ToPerspective(float fov, float ar, float near, float far)
{
	float fovy = fov / ar;
	fovy = tan(fovy * 0.5f);

	return Matrix4f(
		1.0f / (ar * fovy), 0.0f,			0.0f,							0.0f,
		0.0f,				1.0f / fovy,	0.0f,							0.0f,
		0.0f,				0.0f,			-(far + near) / (far - near),	-1.0f,
		0.0f,				0.0f,			-2.0f * far * near / (far - near), 0.0f
	);
}

///////////////////////////////////////////////////////////////////////////////