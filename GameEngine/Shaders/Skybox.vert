#version 330 core

layout (location = 0) in vec3 aPos;

uniform mat4 projView;

out vec3 FragPos;

void main()
{
    vec4 pos = projView * vec4(aPos * 1000.0f, 1.0f);

    gl_Position = pos.xyww;
    FragPos = aPos;
}