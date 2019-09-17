#ifndef RENDERER_H
#define RENDERER_H

#include <Core/Array.h>

#include <Math/Matrix4.h>

#include <Graphics/RenderPass.h>

#include <unordered_map>

///////////////////////////////////////////////////////////////////////////////

class VertexArray;
class VertexBuffer;
class Renderable;
class Material;
class Model;
class Scene;
class FrameBuffer;

///////////////////////////////////////////////////////////////////////////////

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

///////////////////////////////////////////////////////////////////////////////

class Renderer
{
public:
	Renderer();
	~Renderer();

	/* Initialize renderer */
	void Init(Scene* scene);
	/* Post initialization */
	void PostInit();
	/* Render stuff */
	void Render(FrameBuffer* fb = 0);

	/* Add render pass (Use new operator. Object is automatically destroyed by renderer) */
	void AddRenderPass(RenderPass* pass, const char* lighting_name = 0);
	/* Add lighting pass (Use new operator. Object is automatically destroyed by renderer) */
	void AddLightingPass(LightingPass* lighting, const char* name);

	/* Register model */
	void RegisterModel(Model* model, bool isStatic = true, Uint32 num = 64);
	/* Add static renderable object */
	void AddStatic(Renderable* object);
	/* Mark static object for update */
	void UpdateStatic(Renderable* object);
	/* Add dynamic renderable object */
	void AddDynamic(Renderable* object);

private:
	/* Update render queue */
	void UpdateQueue(bool isStatic);
	/* Pre-render update */
	void Update();

	/* Do a render pass */
	void DoRenderPass(RenderPass& pass, FrameBuffer* out = 0);

private:
	/* Scene to render */
	Scene* mScene;
	/* List of render passes */
	Array<RenderPass*> mRenderPasses;
	/* Map of available lighting passes */
	std::unordered_map<const char*, LightingPass*> mLightingPasses;

	/* G-buffer for deffered rendering */
	FrameBuffer* mGBuffer;
	/* Vertex array for rendering lighting pass */
	VertexArray* mQuadVao;
	/* Vertex buffer for rendering lighting pass */
	VertexBuffer* mQuadVbo;

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