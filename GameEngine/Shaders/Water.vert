#version 330 core

layout (location = 0) in vec2 aPos;
layout (location = 4) in mat4 aTransform;

out vec2 Vertex;
out mat4 Transform;

void main()
{
    Vertex = aPos;
    Transform = aTransform;
}