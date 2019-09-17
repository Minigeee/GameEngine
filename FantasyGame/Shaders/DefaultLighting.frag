#version 330 core

///////////////////////////////////////////////////////////////////////////////

struct DirLight
{
    vec3 mDiffuse;
    vec3 mSpecular;
    vec3 mDirection;
};

///////////////////////////////////////////////////////////////////////////////

in vec2 TexCoord;

out vec4 FragColor;

uniform sampler2D mPosition;
uniform sampler2D mNormalSpec;
uniform sampler2D mAlbedo;
uniform sampler2D mSpecular;

uniform DirLight mDirLight;
uniform vec3 mCamPos;
uniform vec3 mAmbient;

const float diffFactor = 0.1f;

///////////////////////////////////////////////////////////////////////////////

vec3 CalcDirLight(
    DirLight light, vec3 normal, vec3 viewDir, vec3 albedo, vec4 m_specular)
{
    float diff = dot(normal, -light.mDirection);
    if (diff <= 0.0f)
        diff = diffFactor * diff + diffFactor;
    else
        diff = (1.0f - diffFactor) * diff + diffFactor;
    vec3 diffuse = diff * light.mDiffuse * albedo;

    vec3 reflectDir = reflect(light.mDirection, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0f), m_specular.a);
    vec3 specular = spec * light.mSpecular * m_specular.rgb;

    return diffuse + specular;
}

///////////////////////////////////////////////////////////////////////////////

void main()
{
    // Get information
    vec3 pos = texture(mPosition, TexCoord).rgb;
    vec4 normal_spec = texture(mNormalSpec, TexCoord);
    vec3 albedo = texture(mAlbedo, TexCoord).rgb;
    vec3 specular = texture(mSpecular, TexCoord).rgb;
    vec3 normal = normal_spec.rgb;

    vec3 viewDir = normalize(mCamPos - pos);
    vec3 result = albedo * mAmbient;

    result +=
        CalcDirLight(
            mDirLight,
            normal, viewDir,
            albedo,
            vec4(specular, normal_spec.a));

    FragColor = vec4(result, 1.0f);
}