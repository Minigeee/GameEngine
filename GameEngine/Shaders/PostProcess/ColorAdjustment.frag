#version 330 core

///////////////////////////////////////////////////////////////////////////////

in vec2 TexCoord;
out vec4 FragColor;

uniform sampler2D mColor;
uniform float mGamma;

///////////////////////////////////////////////////////////////////////////////

void main()
{
    vec3 result = texture(mColor, TexCoord).rgb;

    // HDR and gamma correction
    FragColor.rgb = pow(vec3(1.0f) - exp(-result), vec3(1.0f / mGamma));
    FragColor.a = 1.0f;
}