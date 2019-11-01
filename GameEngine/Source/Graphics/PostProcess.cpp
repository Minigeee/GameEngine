#include <Graphics/PostProcess.h>

#include <Resource/Resource.h>

#include <Graphics/Graphics.h>
#include <Graphics/FrameBuffer.h>
#include <Graphics/Shader.h>
#include <Graphics/VertexArray.h>
#include <Graphics/VertexBuffer.h>

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

PostProcess::PostProcess() :
	mVertexArray		(0),
	mVertexBuffer		(0),
	mIsEnabled			(false)
{

}

PostProcess::~PostProcess()
{
	// Free all effects
	for (Uint32 i = 0; i < mRenderQueue.Size(); ++i)
		delete mRenderQueue[i];

	if (mVertexArray)
		Resource<VertexArray>::Free(mVertexArray);
	if (mVertexBuffer)
		Resource<VertexBuffer>::Free(mVertexBuffer);

	mVertexArray = 0;
	mVertexBuffer = 0;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

void PostProcess::Enable()
{
	if (mIsEnabled) return;

	float verts[] =
	{
		-1.0f,  1.0f,
		-1.0f, -1.0f,
		 1.0f,  1.0f,

		-1.0f, -1.0f,
		 1.0f, -1.0f,
		 1.0f,  1.0f
	};

	mVertexBuffer = Resource<VertexBuffer>::Create();
	mVertexBuffer->Bind(VertexBuffer::Array);
	mVertexBuffer->BufferData(verts, sizeof(verts), VertexBuffer::Static);

	mVertexArray = Resource<VertexArray>::Create();
	mVertexArray->Bind();
	mVertexArray->VertexAttrib(0, 2);

	mRenderQueue.Reserve(4);


	mIsEnabled = true;
}

bool PostProcess::IsEnabled() const
{
	return mIsEnabled;
}

///////////////////////////////////////////////////////////////////////////////

void PostProcess::Render(FrameBuffer* output)
{
	if (!mIsEnabled || !mRenderQueue.Size()) return;

	// Bind VAO
	mVertexArray->Bind();

	// Set graphics options
	Graphics::Disable(Graphics::DepthTest);
	Graphics::Disable(Graphics::CullFace);

	for (Uint32 i = 0; i < mRenderQueue.Size() - 1; ++i)
	{
		// Bind next input buffer
		mRenderQueue[i + 1]->GetInput()->Bind();

		// Render
		Graphics::Clear(Graphics::ColorBuffer);
		mRenderQueue[i]->Render(mVertexArray);
	}

	// Render last effect
	output->Bind();
	Graphics::Clear(Graphics::ColorBuffer);
	mRenderQueue.Back()->Render(mVertexArray);
}

///////////////////////////////////////////////////////////////////////////////

void PostProcess::RenderEffect(Effect* effect, FrameBuffer* output)
{
	// Bind next output buffer
	output->Bind();

	// Clear
	Graphics::Disable(Graphics::DepthTest);
	Graphics::Disable(Graphics::CullFace);
	Graphics::Clear(Graphics::ColorBuffer);

	// Render
	mVertexArray->Bind();
	effect->Render(mVertexArray);
}

///////////////////////////////////////////////////////////////////////////////

FrameBuffer* PostProcess::GetInput() const
{
	if (!mRenderQueue.Size())
		return 0;

	return mRenderQueue[0]->GetInput();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

PostProcess::Effect::Effect() :
	mInput		(0),
	mShader		(0)
{
	mInput = Resource<FrameBuffer>::Create();

	// Uses default framebuffer size
	const Vector3u& size = FrameBuffer::Default.GetSize();

	mInput->Bind();
	mInput->SetSize(size.x, size.y);
	mInput->AttachColor(true);
}

PostProcess::Effect::~Effect()
{
	if (mInput)
		Resource<FrameBuffer>::Free(mInput);
	mInput = 0;
}

///////////////////////////////////////////////////////////////////////////////

FrameBuffer* PostProcess::Effect::GetInput() const
{
	return mInput;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

GammaCorrection::GammaCorrection() :
	mGamma		(2.2f)
{
	mShader = Resource<Shader>::Load("Shaders/PostProcess/GammaCorrection.xml");
}

GammaCorrection::~GammaCorrection()
{
	if (mShader)
		Resource<Shader>::Free(mShader);
	mShader = 0;
}

void GammaCorrection::Render(VertexArray* vao)
{
	mShader->Bind();
	mShader->SetUniform("mColor", 0);
	mShader->SetUniform("mGamma", mGamma);
	mShader->ApplyUniforms();

	mInput->GetColorTexture()->Bind(0);

	// Render
	vao->DrawArrays(6);
}

///////////////////////////////////////////////////////////////////////////////