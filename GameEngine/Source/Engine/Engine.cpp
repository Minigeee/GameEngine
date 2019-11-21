#include <Engine/Engine.h>

#include <Core/Clock.h>
#include <Core/Sleep.h>
#include <Core/LogFile.h>
#include <Core/Profiler.h>

#include <Scene/Scene.h>

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

Engine::Engine() :
	mFrameRate		(60),
	mLoopDuration	(1.0f / 60.0f)
{

}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

bool Engine::Init(const Engine::Params& params)
{
	// Create window
	bool success = mWindow.Create(
		params.mWindowWidth,
		params.mWindowHeight,
		params.mWindowTitle,
		params.mFullscreen
	);

	/* Initialize input system */
	mInput.Init(&mWindow);

	if (!success) return false;

	return true;
}

///////////////////////////////////////////////////////////////////////////////

void Engine::Start()
{
	Clock clock;

	if (!mScene)
	{
		LOG_ERROR << "No Scene object\n";
		return;
	}

	while (mWindow.IsOpen())
	{
		START_PROFILER(GameLoop);

		float elapsed = clock.Restart();

		// Game logic
		mWindow.PollEvents();
		mScene->Update(elapsed);

		STOP_PROFILER(GameLoop);

		// Get work time
		float workTime = clock.GetElapsedTime();
		float sleepTime = mLoopDuration - workTime;

		// Sleep for rest of frame
		if (sleepTime > 0.0f)
			Sleep(sleepTime);

		mWindow.Display();
	}
}

///////////////////////////////////////////////////////////////////////////////

void Engine::Stop()
{
	if (mScene)
	{
		mScene->Delete();
		delete mScene;
	}

	mWindow.CleanUp();

	// Print profiler log
	Profiler::Log("profiler.log");
}

///////////////////////////////////////////////////////////////////////////////

void Engine::Close()
{
	mWindow.Close();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

void Engine::SetFrameRate(Uint32 fps)
{
	mFrameRate = fps;
	mLoopDuration = 1.0f / fps;
}

///////////////////////////////////////////////////////////////////////////////

Window* Engine::GetWindow()
{
	return &mWindow;
}

Input* Engine::GetInput()
{
	return &mInput;
}

Uint32 Engine::GetFrameRate() const
{
	return mFrameRate;
}

float Engine::GetLoopDuration() const
{
	return mLoopDuration;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

void Engine::SetScene(Scene* scene)
{
	/* TODO : multithread */
	// For now, single-threaded scene loading
	scene->Create(this);

	// Clean up previous scene
	if (mScene)
	{
		mScene->Delete();
		delete mScene;
	}

	mScene = scene;

	// Switch scenes for input system
	mInput.SetScene(mScene);
}

///////////////////////////////////////////////////////////////////////////////