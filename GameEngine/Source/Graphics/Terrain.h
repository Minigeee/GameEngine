#ifndef TERRAIN_H
#define TERRAIN_H

#include <Core/Array.h>

#include <Graphics/Renderable.h>

///////////////////////////////////////////////////////////////////////////////

class Texture;
class Image;

class Terrain : public Renderable
{
public:
	Terrain();
	~Terrain();

	/* Set number of LOD levels */
	void SetLodLevels(const Array<float>& lod);
	/* Set square size at LOD-0 */
	void SetSquareSize(float size);
	/* Create terrain based on set options */
	void Create();

	/* Set size of terrain */
	void SetSize(float size);
	/* Set height map */
	void SetHeightMap(Texture* map);
	/* Set color map */
	void SetColorMap(Texture* map);

private:
	/* Lod distances */
	Array<float> mLodLevels;
	/* Square size at LOD-0 */
	float mSquareSize;

	/* Height map */
	Texture* mHeightMap;
	/* Color map */
	Texture* mColorMap;
	/* Size of terrain (square) */
	float mSize;
};

///////////////////////////////////////////////////////////////////////////////

#endif