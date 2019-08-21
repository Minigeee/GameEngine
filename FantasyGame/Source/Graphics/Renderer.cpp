#include <Graphics/Renderer.h>

#include <Resource/Resource.h>

#include <Graphics/Graphics.h>
#include <Graphics/VertexArray.h>
#include <Graphics/VertexBuffer.h>
#include <Graphics/Material.h>
#include <Graphics/Model.h>
#include <Graphics/Camera.h>
#include <Graphics/Renderable.h>
#include <Graphics/Shader.h>
#include <Graphics/Skybox.h>
#include <Graphics/FrameBuffer.h>

#include <Scene/Scene.h>

#include <map>

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

Renderer::Renderer() :
	mStaticDataUpdated		(false),
	mNumDynamic				(0)
{
	
}

Renderer::~Renderer()
{

}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

void Renderer::Init()
{
	Uint32 initSize = 64;

	mVaoData.Resize(initSize);
	mDataMap.Resize(initSize);
	mStaticRenderData.Resize(initSize);
	mDynamicRenderData.Resize(initSize);
	mStaticQueue.Resize(initSize);
	mDynamicQueue.Resize(initSize);
	mDynamicVaos.Resize(initSize);

	// Create dynamic instance buffer
	const Uint32 bufferSize = 1 * 1024 * 1024;
	mDynamicBuffer = Resource<VertexBuffer>::Create();
	mDynamicBuffer->Bind(VertexBuffer::Array);
	mDynamicBuffer->BufferData(NULL, bufferSize, VertexBuffer::Stream);
	mDynamicSize = bufferSize;
	mDynamicOffset = 0;
}

///////////////////////////////////////////////////////////////////////////////

void Renderer::Update()
{
	if (mStaticDataUpdated)
	{
		for (Uint32 i = 0; i < mStaticRenderData.Size(); ++i)
		{
			StaticRenderData& data = mStaticRenderData[i];
			if (data.mNeedsUpdate)
			{
				Uint32 size = data.mTransforms.Size();
				if (size > data.mBufferSize)
				{
					// Recreate buffer
					data.mInstanceBuffer->Bind(VertexBuffer::Array);
					data.mInstanceBuffer->BufferData(
						&data.mTransforms.Front(),
						size * sizeof(Matrix4f),
						VertexBuffer::Static
					);

					data.mBufferSize = size;
				}
				else
				{
					// Otherwise, just update data
					data.mInstanceBuffer->Bind(VertexBuffer::Array);
					data.mInstanceBuffer->UpdateData(
						&data.mTransforms.Front(),
						size * sizeof(Matrix4f)
					);
				}

				data.mNeedsUpdate = false;
			}
		}

		mStaticDataUpdated = false;
	}

	// ===========================================================================

	// Update dynamic objects
	if (!mDynamicRenderData.Size()) return;

	// Bind dynamic instance buffer
	mDynamicBuffer->Bind(VertexBuffer::Array);

	// Map buffer
	Matrix4f* buffer = 0;
	Uint32 mapSize = mNumDynamic * sizeof(Matrix4f);
	if (mDynamicOffset + mapSize > mDynamicSize)
	{
		// Unsynchronized for speed
		buffer = (Matrix4f*)mDynamicBuffer->MapWrite(
			mapSize,
			VertexBuffer::Unsynchronized,
			mDynamicOffset);
	}
	else
	{
		// Reset buffer
		buffer = (Matrix4f*)mDynamicBuffer->MapWrite(
			mapSize,
			VertexBuffer::InvalidateBuffer,
			0);
		mDynamicOffset = 0;
	}

	for (Uint32 i = 0; i < mDynamicRenderData.Size(); ++i)
	{
		// Update all transforms
		DynamicRenderData& data = mDynamicRenderData[i];
		Array<Renderable*>& objs = data.mRenderables;

		// Copy transforms
		for (Uint32 inst = 0; inst < objs.Size(); ++inst, ++buffer)
			* buffer = objs[i]->GetTransform();

		// Update data offsets
		data.mDataOffset = mDynamicOffset;
		mDynamicOffset += objs.Size() * sizeof(Matrix4f);
	}

	// Unmap buffer
	mDynamicBuffer->Unmap();

	for (Uint32 i = 0; i < mDynamicVaos.Size(); ++i)
	{
		Uint32 id = mDynamicVaos[i];

		VertexArrayData& vaoData = mVaoData[id];
		DynamicRenderData& data = mDynamicRenderData[mDataMap[id]];
		VertexArray* vao = vaoData.mVertexArray;

		// Set up vertex attribs
		Uint32 offset = data.mDataOffset;

		vao->Bind();
		vao->VertexAttrib(4, 4, sizeof(Matrix4f), offset + 0, 1);
		vao->VertexAttrib(5, 4, sizeof(Matrix4f), offset + 16, 1);
		vao->VertexAttrib(6, 4, sizeof(Matrix4f), offset + 32, 1);
		vao->VertexAttrib(7, 4, sizeof(Matrix4f), offset + 48, 1);
	}
}

