#version 330 core

layout (location = 0) in vec2 aPos;
layout (location = 4) in mat4 aTransform;

uniform mat4 mProjView;

uniform float mAltitude;

out vec3 FragPos;
out vec3 Normal;
out vec4 ClipSpace;

void main()
{
    vec4 worldTransform = aTransform * vec4(aPos.x, mAltitude, aPos.y, 1.0);
    gl_Position = mProjView * worldTransform;
    ClipSpace = gl_Position;

    FragPos = worldTransform.xyz;
    Normal = mat3(aTransform) * vec3(0, 1, 0);
}