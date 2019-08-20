#include <Graphics/Skybox.h>

#include <Resource/Resource.h>

#include <Graphics/Shader.h>
#include <Graphics/VertexArray.h>
#include <Graphics/VertexBuffer.h>

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

Skybox::Skybox() :
	mShader				(0),
	mVertexArray		(0),
	mVertexBuffer		(0)
{
	Init();

	mColors[0].mColor = Vector3f(0.680, 0.814, 1.000);
	mColors[0].mHeight = 0.5f;
	mColors[1].mColor = Vector3f(0.380, 0.561, 1.000);
	mColors[1].mHeight = 0.6f;
	mColors[2].mColor = Vector3f(0.231, 0.373, 0.900);
	mColors[2].mHeight = 1.0f;
}

Skybox::~Skybox()
{
	Resource<VertexArray>::Free(mVertexArray);
	Resource<VertexBuffer>::Free(mVertexBuffer);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

void Skybox::Init()
{
	float verts[] = {    
		-1.0f,  1.0f, -1.0f,
		-1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,
		 1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,

		-1.0f, -1.0f,  1.0f,
		-1.0f, -1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,

		 1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,

		-1.0f, -1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f, -1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,

		-1.0f,  1.0f, -1.0f,
		 1.0f,  1.0f, -1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f, -1.0f,

		-1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
		 1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
		 1.0f, -1.0f,  1.0f
	};

	mVertexBuffer = Resource<VertexBuffer>::Create();
	mVertexBuffer->Bind(VertexBuffer::Array);
	mVertexBuffer->BufferData(verts, sizeof(verts), VertexBuffer::Static);

	mVertexArray = Resource<VertexArray>::Create();
	mVertexArray->Bind();
	mVertexArray->VertexAttrib(0, 3);

	mShader = Resource<Shader>::Load("Shaders/Skybox.xml");
}

///////////////////////////////////////////////////////////////////////////////

void Skybox::SetColor(const SkyboxColor& color, Uint32 index)
{
	mColors[index] = color;
}

void Skybox::SetGradient(const SkyboxGrad& grad, Uint32 index)
{
	mGradients[index] = grad;
}

///////////////////////////////////////////////////////////////////////////////

void Skybox::Render(const Matrix4f& projView)
{
	mShader->Bind();

	// Update proj-view matrix
	Matrix4f mod = projView;
	mod.w = Vector4f(0.0f, 0.0f, 0.0f, 1.0f);
	mShader->SetUniform("projView", mod);

	mShader->SetUniform("colors[0].color", mColors[0].mColor);
	mShader->SetUniform("colors[0].height", mColors[0].mHeight);
	mShader->SetUniform("colors[1].color", mColors[1].mColor);
	mShader->SetUniform("colors[1].height", mColors[1].mHeight);
	mShader->SetUniform("colors[2].color", mColors[2].mColor);
	mShader->SetUniform("colors[2].height", mColors[2].mHeight);
	mShader->SetUniform("gradients[0].steepness", mGradients[0].mSteepness);
	mShader->SetUniform("gradients[0].midpoint", mGradients[0].mMidpoint);
	mShader->SetUniform("gradients[1].steepness", mGradients[1].mSteepness);
	mShader->SetUniform("gradients[1].midpoint", mGradients[1].mMidpoint);
	mShader->UpdateUniforms();

	mVertexArray->Bind();
	mVertexArray->DrawArrays(36);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

Skybox::SkyboxColor::SkyboxColor(const Vector3f& color, float h) :
	mColor		(color),
	mHeight		(h)
{

}

///////////////////////////////////////////////////////////////////////////////

Skybox::SkyboxGrad::SkyboxGrad(float steep, float mid) :
	mSteepness		(steep),
	mMidpoint		(mid)
{

}

///////////////////////////////////////////////////////////////////////////////