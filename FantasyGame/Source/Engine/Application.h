#ifndef APPLICATION_H
#define APPLICATION_H

#include <Engine/Engine.h>

///////////////////////////////////////////////////////////////////////////////

class Application
{
public:
	/* Run application */
	bool Run();

private:
	/* Game engine */
	Engine mEngine;
};

///////////////////////////////////////////////////////////////////////////////

#endif