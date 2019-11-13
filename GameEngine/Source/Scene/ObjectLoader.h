#ifndef OBJECT_LOADER_H
#define OBJECT_LOADER_H

#include <Core/TypeInfo.h>
#include <Core/Array.h>

#include <Math/Vector2.h>
#include <Math/BoundingBox.h>

///////////////////////////////////////////////////////////////////////////////

class Scene;
class Renderer;
class Renderable;

///////////////////////////////////////////////////////////////////////////////

class ObjectChunk
{
public:
	ObjectChunk() = default;
	ObjectChunk(const Vector2f& s, const Vector2f& e);

	/* Add renderable to chunk */
	void AddRenderable(Renderable* object);
	/* Marks chunk as loaded */
	void MarkLoaded();

	/* Get list of renderables */
	const Array<Renderable*>& GetRenderables() const;
	/* Get bounding box */
	const BoundingBox& GetBoundingBox() const;
	/* Returns true if chunk has been loaded */
	bool IsLoaded() const;

private:
	/* List of renderables */
	Array<Renderable*> mRenderables;
	/* The chunk bounding box */
	BoundingBox mBoundingBox;
	/* True if chunk has been loaded */
	bool mIsLoaded;
};

///////////////////////////////////////////////////////////////////////////////

/* Base class of loader used to handle loading in
   very large worlds (i.e. grass, trees, rocks, water chunks, etc.)
   It can only handle static rendrables */
class ObjectLoader
{
	REQUIRES_TYPE_INFO;

public:
	ObjectLoader();
	virtual ~ObjectLoader();

	/* Initialize loader */
	void Init(Scene* scene);
	/* Update loader */
	void Update();

protected:
	/* Access to scene */
	Scene* mScene;
	/* Access to renderer */
	Renderer* mRenderer;

	/* Chunk size */
	float mChunkSize;
	/* Load range */
	float mLoadRange;
	/* Unload range */
	float mUnloadRange;
	/* Previous camera position */
	Vector2f mPrevPos;

private:
	/* Called when loader is initialized */
	virtual void OnInit();
	/* Called when chunk enters range */
	virtual void OnChunkEnter(ObjectChunk& chunk) = 0;
	/* Called when chunk leaves range */
	virtual void OnChunkLeave(ObjectChunk& chunk) = 0;

	/* Update chunks in list */
	void UpdateChunks();

private:
	/* List of chunks */
	Array<ObjectChunk> mChunks;
};

///////////////////////////////////////////////////////////////////////////////

#endif