#ifndef CAMERA_SYSTEM_H
#define CAMERA_SYSTEM_H

#include <Math/Vector3.h>

#include <Engine/Input.h>

#include <Scene/GameSystem.h>
#include <Scene/EventListener.h>

///////////////////////////////////////////////////////////////////////////////

class Camera;
class PlayerObject;

class InputSystem :
	public GameSystem,
	public EventListener
{
	TYPE_INFO(InputSystem);

	HANDLE_EVENTS(
		E_MouseMove,
		E_MouseButton
	);

public:
	InputSystem();
	~InputSystem();

	void Update(float dt) override;

	/* Handle mouse movements */
	void HandleEvent(const E_MouseMove& event);
	/* Handle mouse button events */
	void HandleEvent(const E_MouseButton& event);

	/* Set main player */
	void SetMainPlayer(PlayerObject* object);

private:
	void OnInit() override;

private:
	/* Input system */
	Input* mInput;
	/* Camera access */
	Camera* mCamera;

	/* Keep track of mouse position */
	Vector2f mMousePos;

	/* Player game object */
	PlayerObject* mPlayerObject;
	/* Camera distance from player */
	float mCameraDist;
	/* Camera offset from player */
	Vector3f mCameraOffset;

	/* Used so that first camera rotation isn't too big */
	bool mFirstRun;
};

///////////////////////////////////////////////////////////////////////////////

#endif