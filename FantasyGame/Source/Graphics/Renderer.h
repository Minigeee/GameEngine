#ifndef RENDERER_H
#define RENDERER_H

#include <Core/Array.h>

#include <Math/Matrix4.h>

///////////////////////////////////////////////////////////////////////////////

class VertexArray;
class VertexBuffer;
class Renderable;
class Model;
class Camera;

class Renderer
{
public:
	Renderer();
	~Renderer();

	/* Initialize renderer */
	void Init();
	/* Render stuff */
	void Render();

	/* Set camera to render with */
	void SetCamera(Camera* camera);

	/* Register model */
	void RegisterModel(Model* model, Uint32 num = 64);
	/* Add static renderable object */
	void AddStatic(Renderable* object);
	/* Mark static object for update */
	void UpdateStatic(Renderable* object);

private:
	struct VertexArrayData
	{
		/* Vertex array to render */
		VertexArray* mVertexArray;
		/* How many vertices to render */
		Uint32 mNumVertices;
	};

	struct RenderData
	{
		/* Transform matrices */
		Array<Matrix4f> mTransforms;
		/* Instance data buffer */
		VertexBuffer* mInstanceBuffer;
		/* Keeps track of buffer size */
		Uint32 mBufferSize;

		/* Marks if needs update */
		bool mNeedsUpdate;
	};

	/* Pre-render update */
	void Update();

private:
	/* Camera to render with */
	Camera* mCamera;

	/* List of vertex arrays */
	Array<VertexArrayData> mVaoData;
	/* Map vertex array to render data */
	Array<int> mDataMap;
	/* Render data */
	Array<RenderData> mRenderData;

	/* Marks if render data updated */
	bool mRenderDataUpdated;
};

///////////////////////////////////////////////////////////////////////////////

#endif