#include <Graphics/Water.h>

#include <Resource/Resource.h>

#include <Scene/Scene.h>

#include <Graphics/VertexArray.h>
#include <Graphics/VertexBuffer.h>
#include <Graphics/Model.h>
#include <Graphics/Shader.h>
#include <Graphics/Mesh.h>
#include <Graphics/Material.h>
#include <Graphics/Terrain.h>

#include <queue>

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

INIT_GAME_OBJECT(WaterChunk);

///////////////////////////////////////////////////////////////////////////////

Water::Water() :
	mTerrain		(0),
	mLoader			(0),
	mMinDepth		(0.0f),
	mAltitude		(0.0f)
{

}

Water::~Water()
{

}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

void Water::SetTerrain(Terrain* terrain)
{
	mTerrain = terrain;
}

void Water::SetMinDepth(float depth)
{
	mMinDepth = depth;
}

void Water::SetAltitude(float h)
{
	mAltitude = h;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

float FindMin(Image* hmap, const Vector2i& s, const Vector2i& e)
{
	float* h = (float*)hmap->GetData();
	float min = INFINITY;
	int w = (int)hmap->GetWidth();

	for (int r = s.y; r <= e.y; ++r)
	{
		for (int c = s.x; c <= e.x; ++c)
		{
			float height = h[r * w + c];
			if (height < min)
				min = height;
		}
	}

	return min;
}

///////////////////////////////////////////////////////////////////////////////

void Water::Create(Scene* scene, float viewDist, float chunkSize)
{
	// Create loader
	mLoader = scene->RegisterLoader<WaterLoader>();
	mLoader->SetChunkSize(chunkSize);
	mLoader->SetLoadDist(viewDist);
	mLoader->SetUnloadDist(viewDist + chunkSize);


	// Create model
	Model* model = Resource<Model>::Create();

	// Create quad
	float halfChunk = chunkSize * 0.5f;
	float verts[] =
	{
		-halfChunk, -halfChunk,
		-halfChunk,  halfChunk,
		 halfChunk, -halfChunk,

		-halfChunk,  halfChunk,
		 halfChunk,  halfChunk,
		 halfChunk, -halfChunk
	};

	// Create vertex buffer
	VertexBuffer* vbo = Resource<VertexBuffer>::Create();
	vbo->Bind(VertexBuffer::Array);
	vbo->BufferData(verts, sizeof(verts), VertexBuffer::Static);

	// Create vertex array
	VertexArray* vao = Resource<VertexArray>::Create();
	vao->Bind();
	vao->VertexAttrib(0, 2);

	// Material
	Shader* shader = Resource<Shader>::Load("Shaders/Water.xml");
	shader->SetUniform("mAltitude", mAltitude);

	Material* material = Resource<Material>::Create();
	material->mShader = shader;
	material->mDiffuse = Vector3f(0.0f, 0.0f, 1.0f);

	// Mesh
	Mesh mesh;
	mesh.mVertexArray = vao;
	mesh.mNumVertices = 6;
	mesh.mBoundingBox.mMin = Vector3f(-halfChunk, 0.0f, -halfChunk);
	mesh.mBoundingBox.mMax = Vector3f(halfChunk, 0.0f, halfChunk);
	mesh.mMaterial = material;

	// Set model
	model->AddMesh(mesh);
	mLoader->SetModel(model);
	// Register model
	scene->GetRenderer().RegisterStaticModel(model, chunkSize);


	// Create water map
	float terrainSize = mTerrain->GetSize();
	int sizeInChunks = 2 * (int)ceil(terrainSize * 0.5f / chunkSize);
	Image* hmap = mTerrain->GetHeightMap()->GetImage();
	float terrainScale = mTerrain->GetMaxHeight();

	assert(hmap);
	float chunkToPixel = (float)hmap->GetWidth() / terrainSize * chunkSize;
	float pixelOffset = ((sizeInChunks * chunkSize - terrainSize) * 0.5f) / chunkSize * chunkToPixel;
	int w = (int)hmap->GetWidth();

	// Keep track of which chunks are in water
	Uint32 numChunks = sizeInChunks * sizeInChunks;
	Array<float> minHeights(numChunks);
	Array<bool> hasWater, visited;
	hasWater.Resize(numChunks, false);
	visited.Resize(numChunks, false);

	// Scan heights
	for (int r = 0; r < sizeInChunks; ++r)
	{
		for (int c = 0; c < sizeInChunks; ++c)
		{
			Vector2i s(
				(int)floor(c * chunkToPixel - pixelOffset),
				(int)floor(r * chunkToPixel - pixelOffset));
			Vector2i e(
				(int)ceil((c + 1) * chunkToPixel - pixelOffset),
				(int)ceil((r + 1) * chunkToPixel - pixelOffset));

			// Clamp values
			if (s.x < 0) s.x = 0;
			if (s.y < 0) s.y = 0;
			if (e.x >= w) e.x = w - 1;
			if (e.y >= w) e.y = w - 1;

			float min = FindMin(hmap, s, e) * terrainScale;

			// Add height to grid
			minHeights.Push(min);
		}
	}

	
	std::queue<int> queue;

	// Loop all chunks to fill water
	for (int i = 0; i < numChunks; ++i)
	{
		// Skip if already visited
		if (visited[i]) continue;

		// If chunk is under water
		if (minHeights[i] > mAltitude - mMinDepth) continue;

		// Add to queue
		queue.push(i);

		// Fill all surrounding chunks with height level below water level
		while (!queue.empty())
		{
			int index = queue.front();
			queue.pop();

			// Do stuff if chunk is under water
			if (minHeights[index] < mAltitude)
			{
				// Mark this chunk as water
				hasWater[index] = true;

				int t = index - sizeInChunks;
				int b = index + sizeInChunks;
				int l = index - 1;
				int r = index + 1;
				int x = index % sizeInChunks;

				// Add chunks that haven't been visited
				if (t >= 0 && !visited[t])
					queue.push(t);
				if (b < sizeInChunks && !visited[b])
					queue.push(b);
				if (x != 0 && !visited[l])
					queue.push(l);
				if (x != sizeInChunks - 1 && !visited[r])
					queue.push(r);
			}

			// Mark as visited
			visited[index] = true;
		}
	}

	// Add to loader
	mLoader->SetWaterMap(hasWater, (Uint32)sizeInChunks);
	mLoader->ReloadChunks();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

WaterLoader::WaterLoader() :
	mModel			(0)
{

}

WaterLoader::~WaterLoader()
{

}

///////////////////////////////////////////////////////////////////////////////

void WaterLoader::SetModel(Model* model)
{
	mModel = model;
}

void WaterLoader::SetWaterMap(const Array<bool>& map, Uint32 size)
{
	mWaterMap = map;
	mSize = size;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

void WaterLoader::OnInit()
{

}

///////////////////////////////////////////////////////////////////////////////

void WaterLoader::OnChunkEnter(ObjectChunk& chunk)
{
	Vector3f chunkPos = chunk.GetBoundingBox().GetPosition();
	Vector2i chunkIndex = Vector2i(
		(int)floor(chunkPos.x / mChunkSize),
		(int)floor(chunkPos.z / mChunkSize));
	chunkIndex += (int)mSize / 2;
	Uint32 index = chunkIndex.y * mSize + chunkIndex.x;

	// Check if chunk has water
	if (index >= 0 && index < mWaterMap.Size() && mWaterMap[index])
	{
		ComponentMap components;
		Array<GameObjectID> ids = mScene->CreateObjects<WaterChunk>(1, &components);

		// Set position
		TransformComponent& t = *components.Get<TransformComponent>();
		t.mPosition = chunkPos;
		t.mPosition.y = 0.0f;

		// Set model
		RenderComponent& r = *components.Get<RenderComponent>();
		r.mModel = mModel;

		// Add to chunk
		chunk.AddRenderables(mRenderer, ids, components);
	}
}

///////////////////////////////////////////////////////////////////////////////

void WaterLoader::OnChunkLeave(ObjectChunk& chunk)
{
	mScene->RemoveObjects<WaterChunk>(chunk.GetRenderables());
}

///////////////////////////////////////////////////////////////////////////////