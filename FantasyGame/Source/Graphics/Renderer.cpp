#include <Graphics/Renderer.h>

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

#include <Graphics/Graphics.h>
#include <Graphics/VertexArray.h>
#include <Graphics/VertexBuffer.h>
#include <Graphics/Shader.h>

#include <Resource/Resource.h>

#include <Math/Vector3.h>

Shader* shader;
VertexArray* vao;
VertexBuffer* vbo;

Renderer::Renderer()
{
	
}

Renderer::~Renderer()
{

}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

void Renderer::Init()
{
	shader = Resource<Shader>::Load("Test.xml");

	Vector3f vertices[] =
	{
		Vector3f(-0.5f, 0.5f, 0.0f),
		Vector3f(-0.5f, -0.5f, 0.0f),
		Vector3f(0.5f, 0.5f, 0.0f),

		Vector3f(-0.5f, -0.5f, 0.0f),
		Vector3f(0.5f, -0.5f, 0.0f),
		Vector3f(0.5f, 0.5f, 0.0f)
	};

	vbo = Resource<VertexBuffer>::Create();
	vbo->Bind(VertexBuffer::Array);
	vbo->BufferData(vertices, sizeof(vertices), VertexBuffer::Static);

	vao = Resource<VertexArray>::Create();
	vao->Bind();
	vao->VertexAttrib(0, 3);
}

///////////////////////////////////////////////////////////////////////////////

void Renderer::Render()
{
	shader->Bind();

	Graphics::SetClearColor(0.2f, 0.2f, 0.3f);
	Graphics::Clear();

	vao->Bind();
	vao->DrawArrays(6);
}

///////////////////////////////////////////////////////////////////////////////