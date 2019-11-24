#version 330 core

layout (points) in;
layout (points, max_vertices = 1) out;

in vec4 bSphere[];
in mat4 bTransform[];

out mat4 Transform;

void main()
{
    Transform = mat4(1.0f);
}