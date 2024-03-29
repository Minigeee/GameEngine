#include <Engine/Application.h>
#include <Game/WorldScene.h>

///////////////////////////////////////////////////////////////////////////////

bool Application::Run()
{
	// Engine params
	Engine::Params params;
	params.mWindowWidth = 1280;
	params.mWindowHeight = 720;
	params.mWindowTitle = "Fantasy Game";
	params.mFullscreen = false;


	// Initialize engine
	if (!mEngine.Init(params))
		return false;
	mEngine.SetScene(new WorldScene());

	// Game loop
	mEngine.Start();

	// Cleanup
	mEngine.Stop();

	return true;
}

///////////////////////////////////////////////////////////////////////////////