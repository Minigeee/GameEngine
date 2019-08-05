#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 4) in mat4 aTransform;

uniform mat4 projView;

void main()
{
    gl_Position = projView * aTransform * vec4(aPos, 1.0);
}