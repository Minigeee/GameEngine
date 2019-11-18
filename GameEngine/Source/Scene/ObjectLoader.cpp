#include <Scene/ObjectLoader.h>

#include <Scene/Scene.h>

#include <Graphics/Model.h>

#include <assert.h>

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

ObjectLoader::ObjectLoader() :
	mScene				(0),
	mRenderer			(0),
	mChunkSize			(0.0f),
	mLoadRange			(0.0f),
	mUnloadRange		(0.0f),
	mPrevPos			(0.0f)
{

}

ObjectLoader::~ObjectLoader()
{

}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

void ObjectLoader::Init(Scene* scene)
{
	mScene = scene;
	mRenderer = &mScene->GetRenderer();

	OnInit();

	assert(mUnloadRange > mLoadRange);
	assert(mChunkSize > 0.0f);

	// Initialize
	mChunks.Reserve(64);

	// Add initial points
	Camera& cam = mScene->GetCamera();
	mPrevPos = Vector2f(cam.GetPosition().x, cam.GetPosition().z);

	Vector2i s = Floor((mPrevPos - mUnloadRange) / mChunkSize);
	Vector2i e = Floor((mPrevPos + mUnloadRange + mChunkSize) / mChunkSize);
	float unloadSquared = mUnloadRange * mUnloadRange;
	float halfChunk = mChunkSize * 0.5f;

	// Loop box
	for (int r = s.y; r < e.y; ++r)
	{
		for (int c = s.x; c < e.x; ++c)
		{
			Vector2f p((float)c, (float)r);
			p = p * mChunkSize + halfChunk;

			// Test if point is inside unload range
			if (DistanceSquared(p, mPrevPos) < unloadSquared)
			{
				ObjectChunk chunk(p - halfChunk, p + halfChunk);
				// Add to list
				mChunks.Push(std::move(chunk));
			}
		}
	}

	// Update chunks
	UpdateChunks();
}

///////////////////////////////////////////////////////////////////////////////

void ObjectLoader::Update()
{
	Camera& cam = mScene->GetCamera();
	Vector2f newPos(cam.GetPosition().x, cam.GetPosition().z);
	Vector2b moved = newPos != mPrevPos;

	// Don't update if camera didn't move
	if (!moved.x && !moved.y) return;

	float unloadSquared = mUnloadRange * mUnloadRange;
	float halfChunk = mChunkSize * 0.5f;

	// Get bounding box of new unloading zone
	Vector2i s = Floor((newPos - mUnloadRange) / mChunkSize);
	Vector2i e = Floor((newPos + mUnloadRange + mChunkSize) / mChunkSize);

	// Find new chunks inside unload zone
	for (int r = s.y; r < e.y; ++r)
	{
		for (int c = s.x; c < e.x; ++c)
		{
			Vector2f p((float)c, (float)r);
			p = p * mChunkSize + halfChunk;

			if (
				// Test if point is outside old loading range
				DistanceSquared(p, mPrevPos) > unloadSquared &&
				// Test if point is inside new loading range
				DistanceSquared(p, newPos) < unloadSquared
				)
			{
				ObjectChunk chunk(p - halfChunk, p + halfChunk);
				mChunks.Push(std::move(chunk));
			}
		}
	}

	// Update camera position
	mPrevPos = newPos;

	// Update chunks
	UpdateChunks();
}

///////////////////////////////////////////////////////////////////////////////

void ObjectLoader::UpdateChunks()
{
	float loadSquared = mLoadRange * mLoadRange;
	float unloadSquared = mUnloadRange * mUnloadRange;

	for (Uint32 i = 0; i < mChunks.Size(); ++i)
	{
		ObjectChunk& chunk = mChunks[i];
		Vector3f p3 = chunk.GetBoundingBox().GetPosition();
		Vector2f p(p3.x, p3.z);

		float distSquared = DistanceSquared(p, mPrevPos);

		if (chunk.IsLoaded() && distSquared > unloadSquared)
		{
			// Remove chunk from renderer first
			if (chunk.GetRenderables().Size())
			{
				Model* model = mScene->GetComponent<RenderComponent>(chunk.GetRenderables()[0])->mModel;
				mRenderer->RemoveStaticChunk(model, p3);
			}

			// Unload chunk
			OnChunkLeave(chunk);

			// Remove chunk from list
			mChunks.SwapPop(i--);
		}
		else if (!chunk.IsLoaded() && distSquared < loadSquared)
		{
			// Mark as loaded
			chunk.MarkLoaded();

			// Load chunk
			OnChunkEnter(chunk);
		}
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

void ObjectLoader::OnInit()
{

}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

ObjectChunk::ObjectChunk(const Vector2f& s, const Vector2f& e) :
	mIsLoaded		(false)
{
	mRenderables.Reserve(8);
	mBoundingBox.mMin = Vector3f(s.x, INFINITY, s.y);
	mBoundingBox.mMax = Vector3f(e.x, -INFINITY, e.y);
}

///////////////////////////////////////////////////////////////////////////////

void ObjectChunk::AddRenderables(
	Renderer* renderer, const Array<GameObjectID>& ids, ComponentMap& components)
{
	mRenderables = ids;

	TransformComponent* t = components.Get<TransformComponent>();
	RenderComponent* r = components.Get<RenderComponent>();

	for (Uint32 i = 0; i < mRenderables.Size(); ++i)
	{
		// Update bounding box
		const BoundingBox& modelBox = r[i].mModel->GetBoundingBox();
		Vector3f boxPos = modelBox.GetPosition();

		BoundingSphere& sphere = r[i].mBoundingSphere;
		sphere.p = boxPos + t[i].mPosition;
		sphere.r = Distance(boxPos, modelBox.mMin) * t[i].mScale;

		BoundingBox box(sphere.p - sphere.r, sphere.p + sphere.r);

		if (box.mMin.x < mBoundingBox.mMin.x)
			mBoundingBox.mMin.x = box.mMin.x;
		if (box.mMax.x > mBoundingBox.mMax.x)
			mBoundingBox.mMax.x = box.mMax.x;

		if (box.mMin.y < mBoundingBox.mMin.y)
			mBoundingBox.mMin.y = box.mMin.y;
		if (box.mMax.y > mBoundingBox.mMax.y)
			mBoundingBox.mMax.y = box.mMax.y;

		if (box.mMin.z < mBoundingBox.mMin.z)
			mBoundingBox.mMin.z = box.mMin.z;
		if (box.mMax.z > mBoundingBox.mMax.z)
			mBoundingBox.mMax.z = box.mMax.z;
	}

	// Add to renderer
	renderer->AddStaticChunk(t, r, mRenderables.Size(), mBoundingBox);
}

///////////////////////////////////////////////////////////////////////////////

void ObjectChunk::MarkLoaded()
{
	mIsLoaded = true;
}

///////////////////////////////////////////////////////////////////////////////

const Array<GameObjectID>& ObjectChunk::GetRenderables() const
{
	return mRenderables;
}

const BoundingBox& ObjectChunk::GetBoundingBox() const
{
	return mBoundingBox;
}

bool ObjectChunk::IsLoaded() const
{
	return mIsLoaded;
}

///////////////////////////////////////////////////////////////////////////////