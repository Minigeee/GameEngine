#include <Graphics/RenderPass.h>

#include <Resource/Resource.h>

#include <Graphics/FrameBuffer.h>
#include <Graphics/Shader.h>

#include <Scene/Scene.h>

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

RenderPass::RenderPass(Type type, bool create_fb) :
	mType				(type),
	mOutput				(0),
	mLightingPass		(0),
	mReflectionPlane	(0.0f)
{
	if (create_fb)
	{
		FrameBuffer::TextureOptions options;
		const Vector3u& size = FrameBuffer::Default.GetSize();

		mOutput = Resource<FrameBuffer>::Create();
		mOutput->Bind();
		mOutput->SetSize(size.x, size.y);

		// Use half float for HDR rendering
		options.mFormat = Texture::Rgb;
		options.mDataType = Image::Ushort;
		mOutput->AttachColor(true, options);
		mOutput->AttachDepth(true);
	}
}

RenderPass::~RenderPass()
{
	FreeOutput();
}

///////////////////////////////////////////////////////////////////////////////

void RenderPass::SetLightingPass(LightingPass* pass)
{
	mLightingPass = pass;
}

void RenderPass::FreeOutput()
{
	if (mOutput)
		Resource<FrameBuffer>::Free(mOutput);
	mOutput = 0;
}

///////////////////////////////////////////////////////////////////////////////

RenderPass::Type RenderPass::GetType() const
{
	return mType;
}

FrameBuffer* RenderPass::GetOutput() const
{
	return mOutput;
}

LightingPass* RenderPass::GetLightingPass() const
{
	return mLightingPass;
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

void DefaultLighting::Render(FrameBuffer* gbuffer)
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