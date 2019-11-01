#version 330 core

out vec3 Output;
in vec2 TexCoord;

vec3 CalcTransmittanceUV(vec2 tex_coord);

void main()
{
    Output = CalcTransmittanceUV(TexCoord);
}