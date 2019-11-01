#include <Math/Transform.h>
#include <Math/Math.h>

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

Matrix4f ToTransform(const Vector3f& t, const Vector3f& r, float k)
{
	Vector3f rot(ToRadians(r.x), ToRadians(r.y), ToRadians(r.z));
	Vector3f c(cos(rot.x), cos(rot.y), cos(rot.z));
	Vector3f s(sin(rot.x), sin(rot.y), sin(rot.z));

	return Matrix4f(
		k * (c.z * c.y),
		k * (s.z * c.y),
		k * (-s.y),
		0.0f,

		k * (-s.z * c.x + c.z * s.y * s.x),
		k * (c.z * c.x + s.z * s.y * s.x),
		k * (c.y * s.x),
		0.0f,

		k * (s.z * s.x + c.z * s.y * c.x),
		k * (-c.z * s.x + s.z * s.y * c.x),
		k * (c.y * c.x),
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
		r.x, u.x, -f.x, 0.0f,
		r.y, u.y, -f.y, 0.0f,
		r.z, u.z, -f.z, 0.0f,
		-Dot(r, p), -Dot(u, p), Dot(f, p), 1.0f
	);
}

///////////////////////////////////////////////////////////////////////////////

Matrix4f ToPerspective(float fov, float ar, float near, float far)
{
	float fovy = fov / ar;
	fovy = tan(ToRadians(fovy * 0.5f));

	return Matrix4f(
		1.0f / (ar * fovy), 0.0f,			0.0f,							0.0f,
		0.0f,				1.0f / fovy,	0.0f,							0.0f,
		0.0f,				0.0f,			-(far + near) / (far - near),	-1.0f,
		0.0f,				0.0f,			-2.0f * far * near / (far - near), 0.0f
	);
}

///////////////////////////////////////////////////////////////////////////////