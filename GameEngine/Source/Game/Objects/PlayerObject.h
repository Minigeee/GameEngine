#ifndef PLAYER_OBJECT_H
#define PLAYER_OBJECT_H

#include <Scene/GameObject.h>

#include <Scene/Components.h>
#include <Graphics/Components.h>

///////////////////////////////////////////////////////////////////////////////

class PlayerObject :
	public GameObject
{
	GAME_OBJECT(PlayerObject);

	REGISTER_COMPONENTS(
		TransformComponent,
		RenderComponent
	);

	REGISTER_TAGS(
		"Player",
		"Dynamic"
	);
};

///////////////////////////////////////////////////////////////////////////////

#endif