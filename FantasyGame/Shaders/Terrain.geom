#version 330 core

layout (triangles) in;
layout (triangle_strip, max_vertices = 3) out;

in vec2 Vertex[];
in vec2 Ind[];
in float Lod[];

uniform mat4 projView;
uniform vec3 camPos;
uniform float res;

uniform float terrainSize;
uniform sampler2D heightMap;
uniform sampler2D colorMap;

out vec3 FragPos;
out vec3 Color;
out vec3 Normal;

///////////////////////////////////////////////////////////////////////////////

float calcHeight(vec2 p, vec2 ind, float lod)
{
    vec2 texCoord = p / terrainSize * 0.5f + 0.5f;
    float h = texture(heightMap, texCoord).r * 10.0f;

    if (lod < 0.0f)
        return h;
    else
    {
        texCoord = (p + ind) / terrainSize * 0.5f + 0.5f;
        float h1 = texture(heightMap, texCoord).r * 10.0f;
        
        texCoord = (p - ind) / terrainSize * 0.5f + 0.5f;
        float h2 = texture(heightMap, texCoord).r * 10.0f;

        vec2 dist = vec2(lod) - abs(p - camPos.xz);
        float factor = clamp(min(dist.x, dist.y) / (0.5f * res), 0.0f, 1.0f);

        return mix(0.5f * (h1 + h2), h, factor);
    }
}

///////////////////////////////////////////////////////////////////////////////

void main()
{
    vec2 offset = round(camPos.xz / res) * res;

    vec2 p1 = Vertex[0] + offset;
    float h1 = calcHeight(p1, Ind[0], Lod[0]);
    vec3 v1 = vec3(p1.x, h1, p1.y);
    
    vec2 p2 = Vertex[1] + offset;
    float h2 = calcHeight(p2, Ind[1], Lod[1]);
    vec3 v2 = vec3(p2.x, h2, p2.y);
    
    vec2 p3 = Vertex[2] + offset;
    float h3 = calcHeight(p3, Ind[2], Lod[2]);
    vec3 v3 = vec3(p3.x, h3, p3.y);

    // Calculate normal
    Normal = normalize(cross(v2 - v1, v3 - v1));

    // Calculate color
    vec2 avg = (p1 + p2 + p3) / 3.0f;
    vec2 texCoord = avg / terrainSize * 0.5f + 0.5f;
    Color = texture(colorMap, texCoord).rgb;

    // Create triangle
    gl_Position = projView * vec4(v1, 1.0f);
    FragPos = v1;
    EmitVertex();
    
    gl_Position = projView * vec4(v2, 1.0f);
    FragPos = v2;
    EmitVertex();
    
    gl_Position = projView * vec4(v3, 1.0f);
    FragPos = v3;
    EmitVertex();

    EndPrimitive();
}