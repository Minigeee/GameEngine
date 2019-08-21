#include <Graphics/Terrain.h>

#include <Math/Rect.h>

#include <Resource/Resource.h>

#include <Graphics/VertexArray.h>
#include <Graphics/VertexBuffer.h>
#include <Graphics/Material.h>
#include <Graphics/Model.h>
#include <Graphics/Mesh.h>
#include <Graphics/Shader.h>

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

Terrain::Terrain() :
	mSquareSize		(0.0f),
	mTotalSize		(0.0f)
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

void AddTriangle(Array<Vector2f>& verts, const Vector2f& v1, const Vector2f& v2, const Vector2f& v3)
{
	verts.Push(v1);
	verts.Push(v2);
	verts.Push(v3);
}

void Terrain::Create()
{
	if (!mModel)
		mModel = Resource<Model>::Create();

	// Area of square beneath current lod level
	Recti sublevel(0, 0, 0, 0);
	// Array of vertices
	Array<Vector2f> vertices(1024);

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
					AddTriangle(vertices, mm, mr, tr);
					AddTriangle(vertices, mm, br, mr);
				}
				else
				{
					AddTriangle(vertices, mm, br, tr);
				}

				// Right edge
				if (c == sublevel.x + sublevel.w && r >= sublevel.y && r < sublevel.y + sublevel.h)
				{
					AddTriangle(vertices, tl, ml, mm);
					AddTriangle(vertices, ml, bl, mm);
				}
				else
				{
					AddTriangle(vertices, tl, bl, mm);
				}

				// Top edge
				if (r + 1 == sublevel.y && c >= sublevel.x && c < sublevel.x + sublevel.w)
				{
					AddTriangle(vertices, bl, bm, mm);
					AddTriangle(vertices, bm, br, mm);
				}
				else
				{
					AddTriangle(vertices, bl, br, mm);
				}

				// Bottom edge
				if (r == sublevel.y + sublevel.h && c >= sublevel.x && c < sublevel.x + sublevel.w)
				{
					AddTriangle(vertices, tl, mm, tm);
					AddTriangle(vertices, tm, mm, tr);
				}
				else
				{
					AddTriangle(vertices, tl, mm, tr);
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
	vbo->BufferData(&vertices.Front(), vertices.Size() * sizeof(Vector2f), VertexBuffer::Static);

	VertexArray* vao = Resource<VertexArray>::Create();
	vao->Bind();
	vao->VertexAttrib(0, 2);

	// Material
	Shader* shader = Resource<Shader>::Load("Shaders/Terrain.xml");

	Material* material = Resource<Material>::Create();
	material->mShader = shader;

	// Mesh
	Mesh mesh;
	mesh.mVertexArray = vao;
	mesh.mNumVertices = vertices.Size();
	mesh.mMaterial = material;
	mModel->AddMesh(mesh);
}

///////////////////////////////////////////////////////////////////////////////