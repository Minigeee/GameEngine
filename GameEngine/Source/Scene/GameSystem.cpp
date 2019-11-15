#include <Scene/GameSystem.h>

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

GameSystem::GameSystem() :
	mScene		(0)
{

}

GameSystem::~GameSystem()
{

}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

void GameSystem::Init(Scene* scene)
{
	mScene = scene;

	mObjectTypes.Reserve(4);

	OnInit();
}

void GameSystem::CleanUp()
{
	OnCleanUp();
}

///////////////////////////////////////////////////////////////////////////////

void GameSystem::RegisterDependencies()
{

}

void GameSystem::Update(float dt)
{

}

void GameSystem::OnInit()
{

}

void GameSystem::OnCleanUp()
{

}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

void GameSystem::RegisterObjectType(
	Uint32 typeID,
	const std::unordered_set<Uint32>& set,
	const std::unordered_set<Uint32>& tags)
{
	// Add object type if it matches requirements
	if (MatchesRequirements(set) && MatchesTags(tags))
		mObjectTypes.Push(typeID);
}

///////////////////////////////////////////////////////////////////////////////