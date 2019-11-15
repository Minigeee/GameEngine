#ifndef SCENE_COMPONENTS_H
#define SCENE_COMPONENTS_H

#include <Math/Vector3.h>

#include <Scene/Component.h>

///////////////////////////////////////////////////////////////////////////////

struct TransformComponent : public Component
{
	TYPE_INFO(TransformComponent);

	/* Position */
	Vector3f mPosition;
	/* Rotation */
	Vector3f mRotation;
	/* Scale */
	float mScale;
};

///////////////////////////////////////////////////////////////////////////////

#endif