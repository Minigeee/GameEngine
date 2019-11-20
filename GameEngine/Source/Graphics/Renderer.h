#ifndef RENDERER_H
#define RENDERER_H

#include <Core/HandleArray.h>

#include <Math/Vector3.h>
#include <Math/Matrix4.h>
#include <Math/BoundingBox.h>
#include <Math/Frustum.h>

#include <Graphics/Components.h>
#include <Graphics/RenderPass.h>

#include <Scene/Components.h>

#include <unordered_map>

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

#define DYNAMIC_INSTANCE_BUFFER_SIZE 1024 * 1024

class Scene;

class VertexArray;
class VertexBuffer;
class Material;
class Shader;
class Mesh;
class Model;
class FrameBuffer;

class Camera;

///////////////////////////////////////////////////////////////////////////////

struct RenderChunk
{
	/* List of transform matrices */
	HandleArray<Matrix4f> mTransforms;
	/* Instance buffer w/ transform data */
	VertexBuffer* mInstanceBuffer;
	/* The current size of instance buffer */
	Uint32 mBufferSize;

	/* Bounding box of chunk */
	BoundingBox mBoundingBox;
	/* True if chunk has been updated */
	bool mUpdated;
};

///////////////////////////////////////////////////////////////////////////////

struct StaticRenderData
{
public:
	/* Array of chunks */
	HandleArray<RenderChunk> mRenderChunks;
	/* Map chunk index hash to chunk handle */
	std::unordered_map<Uint32, Handle> mIndexToHandle;
	/* List of visible chunks (reconstructed every frame) */
	Array<Uint32> mVisibleChunks;

	/* Chunk size */
	float mChunkSize;
	/* True if culling is enabled for this model */
	bool mCullable;
};

///////////////////////////////////////////////////////////////////////////////

struct DynamicRenderData
{
public:
	/* Type ID of renderables */
	Uint32 mTypeID;
	/* Matrix offset in instance buffer */
	Uint32 mInstanceOffset;
	/* Number of visible instances */
	Uint32 mNumVisible;
};

///////////////////////////////////////////////////////////////////////////////

struct RenderData
{
	/* Used to draw mesh */
	VertexArray* mVertexArray;
	/* Material used for mesh */
	Material* mMaterial;
	/* Shader for convenience */
	Shader* mShader;

	/* Number of vertices in mesh */
	Uint32 mNumVertices;
	/* Instance data */
	Uint32 mDataIndex;
};

///////////////////////////////////////////////////////////////////////////////

struct CommonUniforms
{
public:
	/* Projection-view matrix */
	Matrix4f mProjView;
	/* Camera object */
	Camera* mCamera;

	/* Bind all common uniforms */
	void ApplyToShader(Shader* shader);
};

///////////////////////////////////////////////////////////////////////////////
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

	/* Render scene */
	void Render(FrameBuffer* target);

	/* Register model for static renderables */
	Uint32 RegisterStaticModel(Model* model, float chunkSize, bool cullable = true);
	/* Register model for dynamic renderables */
	Uint32 RegisterDynamicModel(Model* model);
	/* Add static renderable object */
	Uint64 AddStaticObject(const TransformComponent& t, RenderComponent& r);
	/* Remove static renderable object */
	void RemoveStaticObject(RenderComponent& r);
	/* Register dynamic object type */
	void RegisterDynamicType(Uint32 typeID, Model* model);
	/* Register dynamic object type */
	template <typename T> void RegisterDynamicType(Model* model) { RegisterDynamicType(T::StaticTypeID(), model); }

	/* Add a chunk of static renderables */
	void AddStaticChunk(const TransformComponent* t, RenderComponent* r, Uint32 n, const BoundingBox& box);
	/* Remove render chunk that contains the given point */
	void RemoveStaticChunk(Model* model, const Vector3f& pos);

	/* Add a render pass (Pass lighting pass type as template parameter) */
	template <typename L> RenderPass* AddRenderPass(RenderPass::Type type)
	{
		RenderPass* pass = new RenderPass(type);
		pass->SetLightingPass(new L(mScene));
		mRenderPasses.Push(pass);

		return pass;
	}

private:
	/* Add render data to a queue */
	void AddRenderData(const RenderData& data, Array<RenderData>& queue);

	/* Do any pre-render updates */
	void Update();
	/* Update (cull) static objects */
	void UpdateStatic(const Frustum& frustum);
	/* Update (cull) dynamic objects */
	void UpdateDynamic(const Frustum& frustum);

	/* Do a render pass */
	void DoRenderPass(RenderPass* pass, FrameBuffer* target);
	/* Render static objects */
	void RenderStatic(RenderPass* pass, CommonUniforms& uniforms);
	/* Render dynamic objects */
	void RenderDynamic(RenderPass* pass, CommonUniforms& uniforms);

private:
	/* Scene to render */
	Scene* mScene;

	/* List of static render data */
	Array<StaticRenderData> mStaticRenderData;
	/* List of dynamic render data */
	Array<DynamicRenderData> mDynamicRenderData;
	/* Map model pointer to render data index */
	std::unordered_map<Model*, Uint32> mModelToDataIndex;

	/* List of render passes */
	Array<RenderPass*> mRenderPasses;
	/* Static render queue */
	Array<RenderData> mStaticQueue;
	/* Dynamic render queue */
	Array<RenderData> mDynamicQueue;

	/* G-buffer for deffered lighting */
	FrameBuffer* mGBuffer;
	/* Quad vertex array */
	VertexArray* mQuadVao;
	/* Quad vertex buffer */
	VertexBuffer* mQuadVbo;

	/* Dynamic instance buffer */
	VertexBuffer* mDynamicBuffer;
	/* Dynamic buffer size */
	Uint32 mDynBufferSize;
	/* Dynamic buffer offset */
	Uint32 mDynBufferOffset;
};

///////////////////////////////////////////////////////////////////////////////

#endif