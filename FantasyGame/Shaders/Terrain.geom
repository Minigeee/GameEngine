#version 330 core

layout (triangles) in;
layout (triangle_strip, max_vertices = 3) out;

in vec2 Vertex[];
in float Res[];

uniform mat4 projView;
uniform vec3 camPos;

uniform float terrainSize;
uniform sampler2D heightMap;

out vec3 FragPos;
out vec3 Normal;

void main()
{
    vec2 p1 = Vertex[0] + round(camPos.xz / Res[0]) * Res[0];
    vec2 texCoord = p1 / terrainSize * 0.5f + 0.5f;
    float h1 = texture(heightMap, texCoord).r * 10.0f;
    vec3 v1 = vec3(p1.x, h1, p1.y);
    
    vec2 p2 = Vertex[1] + round(camPos.xz / Res[1]) * Res[1];
    texCoord = p2 / terrainSize * 0.5f + 0.5f;
    float h2 = texture(heightMap, texCoord).r * 10.0f;
    vec3 v2 = vec3(p2.x, h2, p2.y);
    
    vec2 p3 = Vertex[2] + round(camPos.xz / Res[2]) * Res[2];
    texCoord = p3 / terrainSize * 0.5f + 0.5f;
    float h3 = texture(heightMap, texCoord).r * 10.0f;
    vec3 v3 = vec3(p3.x, h3, p3.y);

    // Calculate normal
    Normal = normalize(cross(v2 - v1, v3 - v1));

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