#ifndef WORLD_SCENE_H
#define WORLD_SCENE_H

#include <Scene/Scene.h>

///////////////////////////////////////////////////////////////////////////////

class WorldScene : public Scene
{
public:
	WorldScene();
	~WorldScene();

private:
	/* Setup world */
	void OnCreate() override;
	/* Clean up resources */
	void OnDelete() override;
};

///////////////////////////////////////////////////////////////////////////////

#endif