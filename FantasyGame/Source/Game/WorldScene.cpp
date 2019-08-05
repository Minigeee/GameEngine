#include <Game/WorldScene.h>

#include <Core/LogFile.h>

#include <Resource/Resource.h>
#include <Graphics/Material.h>
#include <Graphics/Model.h>
#include <Graphics/Renderable.h>
#include <Graphics/Shader.h>

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

WorldScene::WorldScene()
{

}

WorldScene::~WorldScene()
{

}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

void WorldScene::OnCreate()
{
	LOG << "Creating world\n";

	Model* model = Resource<Model>::Load("Models/Box/Box.dae");
	Renderable* object = Resource<Renderable>::Create();
	object->SetModel(model);
	object->SetPosition(0.2f, 0.0f, 0.0f);

	Shader* shader = Resource<Shader>::Load("Shaders/Default.xml");

	Material* material = Resource<Material>::Create();
	material->mShader = shader;
	material->mDiffuse = Vector3f(0.6f, 1.0f, 0.6f);

	model->GetMesh(0).mMaterial = material;

	mRenderer.AddStatic(object);


	mDirLight.SetDirection(0.0f, -1.0f, 2.0f);
}

///////////////////////////////////////////////////////////////////////////////

void WorldScene::OnDelete()
{
	LOG << "Deleting world\n";
}

///////////////////////////////////////////////////////////////////////////////