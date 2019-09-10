#include <Graphics/Atmosphere.h>

#include <Resource/Resource.h>

#include <Graphics/Graphics.h>
#include <Graphics/VertexArray.h>
#include <Graphics/VertexBuffer.h>
#include <Graphics/Shader.h>
#include <Graphics/FrameBuffer.h>

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

Atmosphere::Atmosphere() :
	mInitialized				(false),

	mTransmittanceTexture_W		(256),
	mTransmittanceTexture_H		(64),

	mScatteringTexture_R		(32),
	mScatteringTexture_Mu		(128),
	mScatteringTexture_MuS		(32),
	mScatteringTexture_Nu		(8)
{

}

Atmosphere::~Atmosphere()
{

}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

void Atmosphere::Init()
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
	Shader* transmittanceShader = Resource<Shader>::Load("Shaders/Atmosphere/Transmittance.xml");
	Shader* scatterShader = Resource<Shader>::Load("Shaders/Atmosphere/SingleScatter.xml");


	// Create framebuffers
	FrameBuffer::TextureOptions options;

	mTransmittanceBuffer = Resource<FrameBuffer>::Create();
	mTransmittanceBuffer->Bind();
	mTransmittanceBuffer->SetSize(
		mTransmittanceTexture_W,
		mTransmittanceTexture_H
	);

	options.mDataType = Image::Ushort;
	mTransmittanceBuffer->AttachColor(true, options);


	mScatteringBuffer = Resource<FrameBuffer>::Create();
	mScatteringBuffer->Bind();
	mScatteringBuffer->SetSize(
		mScatteringTexture_Nu * mScatteringTexture_MuS,
		mScatteringTexture_Mu,
		mScatteringTexture_R
	);

	options.mDataType = Image::Ushort;
	options.mDimensions = Texture::_3D;
	options.mFormat = Texture::Rgba;
	mScatteringBuffer->AttachColor(true, options);


	// Do calculations
	Graphics::Disable(Graphics::DepthTest);

	// Transmittance
	mTransmittanceBuffer->Bind();
	Graphics::Clear(Graphics::ColorBuffer);

	transmittanceShader->Bind();
	transmittanceShader->SetUniform("TRANSMITTANCE_TEXTURE_WIDTH", mTransmittanceTexture_W);
	transmittanceShader->SetUniform("TRANSMITTANCE_TEXTURE_HEIGHT", mTransmittanceTexture_H);
	transmittanceShader->SetUniform("mTopRadius", 6420.0f);
	transmittanceShader->SetUniform("mBotRadius", 6360.0f);
	transmittanceShader->SetUniform("mHr", 8.0f);
	transmittanceShader->SetUniform("mHm", 1.2f);
	transmittanceShader->SetUniform("mBr", Vector3f(5.8e-3f, 13.5e-3f, 33.1e-3f));
	transmittanceShader->SetUniform("mBm", Vector3f(4.0e-3f / 0.9f));
	transmittanceShader->ApplyUniforms();

	vao->DrawArrays(6);

	// Single scatter
	mScatteringBuffer->Bind();

	scatterShader->Bind();
	scatterShader->SetUniform("SCATTERING_TEXTURE_R_SIZE", mScatteringTexture_R);
	scatterShader->SetUniform("SCATTERING_TEXTURE_MU_SIZE", mScatteringTexture_Mu);
	scatterShader->SetUniform("SCATTERING_TEXTURE_MU_S_SIZE", mScatteringTexture_MuS);
	scatterShader->SetUniform("SCATTERING_TEXTURE_NU_SIZE", mScatteringTexture_Nu);
	scatterShader->SetUniform("mTransmittanceTexture", 1);
	scatterShader->SetUniform("mSolarIrradiance", Vector3f(1.0f));
	scatterShader->SetUniform("mSunAngularRadius", 0.1f);
	scatterShader->SetUniform("mTopRadius", 6420.0f);
	scatterShader->SetUniform("mBotRadius", 6360.0f);
	scatterShader->SetUniform("mHr", 8.0f);
	scatterShader->SetUniform("mHm", 1.2f);
	scatterShader->SetUniform("mBr", Vector3f(5.8e-3f, 13.5e-3f, 33.1e-3f));
	scatterShader->SetUniform("mBm", Vector3f(4.0e-3f / 0.9f));
	scatterShader->ApplyUniforms();

	mTransmittanceBuffer->GetColorTexture()->Bind(1);

	for (Uint32 r = 0; r < mScatteringTexture_R; ++r)
	{
		// Update z-component of render texture
		mScatteringBuffer->SetZValue(r);
		Graphics::Clear(Graphics::ColorBuffer);

		// Update z-value uniform
		scatterShader->SetUniform("mZ", (r + 0.5f) / mScatteringTexture_R);
		scatterShader->ApplyUniforms();

		// Render
		vao->DrawArrays(6);
	}


	// Free resources
	Resource<VertexBuffer>::Free(vbo);
	Resource<VertexArray>::Free(vao);
	Resource<Shader>::Free(transmittanceShader);
	Resource<Shader>::Free(scatterShader);

	mInitialized = true;
}

///////////////////////////////////////////////////////////////////////////////