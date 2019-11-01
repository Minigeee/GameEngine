#include <Graphics/Terrain.h>

#include <Math/Rect.h>

#include <Resource/Resource.h>

#include <Graphics/VertexArray.h>
#include <Graphics/VertexBuffer.h>
#include <Graphics/Material.h>
#include <Graphics/Model.h>
#include <Graphics/Mesh.h>
#include <Graphics/Shader.h>
#include <Graphics/Image.h>
#include <Graphics/Texture.h>

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

Terrain::Terrain() :
	mSquareSize		(0.0f),
	mHeightMap		(0),
	mSize			(0.0f)
{

}

Terrain::~Terrain()
{

}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

void Terrain::SetLodLevels(const Array<float>& levels)
{
	mLodLevels = levels;
}

void Terrain::SetSquareSize(float size)
{
	mSquareSize = size;
}

///////////////////////////////////////////////////////////////////////////////

struct TerrainVert
{
	TerrainVert(const Vector2f& p, const Vector2f& ind, float lod = -1.0f) :
		mPos(p),
		mInd(ind),
		mLod(lod)
	{ }

	Vector2f mPos;
	Vector2f mInd;
	float mLod;
};

void CreateTransition(Array<TerrainVert>& vertices, float squareSize, float x, float y, float lod)
{
	float halfSquare = 0.5f * squareSize;
	float qSquare = 0.25f * squareSize;

	TerrainVert tl(Vector2f(x, y), Vector2f(0.0f));
	TerrainVert tm(tl.mPos + Vector2f(halfSquare, 0.0f), Vector2f(halfSquare, 0.0f), lod);
	TerrainVert tr(tl.mPos + Vector2f(squareSize, 0.0f), Vector2f(0.0f));
	TerrainVert ml(tl.mPos + Vector2f(0.0f, halfSquare), Vector2f(0.0f, halfSquare), lod);
	TerrainVert mm(ml.mPos + Vector2f(halfSquare, 0.0f), Vector2f(0.0f));
	TerrainVert mr(ml.mPos + Vector2f(squareSize, 0.0f), Vector2f(0.0f, halfSquare), lod);
	TerrainVert bl(tl.mPos + Vector2f(0.0f, squareSize), Vector2f(0.0f));
	TerrainVert bm(bl.mPos + Vector2f(halfSquare, 0.0f), Vector2f(halfSquare, 0.0f), lod);
	TerrainVert br(bl.mPos + Vector2f(squareSize, 0.0f), Vector2f(0.0f));
	TerrainVert m1(tl.mPos + Vector2f(0.5f * halfSquare), Vector2f(qSquare, qSquare), lod);
	TerrainVert m2(ml.mPos + Vector2f(0.5f * halfSquare), Vector2f(-qSquare, qSquare), lod);
	TerrainVert m3(tm.mPos + Vector2f(0.5f * halfSquare), Vector2f(-qSquare, qSquare), lod);
	TerrainVert m4(mm.mPos + Vector2f(0.5f * halfSquare), Vector2f(qSquare, qSquare), lod);


	// Top-Left
	vertices.Push(tl);
	vertices.Push(ml);
	vertices.Push(m1);

	vertices.Push(ml);
	vertices.Push(mm);
	vertices.Push(m1);

	vertices.Push(m1);
	vertices.Push(mm);
	vertices.Push(tm);

	vertices.Push(tl);
	vertices.Push(m1);
	vertices.Push(tm);

	// Top-Right
	vertices.Push(tm);
	vertices.Push(mm);
	vertices.Push(m3);

	vertices.Push(mm);
	vertices.Push(mr);
	vertices.Push(m3);

	vertices.Push(m3);
	vertices.Push(mr);
	vertices.Push(tr);

	vertices.Push(tm);
	vertices.Push(m3);
	vertices.Push(tr);

	// Bot-Left
	vertices.Push(ml);
	vertices.Push(bl);
	vertices.Push(m2);

	vertices.Push(bl);
	vertices.Push(bm);
	vertices.Push(m2);

	vertices.Push(m2);
	vertices.Push(bm);
	vertices.Push(mm);

	vertices.Push(ml);
	vertices.Push(m2);
	vertices.Push(mm);

	// Bot-Right
	vertices.Push(mm);
	vertices.Push(bm);
	vertices.Push(m4);

	vertices.Push(bm);
	vertices.Push(br);
	vertices.Push(m4);

	vertices.Push(m4);
	vertices.Push(br);
	vertices.Push(mr);

	vertices.Push(mm);
	vertices.Push(m4);
	vertices.Push(mr);
}

