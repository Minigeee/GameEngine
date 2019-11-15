#ifndef CAMERA_SYSTEM_H
#define CAMERA_SYSTEM_H

#include <Math/Vector3.h>

#include <Engine/Input.h>

#include <Scene/GameSystem.h>
#include <Scene/EventListener.h>

#include <Game/Objects/PlayerObject.h>

///////////////////////////////////////////////////////////////////////////////

class Camera;

class InputSystem :
	public GameSystem,
	public EventListener
{
	TYPE_INFO(InputSystem);

	HANDLE_EVENTS(
		E_MouseMove,
		E_MouseButton
	);

	REQUIRES_NO_COMPONENTS;

	REQUIRES_TAGS(
		"Player",
		"Dynamic"
	);

public:
	InputSystem();
	~InputSystem();

	void Update(float dt) override;

	/* Handle mouse movements */
	void HandleEvent(const E_MouseMove& event);
	/* Handle mouse button events */
	void HandleEvent(const E_MouseButton& event);

private:
	void OnInit() override;

private:
	/* Input system */
	Input* mInput;
	/* Camera access */
	Camera* mCamera;

	/* Keep track of mouse position */
	Vector2f mMousePos;

	/* Camera distance from player */
	float mCameraDist;
	/* Camera offset from player */
	Vector3f mCameraOffset;
	/* Keep track of camera rotation */
	Vector2f mCameraRot;

	/* Used so that first camera rotation isn't too big */
	bool mFirstRun;
};

///////////////////////////////////////////////////////////////////////////////

#endif