#version 330 core

///////////////////////////////////////////////////////////////////////////////

struct Material
{
    vec3 diffuse;
    vec3 specular;
    float specFactor;
};

struct DirLight
{
    vec3 diffuse;
    vec3 specular;
    vec3 direction;
};

///////////////////////////////////////////////////////////////////////////////

uniform Material material;

uniform vec3 camPos;

uniform vec3 ambient;
uniform DirLight dirLight;


in vec3 FragPos;
in vec3 Normal;
in vec3 Color;

out vec4 FragColor;

const float diffFactor = 0.1f;

///////////////////////////////////////////////////////////////////////////////

vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir)
{
    float diff = dot(normal, -light.direction);
    if (diff <= 0.0f)
        diff = diffFactor * diff + diffFactor;
    else
        diff = (1.0f - diffFactor) * diff + diffFactor;
    vec3 diffuse = diff * light.diffuse * Color;

    vec3 reflectDir = reflect(light.direction, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0f), material.specFactor);
    vec3 specular = spec * light.specular * material.specular;

    return diffuse + specular;
}

///////////////////////////////////////////////////////////////////////////////

void main()
{
    vec3 viewDir = normalize(camPos - FragPos);
    vec3 result = ambient * Color;

    result += CalcDirLight(dirLight, Normal, viewDir);

    FragColor = vec4(result, 1.0f);
}