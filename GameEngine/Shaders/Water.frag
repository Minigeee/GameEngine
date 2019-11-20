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
uniform sampler2D mReflectTex;

in vec3 FragPos;
in vec3 Normal;
in vec4 ClipSpace;

layout (location = 0) out vec3 gPosition;
layout (location = 1) out vec4 gNormalSpec;
layout (location = 2) out vec3 gAlbedo;
layout (location = 3) out vec3 gSpecular;

///////////////////////////////////////////////////////////////////////////////

vec3 ReflectColor(vec2 uv)
{
    vec3 color = texture(mReflectTex, vec2(uv.x, 1.0f - uv.y)).rgb;
    return color;
}

///////////////////////////////////////////////////////////////////////////////

void main()
{
    vec2 ndc = ClipSpace.xy / ClipSpace.w;
    vec2 uv = ndc * 0.5f + 0.5f;

    vec3 reflectColor = ReflectColor(uv);

    gPosition = FragPos;
    gNormalSpec = vec4(Normal, mMaterial.mSpecFactor);
    gAlbedo = reflectColor;
    gSpecular = mMaterial.mSpecular;
}