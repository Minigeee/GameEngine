#include <Graphics/Systems.h>

#include <Math/Transform.h>

#include <Graphics/Model.h>

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

TransformMatrixSystem::TransformMatrixSystem()
{

}

TransformMatrixSystem::~TransformMatrixSystem()
{

}

///////////////////////////////////////////////////////////////////////////////

void TransformMatrixSystem::Execute(const TransformComponent& t, RenderComponent& r, float dt)
{
	// Update transform matrix
	r.mTransform = ToTransform(
		t.mPosition, t.mRotation, t.mScale
	);

	// Update bounding sphere
	const BoundingBox& box = r.mModel->GetBoundingBox();
	Vector3f boxPos = box.GetPosition();
	r.mBoundingSphere.p = boxPos + t.mPosition;
	r.mBoundingSphere.r = Distance(boxPos, box.mMin) * t.mScale;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////