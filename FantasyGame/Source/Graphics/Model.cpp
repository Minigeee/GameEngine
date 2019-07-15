#include <Graphics/Model.h>

#include <Core/LogFile.h>

#include <Resource/Resource.h>
#include <Graphics/VertexArray.h>
#include <Graphics/VertexBuffer.h>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

Model::Model()
{

}

Model::~Model()
{

}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

Mesh ProcessMesh(aiMesh* mesh, const aiScene* scene)
{
	Array<float> data;

	// Calculate data size
	Uint32 vertexSize = 6;
	if (mesh->HasTextureCoords(0))
		vertexSize += 2;
	if (mesh->HasVertexColors(0))
		vertexSize += 3;

	data.Resize(mesh->mNumVertices * vertexSize);

	// Keep track of min and max for bounding box
	aiVector3D& first = mesh->mVertices[0];
	Vector3f min(first.x, first.y, first.z);
	Vector3f max(min);

	for (Uint32 i = 0; i < mesh->mNumVertices; ++i)
	{
		aiVector3D& v = mesh->mVertices[i];
		aiVector3D& n = mesh->mNormals[i];

		data.Push(v.x);
		data.Push(v.y);
		data.Push(v.z);
		data.Push(n.x);
		data.Push(n.y);
		data.Push(n.z);

		if (v.x < min.x)
			min.x = v.x;
		else if (v.x > max.x)
			max.x = v.x;

		if (v.y < min.y)
			min.y = v.y;
		else if (v.y > max.y)
			max.y = v.y;

		if (v.z < min.z)
			min.z = v.z;
		else if (v.z > max.z)
			max.z = v.z;
	}

	// Push data to vertex buffer
	VertexBuffer* vbo = Resource<VertexBuffer>::Create();
	vbo->Bind(VertexBuffer::Array);
	vbo->BufferData(&data.Front(), data.Size() * sizeof(float), VertexBuffer::Static);

	// Set up vertex array
	VertexArray* vao = Resource<VertexArray>::Create();
	vao->Bind();

	vao->VertexAttrib(0, 3, vertexSize, 0);
	vao->VertexAttrib(1, 3, vertexSize, 12);

	Uint32 offset = 24;
	if (mesh->HasTextureCoords(0))
	{
		vao->VertexAttrib(2, 2, vertexSize, offset);
		offset += 2;
	}
	if (mesh->HasVertexColors(0))
	{
		vao->VertexAttrib(3, 3, vertexSize, offset);
	}


	// Add all data to mesh
	Mesh m;
	m.mNumVertices = mesh->mNumVertices;
	m.mVertexArray = vao;
	m.mBoundingBox = BoundingBox(min, max);

	return m;
}

///////////////////////////////////////////////////////////////////////////////

void ProcessNode(Array<Mesh>& meshes, aiNode* node, const aiScene* scene)
{
	for (Uint32 i = 0; i < node->mNumMeshes; ++i)
	{
		aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
		meshes.Push(ProcessMesh(mesh, scene));
	}

	// Process children
	for (Uint32 i = 0; i < node->mNumChildren; ++i)
		ProcessNode(meshes, node->mChildren[i], scene);
}

///////////////////////////////////////////////////////////////////////////////

bool Model::Load(const char* fname)
{
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(fname, aiProcess_Triangulate | aiProcess_FlipUVs);

	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
	{
		LOG_ERROR << "Failed to load model " << fname << "\n" << importer.GetErrorString() << "\n";
		return false;
	}

	// Resize mesh array
	mMeshes.Resize(scene->mNumMeshes > 4 ? scene->mNumMeshes : 4);

	ProcessNode(mMeshes, scene->mRootNode, scene);


	LOG << "Loaded model file " << fname << "\n";
	return true;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

void Model::SetMaxMeshes(Uint32 max)
{
	mMeshes.Resize(max);
}

void Model::AddMesh(const Mesh& mesh)
{
	mMeshes.Push(mesh);

	// Update bounding box
	const BoundingBox& box = mesh.mBoundingBox;

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

///////////////////////////////////////////////////////////////////////////////

Mesh& Model::GetMesh(Uint32 i) const
{
	return mMeshes[i];
}

const BoundingBox& Model::GetBoundingBox() const
{
	return mBoundingBox;
}

///////////////////////////////////////////////////////////////////////////////