#ifndef SCENE_COMPONENTS_H
#define SCENE_COMPONENTS_H

#include <Math/Vector3.h>

#include <Scene/Component.h>

///////////////////////////////////////////////////////////////////////////////

struct TransformComponent : public Component
{
	COMPONENT_TYPE(TransformComponent);

	TransformComponent(GameObjectID id) :
		Component		(id),
		mPosition		(0.0f),
		mRotation		(0.0f),
		mScale			(1.0f)
	{ }

	/* Position */
	Vector3f mPosition;
	/* Rotation */
	Vector3f mRotation;
	/* Scale */
	float mScale;
};

///////////////////////////////////////////////////////////////////////////////

#endif