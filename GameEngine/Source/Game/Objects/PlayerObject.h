#ifndef PLAYER_OBJECT_H
#define PLAYER_OBJECT_H

#include <Scene/GameObject.h>

#include <Graphics/Renderable.h>

#include <Scene/Components.h>
#include <Graphics/Components.h>

///////////////////////////////////////////////////////////////////////////////

class PlayerObject :
	public GameObject,
	public Renderable
{
	GAME_OBJECT(PlayerObject);

	REGISTER_COMPONENTS(
		TransformComponent,
		RenderComponent
	);
};

///////////////////////////////////////////////////////////////////////////////

#endif