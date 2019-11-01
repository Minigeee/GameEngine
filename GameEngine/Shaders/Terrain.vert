#version 330 core

layout (location = 0) in vec2 aPos;
layout (location = 1) in vec2 aInd;
layout (location = 2) in float aLod;

out vec2 Vertex;
out vec2 Ind;
out float Lod;

void main()
{
    Vertex = aPos;
    Ind = aInd;
    Lod = aLod;
}