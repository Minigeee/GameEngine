#ifndef ENGINE_H
#define ENGINE_H

#include <Core/DataTypes.h>

#include <Engine/Window.h>

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

	/* Set game framerate */
	void SetFrameRate(Uint32 fps);

	/* Get game framerate */
	Uint32 GetFrameRate() const;
	/* Get loop duration (seconds) */
	float GetLoopDuration() const;

	/* Set current scene */
	void SetScene(Scene* scene);

private:
	/* Game window */
	Window mWindow;

	/* Engine framerate */
	Uint32 mFrameRate;
	/* Game loop duration (seconds) */
	float mLoopDuration;

	/* Current scene */
	Scene* mScene;
};

///////////////////////////////////////////////////////////////////////////////

#endif