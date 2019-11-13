#ifndef COMPONENT_H
#define COMPONENT_H

#include <Scene/GameObject.h>

///////////////////////////////////////////////////////////////////////////////

/* Base component class */
struct Component
{
	Component() = default;
	Component(GameObjectID id) : mID(id) { }

	/* ID of parent game object */
	GameObjectID mID;
};

///////////////////////////////////////////////////////////////////////////////

#endif