#ifndef CAMERA_H
#define CAMERA_H

#include <Math/Vector2.h>
#include <Math/Vector3.h>
#include <Math/Matrix4.h>

///////////////////////////////////////////////////////////////////////////////

class Camera
{
public:
	Camera();
	~Camera();

	/* Set position */
	void SetPosition(const Vector3f& pos);
	/* Set position */
	void SetPosition(float x, float y, float z);
	/* Set direction */
	void SetDirection(const Vector3f& pos);
	/* Set direction */
	void SetDirection(float x, float y, float z);
	/* Set rotation */
	void SetRotation(const Vector2f& rotation);
	/* Set rotation */
	void SetRotation(float x, float y);
	/* Set zoom */
	void SetZoom(float zoom);

	/* Move (along local axes) */
	void Move(const Vector3f& pos);
	/* Move (along local axes) */
	void Move(float x, float y, float z);
	/* Zoom */
	void Zoom(float zoom);

	/* Get position */
	const Vector3f& GetPosition() const;
	/* Get direction */
	const Vector3f& GetDirection() const;
	/* Get right vector */
	const Vector3f& GetRight() const;
	/* Get zoom */
	float GetZoom() const;

	/* Set perspective parameters */
	void SetPerspective(float fov, float ar, float near, float far);
	/* Set FOV (along x-axis) */
	void SetFOV(float fov);
	/* Set aspect ratio */
	void SetAspectRatio(float ar);
	/* Set near plane */
	void SetNear(float near);
	/* Set far plane */
	void SetFar(float far);

	/* Get projection matrix */
	const Matrix4f& GetProjection();
	/* Get view matrix */
	const Matrix4f& GetView();
	/* Get FOV */
	float GetFOV() const;
	/* Get aspect ratio */
	float GetAspectRatio() const;
	/* Get near plane */
	float GetNear() const;
	/* Get far plane */
	float GetFar() const;

private:
	/* Projection matrix */
	Matrix4f mProjection;
	/* View matrix */
	Matrix4f mView;

	/* Camera position */
	Vector3f mPosition;
	/* Camera direction */
	Vector3f mDirection;
	/* Camera right vector */
	Vector3f mRight;
	/* Camera zoom */
	float mZoom;

	/* Field of view */
	float mFOV;
	/* Aspect ratio */
	float mAspectRatio;
	/* Near plane */
	float mNear;
	/* Far plane */
	float mFar;

	/* Dirty flags */
	bool mProjDirty;
	bool mViewDirty;
};

///////////////////////////////////////////////////////////////////////////////

#endif