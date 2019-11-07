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

		// Update bounding box
		// The method used to calculate new bounding box is not very accurate, but is good enough most of the time
		const BoundingBox& box = mModel->GetBoundingBox();

		// All vertices in box
		Vector4f v[8] =
		{
			mTransform * Vector4f(box.mMin.x, box.mMin.y, box.mMin.z, 1.0f),
			mTransform * Vector4f(box.mMin.x, box.mMin.y, box.mMax.z, 1.0f),
			mTransform * Vector4f(box.mMin.x, box.mMax.y, box.mMin.z, 1.0f),
			mTransform * Vector4f(box.mMin.x, box.mMax.y, box.mMax.z, 1.0f),
			mTransform * Vector4f(box.mMax.x, box.mMin.y, box.mMin.z, 1.0f),
			mTransform * Vector4f(box.mMax.x, box.mMin.y, box.mMax.z, 1.0f),
			mTransform * Vector4f(box.mMax.x, box.mMax.y, box.mMin.z, 1.0f),
			mTransform * Vector4f(box.mMax.x, box.mMax.y, box.mMax.z, 1.0f)
		};

		Vector3f min = Vector3f(v[0].x, v[0].y, v[0].z);
		Vector3f max = min;

		for (Uint32 i = 0; i < 8; ++i)
		{
			if (v[i].x < min.x)
				min.x = v[i].x;
			else if (v[i].x > max.x)
				max.x = v[i].x;

			if (v[i].y < min.y)
				min.y = v[i].y;
			else if (v[i].y > max.y)
				max.y = v[i].y;

			if (v[i].z < min.z)
				min.z = v[i].z;
			else if (v[i].z > max.z)
				max.z = v[i].z;
		}

		// Set new bounds
		mBoundingBox.mMin = min;
		mBoundingBox.mMax = max;

		mTransformDirty = false;
	}
}

///////////////////////////////////////////////////////////////////////////////

const Matrix4f& Renderable::GetTransform()
{
	UpdateTransform();
	return mTransform;
}

const BoundingBox& Renderable::GetBoundingBox()
{
	UpdateTransform();
	return mBoundingBox;
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

Uint32 Renderable::GetInstanceID() const
{
	return mInstanceID;
}

///////////////////////////////////////////////////////////////////////////////