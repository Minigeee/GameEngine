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

Shader::Shader() :
	mUniforms		(8)
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
	Uint32 program = 0;
	program = glCreateProgram();
	// List of shaders
	Array<Uint32> shaders(4);

	XmlNode programNode = doc.GetFirstNode("program");
	XmlNode shaderNode = programNode.GetFirstNode("shader");
	while (shaderNode.Exists())
	{
		const char* type = shaderNode.GetFirstAttribute("type").GetValue();
		Uint32 shader = 0;

		if (strcmp(type, "Vertex") == 0)
			shader = LoadShader(shaderNode.GetValue(), GL_VERTEX_SHADER);

		else if (strcmp(type, "Geometry") == 0)
			shader = LoadShader(shaderNode.GetValue(), GL_GEOMETRY_SHADER);

		else if (strcmp(type, "Fragment") == 0)
			shader = LoadShader(shaderNode.GetValue(), GL_FRAGMENT_SHADER);

		else
			LOG_WARNING << "Skipping " << shaderNode.GetValue() << ", unknown shader type\n";

		if (shader)
		{
			// Add shader to shader list
			glAttachShader(program, shader);
			shaders.Push(shader);
		}

		shaderNode = shaderNode.GetNextSibling("shader");
	}

	// Add transform feedback varyings
	XmlNode feedbackNode = programNode.GetFirstNode("feedback");
	if (feedbackNode.Exists())
	{
		Array<const char*> varyings(4);
		char* value = feedbackNode.GetValue();

		// Add first varying
		if (*value != ' ' && *value != 0)
			varyings.Push(value);

		while (*value != 0)
		{
			if (*++value == ' ' && *(value + 1) != 0)
			{
				*value = 0;
				varyings.Push(++value);
			}
		}

		Uint32 size = varyings.Size();
		if (varyings.Size())
			glTransformFeedbackVaryings(program, varyings.Size(), &varyings[0], GL_INTERLEAVED_ATTRIBS);
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
	for (Uint32 i = 0; i < shaders.Size(); ++i)
		glDeleteShader(shaders[i]);

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

	// Free text buffer
	free(buffer);

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

Uniform& Shader::FindUniform(const char* name)
{
	// Linear search
	for (Uint32 i = 0; i < mUniforms.Size(); ++i)
	{
		// Depends on string literals having same pointer location for comparison
		if (name == mUniforms[i].mName || strcmp(name, mUniforms[i].mName) == 0)
			return mUniforms[i];
	}

	// Add new uniform
	mUniforms.Push(Uniform(name));

	return mUniforms.Back();
}

void Shader::SetUniform(const char* name, int val)
{
	Uniform& uniform = FindUniform(name);
	uniform.mType = Uniform::Int;
	uniform.mHasChanged = true;

	*reinterpret_cast<int*>(uniform.mVariable) = val;
}

void Shader::SetUniform(const char* name, float val)
{
	Uniform& uniform = FindUniform(name);
	uniform.mType = Uniform::Float;
	uniform.mHasChanged = true;

	*reinterpret_cast<float*>(uniform.mVariable) = val;
}

void Shader::SetUniform(const char* name, const Vector2f& val)
{
	Uniform& uniform = FindUniform(name);
	uniform.mType = Uniform::Vec2;
	uniform.mHasChanged = true;

	*reinterpret_cast<Vector2f*>(uniform.mVariable) = val;
}

void Shader::SetUniform(const char* name, const Vector3f& val)
{
	Uniform& uniform = FindUniform(name);
	uniform.mType = Uniform::Vec3;
	uniform.mHasChanged = true;

	*reinterpret_cast<Vector3f*>(uniform.mVariable) = val;
}

void Shader::SetUniform(const char* name, const Vector4f& val)
{
	Uniform& uniform = FindUniform(name);
	uniform.mType = Uniform::Vec4;
	uniform.mHasChanged = true;

	*reinterpret_cast<Vector4f*>(uniform.mVariable) = val;
}

void Shader::SetUniform(const char* name, const Matrix2f& val)
{
	Uniform& uniform = FindUniform(name);
	uniform.mType = Uniform::Mat2;
	uniform.mHasChanged = true;

	*reinterpret_cast<Matrix2f*>(uniform.mVariable) = val;
}

void Shader::SetUniform(const char* name, const Matrix3f& val)
{
	Uniform& uniform = FindUniform(name);
	uniform.mType = Uniform::Mat3;
	uniform.mHasChanged = true;

	*reinterpret_cast<Matrix3f*>(uniform.mVariable) = val;
}

void Shader::SetUniform(const char* name, const Matrix4f& val)
{
	Uniform& uniform = FindUniform(name);
	uniform.mType = Uniform::Mat4;
	uniform.mHasChanged = true;

	*reinterpret_cast<Matrix4f*>(uniform.mVariable) = val;
}

///////////////////////////////////////////////////////////////////////////////

void Shader::ApplyUniforms()
{
	assert(mID == sCurrentBound);

	for (Uint32 i = 0; i < mUniforms.Size(); ++i)
	{
		Uniform& uniform = mUniforms[i];

		// Only set uniform if it has changed
		if (uniform.mHasChanged)
		{
			int loc = glGetUniformLocation(mID, uniform.mName);
			Uniform::Type type = (Uniform::Type)uniform.mType;
			float* var = uniform.mVariable;

			switch (type)
			{
			case Uniform::Int:
				glUniform1iv(loc, 1, (int*)var);
				break;
			case Uniform::Float:
				glUniform1fv(loc, 1, var);
				break;
			case Uniform::Vec2:
				glUniform2fv(loc, 1, var);
				break;
			case Uniform::Vec3:
				glUniform3fv(loc, 1, var);
				break;
			case Uniform::Vec4:
				glUniform4fv(loc, 1, var);
				break;
			case Uniform::Mat2:
				glUniformMatrix2fv(loc, 1, GL_FALSE, var);
				break;
			case Uniform::Mat3:
				glUniformMatrix3fv(loc, 1, GL_FALSE, var);
				break;
			case Uniform::Mat4:
				glUniformMatrix4fv(loc, 1, GL_FALSE, var);
				break;
			default:
				break;
			}

			// Uniform is up to date
			uniform.mHasChanged = false;
		}
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

Uniform::Uniform(const char* name) :
	mName		(name),
	mType		(0),
	mHasChanged	(false)
{

}

///////////////////////////////////////////////////////////////////////////////