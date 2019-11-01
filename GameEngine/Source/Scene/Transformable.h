#ifndef TRANSFORMABLE_H
#define TRANSFORMABLE_H

#include <Math/Vector3.h>

///////////////////////////////////////////////////////////////////////////////

class Transformable
{
public:
	Transformable();
	virtual ~Transformable();

	/* Set position */
	virtual void SetPosition(const Vector3f& pos);
	/* Set position */
	virtual void SetPosition(float x, float y, float z);
	/* Set rotation */
	virtual void SetRotation(const Vector3f& rot);
	/* Set rotation */
	virtual void SetRotation(float x, float y, float z);
	/* Set scale */
	virtual void SetScale(float scale);

	/* Move (global coordinates) */
	virtual void Move(const Vector3f& pos);
	/* Move (global coordinates) */
	virtual void Move(float x, float y, float z);
	/* Rotate */
	virtual void Rotate(const Vector3f& rot);
	/* Rotate */
	virtual void Rotate(float x, float y, float z);
	/* Scale */
	virtual void Scale(float scale);

	/* Get position */
	const Vector3f& GetPosition() const;
	/* Get rotation */
	const Vector3f& GetRotation() const;
	/* Get scale */
	float GetScale() const;

protected:
	/* Position */
	Vector3f mPosition;
	/* Rotation */
	Vector3f mRotation;
	/* Scale */
	float mScale;

	/* Dirty flag */
	bool mTransformDirty;
};

///////////////////////////////////////////////////////////////////////////////

#endif