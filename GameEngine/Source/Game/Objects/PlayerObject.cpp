#include <Game/Objects/PlayerObject.h>

#include <Resource/Resource.h>
#include <Graphics/Material.h>
#include <Graphics/Model.h>
#include <Graphics/Shader.h>

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

PlayerObject::PlayerObject()
{

}

PlayerObject::~PlayerObject()
{

}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

void PlayerObject::OnCreate()
{
	Model* model = Resource<Model>::Load("Models/Box/Box.dae");

	Shader* shader = Resource<Shader>::Load("Shaders/Default.xml");

	Material* material = Resource<Material>::Create();
	material->mShader = shader;
	material->mDiffuse = Vector3f(0.8f, 0.2f, 0.2f);

	model->GetMesh(0).mMaterial = material;


	// Set model
	SetModel(model);
	SetScale(0.5f);
}

///////////////////////////////////////////////////////////////////////////////