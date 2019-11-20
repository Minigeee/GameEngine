#include <Graphics/Renderer.h>

#include <Core/Hash.h>

#include <Math/Transform.h>

#include <Graphics/Graphics.h>
#include <Graphics/VertexArray.h>
#include <Graphics/VertexBuffer.h>
#include <Graphics/FrameBuffer.h>
#include <Graphics/Model.h>
#include <Graphics/Material.h>
#include <Graphics/Shader.h>

#include <Resource/Resource.h>

#include <Scene/Scene.h>

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

Renderer::Renderer()
{

}

Renderer::~Renderer()
{
	for (Uint32 i = 0; i < mRenderPasses.Size(); ++i)
		delete mRenderPasses[i];

	Resource<FrameBuffer>::Free(mGBuffer);
	Resource<VertexArray>::Free(mQuadVao);
	Resource<VertexBuffer>::Free(mQuadVbo);
	Resource<VertexBuffer>::Free(mDynamicBuffer);

	// Free chunk instance buffers
	for (Uint32 i = 0; i < mStaticRenderData.Size(); ++i)
	{
		StaticRenderData& data = mStaticRenderData[i];
		Array<RenderChunk>& chunks = data.mRenderChunks.GetData();

		for (Uint32 chunk_n = 0; chunk_n < chunks.Size(); ++chunk_n)
			Resource<VertexBuffer>::Free(chunks[chunk_n].mInstanceBuffer);
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

void Renderer::Init(Scene* scene)
{
	mScene = scene;

	// Containers
	mStaticRenderData.Reserve(32);
	mDynamicRenderData.Reserve(32);
	mStaticQueue.Reserve(32);
	mDynamicQueue.Reserve(32);
	mRenderPasses.Reserve(4);


	// G-buffer
	mGBuffer = Resource<FrameBuffer>::Create();
	mGBuffer->Bind();

	FrameBuffer::TextureOptions options;
	const Vector3i& size = FrameBuffer::Default.GetSize();
	mGBuffer->SetSize(size.x, size.y);

	// Position
	options.mDimensions = Texture::_2D;
	options.mFormat = Texture::Rgb;
	options.mDataType = Image::Float;
	mGBuffer->AttachColor(true, options);
	// Normals + Specular factor
	options.mDimensions = Texture::_2D;
	options.mFormat = Texture::Rgba;
	options.mDataType = Image::Ushort;
	mGBuffer->AttachColor(true, options);
	// Albedo
	options.mDimensions = Texture::_2D;
	options.mFormat = Texture::Rgb;
	options.mDataType = Image::Ubyte;
	mGBuffer->AttachColor(true, options);
	// Specular
	options.mDimensions = Texture::_2D;
	options.mFormat = Texture::Rgb;
	options.mDataType = Image::Ubyte;
	mGBuffer->AttachColor(true, options);
	// Depth
	mGBuffer->AttachDepth(true);


	// Create quad
	float verts[] =
	{
		-1.0f,  1.0f,
		-1.0f, -1.0f,
		 1.0f,  1.0f,

		-1.0f, -1.0f,
		 1.0f, -1.0f,
		 1.0f,  1.0f
	};

	mQuadVbo = Resource<VertexBuffer>::Create();
	mQuadVbo->Bind(VertexBuffer::Array);
	mQuadVbo->BufferData(verts, sizeof(verts), VertexBuffer::Static);

	mQuadVao = Resource<VertexArray>::Create();
	mQuadVao->Bind();
	mQuadVao->VertexAttrib(0, 2);


	// Create dynamic instance buffer
	mDynamicBuffer = Resource<VertexBuffer>::Create();
	mDynamicBuffer->Bind(VertexBuffer::Array);
	mDynamicBuffer->BufferData(NULL, DYNAMIC_INSTANCE_BUFFER_SIZE, VertexBuffer::Stream);
	mDynBufferSize = DYNAMIC_INSTANCE_BUFFER_SIZE / sizeof(Matrix4f);
	mDynBufferOffset = 0;
}

///////////////////////////////////////////////////////////////////////////////

void Renderer::PostInit()
{
	// Create default rendering order, if no custom rendering order
	if (!mRenderPasses.Size())
	{
		AddRenderPass<DefaultLighting>(RenderPass::Normal);
	}

	// Create target framebuffers if necessary (Except for the last one)
	for (Uint32 i = 0; i < mRenderPasses.Size() - 1; ++i)
	{
		RenderPass* pass = mRenderPasses[i];

		if (!pass->GetTarget())
			pass->CreateTarget();
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

void Renderer::Update()
{
	// Get camera frustum
	Frustum frustum = mScene->GetCamera().GetFrustum();

	UpdateStatic(frustum);
	UpdateDynamic(frustum);
}

///////////////////////////////////////////////////////////////////////////////

void Renderer::UpdateStatic(const Frustum& frustum)
{
	for (Uint32 i = 0; i < mStaticRenderData.Size(); ++i)
	{
		StaticRenderData& data = mStaticRenderData[i];
		Array<RenderChunk>& chunks = data.mRenderChunks.GetData();

		// Clear list of visible chunks
		data.mVisibleChunks.Clear();

		for (Uint32 chunk_n = 0; chunk_n < chunks.Size(); ++chunk_n)
		{
			RenderChunk& chunk = chunks[chunk_n];

			// Update instance buffer if needed
			if (chunk.mUpdated)
			{
				Uint32 size = chunk.mTransforms.Size();

				// If there is not enough space in instance buffer
				if (size > chunk.mBufferSize)
				{
					// Recreate (allocate new) buffer
					chunk.mInstanceBuffer->Bind(VertexBuffer::Array);
					chunk.mInstanceBuffer->BufferData(
						&chunk.mTransforms.GetData().Front(),
						size * sizeof(Matrix4f),
						VertexBuffer::Static
					);

					chunk.mBufferSize = size;
				}
				else
				{
					// Otherwise, just update the buffer
					chunk.mInstanceBuffer->Bind(VertexBuffer::Array);
					chunk.mInstanceBuffer->UpdateData(
						&chunk.mTransforms.GetData().Front(),
						size * sizeof(Matrix4f)
					);
				}

				chunk.mUpdated = false;
			}


			// If chunk is visible, add it to the list
			if (true)
				data.mVisibleChunks.Push(chunk_n);
		}
	}
}

///////////////////////////////////////////////////////////////////////////////

void Renderer::UpdateDynamic(const Frustum& frustum)
{
	// Count number of dynamic instances
	Array<ComponentList<RenderComponent>> rs(mDynamicRenderData.Size());
	Uint32 numInstances = 0;

	for (Uint32 i = 0; i < mDynamicRenderData.Size(); ++i)
	{
		DynamicRenderData& data = mDynamicRenderData[i];

		// Create component list
		const Array<RenderComponent>& list = ComponentData<RenderComponent>::GetData(data.mTypeID);
		ComponentList<RenderComponent> components;
		components.mData = &list.Front();
		components.mSize = list.Size();
		rs.Push(components);

		// Add number of instances;
		numInstances += components.mSize;
	}

	// Map instance buffer
	mDynamicBuffer->Bind(VertexBuffer::Array);
	Matrix4f* buffer = 0;
	if (mDynBufferOffset + numInstances > mDynBufferSize)
	{
		// Reset buffer
		buffer = (Matrix4f*)mDynamicBuffer->MapWrite(
			numInstances * sizeof(Matrix4f),
			VertexBuffer::InvalidateBuffer,
			0);
		mDynBufferOffset = 0;
	}
	else
	{
		// Unsynchronized for speed
		buffer = (Matrix4f*)mDynamicBuffer->MapWrite(
			numInstances * sizeof(Matrix4f),
			VertexBuffer::Unsynchronized,
			mDynBufferOffset * sizeof(Matrix4f));
	}


	// Iterate dynamic render data
	for (Uint32 i = 0; i < mDynamicRenderData.Size(); ++i)
	{
		DynamicRenderData& data = mDynamicRenderData[i];
		ComponentList<RenderComponent>& r = rs[i];
		Uint32 numVisible = 0;

		// Set data offset
		data.mInstanceOffset = mDynBufferOffset;

		// Iterate renderables
		for (Uint32 n = 0; n < r.mSize; ++n)
		{
			// If visible, add transform
			if (frustum.Contains(r[n].mBoundingSphere))
				buffer[numVisible++] = r[n].mTransform;
		}

		// Set number of visible instances
		data.mNumVisible = numVisible;

		// Update buffer offset
		mDynBufferOffset += numVisible;
		buffer += numVisible;
	}

	// Unbind buffer
	mDynamicBuffer->Unmap();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

void CommonUniforms::ApplyToShader(Shader* shader)
{
	shader->SetUniform("mProjView", mProjView);
	shader->SetUniform("mCamPos", mCamera->GetPosition());
}

///////////////////////////////////////////////////////////////////////////////

void Renderer::Render(FrameBuffer* target)
{
	// Update stuff
	Update();

	for (Uint32 i = 0; i < mRenderPasses.Size(); ++i)
	{
		RenderPass* pass = mRenderPasses[i];
		FrameBuffer* fbuffer = pass->GetTarget();
		if (i == mRenderPasses.Size() - 1)
		{
			if (target)
				fbuffer = target;
			else
				fbuffer = &FrameBuffer::Default;
		}

		DoRenderPass(pass, fbuffer);
	}
}

///////////////////////////////////////////////////////////////////////////////

void Renderer::DoRenderPass(RenderPass* pass, FrameBuffer* target)
{
	// Bind G-buffer
	mGBuffer->Bind();

	// Set up render state
	Graphics::Enable(Graphics::DepthTest);
	Graphics::EnableCull(Graphics::Back);
	Graphics::Clear();

	// Get all common uniforms
	CommonUniforms uniforms;
	uniforms.mCamera = &mScene->GetCamera();
	uniforms.mProjView = uniforms.mCamera->GetProjection() * uniforms.mCamera->GetView();

	// Render static objects
	RenderStatic(pass, uniforms);
	// Render dynamic objects
	RenderDynamic(pass, uniforms);


	// Combine into final image

	// Setup lighting pass
	pass->GetLightingPass()->RenderSetup(mGBuffer);

	// Disable depth test for quad render
	Graphics::Disable(Graphics::DepthTest);

	// Bind target framebuffer
	if (target)
		target->Bind();
	else
		FrameBuffer::Default.Bind();

	// Clear buffers
	Graphics::Clear();

	// Draw quad
	mQuadVao->Bind();
	mQuadVao->DrawArrays(6);
}

///////////////////////////////////////////////////////////////////////////////

void Renderer::RenderStatic(RenderPass* pass, CommonUniforms& uniforms)
{
	if (!mStaticQueue.Size()) return;

	// Get first shader
	Shader* shader = mStaticQueue.Front().mShader;
	shader->Bind();
	uniforms.ApplyToShader(shader);

	// Iterate static queue
	for (Uint32 i = 0; i < mStaticQueue.Size(); ++i)
	{
		RenderData& renderData = mStaticQueue[i];

		// Change shaders if needed
		if (renderData.mShader != shader)
		{
			shader = renderData.mShader;
			shader->Bind();
			uniforms.ApplyToShader(shader);
		}

		// Test view mask and skip render if necessary
		if (pass->GetType() & renderData.mMaterial->mViewMask)
		{
			// Apply material
			renderData.mMaterial->Use();
			shader->ApplyUniforms();

			// Bind vertex array
			renderData.mVertexArray->Bind();

			// Render all visible chunks
			StaticRenderData& data = mStaticRenderData[renderData.mDataIndex];
			Array<RenderChunk>& chunks = data.mRenderChunks.GetData();

			for (Uint32 chunk_n = 0; chunk_n < data.mVisibleChunks.Size(); ++chunk_n)
			{
				RenderChunk& chunk = chunks[data.mVisibleChunks[chunk_n]];

				// Bind instance buffer
				chunk.mInstanceBuffer->Bind(VertexBuffer::Array);
				renderData.mVertexArray->VertexAttrib(4, 4, sizeof(Matrix4f), 0 * sizeof(Vector4f), 1);
				renderData.mVertexArray->VertexAttrib(5, 4, sizeof(Matrix4f), 1 * sizeof(Vector4f), 1);
				renderData.mVertexArray->VertexAttrib(6, 4, sizeof(Matrix4f), 2 * sizeof(Vector4f), 1);
				renderData.mVertexArray->VertexAttrib(7, 4, sizeof(Matrix4f), 3 * sizeof(Vector4f), 1);

				// Draw objects
				renderData.mVertexArray->DrawArrays(renderData.mNumVertices, chunk.mBufferSize);
			}
		}
	}
}

///////////////////////////////////////////////////////////////////////////////

void Renderer::RenderDynamic(RenderPass* pass, CommonUniforms& uniforms)
{
	if (!mDynamicQueue.Size()) return;

	mDynamicBuffer->Bind(VertexBuffer::Array);

	// Get first shader
	Shader* shader = mDynamicQueue.Front().mShader;
	shader->Bind();
	uniforms.ApplyToShader(shader);

	// Iterate dynamic queue
	for (Uint32 i = 0; i < mDynamicQueue.Size(); ++i)
	{
		RenderData& renderData = mDynamicQueue[i];

		// Change shaders if needed
		if (renderData.mShader != shader)
		{
			shader = renderData.mShader;
			shader->Bind();
			uniforms.ApplyToShader(shader);
		}

		// Test view mask and skip render if necessary
		if (pass->GetType() & renderData.mMaterial->mViewMask)
		{
			// Apply material
			renderData.mMaterial->Use();
			shader->ApplyUniforms();

			// Bind vertex array
			DynamicRenderData& data = mDynamicRenderData[renderData.mDataIndex];
			Uint32 offset = data.mInstanceOffset * sizeof(Matrix4f);

			renderData.mVertexArray->Bind();
			renderData.mVertexArray->VertexAttrib(4, 4, sizeof(Matrix4f), offset + 0 * sizeof(Vector4f), 1);
			renderData.mVertexArray->VertexAttrib(5, 4, sizeof(Matrix4f), offset + 1 * sizeof(Vector4f), 1);
			renderData.mVertexArray->VertexAttrib(6, 4, sizeof(Matrix4f), offset + 2 * sizeof(Vector4f), 1);
			renderData.mVertexArray->VertexAttrib(7, 4, sizeof(Matrix4f), offset + 3 * sizeof(Vector4f), 1);

			// Render instances
			renderData.mVertexArray->DrawArrays(renderData.mNumVertices, data.mNumVisible);
		}
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

void Renderer::AddRenderData(const RenderData& data, Array<RenderData>& queue)
{
	// Add data to end of queue
	queue.Push(data);

	// Return if this is the first data item
	if (queue.Size() == 1) return;

	// Start with second to last item
	int dst = queue.Size() - 1;
	int src = queue.Size() - 2;
	Shader* shader = queue[src].mShader;

	while (src > 0)
	{
		// If target shader group has been found, quit
		if (shader == data.mShader) break;

		// Find beggining of shader group
		for (; src >= 0 && shader == queue[src].mShader; --src);

		// Move item at (src + 1) to dst
		queue[dst] = queue[src + 1];
		// Update dst to (src + 1)
		dst = src + 1;

		// Update current shader
		if (src >= 0)
			shader = queue[src].mShader;
	}

	// Dst now points to target location
	queue[dst] = data;
}

///////////////////////////////////////////////////////////////////////////////

Uint32 Renderer::RegisterStaticModel(Model* model, float chunkSize, bool cullable)
{
	auto it = mModelToDataIndex.find(model);
	// If ID exists, quit
	if (it != mModelToDataIndex.end()) return it->second;

	StaticRenderData data;
	data.mRenderChunks.Reserve(256);
	data.mVisibleChunks.Reserve(64);
	data.mChunkSize = chunkSize;
	data.mCullable = cullable;

	// Map model pointer to index
	Uint32 id = mStaticRenderData.Size();
	mModelToDataIndex[model] = id;

	// Add data to list
	mStaticRenderData.Push(std::move(data));

	// Add all meshes to queue
	for (Uint32 i = 0; i < model->GetNumMeshes(); ++i)
	{
		Mesh& mesh = model->GetMesh(i);

		// Create render data
		RenderData renderData;
		renderData.mDataIndex = id;
		renderData.mNumVertices = mesh.mNumVertices;
		renderData.mVertexArray = mesh.mVertexArray;
		renderData.mMaterial = mesh.mMaterial;
		renderData.mShader = mesh.mMaterial->mShader;

		// Add to queue
		AddRenderData(renderData, mStaticQueue);
	}

	return id;
}

///////////////////////////////////////////////////////////////////////////////

Uint64 Renderer::AddStaticObject(const TransformComponent& t, RenderComponent& r)
{
	// Don't add if object is already added
	if (r.mInstanceID) return 0;

	/* TODO : Handle LOD models */

	int modelID = 0;
	{
		auto it = mModelToDataIndex.find(r.mModel);
		// If model group does not exist, create it
		if (it == mModelToDataIndex.end())
			modelID = RegisterStaticModel(r.mModel, 32.0f);
		else
			modelID = it->second;
	}

	// Get model group
	StaticRenderData& data = mStaticRenderData[modelID];


	// Get chunk handle
	Vector3i index = Floor(t.mPosition / data.mChunkSize);
	Uint32 indexHash = GetHash(&index, sizeof(index));
	Handle chunkHandle = 0;

	{
		auto it = data.mIndexToHandle.find(indexHash);
		if (it == data.mIndexToHandle.end())
		{
			// Create new chunk if it doesn't exist
			RenderChunk chunk;
			chunk.mTransforms.Reserve(4);
			chunk.mInstanceBuffer = Resource<VertexBuffer>::Create();
			chunk.mBufferSize = 0;
			chunk.mUpdated = false;
			chunk.mBoundingBox.mMax = (Vector3f)(index + 1) * data.mChunkSize;
			chunk.mBoundingBox.mMin = (Vector3f)(index)*data.mChunkSize;

			// Add chunk to handled array
			chunkHandle = data.mRenderChunks.Add(std::move(chunk));
			data.mIndexToHandle[indexHash] = chunkHandle;
		}
		else
			chunkHandle = it->second;
	}

	// Get render chunk
	RenderChunk& chunk = data.mRenderChunks[chunkHandle];


	// Add transform to list
	Handle transformHandle = chunk.mTransforms.Add(
		ToTransform(t.mPosition, t.mRotation, t.mScale)
	);

	// Update chunk bounding box
	const BoundingBox& modelBox = r.mModel->GetBoundingBox();
	Vector3f boxPos = modelBox.GetPosition();
	r.mBoundingSphere.p = boxPos + t.mPosition;
	r.mBoundingSphere.r = Distance(boxPos, modelBox.mMin) * t.mScale;

	const BoundingSphere& sphere = r.mBoundingSphere;
	BoundingBox box(sphere.p - sphere.r, sphere.p + sphere.r);

	if (box.mMin.x < chunk.mBoundingBox.mMin.x)
		chunk.mBoundingBox.mMin.x = box.mMin.x;
	if (box.mMax.x > chunk.mBoundingBox.mMax.x)
		chunk.mBoundingBox.mMax.x = box.mMax.x;

	if (box.mMin.y < chunk.mBoundingBox.mMin.y)
		chunk.mBoundingBox.mMin.y = box.mMin.y;
	if (box.mMax.y > chunk.mBoundingBox.mMax.y)
		chunk.mBoundingBox.mMax.y = box.mMax.y;

	if (box.mMin.z < chunk.mBoundingBox.mMin.z)
		chunk.mBoundingBox.mMin.z = box.mMin.z;
	if (box.mMax.z > chunk.mBoundingBox.mMax.z)
		chunk.mBoundingBox.mMax.z = box.mMax.z;

	// Set instance ID
	Uint64 instanceID = (indexHash << 16) | transformHandle;
	r.mInstanceID = instanceID + 1;

	// Mark for update
	chunk.mUpdated = true;

	return instanceID;
}

///////////////////////////////////////////////////////////////////////////////

void Renderer::RemoveStaticObject(RenderComponent& r)
{
	if (!r.mInstanceID) return;

	Uint64 instanceID = r.mInstanceID - 1;
	Uint32 indexHash = (Uint32)(instanceID >> 16);
	Handle transformHandle = (Handle)(instanceID - indexHash);

	// Get model group
	int modelID = 0;
	{
		auto it = mModelToDataIndex.find(r.mModel);
		// If model group does not exist, quit
		if (it == mModelToDataIndex.end())
			return;
		else
			modelID = it->second;
	}
	StaticRenderData& data = mStaticRenderData[modelID];

	// Get chunk
	Handle chunkHandle = 0;
	{
		auto it = data.mIndexToHandle.find(indexHash);
		if (it == data.mIndexToHandle.end())
		{
			// If chunk doesn't exist (if chunk was removed already), then reset instance ID and quit
			r.mInstanceID = 0;
			return;
		}
		else
			chunkHandle = it->second;
	}
	RenderChunk& chunk = data.mRenderChunks[chunkHandle];

	// Remove transform
	chunk.mTransforms.Remove(transformHandle);

	// If there are no transforms left, remove chunk
	if (!chunk.mTransforms.Size())
	{
		Resource<VertexBuffer>::Free(chunk.mInstanceBuffer);
		data.mRenderChunks.Remove(chunkHandle);
		data.mIndexToHandle.erase(indexHash);
	}
	else
		// Otherwise, mark it for update
		chunk.mUpdated = true;

	// Reset instance ID
	r.mInstanceID = 0;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

Uint32 Renderer::RegisterDynamicModel(Model* model)
{
	auto it = mModelToDataIndex.find(model);
	// If ID exists, quit
	if (it != mModelToDataIndex.end()) return it->second;

	DynamicRenderData data;
	data.mInstanceOffset = 0;
	data.mNumVisible = 0;

	// Map model pointer to index
	Uint32 id = mDynamicRenderData.Size();
	mModelToDataIndex[model] = id;

	// Add data to list
	mDynamicRenderData.Push(std::move(data));

	// Add all meshes to queue
	for (Uint32 i = 0; i < model->GetNumMeshes(); ++i)
	{
		Mesh& mesh = model->GetMesh(i);

		// Create render data
		RenderData renderData;
		renderData.mDataIndex = id;
		renderData.mNumVertices = mesh.mNumVertices;
		renderData.mVertexArray = mesh.mVertexArray;
		renderData.mMaterial = mesh.mMaterial;
		renderData.mShader = mesh.mMaterial->mShader;

		// Add to queue
		AddRenderData(renderData, mDynamicQueue);
	}

	return id;
}

///////////////////////////////////////////////////////////////////////////////

void Renderer::RegisterDynamicType(Uint32 typeID, Model* model)
{
	int modelID = 0;
	{
		auto it = mModelToDataIndex.find(model);
		// If model group does not exist, create it
		if (it == mModelToDataIndex.end())
			modelID = RegisterDynamicModel(model);
		else
			modelID = it->second;
	}

	// Get model group
	DynamicRenderData& data = mDynamicRenderData[modelID];
	data.mTypeID = typeID;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

void Renderer::AddStaticChunk(const TransformComponent* t, RenderComponent* r, Uint32 n, const BoundingBox& box)
{
	// Only add chunk if there are renderables to add
	if (!t || !r || !n) return;

	int modelID = 0;
	{
		auto it = mModelToDataIndex.find(r->mModel);
		// If model group does not exist, create it
		if (it == mModelToDataIndex.end())
			modelID = RegisterStaticModel(r->mModel, 32.0f);
		else
			modelID = it->second;
	}

	// Get model group
	StaticRenderData& data = mStaticRenderData[modelID];


	// Get chunk handle using bounding box position
	Vector3i index = Floor(box.GetPosition() / data.mChunkSize);
	Uint32 indexHash = GetHash(&index, sizeof(index));
	Handle chunkHandle = 0;

	{
		auto it = data.mIndexToHandle.find(indexHash);
		if (it == data.mIndexToHandle.end())
		{
			// Create new chunk if it doesn't exist
			RenderChunk chunk;
			chunk.mTransforms.Reserve(4);
			chunk.mInstanceBuffer = Resource<VertexBuffer>::Create();
			chunk.mBufferSize = 0;
			chunk.mUpdated = false;
			chunk.mBoundingBox.mMax = (Vector3f)(index + 1) * data.mChunkSize;
			chunk.mBoundingBox.mMin = (Vector3f)(index) * data.mChunkSize;

			// Add chunk to handled array
			chunkHandle = data.mRenderChunks.Add(std::move(chunk));
			data.mIndexToHandle[indexHash] = chunkHandle;
		}
		else
			chunkHandle = it->second;
	}

	// Get render chunk
	RenderChunk& chunk = data.mRenderChunks[chunkHandle];

	// Update bounding box if needed
	if (box.mMin.x < chunk.mBoundingBox.mMin.x)
		chunk.mBoundingBox.mMin.x = box.mMin.x;
	if (box.mMax.x > chunk.mBoundingBox.mMax.x)
		chunk.mBoundingBox.mMax.x = box.mMax.x;

	if (box.mMin.y < chunk.mBoundingBox.mMin.y)
		chunk.mBoundingBox.mMin.y = box.mMin.y;
	if (box.mMax.y > chunk.mBoundingBox.mMax.y)
		chunk.mBoundingBox.mMax.y = box.mMax.y;

	if (box.mMin.z < chunk.mBoundingBox.mMin.z)
		chunk.mBoundingBox.mMin.z = box.mMin.z;
	if (box.mMax.z > chunk.mBoundingBox.mMax.z)
		chunk.mBoundingBox.mMax.z = box.mMax.z;

	for (Uint32 i = 0; i < n; ++i)
	{
		Handle transformHandle = chunk.mTransforms.Add(
			ToTransform(t[i].mPosition, t[i].mRotation, t[i].mScale)
		);

		// Set instance ID
		Uint64 instanceID = (indexHash << 16) | transformHandle;
		r[i].mInstanceID = instanceID + 1;
	}

	chunk.mUpdated = true;
}

///////////////////////////////////////////////////////////////////////////////

void Renderer::RemoveStaticChunk(Model* model, const Vector3f& pos)
{
	// Get model group
	int modelID = 0;
	{
		auto it = mModelToDataIndex.find(model);
		// If model group does not exist, quit
		if (it == mModelToDataIndex.end())
			return;
		else
			modelID = it->second;
	}
	StaticRenderData& data = mStaticRenderData[modelID];

	// Get chunk handle
	Vector3i index = Floor(pos / data.mChunkSize);
	Uint32 indexHash = GetHash(&index, sizeof(index));
	Handle chunkHandle = 0;

	{
		auto it = data.mIndexToHandle.find(indexHash);
		if (it == data.mIndexToHandle.end())
			// If chunk doesn't exist, quit
			return;
		else
			chunkHandle = it->second;
	}
	RenderChunk& chunk = data.mRenderChunks[chunkHandle];

	// Remove chunk
	Resource<VertexBuffer>::Free(chunk.mInstanceBuffer);
	data.mRenderChunks.Remove(chunkHandle);
	data.mIndexToHandle.erase(indexHash);
}

///////////////////////////////////////////////////////////////////////////////