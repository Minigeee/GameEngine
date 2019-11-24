#include <Graphics/Atmosphere.h>

#include <Resource/Resource.h>

#include <Graphics/Graphics.h>
#include <Graphics/VertexArray.h>
#include <Graphics/VertexBuffer.h>
#include <Graphics/Shader.h>
#include <Graphics/FrameBuffer.h>

#include <Scene/Scene.h>

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

Atmosphere::Atmosphere(Scene* scene) :
	LightingPass				(scene),

	mInitialized				(false),

	mSolarIntensity				(10.0f),
	mSolarIrradiance			(1.0f),
	mSunAngularRadius			(0.00935f / 2.0f),
	mTopRadius					(6420.0f),
	mBotRadius					(6360.0f),
	mScaleHeight_R				(8.0f),
	mScaleHeight_M				(1.2f),
	mScattering_R				(5.8e-3f, 13.5e-3f, 33.1e-3f),
	mScattering_M				(4.0e-3f / 0.9f),
	mMiePhase_G					(0.8f),
	mBaseHeight					(0.8f),
	mDistScale					(50.0f),

	mTransmittanceTexture_W		(256),
	mTransmittanceTexture_H		(64),

	mScatteringTexture_R		(32),
	mScatteringTexture_Mu		(128),
	mScatteringTexture_MuS		(32),
	mScatteringTexture_Nu		(8),

	mIrradianceTexture_W		(64),
	mIrradianceTexture_H		(16)
{
	mSunSize.x = tan(mSunAngularRadius);
	mSunSize.y = cos(mSunAngularRadius);
}

