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

void TransformMatrixSystem::Update(float dt)
{
	Array<ComponentList<TransformComponent>> a = GetComponentLists<TransformComponent>();
	Array<ComponentList<RenderComponent>> b = GetComponentLists<RenderComponent>();

	for (Uint32 i = 0; i < a.Size(); ++i)
	{
		ComponentList<TransformComponent>& transforms = a[i];
		ComponentList<RenderComponent>& renderables = b[i];

		for (Uint32 n = 0; n < transforms.mSize; ++n)
		{
			TransformComponent& t = transforms[n];
			RenderComponent& r = renderables[n];

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
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////