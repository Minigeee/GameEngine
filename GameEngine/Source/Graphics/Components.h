#ifndef GRAPHICS_COMPONENTS_H
#define GRAPHICS_COMPONENTS_H

#include <Math/Matrix4.h>
#include <Math/BoundingSphere.h>

#include <Scene/Component.h>

///////////////////////////////////////////////////////////////////////////////

class Model;

struct RenderComponent : public Component
{
	COMPONENT_TYPE(RenderComponent);

	RenderComponent(GameObjectID id) :
		mInstanceID		(0),
		mModel			(0),
		mTransform		(1.0f)
	{ }

	/* Renderable instance ID */
	Uint64 mInstanceID;
	/* Model used to render */
	Model* mModel;
	/* Bounding sphere */
	BoundingSphere mBoundingSphere;
	/* Transform matrix */
	Matrix4f mTransform;
};

///////////////////////////////////////////////////////////////////////////////

#endif