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
class FrameBuffer;

class Renderer
{
public:
	Renderer();
	~Renderer();

	/* Initialize renderer */
	void Init();
	/* Render stuff */
	void Render(FrameBuffer* fb = 0);

	/* Set scene to render */
	void SetScene(Scene* scene);

	/* Register model */
	void RegisterModel(Model* model, bool isStatic = true, Uint32 num = 64);
	/* Add static renderable object */
	void AddStatic(Renderable* object);
	/* Mark static object for update */
	void UpdateStatic(Renderable* object);
	/* Add dynamic renderable object */
	void AddDynamic(Renderable* object);

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

	struct StaticRenderData
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

	struct DynamicRenderData
	{
		/* List of renderables */
		Array<Renderable*> mRenderables;
		/* Offset into instance buffer (in bytes) */
		Uint32 mDataOffset;
	};

	/* Render queue */
	void RenderQueue(Array<VertexArrayData>& queue);
	/* Update render queue */
	void UpdateQueue(bool isStatic);
	/* Pre-render update */
	void Update();

private:
	/* Scene to render */
	Scene* mScene;

	/* List of vertex arrays */
	Array<VertexArrayData> mVaoData;
	/* Map vertex array to render data */
	Array<int> mDataMap;
	/* Static render data */
	Array<StaticRenderData> mStaticRenderData;
	/* Dynamic render data */
	Array<DynamicRenderData> mDynamicRenderData;
	/* Static render queue */
	Array<VertexArrayData> mStaticQueue;
	/* Dynamic render queue */
	Array<VertexArrayData> mDynamicQueue;
	/* List of static labels*/
	Array<bool> mIsStatic;

	/* Dynamic instance buffer */
	VertexBuffer* mDynamicBuffer;
	/* Dynamic buffer size */
	Uint32 mDynamicSize;
	/* Dynamic buffer current offset */
	Uint32 mDynamicOffset;
	/* Keep track of number of dynamic objects */
	Uint32 mNumDynamic;

	/* Marks if render data updated */
	bool mStaticDataUpdated;
};

///////////////////////////////////////////////////////////////////////////////

#endif