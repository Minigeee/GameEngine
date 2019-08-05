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

#include <Scene/Scene.h>

#include <map>

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

Renderer::Renderer() :
	mRenderDataUpdated		(false)
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
	mRenderData.Resize(initSize);
	mRenderQueue.Resize(initSize);

	Graphics::Enable(Graphics::DepthTest);
}

///////////////////////////////////////////////////////////////////////////////

void Renderer::Update()
{
	if (mRenderDataUpdated)
	{
		for (Uint32 i = 0; i < mRenderData.Size(); ++i)
		{
			RenderData& data = mRenderData[i];
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

		mRenderDataUpdated = false;
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
}

void Renderer::Render()
{
	Update();

	// Clear
	Graphics::SetClearColor(0.2f, 0.2f, 0.3f);
	Graphics::Clear();

	// Get camera projection-view matrix
	Camera& camera = mScene->GetCamera();
	Matrix4f projView = camera.GetProjection() * camera.GetView();

	// Set up initial shader
	Shader* shader = mRenderQueue.Front().mMaterial->mShader;
	BindShader(shader, projView, mScene);


	for (Uint32 i = 0; i < mRenderQueue.Size(); ++i)
	{
		VertexArrayData& data = mRenderQueue[i];

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
		Uint32 id = data.mVertexArray->GetID();
		data.mVertexArray->DrawArrays(data.mNumVertices, mRenderData[mDataMap[id]].mBufferSize);
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

void Renderer::RegisterModel(Model* model, Uint32 num)
{
	{
		// Quit if already registered
		Uint32 id = model->GetMesh(0).mVertexArray->GetID();
		if (id < mDataMap.Size() && mDataMap[id] >= 0) return;
	}

	{
		// Set up render data
		mRenderData.Push(RenderData());

		RenderData& data = mRenderData.Back();
		data.mTransforms.Resize(num);
		data.mInstanceBuffer = Resource<VertexBuffer>::Create();
		data.mBufferSize = 0;
		data.mNeedsUpdate = false;

		data.mInstanceBuffer->Bind(VertexBuffer::Array);
	}

	for (Uint32 i = 0; i < model->GetNumMeshes(); ++i)
	{
		const Mesh& mesh = model->GetMesh(i);

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
		mDataMap[id] = mRenderData.Size() - 1;

		// Set up vao
		VertexArray* vao = data.mVertexArray;
		vao->Bind();
		vao->VertexAttrib(4, 4, sizeof(Matrix4f), 0, 1);
		vao->VertexAttrib(5, 4, sizeof(Matrix4f), 16, 1);
		vao->VertexAttrib(6, 4, sizeof(Matrix4f), 32, 1);
		vao->VertexAttrib(7, 4, sizeof(Matrix4f), 48, 1);
	}

	UpdateQueue();
}

///////////////////////////////////////////////////////////////////////////////

void Renderer::UpdateQueue()
{
	mRenderQueue.Clear();

	std::map<Uint32, Array<VertexArrayData*>> byShader;

	for (Uint32 i = 1; i < mVaoData.Size(); ++i)
	{
		VertexArrayData& data = mVaoData[i];

		// Get shader id (used to sort vao data)
		Uint32 id = data.mMaterial->mShader->GetID();

		// Add if group does not exist
		auto it = byShader.find(id);
		if (it == byShader.end())
		{
			byShader[id] = Array<VertexArrayData*>(8);
			it = byShader.find(id);
		}

		// Add data to group
		it->second.Push(&data);
	}

	for (auto it = byShader.begin(); it != byShader.end(); ++it)
	{
		Array<VertexArrayData*>& list = it->second;
		// Add all vao data
		for (Uint32 i = 0; i < list.Size(); ++i)
			mRenderQueue.Push(*list[0]);
	}
}

///////////////////////////////////////////////////////////////////////////////

void Renderer::AddStatic(Renderable* renderable)
{
	// Make sure this is first time adding
	if (renderable->mInstanceID) return;

	Model* model = renderable->GetModel();

	// Make sure model has been registered
	Uint32 id = model->GetMesh(0).mVertexArray->GetID();
	if (id >= mDataMap.Size() || mDataMap[id] < 0)
		RegisterModel(model);

	RenderData& data = mRenderData[mDataMap[id]];
	data.mTransforms.Push(renderable->GetTransform());
	data.mNeedsUpdate = true;

	mRenderDataUpdated = true;

	// Give instance ID
	renderable->mInstanceID = data.mTransforms.Size();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

void Renderer::SetScene(Scene* scene)
{
	mScene = scene;
}

///////////////////////////////////////////////////////////////////////////////