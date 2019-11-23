#ifndef ENGINE_H
#define ENGINE_H

#include <Core/DataTypes.h>
#include <Core/Thread.h>

#include <Engine/Window.h>
#include <Engine/Input.h>

///////////////////////////////////////////////////////////////////////////////

class Scene;

class Engine
{
public:
	struct Params
	{
		/* Width of window */
		Uint32 mWindowWidth;
		/* Height of window */
		Uint32 mWindowHeight;
		/* Window title */
		const char* mWindowTitle;
		/* Is window fullscreen */
		bool mFullscreen;
	};

public:
	Engine();

	/* Initialize engine */
	bool Init(const Params& params);
	/* Start engine */
	void Start();
	/* Engine cleanup */
	void Stop();

	/* Close engine */
	void Close();

	/* Set game framerate */
	void SetFrameRate(Uint32 fps);

	/* Get window */
	Window* GetWindow();
	/* Get input system */
	Input* GetInput();
	/* Get game framerate */
	Uint32 GetFrameRate() const;
	/* Get loop duration (seconds) */
	float GetLoopDuration() const;

	/* Set current scene */
	void SetScene(Scene* scene);

private:
	/* Game window */
	Window mWindow;
	/* Input system */
	Input mInput;

	/* Engine framerate */
	Uint32 mFrameRate;
	/* Game loop duration (seconds) */
	float mLoopDuration;

	/* Current scene */
	Scene* mScene;
};

///////////////////////////////////////////////////////////////////////////////

#endif