#version 330 core

out vec4 FragColor;
in vec2 TexCoord;

uniform sampler2D mColor;
uniform sampler2D mDepth;
uniform sampler3D mScatteringTexture;

uniform mat4 mInvProjView;
uniform vec3 mCamPos;
uniform vec3 mSunDir;

uniform float mBotRadius;

///////////////////////////////////////////////////////////////////////////////

vec3 GetSkyRadiance(float r, vec3 dir, vec3 sun_dir, out vec3 transmittance);
vec3 TransmittanceToTop(float r, float mu);

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
    float d = length(viewDir);
    viewDir = normalize(viewDir);

    // Altitude
    float r = mCamPos.y + mBotRadius;

    // If further than certain point, render as sky
    if (depth >= 1.0f)
    {
        vec3 transmittance;
        FragColor.rgb = GetSkyRadiance(r, viewDir, mSunDir, transmittance);
    }
    else
    {
        FragColor.rgb = texture(mColor, TexCoord).rgb;
    }

    FragColor.a = 1.0f;
}