#ifndef GAME_SYSTEM_H
#define GAME_SYSTEM_H

#include <Core/TypeInfo.h>

///////////////////////////////////////////////////////////////////////////////

class Scene;

class GameSystem
{
	REQUIRES_TYPE_INFO;

public:
	GameSystem();
	virtual ~GameSystem();

	/* Initialize system */
	void Init(Scene* scene);
	/* Clean up system */
	void CleanUp();

	/* Recieve updates every frame */
	virtual void Update(float dt);

	/* Override to add system dependencies */
	virtual void RegisterDependencies();

protected:
	/* Custom initialization */
	virtual void OnInit();
	/* Custom clean up */
	virtual void OnCleanUp();

protected:
	/* Scene access */
	Scene* mScene;
};

///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////

#endif