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
	if (mDynamicBuffer)
		Resource<VertexBuffer>::Free(mDynamicBuffer);
	if (mGBuffer)
		Resource<FrameBuffer>::Free(mGBuffer);
	if (mQuadVao)
		Resource<VertexArray>::Free(mQuadVao);
	if (mQuadVbo)
		Resource<VertexBuffer>::Free(mQuadVbo);
	
	mDynamicBuffer = 0;
	mGBuffer = 0;
	mQuadVao = 0;
	mQuadVbo = 0;

	for (Uint32 i = 0; i < mStaticRenderData.Size(); ++i)
	{
		if (mStaticRenderData[i].mInstanceBuffer)
			Resource<VertexBuffer>::Free(mStaticRenderData[i].mInstanceBuffer);
	}

	for (Uint32 i = 0; i < mRenderPasses.Size(); ++i)
		delete mRenderPasses[i];

	for (auto it : mLightingPasses)
		delete it.second;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

void Renderer::Init(Scene* scene)
{
	mScene = scene;

	Uint32 initSize = 64;

	mVaoData.Reserve(initSize);
	mDataMap.Reserve(initSize);
	mStaticRenderData.Reserve(initSize);
	mDynamicRenderData.Reserve(initSize);
	mStaticQueue.Reserve(initSize);
	mDynamicQueue.Reserve(initSize);
	mIsStatic.Reserve(initSize);

	// Create dynamic instance buffer
	const Uint32 bufferSize = 1 * 1024 * 1024;
	mDynamicBuffer = Resource<VertexBuffer>::Create();
	mDynamicBuffer->Bind(VertexBuffer::Array);
	mDynamicBuffer->BufferData(NULL, bufferSize, VertexBuffer::Stream);
	mDynamicSize = bufferSize;
	mDynamicOffset = 0;

	// Default render passes
	mRenderPasses.Reserve(4);


	// Create G-buffer
	mGBuffer = Resource<FrameBuffer>::Create();
	mGBuffer->Bind();

	FrameBuffer::TextureOptions options;
	const Vector3u& size = FrameBuffer::Default.GetSize();
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

	// Create default lighting pass
	CreateLightingPass(new DefaultLighting(mScene), "Default");
}

///////////////////////////////////////////////////////////////////////////////

void Renderer::PostInit()
{
	// Create default rendering order, if no custom rendering order
	if (!mRenderPasses.Size())
	{
		AddRenderPass(new RenderPass(RenderPass::Normal));
	}
}

///////////////////////////////////////////////////////////////////////////////
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
						&data.mTransforms.GetData().Front(),
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
						&data.mTransforms.GetData().Front(),
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

	for (Uint32 i = 1; i < mVaoData.Size(); ++i)
	{
		if (mIsStatic[i]) continue;

		VertexArrayData& vaoData = mVaoData[i];
		DynamicRenderData& data = mDynamicRenderData[mDataMap[i]];
		VertexArray* vao = vaoData.mVertexArray;
		if (!vao) continue;

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
	shader->SetUniform("mCamPos", scene->GetCamera().GetPosition());
	shader->SetUniform("mProjView", projView);

	shader->ApplyUniforms();
}

void Renderer::Render(FrameBuffer* out)
{
	Update();

	// Use default framebuffer is no output buffer
	if (!out)
		out = &FrameBuffer::Default;

	for (Uint32 i = 0; i < mRenderPasses.Size(); ++i)
	{
		DoRenderPass(*mRenderPasses[i], i == mRenderPasses.Size() - 1 ? out : 0);
	}
}

void Renderer::DoRenderPass(RenderPass& pass, FrameBuffer* out)
{
	// Bind G-Buffer
	mGBuffer->Bind();

	// Clear
	Graphics::Enable(Graphics::DepthTest);
	Graphics::EnableCull(false);

	Graphics::Clear();

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
				shader->ApplyUniforms();
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
				shader->ApplyUniforms();
			}

			// Render vao
			VertexArray* vao = data.mVertexArray;
			Uint32 id = vao->GetID();
			vao->Bind();
			vao->DrawArrays(data.mNumVertices, mDynamicRenderData[mDataMap[id]].mRenderables.Size());
		}
	}



	// Render lighting pass
	pass.GetLightingPass()->Render(mGBuffer);

	// Graphics options
	Graphics::Disable(Graphics::DepthTest);

	// Bind output framebuffer
	if (out)
		out->Bind();
	else if (pass.GetOutput())
		pass.GetOutput()->Bind();
	else
		FrameBuffer::Default.Bind();
	
	Graphics::Clear(Graphics::ColorBuffer);

	// Render
	mQuadVao->Bind();
	mQuadVao->DrawArrays(6);
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
		data.mTransforms.Reserve(num);
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
		data.mRenderables.Reserve(num);
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
			mIsStatic.Push(false);
		}

		// Add vao data
		mVaoData[id] = data;
		mIsStatic[id] = isStatic;
		if (isStatic)
			mDataMap[id] = mStaticRenderData.Size() - 1;
		else
			mDataMap[id] = mDynamicRenderData.Size() - 1;

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

	UpdateQueue(isStatic);
}

///////////////////////////////////////////////////////////////////////////////

void Renderer::UpdateQueue(bool isStatic)
{
	// Choose correct queue
	Array<VertexArrayData>& queue = isStatic ? mStaticQueue : mDynamicQueue;

	queue.Clear();

	std::map<Uint32, Array<VertexArrayData*>> byShader;

	for (Uint32 i = 1; i < mVaoData.Size(); ++i)
	{
		// Only add correct object types
		if (mIsStatic[i] != isStatic) continue;

		VertexArrayData& data = mVaoData[i];
		// If vao isn't being used, skip
		if (!data.mVertexArray) continue;

		// Get shader id (used to sort vao data)
		Uint32 id = data.mMaterial->mShader->GetID();

		// Add if group does not exist
		Array<VertexArrayData*>& list = byShader[id];
		if (!list.Capacity())
			list.Reserve(8);

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
	Model* model = renderable->GetModel();

	// Make sure model has been registered
	Uint32 id = model->GetMesh(0).mVertexArray->GetID();
	if (id >= mDataMap.Size() || mDataMap[id] < 0)
		RegisterModel(model, true);

	StaticRenderData& data = mStaticRenderData[mDataMap[id]];
	data.mNeedsUpdate = true;
	// Add transform to handle array and store handle as instance ID
	renderable->mInstanceID = data.mTransforms.Add(renderable->GetTransform());

	mStaticDataUpdated = true;
}

///////////////////////////////////////////////////////////////////////////////

void Renderer::AddDynamic(Renderable* renderable)
{
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

void Renderer::AddRenderPass(RenderPass* pass, const char* lighting_name)
{
	mRenderPasses.Push(pass);

	// If there is no lighting pass, use specified lighting pass or default
	LightingPass* lighting = mLightingPasses[lighting_name ? lighting_name : "Default"];
	if (!pass->GetLightingPass())
		mRenderPasses.Back()->SetLightingPass(lighting);
}

///////////////////////////////////////////////////////////////////////////////

void Renderer::CreateLightingPass(LightingPass* pass, const char* name)
{
	mLightingPasses[name] = pass;
}

///////////////////////////////////////////////////////////////////////////////