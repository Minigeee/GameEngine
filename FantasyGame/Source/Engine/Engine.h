#ifndef ENGINE_H
#define ENGINE_H

#include <Core/DataTypes.h>

///////////////////////////////////////////////////////////////////////////////

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
	/* Initialize engine */
	bool Init(const Params& params);
	/* Start engine */
	void Start();
	/* Engine cleanup */
	void Stop();
};

///////////////////////////////////////////////////////////////////////////////

#endif