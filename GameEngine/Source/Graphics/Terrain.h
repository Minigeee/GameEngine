#ifndef TERRAIN_H
#define TERRAIN_H

#include <Core/Array.h>

#include <Scene/GameObject.h>

#include <Scene/Components.h>
#include <Graphics/Components.h>

///////////////////////////////////////////////////////////////////////////////

class Texture;
class Image;
class Scene;

///////////////////////////////////////////////////////////////////////////////

/* Game object used to render terrain */
class TerrainObject : public GameObject
{
	GAME_OBJECT(TerrainObject);

	REGISTER_COMPONENTS(
		TransformComponent,
		RenderComponent
	);

	REGISTER_TAGS();
};

///////////////////////////////////////////////////////////////////////////////

class Terrain
{
public:
	Terrain();
	~Terrain();

	/* Set number of LOD levels */
	void SetLodLevels(const Array<float>& lod);
	/* Set square size at LOD-0 */
	void SetSquareSize(float size);
	/* Create terrain based on set options */
	void Create(Scene* scene);

	/* Set size of terrain */
	void SetSize(float size);
	/* Set max height */
	void SetMaxHeight(float h);
	/* Set height map */
	void SetHeightMap(Texture* map);
	/* Set color map */
	void SetColorMap(Texture* map);

	/* Get terrain size */
	float GetSize() const;
	/* Get square size */
	float GetSquareSize() const;
	/* Get max height */
	float GetMaxHeight() const;
	/* Get terrain height map */
	Texture* GetHeightMap() const;
	/* Get terrain color map */
	Texture* GetColorMap() const;

private:
	/* ID of terrain object */
	GameObjectID mObjectID;
	/* Lod distances */
	Array<float> mLodLevels;
	/* Square size at LOD-0 */
	float mSquareSize;
	/* Scene access */
	Scene* mScene;

	/* Height map */
	Texture* mHeightMap;
	/* Color map */
	Texture* mColorMap;
	/* Size of terrain (square) */
	float mSize;
	/* Maximum height of terrain */
	float mMaxHeight;
};

///////////////////////////////////////////////////////////////////////////////

#endif