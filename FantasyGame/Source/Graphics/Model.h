#ifndef MODEL_H
#define MODEL_H

#include <Core/TypeInfo.h>
#include <Core/Array.h>

#include <Math/BoundingBox.h>
#include <Graphics/Mesh.h>

///////////////////////////////////////////////////////////////////////////////

class Model
{
	TYPE_INFO(Model);

public:
	Model();
	~Model();

	/* Load model from file */
	bool Load(const char* fname);

	/* Set max number of meshes (Default: 4 or however many meshes th model file contains) */
	void SetMaxMeshes(Uint32 max);
	/* Add mesh (Make sure mesh bounding box has been set) */
	void AddMesh(const Mesh& mesh);

	/* Get number of meshes in model */
	Uint32 GetNumMeshes() const;
	/* Get mesh */
	Mesh& GetMesh(Uint32 index) const;
	/* Get bounding box */
	const BoundingBox& GetBoundingBox() const;

private:
	/* List of meshes */
	Array<Mesh> mMeshes;
	/* Model bounding box */
	BoundingBox mBoundingBox;
};

///////////////////////////////////////////////////////////////////////////////

#endif