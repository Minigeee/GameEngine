#include <Graphics/RenderPass.h>

#include <Resource/Resource.h>

#include <Graphics/Shader.h>

#include <Scene/Scene.h>

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

RenderPass::RenderPass() :
	mType				(None),
	mTarget				(0),
	mLightingPass		(0)
{
	
}

RenderPass::RenderPass(Type type) :
	mType				(type),
	mTarget				(0),
	mLightingPass		(0)
{
	
}

RenderPass::~RenderPass()
{
	if (mTarget)
		Resource<FrameBuffer>::Free(mTarget);
	mTarget = 0;
}

///////////////////////////////////////////////////////////////////////////////

void RenderPass::SetLightingPass(LightingPass* pass)
{
	mLightingPass = pass;
}

void RenderPass::SetPlane(const Plane& plane)
{
	mPlane = plane;
}

void RenderPass::SetClippingEnabled(bool enabled)
{
	mIsClippingEnabled = enabled;
}

///////////////////////////////////////////////////////////////////////////////

void RenderPass::CreateTarget(Texture::Format fmt, Image::DataType dtype)
{
	FrameBuffer::TextureOptions options;
	const Vector3u& size = FrameBuffer::Default.GetSize();

	mTarget = Resource<FrameBuffer>::Create();
	mTarget->Bind();
	mTarget->SetSize(size.x, size.y);

	// Use half float for HDR rendering
	options.mFormat = fmt;
	options.mDataType = dtype;
	mTarget->AttachColor(true, options);
	mTarget->AttachDepth(true);
}

///////////////////////////////////////////////////////////////////////////////

RenderPass::Type RenderPass::GetType() const
{
	return mType;
}

FrameBuffer* RenderPass::GetTarget() const
{
	return mTarget;
}

LightingPass* RenderPass::GetLightingPass() const
{
	return mLightingPass;
}

const Plane& RenderPass::GetPlane() const
{
	return mPlane;
}

bool RenderPass::IsClippingEnabled() const
{
	return mIsClippingEnabled;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

LightingPass::LightingPass(Scene* scene) :
	mShader			(0),
	mScene			(scene)
{

}

LightingPass::~LightingPass()
{

}

///////////////////////////////////////////////////////////////////////////////

Shader* LightingPass::GetShader() const
{
	return mShader;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

DefaultLighting::DefaultLighting(Scene* scene) :
	LightingPass		(scene)
{
	mShader = Resource<Shader>::Load("Shaders/DefaultLighting.xml");
}

DefaultLighting::~DefaultLighting()
{
	if (mShader)
		Resource<Shader>::Free(mShader);
}

///////////////////////////////////////////////////////////////////////////////

void DefaultLighting::RenderSetup(FrameBuffer* gbuffer)
{
	mShader->Bind();

	mShader->SetUniform("mPosition", 0);
	mShader->SetUniform("mNormalSpec", 1);
	mShader->SetUniform("mAlbedo", 2);
	mShader->SetUniform("mSpecular", 3);
	gbuffer->GetColorTexture(0)->Bind(0);
	gbuffer->GetColorTexture(1)->Bind(1);
	gbuffer->GetColorTexture(2)->Bind(2);
	gbuffer->GetColorTexture(3)->Bind(3);

	// Lights
	mScene->GetDirLight().Use(mShader);

	mShader->SetUniform("mCamPos", mScene->GetCamera().GetPosition());
	mShader->SetUniform("mAmbient", mScene->GetAmbient());

	mShader->ApplyUniforms();
}

///////////////////////////////////////////////////////////////////////////////