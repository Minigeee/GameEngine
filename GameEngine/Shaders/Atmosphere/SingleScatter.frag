#version 330 core

out vec4 Output;
in vec2 TexCoord;

uniform float mZ;

void CalcScatteringUV3(vec3 uv3, out vec3 rayleigh, out vec3 mie);

void main()
{
    vec3 rayleigh, mie;
    CalcScatteringUV3(vec3(TexCoord, mZ), rayleigh, mie);
    
    Output = vec4(rayleigh, mie.r);
}