#version 330 core

layout (triangles) in;
layout (triangle_strip, max_vertices = 3) out;

in vec2 Vertex[];

uniform mat4 projView;

uniform float terrainSize;
uniform sampler2D heightMap;

out vec3 FragPos;
out vec3 Normal;

void main()
{    
    vec2 texCoord = Vertex[0] / terrainSize * 0.5f + 0.5f;
    float h1 = texture(heightMap, texCoord).r * 10.0f;
    vec3 v1 = vec3(Vertex[0].x, h1, Vertex[0].y);
    
    texCoord = Vertex[1] / terrainSize * 0.5f + 0.5f;
    float h2 = texture(heightMap, texCoord).r * 10.0f;
    vec3 v2 = vec3(Vertex[1].x, h2, Vertex[1].y);
    
    texCoord = Vertex[2] / terrainSize * 0.5f + 0.5f;
    float h3 = texture(heightMap, texCoord).r * 10.0f;
    vec3 v3 = vec3(Vertex[2].x, h3, Vertex[2].y);

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