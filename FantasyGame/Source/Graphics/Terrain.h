#ifndef TERRAIN_H
#define TERRAIN_H

#include <Core/Array.h>

#include <Graphics/Renderable.h>

///////////////////////////////////////////////////////////////////////////////

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

	/* Get total size of terrain (Calculated in Create() function) */
	float GetTotalSize() const;

private:
	/* Lod distances */
	Array<float> mLodLevels;
	/* Square size at LOD-0 */
	float mSquareSize;

	/* Size of terrain (square) */
	float mTotalSize;
};

///////////////////////////////////////////////////////////////////////////////

#endif