///////////////////////////////////////////////////////////////////////////////

void BindShader(Shader* shader, const Matrix4f& projView, Scene* scene)
{
	shader->Bind();
	shader->SetUniform("camPos", scene->GetCamera().GetPosition());
	shader->SetUniform("projView", projView);
	shader->SetUniform("ambient", scene->GetAmbient());
	scene->GetDirLight().Use(shader);

	shader->UpdateUniforms();
}

void Renderer::Render(FrameBuffer* fb)
{
	Update();

	// Bind framebuffer
	if (fb)
		fb->Bind();
	else
		FrameBuffer::Default.Bind();

	// Clear
	Graphics::SetClearColor(0.2f, 0.2f, 0.3f);
	Graphics::Clear();

	Graphics::Enable(Graphics::DepthTest);
	Graphics::EnableCull(false);

	// Get camera projection-view matrix
	Camera& camera = mScene->GetCamera();
	Matrix4f projView = camera.GetProjection() * camera.GetView();


	// ======================== Static ========================

	if (mStaticQueue.Size())
	{
		// Set up initial shader
		Shader* shader = mStaticQueue.Front().mMaterial->mShader;
		BindShader(shader, projView, mScene);

		// Render static renderables
		for (Uint32 i = 0; i < mStaticQueue.Size(); ++i)
		{
			VertexArrayData& data = mStaticQueue[i];

			if (data.mMaterial)
			{
				// Switch shader if needed
				Shader* next = data.mMaterial->mShader;
				if (next != shader)
				{
					shader = next;
					BindShader(shader, projView, mScene);
				}

				// Use material
				data.mMaterial->Use();
			}

			// Render vao
			VertexArray* vao = data.mVertexArray;
			Uint32 id = vao->GetID();
			vao->Bind();
			vao->DrawArrays(data.mNumVertices, mStaticRenderData[mDataMap[id]].mBufferSize);
		}
	}


	// ======================== Dynamic ========================

	if (mDynamicQueue.Size())
	{
		Shader* shader = mDynamicQueue.Front().mMaterial->mShader;
		BindShader(shader, projView, mScene);

		// Render dynamic renderables
		for (Uint32 i = 0; i < mDynamicQueue.Size(); ++i)
		{
			VertexArrayData& data = mDynamicQueue[i];

			if (data.mMaterial)
			{
				// Switch shader if needed
				Shader* next = data.mMaterial->mShader;
				if (next != shader)
				{
					shader = next;
					BindShader(shader, projView, mScene);
				}

				// Use material
				data.mMaterial->Use();
			}

			// Render vao
			VertexArray* vao = data.mVertexArray;
			Uint32 id = vao->GetID();
			vao->Bind();
			vao->DrawArrays(data.mNumVertices, mDynamicRenderData[mDataMap[id]].mRenderables.Size());
		}
	}


	// ======================== Skybox ========================
	Graphics::SetDepthFunc(Graphics::Lequal);
	mScene->GetSkybox()->Render(projView);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

void Renderer::RegisterModel(Model* model, bool isStatic, Uint32 num)
{
	{
		// Quit if already registered
		Uint32 id = model->GetMesh(0).mVertexArray->GetID();
		if (id < mDataMap.Size() && mDataMap[id] >= 0) return;
	}

	// Only static renderables need render data
	if (isStatic)
	{
		// Set up render data
		mStaticRenderData.Push(StaticRenderData());

		StaticRenderData& data = mStaticRenderData.Back();
		data.mTransforms.Resize(num);
		data.mBufferSize = 0;
		data.mNeedsUpdate = false;

		data.mInstanceBuffer = Resource<VertexBuffer>::Create();
		data.mInstanceBuffer->Bind(VertexBuffer::Array);
	}

	// Dynamic renderables are updated every frame
	else
	{
		mDynamicRenderData.Push(DynamicRenderData());

		DynamicRenderData& data = mDynamicRenderData.Back();
		data.mRenderables.Resize(num);
		data.mDataOffset = 0;
	}

	for (Uint32 i = 0; i < model->GetNumMeshes(); ++i)
	{
		const Mesh& mesh = model->GetMesh(i);

		// Copy VAO data
		VertexArrayData data;
		data.mVertexArray = mesh.mVertexArray;
		data.mNumVertices = mesh.mNumVertices;
		data.mMaterial = mesh.mMaterial;
		
		Uint32 id = mesh.mVertexArray->GetID();

		// Make sure there's enough space for vertex array
		while (id >= mDataMap.Size())
		{
			mVaoData.Push(VertexArrayData());
			mDataMap.Push(-1);
		}

		// Add vao data
		mVaoData[id] = data;
		if (isStatic)
			mDataMap[id] = mStaticRenderData.Size() - 1;
		else
		{
			mDataMap[id] = mDynamicRenderData.Size() - 1;
			mDynamicVaos.Push(id);
		}

		// Dynamic renderables vertex attribs updated every frame
		if (isStatic)
		{
			// Set up vao
			VertexArray* vao = data.mVertexArray;
			vao->Bind();
			vao->VertexAttrib(4, 4, sizeof(Matrix4f), 0, 1);
			vao->VertexAttrib(5, 4, sizeof(Matrix4f), 16, 1);
			vao->VertexAttrib(6, 4, sizeof(Matrix4f), 32, 1);
			vao->VertexAttrib(7, 4, sizeof(Matrix4f), 48, 1);
		}
	}

	UpdateQueue(isStatic ? mStaticQueue : mDynamicQueue);
}

///////////////////////////////////////////////////////////////////////////////

void Renderer::UpdateQueue(Array<VertexArrayData>& queue)
{
	queue.Clear();

	std::map<Uint32, Array<VertexArrayData*>> byShader;

	for (Uint32 i = 1; i < mVaoData.Size(); ++i)
	{
		VertexArrayData& data = mVaoData[i];
		// If vao isn't being used, skip
		if (!data.mVertexArray) continue;

		// Get shader id (used to sort vao data)
		Uint32 id = data.mMaterial->mShader->GetID();

		// Add if group does not exist
		Array<VertexArrayData*>& list = byShader[id];
		if (!list.Capacity())
			list.Resize(8);

		// Add data to group
		list.Push(&data);
	}

	for (auto it = byShader.begin(); it != byShader.end(); ++it)
	{
		Array<VertexArrayData*>& list = it->second;
		// Add all vao data
		for (Uint32 i = 0; i < list.Size(); ++i)
			queue.Push(*list[0]);
	}
}

///////////////////////////////////////////////////////////////////////////////

void Renderer::AddStatic(Renderable* renderable)
{
	// Make sure this is first time adding object
	if (renderable->mInstanceID) return;

	Model* model = renderable->GetModel();

	// Make sure model has been registered
	Uint32 id = model->GetMesh(0).mVertexArray->GetID();
	if (id >= mDataMap.Size() || mDataMap[id] < 0)
		RegisterModel(model, true);

	StaticRenderData& data = mStaticRenderData[mDataMap[id]];
	data.mTransforms.Push(renderable->GetTransform());
	data.mNeedsUpdate = true;

	mStaticDataUpdated = true;

	// Give instance ID
	renderable->mInstanceID = data.mTransforms.Size();
}

///////////////////////////////////////////////////////////////////////////////

void Renderer::AddDynamic(Renderable* renderable)
{
	// Make sure this is first time adding object
	if (renderable->mInstanceID) return;

	Model* model = renderable->GetModel();

	// Make sure model has been registered
	Uint32 id = model->GetMesh(0).mVertexArray->GetID();
	if (id >= mDataMap.Size() || mDataMap[id] < 0)
		RegisterModel(model, false);

	// Add to list of renderables
	DynamicRenderData& data = mDynamicRenderData[mDataMap[id]];
	data.mRenderables.Push(renderable);

	// Give instance ID
	renderable->mInstanceID = data.mRenderables.Size();

	++mNumDynamic;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

void Renderer::SetScene(Scene* scene)
{
	mScene = scene;
}

///////////////////////////////////////////////////////////////////////////////