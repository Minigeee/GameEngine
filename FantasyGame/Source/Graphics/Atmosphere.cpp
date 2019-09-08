#include <Graphics/Atmosphere.h>

#include <Resource/Resource.h>

#include <Graphics/Graphics.h>
#include <Graphics/VertexArray.h>
#include <Graphics/VertexBuffer.h>
#include <Graphics/Shader.h>
#include <Graphics/FrameBuffer.h>

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

Atmosphere::Atmosphere()
{

}

Atmosphere::~Atmosphere()
{

}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

void Atmosphere::DoCalculations()
{
	// Create quad
	float verts[] =
	{
		-1.0f,  1.0f,
		-1.0f, -1.0f,
		 1.0f,  1.0f,

		-1.0f, -1.0f,
		 1.0f, -1.0f,
		 1.0f,  1.0f
	};

	VertexBuffer* vbo = Resource<VertexBuffer>::Create();
	vbo->Bind(VertexBuffer::Array);
	vbo->BufferData(verts, sizeof(verts), VertexBuffer::Static);

	VertexArray* vao = Resource<VertexArray>::Create();
	vao->Bind();
	vao->VertexAttrib(0, 2);

	// Load shaders
	Shader* transmittanceShader = Resource<Shader>::Load("Shaders/Atmosphere/CalcTransmittance.xml");

	// Create framebuffers
	mFB = Resource<FrameBuffer>::Create();
	mFB->Bind();
	mFB->SetSize(1024, 1024);
	mFB->AttachColor(true);


	// Do calculations
	Graphics::Disable(Graphics::DepthTest);

	// Transmittance
	mFB->Bind();
	Graphics::Clear(Graphics::ColorBuffer);

	transmittanceShader->Bind();
	transmittanceShader->SetUniform("mTopRadius", 6420000.0f);
	transmittanceShader->SetUniform("mBotRadius", 6360000.0f);
	transmittanceShader->SetUniform("mHr", 8000.0f);
	transmittanceShader->SetUniform("mHm", 1200.0f);
	transmittanceShader->SetUniform("mBr", Vector3f(5.8e-3f, 1.35e-2f, 3.31e-2f));
	transmittanceShader->SetUniform("mBm", Vector3f(4.0e-3f * 0.9f));
	transmittanceShader->ApplyUniforms();

	vao->DrawArrays(6);


	// Free resources
	Resource<VertexBuffer>::Free(vbo);
	Resource<VertexArray>::Free(vao);
	Resource<Shader>::Free(transmittanceShader);
}

///////////////////////////////////////////////////////////////////////////////