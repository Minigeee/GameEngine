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
	TerrainVert(const Vector2f p, float res) :
		mPos(p),
		mRes(res)
	{ }

	Vector2f mPos;
	float mRes;
};

void AddTriangle(Array<TerrainVert>& verts, const Vector2f& v1, const Vector2f& v2, const Vector2f& v3, float res)
{
	verts.Push(TerrainVert(v1, res));
	verts.Push(TerrainVert(v2, res));
	verts.Push(TerrainVert(v3, res));
}

void Terrain::Create()
{
	if (!mModel)
		mModel = Resource<Model>::Create();

	// Area of square beneath current lod level
	Recti sublevel(0, 0, 0, 0);
	// Array of vertices
	Array<TerrainVert> vertices(4096);

	float squareSize = mSquareSize;

	for (Uint32 lvl = 0; lvl < mLodLevels.Size(); ++lvl)
	{
		float dist = mLodLevels[lvl];
		int numSquares = (int)lround(dist / squareSize);
		numSquares += numSquares % 2 == 0 ? 0 : 1;
		float halfSquare = 0.5f * squareSize;

		// Process squares in grid
		for (int r = -numSquares; r < numSquares; ++r)
		{
			for (int c = -numSquares; c < numSquares; ++c)
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


				// Left edge
				if (c + 1 == sublevel.x && r >= sublevel.y && r < sublevel.y + sublevel.h)
				{
					AddTriangle(vertices, mm, mr, tr, halfSquare);
					AddTriangle(vertices, mm, br, mr, halfSquare);
				}
				else
				{
					AddTriangle(vertices, mm, br, tr, squareSize);
				}

				// Right edge
				if (c == sublevel.x + sublevel.w && r >= sublevel.y && r < sublevel.y + sublevel.h)
				{
					AddTriangle(vertices, tl, ml, mm, halfSquare);
					AddTriangle(vertices, ml, bl, mm, halfSquare);
				}
				else
				{
					AddTriangle(vertices, tl, bl, mm, squareSize);
				}

				// Top edge
				if (r + 1 == sublevel.y && c >= sublevel.x && c < sublevel.x + sublevel.w)
				{
					AddTriangle(vertices, bl, bm, mm, halfSquare);
					AddTriangle(vertices, bm, br, mm, halfSquare);
				}
				else
				{
					AddTriangle(vertices, bl, br, mm, squareSize);
				}

				// Bottom edge
				if (r == sublevel.y + sublevel.h && c >= sublevel.x && c < sublevel.x + sublevel.w)
				{
					AddTriangle(vertices, tl, mm, tm, halfSquare);
					AddTriangle(vertices, tm, mm, tr, halfSquare);
				}
				else
				{
					AddTriangle(vertices, tl, mm, tr, squareSize);
				}
			}
		}

		// Update sublevel
		sublevel.x = -numSquares / 2;
		sublevel.y = -numSquares / 2;
		sublevel.w = numSquares;
		sublevel.h = numSquares;

		// Update square size
		squareSize *= 2.0f;
	}


	// Vertex data
	VertexBuffer* vbo = Resource<VertexBuffer>::Create();
	vbo->Bind(VertexBuffer::Array);
	vbo->BufferData(&vertices.Front(), vertices.Size() * sizeof(TerrainVert), VertexBuffer::Static);

	VertexArray* vao = Resource<VertexArray>::Create();
	vao->Bind();
	vao->VertexAttrib(0, 2, sizeof(TerrainVert), 0);
	vao->VertexAttrib(1, 1, sizeof(TerrainVert), 8);

	// Material
	Shader* shader = Resource<Shader>::Load("Shaders/Terrain.xml");
	shader->SetUniform("terrainSize", mSize);

	Material* material = Resource<Material>::Create();
	material->mShader = shader;
	material->mSpecular = Vector3f(0.1f);

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
		mModel->GetMesh(0).mMaterial->mShader->SetUniform("terrainSize", mSize);
}

///////////////////////////////////////////////////////////////////////////////

void Terrain::SetHeightMap(Texture* texture)
{
	Image* map = texture->GetImage();
	Uint32 w = map->GetWidth();
	Uint32 h = map->GetHeight();
	// Must be a square
	if (w != h) return;

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