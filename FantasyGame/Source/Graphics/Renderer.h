#ifndef RENDERER_H
#define RENDERER_H

#include <Core/Array.h>

#include <Math/Matrix4.h>

///////////////////////////////////////////////////////////////////////////////

class VertexArray;
class VertexBuffer;
class Renderable;
class Material;
class Model;
class Scene;

class Renderer
{
public:
	Renderer();
	~Renderer();

	/* Initialize renderer */
	void Init();
	/* Render stuff */
	void Render();

	/* Set scene to render */
	void SetScene(Scene* scene);

	/* Register model */
	void RegisterModel(Model* model, Uint32 num = 64);
	/* Add static renderable object */
	void AddStatic(Renderable* object);
	/* Mark static object for update */
	void UpdateStatic(Renderable* object);

private:
	struct VertexArrayData
	{
		/* Vertex array to render */
		VertexArray* mVertexArray;
		/* Material to render */
		Material* mMaterial;
		/* How many vertices to render */
		Uint32 mNumVertices;
	};

	struct RenderData
	{
		/* Transform matrices */
		Array<Matrix4f> mTransforms;
		/* Instance data buffer */
		VertexBuffer* mInstanceBuffer;
		/* Keeps track of buffer size */
		Uint32 mBufferSize;

		/* Marks if needs update */
		bool mNeedsUpdate;
	};

	/* Update render queue */
	void UpdateQueue();
	/* Pre-render update */
	void Update();

private:
	/* Scene to render */
	Scene* mScene;

	/* List of vertex arrays */
	Array<VertexArrayData> mVaoData;
	/* Map vertex array to render data */
	Array<int> mDataMap;
	/* Render data */
	Array<RenderData> mRenderData;
	/* Render queue */
	Array<VertexArrayData> mRenderQueue;

	/* Marks if render data updated */
	bool mRenderDataUpdated;
};

///////////////////////////////////////////////////////////////////////////////

#endif