#version 330 core

///////////////////////////////////////////////////////////////////////////////

struct Material
{
    vec3 mDiffuse;
    vec3 mSpecular;
    float mSpecFactor;
};

///////////////////////////////////////////////////////////////////////////////

uniform Material mMaterial;

in vec3 FragPos;
in vec3 Normal;

layout (location = 0) out vec3 gPosition;
layout (location = 1) out vec4 gNormalSpec;
layout (location = 2) out vec3 gAlbedo;
layout (location = 3) out vec3 gSpecular;

///////////////////////////////////////////////////////////////////////////////

void main()
{
    gPosition = FragPos;
    gNormalSpec = vec4(Normal, mMaterial.mSpecFactor);
    gAlbedo = mMaterial.mDiffuse;
    gSpecular = mMaterial.mSpecular;
}