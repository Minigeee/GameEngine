#ifndef RENDERABLE_H
#define RENDERABLE_H

#include <Core/DataTypes.h>
#include <Math/Matrix4.h>
#include <Math/BoundingBox.h>

#include <Scene/Transformable.h>

///////////////////////////////////////////////////////////////////////////////

class Renderer;
class Model;

class Renderable : public Transformable
{
	friend Renderer;

public:
	Renderable();
	virtual ~Renderable();

	/* Get transform matrix */
	const Matrix4f& GetTransform();
	/* Get bounding box that has been adjusted to transform (Not completely accurate) */
	const BoundingBox& GetBoundingBox();

	/* Set model */
	void SetModel(Model* model);
	/* Get model pointer */
	Model* GetModel() const;
	/* Get instance ID */
	Uint32 GetInstanceID() const;

protected:
	/* Transform matrix */
	Matrix4f mTransform;
	/* Renderable bounding box */
	BoundingBox mBoundingBox;

	/* ID used when updating instance data */
	Uint32 mInstanceID;
	/* Model used to render */
	Model* mModel;

private:
	/* Update transform and anything that depends on transform (bounding box) */
	void UpdateTransform();
};

///////////////////////////////////////////////////////////////////////////////

#endif