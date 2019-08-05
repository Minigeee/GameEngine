#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 4) in mat4 aTransform;

uniform mat4 projView;

out vec3 FragPos;
out vec3 Normal;

void main()
{
    vec4 worldTransform = aTransform * vec4(aPos, 1.0);
    gl_Position = projView * worldTransform;

    FragPos = worldTransform.xyz;
    Normal = aNormal;
}