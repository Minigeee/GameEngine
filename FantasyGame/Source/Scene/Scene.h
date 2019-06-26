#ifndef SCENE_H
#define SCENE_H

///////////////////////////////////////////////////////////////////////////////

class Engine;

class Scene
{
public:
	Scene();
	virtual ~Scene();

	/* Create scene */
	void Create(Engine* engine);
	/* Delete scene */
	void Delete();
	/* Update scene and do game logic */
	void Update(float dt);

protected:
	/* Access to engine */
	Engine* mEngine;

private:
	/* Called on scene creation */
	virtual void OnCreate();
	/* Called when scene is deleted */
	virtual void OnDelete();
};

///////////////////////////////////////////////////////////////////////////////

#endif