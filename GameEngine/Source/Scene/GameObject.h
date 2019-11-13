#ifndef GAME_OBJECT_H
#define GAME_OBJECT_H

#include <Core/DataTypes.h>
#include <Core/TypeInfo.h>
#include <Core/HandleArray.h>

///////////////////////////////////////////////////////////////////////////////

struct GameObjectID
{
public:
	GameObjectID() = default;
	GameObjectID(Handle handle, Uint16 typeID);

	operator Uint32() const;

	/* Get object handle */
	Handle Handle() const;
	/* Get object type ID */
	Uint16 TypeID() const;

private:
	/* Object handle */
	::Handle mHandle;
	/* Object type ID */
	Uint16 mTypeID;
};

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