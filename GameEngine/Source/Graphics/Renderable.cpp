#include <Graphics/Renderable.h>

#include <Math/Transform.h>

#include <Graphics/Model.h>

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

Renderable::Renderable() :
	mTransform			(1.0f),
	mInstanceID			(0),
	mModel				(0)
{

}

Renderable::~Renderable()
{

}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

void Renderable::UpdateTransform()
{
	if (mTransformDirty)
	{
		mTransform = ToTransform(mPosition, mRotation, mScale);

		// Update bounding sphere
		const BoundingBox& box = mModel->GetBoundingBox();
		Vector3f boxPos = box.GetPosition();

		mBoundingSphere.p = boxPos + mPosition;
		mBoundingSphere.r = Distance(boxPos, box.mMin) * mScale;

		mTransformDirty = false;
	}
}

///////////////////////////////////////////////////////////////////////////////

const Matrix4f& Renderable::GetTransform()
{
	UpdateTransform();
	return mTransform;
}

const BoundingSphere& Renderable::GetBoundingSphere()
{
	UpdateTransform();
	return mBoundingSphere;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

void Renderable::SetModel(Model* model)
{
	mModel = model;
}

Model* Renderable::GetModel() const
{
	return mModel;
}

Uint64 Renderable::GetInstanceID() const
{
	return mInstanceID;
}

///////////////////////////////////////////////////////////////////////////////