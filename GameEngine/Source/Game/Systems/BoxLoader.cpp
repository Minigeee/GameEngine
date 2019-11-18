#include <Game/Systems/BoxLoader.h>

#include <Resource/Resource.h>

#include <Scene/Scene.h>

#include <Graphics/Renderer.h>
#include <Graphics/Model.h>
#include <Graphics/Material.h>
#include <Graphics/Shader.h>

#include <Game/Objects/PlayerObject.h>

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

BoxLoader::BoxLoader()
{
	mLoadRange = 100.0f;
	mUnloadRange = 120.0f;
	mChunkSize = 8.0f;
}

BoxLoader::~BoxLoader()
{

}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

void BoxLoader::OnInit()
{
	Model* model = Resource<Model>::Load("Models/Box/Box.dae");

	Shader* shader = Resource<Shader>::Load("Shaders/Default.xml");

	Material* material = Resource<Material>::Create();
	material->mShader = shader;
	material->mDiffuse = Vector3f(0.8f, 0.2f, 0.2f);

	model->GetMesh(0).mMaterial = material;

	mRenderer->RegisterStaticModel(model, mChunkSize);

	mModel = model;
}

///////////////////////////////////////////////////////////////////////////////

void BoxLoader::OnChunkEnter(ObjectChunk& chunk)
{
	ComponentMap components;
	Array<GameObjectID> objects = mScene->CreateObjects<PlayerObject>(4, &components);
	Vector3f s = chunk.GetBoundingBox().mMin;

	TransformComponent* t = components.Get<TransformComponent>();
	RenderComponent* r = components.Get<RenderComponent>();

	for (Uint32 i = 0; i < objects.Size(); ++i)
	{
		t[i].mPosition = Vector3f(s.x + 5.0f, 10.0f, s.z + (i * 1.5f) + 1.0f);
		t[i].mScale = 0.5f;
		r[i].mModel = mModel;
	}

	// Add to chunk
	chunk.AddRenderables(mRenderer, objects, components);
}

///////////////////////////////////////////////////////////////////////////////

void BoxLoader::OnChunkLeave(ObjectChunk& chunk)
{
	mScene->RemoveObjects<PlayerObject>(chunk.GetRenderables());
}

///////////////////////////////////////////////////////////////////////////////