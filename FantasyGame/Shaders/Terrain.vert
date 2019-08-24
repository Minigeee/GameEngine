#version 330 core

layout (location = 0) in vec2 aPos;
layout (location = 1) in float aRes;

out vec2 Vertex;
out float Res;

void main()
{
    Vertex = aPos;
    Res = aRes;
}