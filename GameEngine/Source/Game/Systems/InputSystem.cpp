#include <Game/Systems/InputSystem.h>

#include <Scene/Scene.h>

#include <Engine/Engine.h>
#include <Graphics/Camera.h>

#include <Game/Objects/PlayerObject.h>

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

InputSystem::InputSystem() :
	mCamera			(0),
	mMousePos		(0.0f),
	mCameraDist		(3.0f),
	mCameraOffset	(0.0f, 1.0f, 0.0f),
	mCameraRot		(0.0f),
	mFirstRun		(true)
{

}

InputSystem::~InputSystem()
{

}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

void InputSystem::OnInit()
{
	// Register for events
	mScene->RegisterListener(this);

	mInput = mScene->GetEngine()->GetInput();
	mCamera = &mScene->GetCamera();

	// Set cursor disabled
	mInput->SetCursorMode(Input::Disabled);
}

///////////////////////////////////////////////////////////////////////////////

void InputSystem::Update(float dt)
{
	float speed = 4.0f;

	Vector3f move(0.0f);

	if (mInput->KeyPressed(Input::W))
		move.z -= 1.0f;
	if (mInput->KeyPressed(Input::S))
		move.z += 1.0f;
	if (mInput->KeyPressed(Input::A))
		move.x -= 1.0f;
	if (mInput->KeyPressed(Input::D))
		move.x += 1.0f;
	if (mInput->KeyPressed(Input::LeftShift))
		move.y -= 1.0f;
	if (mInput->KeyPressed(Input::Space))
		move.y += 1.0f;

	// Move camera
	float d = Length(move);
	if (d > FLT_EPSILON)
	{
		move = move / d * speed * dt;

		Vector3f offset(0.0f);
		offset -= move.z * mCamera->GetDirection();
		offset += move.x * mCamera->GetRight();
		offset.y += move.y;

		mPlayerObject->Move(offset);
	}

	Vector3f pos =
		mPlayerObject->GetPosition() - mCameraDist * mCamera->GetDirection() + mCameraOffset;
	mCamera->SetPosition(pos);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

void InputSystem::HandleEvent(const E_MouseMove& event)
{
	const float sensitivity = 0.1f;

	Vector2f pos = Vector2f(event.mX, event.mY);
	if (mFirstRun)
	{
		mMousePos = pos;
		mFirstRun = false;
	}

	Vector2f delta = sensitivity * (pos - mMousePos);
	mMousePos = pos;

	// Rotate player
	mPlayerObject->Rotate(0.0f, -delta.x, 0.0f);

	// Update camera
	mCameraRot.x = fmod(mCameraRot.x - delta.y, 360.0f);
	mCameraRot.y = fmod(mCameraRot.y + delta.x, 360.0f);
	if (mCameraRot.x > 89.0f)
		mCameraRot.x = 89.0f;
	else if (mCameraRot.x < -89.0f)
		mCameraRot.x = -89.0f;

	mCamera->SetRotation(mCameraRot);
}

///////////////////////////////////////////////////////////////////////////////

void InputSystem::HandleEvent(const E_MouseButton& event)
{

}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

void InputSystem::SetMainPlayer(PlayerObject* object)
{
	mPlayerObject = object;
}

///////////////////////////////////////////////////////////////////////////////