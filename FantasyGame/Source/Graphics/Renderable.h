#ifndef RENDERABLE_H
#define RENDERABLE_H

#include <Core/DataTypes.h>
#include <Math/Matrix4.h>

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

	/* Set model */
	void SetModel(Model* model);
	/* Get model pointer */
	Model* GetModel() const;
	/* Get instance ID */
	Uint32 GetInstanceID() const;

protected:
	/* Transform matrix */
	Matrix4f mTransform;

	/* ID used when updating instance data */
	Uint32 mInstanceID;
	/* Model used to render */
	Model* mModel;
};

///////////////////////////////////////////////////////////////////////////////

#endif