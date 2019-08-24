#version 330 core

layout (location = 0) in vec2 aPos;

out vec2 Vertex;

void main()
{
    Vertex = aPos;
}