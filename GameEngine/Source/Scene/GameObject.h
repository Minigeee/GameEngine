#ifndef GAME_OBJECT_H
#define GAME_OBJECT_H

#include <Core/DataTypes.h>
#include <Core/TypeInfo.h>

///////////////////////////////////////////////////////////////////////////////

class GameObject
{
	REQUIRES_TYPE_INFO;

public:
	GameObject();
	virtual ~GameObject();

	/* Create game object */
	void Create();

private:
	virtual void OnCreate();
};

///////////////////////////////////////////////////////////////////////////////

#endif