#version 330 core

out vec4 FragColor;
in vec2 TexCoord;

uniform sampler2D mColor;
uniform sampler2D mDepth;
uniform sampler3D mScatteringTexture;

uniform mat4 mInvProjView;
uniform vec3 mCamPos;
uniform vec3 mSunDir;
uniform vec2 mSunSize;

uniform float mBotRadius;
uniform vec3 mSolarIrradiance;

///////////////////////////////////////////////////////////////////////////////

vec3 GetSkyRadiance(float r, vec3 dir, vec3 sun_dir, out vec3 transmittance);
vec3 GetSkyRadianceToPoint(
    float r, vec3 dir, float d, vec3 sun_dir, out vec3 transmittance);

///////////////////////////////////////////////////////////////////////////////

vec3 GetFragPos(vec2 uv, out float depth)
{
    depth = 2.0f * texture(mDepth, uv).r - 1.0f;
    vec4 clipPos = vec4(2.0f * uv - 1.0f, depth, 1.0f);
    vec4 pos = mInvProjView * clipPos;

    return pos.xyz / pos.w;
}

///////////////////////////////////////////////////////////////////////////////

void main()
{
    float depth;
    vec3 fragPos = GetFragPos(TexCoord, depth);

    // Get view dir
    vec3 viewDir = fragPos - mCamPos;
    float d = length(viewDir) * 0.001f * 50.0f;
    viewDir = normalize(viewDir);

    // Altitude
    float r = mCamPos.y * 0.001f + mBotRadius + 1.0f;

    vec3 radiance, transmittance;

    // If further than certain point, render as sky
    if (depth >= 1.0f)
    {
        radiance = GetSkyRadiance(r, viewDir, mSunDir, transmittance) * 10.0f;
        if (dot(viewDir, mSunDir) > mSunSize.y)
            radiance += transmittance * mSolarIrradiance;
    }
    else
    {
        radiance = texture(mColor, TexCoord).rgb;
        vec3 in_scatter =
            GetSkyRadianceToPoint(r, viewDir, d, mSunDir, transmittance) * 10.0f;
        radiance = radiance * transmittance + in_scatter;
    }

    FragColor.rgb = pow(vec3(1.0f) - exp(-radiance), vec3(1.0f / 2.2f));
    FragColor.a = 1.0f;
}