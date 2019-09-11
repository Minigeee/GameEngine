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

	mSolarIrradiance			(1.0f),
	mSunAngularRadius			(0.1f),
	mTopRadius					(6420.0f),
	mBotRadius					(6360.0f),
	mScaleHeight_R				(8.0f),
	mScaleHeight_M				(1.2f),
	mScattering_R				(5.8e-3f, 13.5e-3f, 33.1e-3f),
	mScattering_M				(4.0e-3f / 0.9f),
	mMiePhase_G					(0.8f),

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
	SetUniforms(transmittanceShader);
	transmittanceShader->ApplyUniforms();

	vao->DrawArrays(6);

	// Single scatter
	mScatteringBuffer->Bind();

	scatterShader->Bind();
	SetUniforms(scatterShader);
	BindTransmittance(scatterShader, 1);
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
///////////////////////////////////////////////////////////////////////////////

void Atmosphere::SetUniforms(Shader* shader)
{
	shader->SetUniform("TRANSMITTANCE_TEXTURE_WIDTH", mTransmittanceTexture_W);
	shader->SetUniform("TRANSMITTANCE_TEXTURE_HEIGHT", mTransmittanceTexture_H);
	shader->SetUniform("SCATTERING_TEXTURE_R_SIZE", mScatteringTexture_R);
	shader->SetUniform("SCATTERING_TEXTURE_MU_SIZE", mScatteringTexture_Mu);
	shader->SetUniform("SCATTERING_TEXTURE_MU_S_SIZE", mScatteringTexture_MuS);
	shader->SetUniform("SCATTERING_TEXTURE_NU_SIZE", mScatteringTexture_Nu);

	shader->SetUniform("mSolarIrradiance", mSolarIrradiance);
	shader->SetUniform("mSunAngularRadius", mSunAngularRadius);
	shader->SetUniform("mTopRadius", mTopRadius);
	shader->SetUniform("mBotRadius", mBotRadius);
	shader->SetUniform("mHr", mScaleHeight_R);
	shader->SetUniform("mHm", mScaleHeight_M);
	shader->SetUniform("mBr", mScattering_R);
	shader->SetUniform("mBm", mScattering_M);
	shader->SetUniform("mMiePhaseG", mMiePhase_G);
}

///////////////////////////////////////////////////////////////////////////////

FrameBuffer* Atmosphere::GetTransmittanceBuffer() const
{
	return mTransmittanceBuffer;
}

FrameBuffer* Atmosphere::GetScatteringBuffer() const
{
	return mScatteringBuffer;
}

///////////////////////////////////////////////////////////////////////////////

void Atmosphere::BindTransmittance(Shader* shader, Uint32 slot)
{
	mTransmittanceBuffer->GetColorTexture()->Bind(slot);
	shader->SetUniform("mTransmittanceTexture", (int)slot);
}

void Atmosphere::BindScattering(Shader* shader, Uint32 slot)
{
	mScatteringBuffer->GetColorTexture()->Bind(slot);
	shader->SetUniform("mScatteringTexture", (int)slot);
}

///////////////////////////////////////////////////////////////////////////////