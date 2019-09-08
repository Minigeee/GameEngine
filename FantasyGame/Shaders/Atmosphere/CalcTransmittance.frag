#version 330 core

out vec3 Transmittance;
in vec2 TexCoord;

vec3 TransmittanceFromTexCoord(vec2 tex_coord);

void main()
{
    Transmittance = TransmittanceFromTexCoord(TexCoord);
}