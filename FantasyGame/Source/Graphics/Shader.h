#ifndef SHADER_H
#define SHADER_H

#include <Engine/Resource.h>
#include <iostream>

///////////////////////////////////////////////////////////////////////////////

class Shader : public Resource<Shader>
{
	RESOURCE_TYPE(Shader);

private:
	Shader() { std::cout << "AAA\n"; }
	virtual ~Shader() { }
};

///////////////////////////////////////////////////////////////////////////////

#endif