Atmosphere::~Atmosphere()
{
	if (mTransmittanceBuffer)
		Resource<FrameBuffer>::Free(mTransmittanceBuffer);
	if (mScatteringBuffer)
		Resource<FrameBuffer>::Free(mScatteringBuffer);
	if (mIrradianceBuffer)
		Resource<FrameBuffer>::Free(mIrradianceBuffer);

	mTransmittanceBuffer = 0;
	mScatteringBuffer = 0;
	mIrradianceBuffer = 0;
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
	Shader* irradianceShader = Resource<Shader>::Load("Shaders/Atmosphere/Irradiance.xml");
	mShader = Resource<Shader>::Load("Shaders/Atmosphere/Render.xml");


	// Create framebuffers
	FrameBuffer::TextureOptions options;

	mTransmittanceBuffer = Resource<FrameBuffer>::Create();
	mTransmittanceBuffer->Bind();
	mTransmittanceBuffer->SetSize(
		mTransmittanceTexture_W,
		mTransmittanceTexture_H
	);

	options.mDataType = Image::Float;
	options.mFormat = Texture::Rgb;
	mTransmittanceBuffer->AttachColor(true, options);


	mScatteringBuffer = Resource<FrameBuffer>::Create();
	mScatteringBuffer->Bind();
	mScatteringBuffer->SetSize(
		mScatteringTexture_Nu * mScatteringTexture_MuS,
		mScatteringTexture_Mu,
		mScatteringTexture_R
	);

	options.mDataType = Image::Ushort;
	options.mFormat = Texture::Rgba;
	options.mDimensions = Texture::_3D;
	mScatteringBuffer->AttachColor(true, options);

	
	mIrradianceBuffer = Resource<FrameBuffer>::Create();
	mIrradianceBuffer->Bind();
	mIrradianceBuffer->SetSize(
		mIrradianceTexture_W,
		mIrradianceTexture_H
	);

	options.mDataType = Image::Float;
	options.mFormat = Texture::Rgb;
	options.mDimensions = Texture::_2D;
	mIrradianceBuffer->AttachColor(true, options);


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

	// Irradiance
	mIrradianceBuffer->Bind();

	irradianceShader->Bind();
	SetUniforms(irradianceShader);
	BindTransmittance(irradianceShader, 1);
	BindScattering(irradianceShader, 2);
	irradianceShader->ApplyUniforms();

	vao->DrawArrays(6);


	// Set constant uniforms
	SetUniforms(mShader);
	mShader->Bind();
	mShader->ApplyUniforms();


	// Free resources
	Resource<VertexBuffer>::Free(vbo);
	Resource<VertexArray>::Free(vao);
	Resource<Shader>::Free(transmittanceShader);
	Resource<Shader>::Free(scatterShader);
	Resource<Shader>::Free(irradianceShader);

	mInitialized = true;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

void Atmosphere::RenderSetup(FrameBuffer* gbuffer)
{
	// Calculate inverse proj-view matrix
	Camera& cam = mScene->GetCamera();
	Matrix4f invProjView;
	Inverse(cam.GetProjection() * cam.GetView(), invProjView);

	// Shader uniforms
	mShader->Bind();

	gbuffer->GetColorTexture(0)->Bind(0);
	gbuffer->GetColorTexture(1)->Bind(1);
	gbuffer->GetColorTexture(2)->Bind(2);
	gbuffer->GetColorTexture(3)->Bind(3);
	gbuffer->GetDepthTexture()->Bind(4);
	mTransmittanceBuffer->GetColorTexture()->Bind(5);
	mScatteringBuffer->GetColorTexture()->Bind(6);
	mIrradianceBuffer->GetColorTexture()->Bind(7);

	mShader->SetUniform("mInvProjView", invProjView);
	mShader->SetUniform("mCamPos", cam.GetPosition());
	mShader->SetUniform("mSunDir", -mScene->GetDirLight().GetDirection());

	mScene->GetDirLight().Use(mShader);

	mShader->ApplyUniforms();
}

///////////////////////////////////////////////////////////////////////////////

void Atmosphere::SetUniforms(Shader* shader)
{
	shader->SetUniform("TRANSMITTANCE_TEXTURE_WIDTH", mTransmittanceTexture_W);
	shader->SetUniform("TRANSMITTANCE_TEXTURE_HEIGHT", mTransmittanceTexture_H);
	shader->SetUniform("SCATTERING_TEXTURE_R_SIZE", mScatteringTexture_R);
	shader->SetUniform("SCATTERING_TEXTURE_MU_SIZE", mScatteringTexture_Mu);
	shader->SetUniform("SCATTERING_TEXTURE_MU_S_SIZE", mScatteringTexture_MuS);
	shader->SetUniform("SCATTERING_TEXTURE_NU_SIZE", mScatteringTexture_Nu);
	shader->SetUniform("IRRADIANCE_TEXTURE_WIDTH", mIrradianceTexture_W);
	shader->SetUniform("IRRADIANCE_TEXTURE_HEIGHT", mIrradianceTexture_H);

	shader->SetUniform("mSolarIntensity", mSolarIntensity);
	shader->SetUniform("mSolarIrradiance", mSolarIrradiance);
	shader->SetUniform("mSunAngularRadius", mSunAngularRadius);
	shader->SetUniform("mTopRadius", mTopRadius);
	shader->SetUniform("mBotRadius", mBotRadius);
	shader->SetUniform("mHr", mScaleHeight_R);
	shader->SetUniform("mHm", mScaleHeight_M);
	shader->SetUniform("mBr", mScattering_R);
	shader->SetUniform("mBm", mScattering_M);
	shader->SetUniform("mMiePhaseG", mMiePhase_G);

	// Bind textures
	shader->SetUniform("mNormalSpec", 1);
	shader->SetUniform("mAlbedo", 2);
	shader->SetUniform("mSpecular", 3);
	shader->SetUniform("mDepth", 4);

	shader->SetUniform("mTransmittanceTexture", 5);
	shader->SetUniform("mScatteringTexture", 6);
	shader->SetUniform("mIrradianceTexture", 7);

	// Other unchanging uniforms
	shader->SetUniform("mSunSize", mSunSize);
	shader->SetUniform("mBaseHeight", mBaseHeight);
	shader->SetUniform("mDistScale", mDistScale);
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

FrameBuffer* Atmosphere::GetIrradianceBuffer() const
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

void Atmosphere::BindIrradiance(Shader* shader, Uint32 slot)
{
	mIrradianceBuffer->GetColorTexture()->Bind(slot);
	shader->SetUniform("mIrradianceTexture", (int)slot);
}

///////////////////////////////////////////////////////////////////////////////