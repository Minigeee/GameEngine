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
	mFrameBuffer		(0),
	mShader				(0),
	mVertexArray		(0),
	mVertexBuffer		(0),
	mIsEnabled			(false)
{

}

PostProcess::~PostProcess()
{
	if (mFrameBuffer)
		delete mFrameBuffer;

	Resource<VertexArray>::Free(mVertexArray);
	Resource<VertexBuffer>::Free(mVertexBuffer);

	mFrameBuffer = 0;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

void PostProcess::Enable()
{
	if (!mFrameBuffer)
	{
		mFrameBuffer = new FrameBuffer();
		mFrameBuffer->Bind();
		mFrameBuffer->SetSize(1920, 1080);
		mFrameBuffer->AttachColor(true);
		mFrameBuffer->AttachDepth(true);
	}

	if (!mShader)
	{
		mShader = Resource<Shader>::Load("Shaders/PostProcess.xml");
	}

	if (!mVertexArray || !mVertexBuffer)
	{
		float verts[] = {
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
	}

	mIsEnabled = true;
}

///////////////////////////////////////////////////////////////////////////////

void PostProcess::Render()
{
	// Bind default framebuffer
	FrameBuffer::Default.Bind();

	Graphics::Disable(Graphics::DepthTest);
	Graphics::Clear(Graphics::ColorBuffer);

	// Uniforms
	mShader->Bind();

	mFrameBuffer->GetColorTexture()->Bind(0);
	mShader->SetUniform("color", 0);


	// Render
	mVertexArray->Bind();
	mVertexArray->DrawArrays(6);
}

///////////////////////////////////////////////////////////////////////////////

bool PostProcess::IsEnabled() const
{
	return mIsEnabled;
}

FrameBuffer* PostProcess::GetFrameBuffer() const
{
	return mFrameBuffer;
}

///////////////////////////////////////////////////////////////////////////////