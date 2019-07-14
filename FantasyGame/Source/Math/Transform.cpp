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

Matrix4f ToView(const Vector3f& p, const Vector3f& f, const Vector3f& r)
{
	Vector3f u = Normalize(Cross(r, f));

	return Matrix4f(
		r.x, u.x, -f.x, -Dot(r, p),
		r.y, u.y, -f.y, -Dot(u, p),
		r.z, u.z, -f.z,  Dot(f, p),
		0.0f, 0.0f, 0.0f, 1.0f
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