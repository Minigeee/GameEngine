#ifndef PLAYER_OBJECT_H
#define PLAYER_OBJECT_H

#include <Scene/GameObject.h>
#include <Scene/Component.h>

#include <Graphics/Renderable.h>

///////////////////////////////////////////////////////////////////////////////

struct A : public Component
{
	TYPE_INFO(A);
	Uint32 mA;
	A(GameObjectID id) : Component(id) { }
	~A() { mA = 99; }
};
struct B : public Component { TYPE_INFO(B); Uint32 mA; B(GameObjectID id) : Component(id) { } };
struct C : public Component { TYPE_INFO(C); Uint32 mA; C(GameObjectID id) : Component(id) { } };
struct D : public Component { TYPE_INFO(D); Uint32 mA; D(GameObjectID id) : Component(id) { } };

class PlayerObject :
	public GameObject,
	public Renderable
{
	GAME_OBJECT(PlayerObject);

	REGISTER_COMPONENTS(
		A, B, C, D
	);
};

///////////////////////////////////////////////////////////////////////////////

#endif