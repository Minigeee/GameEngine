#version 330 core

out vec3 Output;
in vec2 TexCoord;

vec3 CalcIrradianceUV(vec2 uv);

void main()
{
    Output = CalcIrradianceUV(TexCoord);
}