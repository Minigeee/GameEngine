#include <Graphics/Camera.h>
#include <Math/Transform.h>

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

Camera::Camera() :
	mProjection		(1.0f),
	mView			(1.0f),

	mPosition		(0.0f),
	mDirection		(0.0f, 0.0f, -1.0f),
	mRight			(1.0f, 0.0f, 0.0f),
	mZoom			(1.0f),

	mFOV			(90.0f),
	mAspectRatio	(16.0f / 9.0f),
	mNear			(0.1f),
	mFar			(500.0f),

	mProjDirty		(true),
	mViewDirty		(true)
{

}

Camera::~Camera()
{

}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

void Camera::SetPosition(const Vector3f& pos)
{
	mPosition = pos;
	mViewDirty = true;
}

void Camera::SetPosition(float x, float y, float z)
{
	mPosition = Vector3f(x, y, z);
	mViewDirty = true;
}

///////////////////////////////////////////////////////////////////////////////

void Camera::SetDirection(const Vector3f& dir)
{
	mDirection = Normalize(dir);
	mRight = Cross(mDirection, Vector3f(0.0f, 1.0f, 0.0f));
	mViewDirty = true;
}

void Camera::SetDirection(float x, float y, float z)
{
	mDirection = Normalize(Vector3f(x, y, z));
	mRight = Cross(mDirection, Vector3f(0.0f, 1.0f, 0.0f));
	mViewDirty = true;
}

///////////////////////////////////////////////////////////////////////////////

void Camera::SetRotation(const Vector2f& rotation)
{
	float cx = cos(rotation.x);
	float cy = cos(rotation.y);
	float sx = sin(rotation.x);
	float sy = sin(rotation.y);

	mDirection = Normalize(Vector3f(cy * cx, sx, sy * cx));
	mRight = Cross(mDirection, Vector3f(0.0f, 1.0f, 0.0f));

	mViewDirty = true;
}

void Camera::SetRotation(float x, float y)
{
	SetRotation(Vector2f(x, y));
}

///////////////////////////////////////////////////////////////////////////////

void Camera::SetZoom(float zoom)
{
	mZoom = zoom;
	mProjDirty = true;
}

///////////////////////////////////////////////////////////////////////////////

void Camera::Move(const Vector3f& pos)
{
	mPosition += pos;
	mViewDirty;
}

void Camera::Move(float x, float y, float z)
{
	mPosition += Vector3f(x, y, z);
	mViewDirty;
}

///////////////////////////////////////////////////////////////////////////////

void Camera::Zoom(float zoom)
{
	mZoom *= zoom;
	mProjDirty = true;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

const Vector3f& Camera::GetPosition() const
{
	return mPosition;
}

const Vector3f& Camera::GetDirection() const
{
	return mDirection;
}

const Vector3f& Camera::GetRight() const
{
	return mRight;
}

float Camera::GetZoom() const
{
	return mZoom;
}

///////////////////////////////////////////////////////////////////////////////

void Camera::SetPerspective(float fov, float ar, float near, float far)
{
	mFOV = fov;
	mAspectRatio = ar;
	mNear = near;
	mFar = far;
	mProjDirty = true;
}

void Camera::SetFOV(float fov)
{
	mFOV = fov;
	mProjDirty = true;
}

void Camera::SetAspectRatio(float ar)
{
	mAspectRatio = ar;
	mProjDirty = true;
}

void Camera::SetNear(float near)
{
	mNear = near;
	mProjDirty = true;
}

void Camera::SetFar(float far)
{
	mFar = far;
	mProjDirty = true;
}

///////////////////////////////////////////////////////////////////////////////

const Matrix4f& Camera::GetProjection()
{
	if (mProjDirty)
	{
		mProjection = ToPerspective(mFOV * mZoom, mAspectRatio, mNear, mFar);
		mProjDirty = false;
	}

	return mProjection;
}

const Matrix4f& Camera::GetView()
{
	if (mViewDirty)
	{
		mView = ToView(mPosition, mDirection, mRight);
		mViewDirty = false;
	}

	return mView;
}

///////////////////////////////////////////////////////////////////////////////

float Camera::GetFOV() const
{
	return mFOV;
}

float Camera::GetAspectRatio() const
{
	return mAspectRatio;
}

float Camera::GetNear() const
{
	return mNear;
}

float Camera::GetFar() const
{
	return mFar;
}

///////////////////////////////////////////////////////////////////////////////