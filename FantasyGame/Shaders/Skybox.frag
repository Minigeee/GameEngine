#version 330 core

///////////////////////////////////////////////////////////////////////////////

struct Color
{
    vec3 color;
    float height;
};

struct Grad
{
    float steepness;
    float midpoint;
};

///////////////////////////////////////////////////////////////////////////////

uniform Color colors[3];
uniform Grad gradients[2];

in vec3 FragPos;

out vec4 FragColor;

///////////////////////////////////////////////////////////////////////////////

vec3 CalcColor(Color a, Color b, float steep, float midpoint, float h)
{
    float mid = midpoint * (a.height + b.height);
    vec3 ac = a.color;
    vec3 bc = b.color;
    float ah = a.height;
    float bh = b.height;

    vec3 even = mix(ac, bc, 0.5f);
    
    if (h > mid)
        return mix(bc, even, pow(1.0f - (h - mid) / (bh - mid), steep));
    else
        return mix(ac, even, pow((h - ah) / (mid - ah), steep));
}

///////////////////////////////////////////////////////////////////////////////

void main()
{
    vec3 result = vec3(0.0f);
    float h = FragPos.y / length(FragPos);
    h = h * 0.5f + 0.5f;

    if (h > colors[1].height)
        result = CalcColor(colors[1], colors[2], gradients[1].steepness, gradients[1].midpoint, h);
    else if (h > colors[0].height)
        result = CalcColor(colors[0], colors[1], gradients[0].steepness, gradients[0].midpoint, h);
    else
        result = colors[0].color;

    FragColor = vec4(result, 1.0f);
}