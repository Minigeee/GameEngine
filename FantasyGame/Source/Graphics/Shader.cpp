#include <Graphics/Shader.h>

#include <Core/LogFile.h>

#include <Graphics/OpenGL.h>

#include <Resource/XmlDocument.h>

#include <assert.h>
#include <fstream>

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

Uint32 Shader::sCurrentBound = 0;

///////////////////////////////////////////////////////////////////////////////

Shader::Shader()
{

}

Shader::~Shader()
{
	if (mID)
	{
		glDeleteProgram(mID);
		mID = 0;
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

bool Shader::Load(const char* fname)
{
	XmlDocument doc;
	if (!doc.Load(fname))
	{
		LOG_ERROR << "Failed to load file " << fname << "\n";
		return false;
	}

	// Create program
	Uint32 program = 0, v = 0, g = 0, f = 0;
	program = glCreateProgram();

	XmlNode programNode = doc.GetFirstNode("program");
	XmlNode shaderNode = programNode.GetFirstNode("shader");
	while (shaderNode.Exists())
	{
		const char* type = shaderNode.GetFirstAttribute("type").GetValue();
		Uint32 shader = 0;

		if (strcmp(type, "Vertex") == 0)
		{
			shader = LoadShader(shaderNode.GetValue(), GL_VERTEX_SHADER);
			v = shader;
		}
		else if (strcmp(type, "Geometry") == 0)
		{
			shader = LoadShader(shaderNode.GetValue(), GL_GEOMETRY_SHADER);
			g = shader;
		}
		else if (strcmp(type, "Fragment") == 0)
		{
			shader = LoadShader(shaderNode.GetValue(), GL_FRAGMENT_SHADER);
			f = shader;
		}
		else
			LOG_WARNING << "Skipping " << shaderNode.GetValue() << ", unknown shader type\n";

		if (shader)
			glAttachShader(program, shader);

		shaderNode = shaderNode.GetNextSibling("shader");
	}

	// Link program
	glLinkProgram(program);

	// Check status
	int success;
	char infoLog[512];
	glGetProgramiv(program, GL_LINK_STATUS, &success);
	if (!success)
	{
		glGetProgramInfoLog(program, 512, NULL, infoLog);
		LOG_ERROR << "Failed to link program:\n" << infoLog << "\n";
		glDeleteProgram(program);
		return false;
	}

	// Delete shaders
	if (v) glDeleteShader(v);
	if (g) glDeleteShader(g);
	if (f) glDeleteShader(f);

	mID = program;


	mFileHash = fname;

	LOG_INFO << "Loaded shader " << fname << "\n";
	return true;
}

///////////////////////////////////////////////////////////////////////////////

Uint32 Shader::LoadShader(const char* fname, Uint32 type)
{
	// Read file
	std::ifstream file(fname, std::ios::binary | std::ios::ate);
	if (!file.is_open())
	{
		LOG_ERROR << "Could not find shader file named " << fname << "\n";
		return 0;
	}

	// Get file size
	Uint32 fsize = (Uint32)file.tellg();
	file.seekg(0, std::ios::beg);

	char* buffer = (char*)malloc(fsize + 1);
	file.read(buffer, fsize);
	buffer[fsize] = 0;

	file.close();


	// Create shader
	Uint32 shader = 0;
	shader = glCreateShader(type);
	glShaderSource(shader, 1, &buffer, NULL);
	glCompileShader(shader);

	// Check status
	int success;
	char infoLog[512];
	glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(shader, 512, NULL, infoLog);
		LOG_ERROR << "Failed to compile " << fname << ":\n" << infoLog << "\n";
		glDeleteShader(shader);
		return 0;
	}

	// Free text buffer
	free(buffer);

	return shader;
}

///////////////////////////////////////////////////////////////////////////////

void Shader::Bind()
{
	glUseProgram(mID);
	sCurrentBound = mID;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

void Shader::SetUniform(const char* name, int val)
{
	assert(sCurrentBound == mID);
	int loc = glGetUniformLocation(mID, name);
	glUniform1i(loc, val);
}

void Shader::SetUniform(const char* name, float val)
{
	assert(sCurrentBound == mID);
	int loc = glGetUniformLocation(mID, name);
	glUniform1f(loc, val);
}

void Shader::SetUniform(const char* name, const Vector2f& val)
{
	assert(sCurrentBound == mID);
	int loc = glGetUniformLocation(mID, name);
	glUniform2fv(loc, 1, (const float*)&val);
}

void Shader::SetUniform(const char* name, const Vector3f& val)
{
	assert(sCurrentBound == mID);
	int loc = glGetUniformLocation(mID, name);
	glUniform3fv(loc, 1, (const float*)&val);
}

void Shader::SetUniform(const char* name, const Vector4f& val)
{
	assert(sCurrentBound == mID);
	int loc = glGetUniformLocation(mID, name);
	glUniform4fv(loc, 1, (const float*)&val);
}

void Shader::SetUniform(const char* name, const Matrix2f& val)
{
	assert(sCurrentBound == mID);
	int loc = glGetUniformLocation(mID, name);
	glUniformMatrix2fv(loc, 1, GL_FALSE, (const float*)&val);
}

void Shader::SetUniform(const char* name, const Matrix3f& val)
{
	assert(sCurrentBound == mID);
	int loc = glGetUniformLocation(mID, name);
	glUniformMatrix3fv(loc, 1, GL_FALSE, (const float*)&val);
}

void Shader::SetUniform(const char* name, const Matrix4f& val)
{
	assert(sCurrentBound == mID);
	int loc = glGetUniformLocation(mID, name);
	glUniformMatrix4fv(loc, 1, GL_FALSE, (const float*)&val);
}

///////////////////////////////////////////////////////////////////////////////