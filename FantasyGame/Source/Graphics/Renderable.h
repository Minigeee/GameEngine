#ifndef RENDERABLE_H
#define RENDERABLE_H

#include <Core/DataTypes.h>

#include <Math/Vector3.h>
#include <Math/Matrix4.h>

///////////////////////////////////////////////////////////////////////////////

class Renderer;
class Model;

class Renderable
{
	friend Renderer;

public:
	Renderable();
	virtual ~Renderable();

	/* Set position */
	void SetPosition(const Vector3f& pos);
	/* Set position */
	void SetPosition(float x, float y, float z);
	/* Set rotation */
	void SetRotation(const Vector3f& rot);
	/* Set rotation */
	void SetRotation(float x, float y, float z);
	/* Set scale */
	void SetScale(float scale);

	/* Move (global coordinates) */
	void Move(const Vector3f& pos);
	/* Move (global coordinates) */
	void Move(float x, float y, float z);
	/* Rotate */
	void Rotate(const Vector3f& rot);
	/* Rotate */
	void Rotate(float x, float y, float z);
	/* Scale */
	void Scale(float scale);

	/* Get transform matrix */
	const Matrix4f& GetTransform();
	/* Get position */
	const Vector3f& GetPosition() const;
	/* Get rotation */
	const Vector3f& GetRotation() const;
	/* Get scale */
	float GetScale() const;

	/* Set model */
	void SetModel(Model* model);
	/* Get model pointer */
	Model* GetModel() const;
	/* Get instance ID */
	Uint32 GetInstanceID() const;

protected:
	/* Transform matrix */
	Matrix4f mTransform;
	/* Position */
	Vector3f mPosition;
	/* Rotation */
	Vector3f mRotation;
	/* Scale */
	float mScale;

	/* ID used when updating instance data */
	Uint32 mInstanceID;
	/* Model used to render */
	Model* mModel;

	/* Dirty flag */
	bool mTransformDirty;
};

///////////////////////////////////////////////////////////////////////////////

#endif