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

    FragColor = vec4(result, 1.0f);
    FragColor.rgb = pow(FragColor.rgb, vec3(1.0f / mGamma));
}