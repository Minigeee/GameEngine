#version 330 core

layout (location = 0) in vec2 aPos;

uniform mat4 projView;

out vec3 FragPos;
out vec3 Normal;

void main()
{
    vec3 pos = vec3(aPos.x, 0.0f, aPos.y);
    gl_Position = projView * vec4(pos, 1.0);

    FragPos = pos;
}