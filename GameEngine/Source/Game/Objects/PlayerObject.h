#ifndef PLAYER_OBJECT_H
#define PLAYER_OBJECT_H

#include <Scene/GameObject.h>

#include <Graphics/Renderable.h>

///////////////////////////////////////////////////////////////////////////////

class PlayerObject :
	public GameObject,
	public Renderable
{
	TYPE_INFO(PlayerObject);

public:
	PlayerObject();
	~PlayerObject();

private:
	void OnCreate() override;
};

///////////////////////////////////////////////////////////////////////////////

#endif