void Terrain::Create()
{
	if (!mModel)
		mModel = Resource<Model>::Create();

	// Area of square beneath current lod level
	Recti sublevel(0, 0, 0, 0);
	// Array of vertices
	Array<TerrainVert> vertices(4096);

	float prevLod = 0.0f;
	float squareSize = mSquareSize;

	for (Uint32 lvl = 0; lvl < mLodLevels.Size(); ++lvl)
	{
		// This determines how big the transition zone is
		int extend = pow(2, mLodLevels.Size() - lvl - 1);
		// If extend is 1 (Creating last lod level), then no transition zone
		if (extend == 1) extend = 0;

		// Calc useful info
		float dist = mLodLevels[lvl];
		int numSquares = (int)lround(dist / squareSize) - extend;
		numSquares += numSquares % 2 == 0 ? 0 : 1;
		float halfSquare = 0.5f * squareSize;
		// Determines if there is a sublevel
		bool hasSublevel = (bool)lvl;

		int start = numSquares;

		// Process squares in grid
		for (int r = -start; r < start; ++r)
		{
			for (int c = -start; c < start; ++c)
			{
				// Process square
				Vector2f tl(c * squareSize, r * squareSize);
				Vector2f tm(tl + Vector2f(halfSquare, 0.0f));
				Vector2f tr(tl + Vector2f(squareSize, 0.0f));
				Vector2f ml(tl + Vector2f(0.0f, halfSquare));
				Vector2f mm(ml + Vector2f(halfSquare, 0.0f));
				Vector2f mr(ml + Vector2f(squareSize, 0.0f));
				Vector2f bl(tl + Vector2f(0.0f, squareSize));
				Vector2f bm(bl + Vector2f(halfSquare, 0.0f));
				Vector2f br(bl + Vector2f(squareSize, 0.0f));

				// Skip square if inside sublevel
				if (sublevel.Contains(Recti(c, r, 1, 1)))
					continue;

				bool touchL =
					hasSublevel &&
					c == sublevel.x - 1 &&
					r >= sublevel.y - 1 &&
					r < sublevel.y + sublevel.h + 1;
				bool touchR =
					hasSublevel &&
					c == sublevel.x + sublevel.w &&
					r >= sublevel.y - 1 &&
					r < sublevel.y + sublevel.h + 1;
				bool touchB =
					hasSublevel &&
					r == sublevel.y + sublevel.h &&
					c >= sublevel.x &&
					c < sublevel.x + sublevel.w;
				bool touchT =
					hasSublevel &&
					r == sublevel.y - 1 &&
					c >= sublevel.x &&
					c < sublevel.x + sublevel.w;


				if (touchL || touchR || touchT || touchB)
				{
					CreateTransition(vertices, squareSize, tl.x, tl.y, prevLod);
				}
				else
				{
					vertices.Push(TerrainVert(mm, Vector2f(0.0f)));
					vertices.Push(TerrainVert(br, Vector2f(0.0f)));
					vertices.Push(TerrainVert(tr, Vector2f(0.0f)));

					vertices.Push(TerrainVert(tl, Vector2f(0.0f)));
					vertices.Push(TerrainVert(bl, Vector2f(0.0f)));
					vertices.Push(TerrainVert(mm, Vector2f(0.0f)));

					vertices.Push(TerrainVert(tl, Vector2f(0.0f)));
					vertices.Push(TerrainVert(mm, Vector2f(0.0f)));
					vertices.Push(TerrainVert(tr, Vector2f(0.0f)));

					vertices.Push(TerrainVert(bl, Vector2f(0.0f)));
					vertices.Push(TerrainVert(br, Vector2f(0.0f)));
					vertices.Push(TerrainVert(mm, Vector2f(0.0f)));
				}
			}
		}

		// Quit if finished last level
		if (lvl == mLodLevels.Size() - 1) break;

		// Create transition zone
		start = numSquares / 2 + 1;
		numSquares += extend;
		float lod = numSquares * squareSize;

		// Update sublevel
		sublevel.x = -numSquares / 2;
		sublevel.y = sublevel.x;
		sublevel.w = numSquares;
		sublevel.h = sublevel.w;

		// Update square size
		squareSize *= 2.0f;
		prevLod = lod;


		extend /= 2;

		for (int i = 0; i < extend; ++i, ++start)
		{
			for (int c = -start; c < start; ++c)
			{
				CreateTransition(vertices, squareSize, c * squareSize, -start * squareSize, lod);
				CreateTransition(vertices, squareSize, c * squareSize, (start - 1) * squareSize, lod);
			}

			for (int r = -start + 1; r < start - 1; ++r)
			{
				CreateTransition(vertices, squareSize, -start * squareSize, r * squareSize, lod);
				CreateTransition(vertices, squareSize, (start - 1) * squareSize, r * squareSize, lod);
			}
		}
	}


	// Vertex data
	VertexBuffer* vbo = Resource<VertexBuffer>::Create();
	vbo->Bind(VertexBuffer::Array);
	vbo->BufferData(&vertices.Front(), vertices.Size() * sizeof(TerrainVert), VertexBuffer::Static);

	VertexArray* vao = Resource<VertexArray>::Create();
	vao->Bind();
	vao->VertexAttrib(0, 2, sizeof(TerrainVert), 0);
	vao->VertexAttrib(1, 2, sizeof(TerrainVert), 8);
	vao->VertexAttrib(2, 1, sizeof(TerrainVert), 16);

	// Material
	Shader* shader = Resource<Shader>::Load("Shaders/Terrain.xml");
	shader->SetUniform("terrainSize", mSize * 0.5f);

	float res = (1 << (mLodLevels.Size() - 1)) * mSquareSize;
	shader->SetUniform("res", res);

	Material* material = Resource<Material>::Create();
	material->mShader = shader;
	material->mSpecular = Vector3f(0.05f);

	// Mesh
	Mesh mesh;
	mesh.mVertexArray = vao;
	mesh.mNumVertices = vertices.Size();
	mesh.mMaterial = material;
	mModel->AddMesh(mesh);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

void Terrain::SetSize(float size)
{
	mSize = size;

	if (mModel)
		mModel->GetMesh(0).mMaterial->mShader->SetUniform("terrainSize", mSize * 0.5f);
}

///////////////////////////////////////////////////////////////////////////////

void Terrain::SetHeightMap(Texture* texture)
{
	// Create height map
	if (!mHeightMap || mHeightMap != texture)
	{
		mHeightMap = texture;
		mHeightMap->Bind();
		mHeightMap->SetWrap(Texture::ClampToEdge);
		mHeightMap->SetFilter(Texture::Linear);
		mModel->GetMesh(0).mMaterial->AddTexture(mHeightMap, "heightMap");
	}
}

///////////////////////////////////////////////////////////////////////////////

void Terrain::SetColorMap(Texture* texture)
{
	// Create color map
	if (!mColorMap || mColorMap != texture)
	{
		mColorMap = texture;
		mColorMap->Bind();
		mColorMap->SetWrap(Texture::ClampToEdge);
		mColorMap->SetFilter(Texture::Linear);
		mModel->GetMesh(0).mMaterial->AddTexture(mColorMap, "colorMap");
	}
}

///////////////////////////////////////////////////////////////////////////////