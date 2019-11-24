#version 330 core

///////////////////////////////////////////////////////////////////////////////

struct Material
{
    vec3 mDiffuse;
    vec3 mSpecular;
    float mSpecFactor;
};

///////////////////////////////////////////////////////////////////////////////

uniform vec3 mCamPos; 
uniform vec2 mCamPlanes;
uniform float mTime;

uniform Material mMaterial;
uniform sampler2D mReflectTex;
uniform sampler2D mRefractTex;
uniform sampler2D mDepthTex;
uniform vec3 mWaterColor;
uniform float mWaterDensity;
uniform float mMinColor;
uniform float mFresnelFactor;
uniform float mReflectStrength;

in vec3 FragPos;
in vec3 Normal;
in vec4 ClipSpace;

layout (location = 0) out vec3 gPosition;
layout (location = 1) out vec4 gNormalSpec;
layout (location = 2) out vec3 gAlbedo;
layout (location = 3) out vec3 gSpecular;

///////////////////////////////////////////////////////////////////////////////

float LinearDepth(float z)
{
    float near = mCamPlanes.x;
    float far = mCamPlanes.y;
    return 2.0f * near * far / (far + near - (2.0f * z - 1.0f) * (far - near));
}

float CalcWaterDepth(vec2 uv)
{
    float depth = texture(mDepthTex, uv).r;
    float floorDist = LinearDepth(depth);

    depth = gl_FragCoord.z;
    float waterDist = LinearDepth(depth);

    return floorDist - waterDist;
}

///////////////////////////////////////////////////////////////////////////////

vec3 ReflectColor(vec2 uv)
{
    vec3 color = texture(mReflectTex, vec2(uv.x, 1.0f - uv.y)).rgb;
    return color;
}

///////////////////////////////////////////////////////////////////////////////

vec3 RefractColor(vec2 uv)
{
    vec3 color = texture(mRefractTex, uv).rgb;
    return color;
}

///////////////////////////////////////////////////////////////////////////////

vec3 Combine(
    vec3 reflectColor, vec3 refractColor, float waterDepth, vec3 viewDir)
{
    float fresnel = dot(viewDir, vec3(0, 1, 0));
    fresnel = pow(fresnel, mFresnelFactor);

    float scatterFactor =
        clamp((1.0f - mMinColor) * pow(mWaterDensity, waterDepth), 0.0f, 1.0f);
    refractColor = mix(mWaterColor, refractColor, scatterFactor);
    reflectColor = mix(mWaterColor, reflectColor, mReflectStrength);

    vec3 color = mix(reflectColor, refractColor, fresnel);

    return color;
}

///////////////////////////////////////////////////////////////////////////////

void main()
{
    vec3 viewDir = normalize(mCamPos - FragPos);

    vec2 ndc = ClipSpace.xy / ClipSpace.w;
    vec2 uv = ndc * 0.5f + 0.5f;
    uv = clamp(uv, 0.0f, 1.0f);

    vec3 reflectColor = ReflectColor(uv);
    vec3 refractColor = RefractColor(uv);
    float waterDepth = CalcWaterDepth(uv);

    gPosition = FragPos;
    gNormalSpec = vec4(Normal, mMaterial.mSpecFactor);
    gAlbedo = Combine(reflectColor, refractColor, waterDepth, viewDir);
    gSpecular = mMaterial.mSpecular;
}