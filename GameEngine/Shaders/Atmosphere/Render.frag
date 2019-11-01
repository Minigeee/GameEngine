#version 330 core

out vec4 FragColor;
in vec2 TexCoord;

uniform sampler2D mNormalSpec;
uniform sampler2D mAlbedo;
uniform sampler2D mSpecular;
uniform sampler2D mDepth;

uniform mat4 mInvProjView;
uniform vec3 mCamPos;
uniform vec3 mSunDir;
uniform vec2 mSunSize;
uniform float mBaseHeight;
uniform float mDistScale;

uniform float mBotRadius;
uniform vec3 mSolarIrradiance;

const float PI = 3.14159265358979323846;

///////////////////////////////////////////////////////////////////////////////

vec3 GetSkyRadiance(float r, vec3 dir, vec3 sun_dir, out vec3 transmittance);
vec3 GetSkyRadianceToPoint(
    float r, vec3 dir, float d, vec3 sun_dir, out vec3 transmittance);
vec3 GetSunAndSkyIrradiance(
    float r, vec3 sun_dir, vec3 normal, out vec3 sky_irradiance);

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
    // Get information
    vec4 normal_spec = texture(mNormalSpec, TexCoord);
    vec3 albedo = texture(mAlbedo, TexCoord).rgb;
    vec3 specular = texture(mSpecular, TexCoord).rgb;
    vec3 normal = normal_spec.rgb;

    // Calc frag position
    float depth;
    vec3 fragPos = GetFragPos(TexCoord, depth);

    // Get view dir
    vec3 viewDir = fragPos - mCamPos;
    float d = length(viewDir) * 0.001f * mDistScale;
    viewDir = normalize(viewDir);

    // Altitude
    float r = mCamPos.y * 0.001f + mBotRadius + mBaseHeight;

    vec3 radiance, transmittance;

    // If further than certain point, render as sky
    if (depth >= 1.0f)
    {
        radiance = GetSkyRadiance(r, viewDir, mSunDir, transmittance);
        if (dot(viewDir, mSunDir) > mSunSize.y)
            radiance += transmittance * mSolarIrradiance * 10.0f;
    }
    else
    {
        vec3 sky_irradiance;
        vec3 sun_irradiance =
            GetSunAndSkyIrradiance(r, mSunDir, normal, sky_irradiance);

        radiance = albedo * (1.0f / PI) * (sun_irradiance + sky_irradiance);

        vec3 in_scatter =
            GetSkyRadianceToPoint(r, viewDir, d, mSunDir, transmittance);
        radiance = radiance * transmittance + in_scatter;
    }

    FragColor.rgb = pow(vec3(1.0f) - exp(-radiance), vec3(1.0f / 2.2f));
    FragColor.a = 1.0f;
}