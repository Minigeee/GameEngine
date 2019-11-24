#version 330 core

layout (location = 0) in vec4 aSphere;
layout (location = 1) in mat4 aTransform;

out vec4 bSphere;
out mat4 bTransform;

void main()
{
    bSphere = aSphere;
    bTransform = aTransform;
}