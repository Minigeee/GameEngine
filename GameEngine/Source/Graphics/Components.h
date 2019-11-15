#ifndef GRAPHICS_COMPONENTS_H
#define GRAPHICS_COMPONENTS_H

#include <Math/Matrix4.h>
#include <Math/BoundingSphere.h>

#include <Scene/Component.h>

///////////////////////////////////////////////////////////////////////////////

struct RenderComponent : public Component
{
	TYPE_INFO(RenderComponent);

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