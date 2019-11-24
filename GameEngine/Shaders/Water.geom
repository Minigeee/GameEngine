#version 330 core

layout (triangles) in;
layout (triangle_strip, max_vertices = 3) out;

///////////////////////////////////////////////////////////////////////////////

in vec2 Vertex[];
in mat4 Transform[];

out vec3 FragPos;
out vec3 Normal;
out vec4 ClipSpace;

uniform mat4 mProjView;
uniform vec4 mClipPlane;
uniform float mTime;

uniform float mAltitude;
uniform float mAmplitude;
uniform float mNoiseSize;
uniform float mWaveSpeed;

const float PI = 3.1415926435f;

///////////////////////////////////////////////////////////////////////////////

float Rand(vec2 c)
{
    return fract(sin(dot(c, vec2(12.9898, 78.233))) * 43758.5453);
}

float Noise(vec2 p)
{
    vec2 ip = floor(p);
    vec2 u = fract(p);
    u = u * u * (3.0f - 2.0f * u);

    float res = mix(
        mix(Rand(ip), Rand(ip + vec2(1.0f, 0.0f)), u.x),
        mix(Rand(ip + vec2(0.0f, 1.0f)), Rand(ip + vec2(1.0f, 1.0f)), u.x),
        u.y
    );

    return res * res;
}

float GetHeight(vec2 p)
{
    return
        (Noise(p / mNoiseSize + vec2(mTime * mWaveSpeed)) - 0.5f) * mAmplitude;
}

///////////////////////////////////////////////////////////////////////////////

void main()
{
    vec4 v1 = Transform[0] * vec4(Vertex[0].x, mAltitude, Vertex[0].y, 1.0);
    vec4 v2 = Transform[0] * vec4(Vertex[1].x, mAltitude, Vertex[1].y, 1.0);
    vec4 v3 = Transform[0] * vec4(Vertex[2].x, mAltitude, Vertex[2].y, 1.0);
    
    v1.y += GetHeight(v1.xz);
    v2.y += GetHeight(v2.xz);
    v3.y += GetHeight(v3.xz);
    Normal = normalize(cross(v1.xyz - v2.xyz, v1.xyz - v3.xyz));

    gl_Position = mProjView * v1;
    gl_ClipDistance[0] = dot(v1, mClipPlane);
    FragPos = v1.xyz;
    ClipSpace = gl_Position;
    EmitVertex();

    gl_Position = mProjView * v2;
    gl_ClipDistance[0] = dot(v2, mClipPlane);
    FragPos = v2.xyz;
    ClipSpace = gl_Position;
    EmitVertex();

    gl_Position = mProjView * v3;
    gl_ClipDistance[0] = dot(v3, mClipPlane);
    FragPos = v3.xyz;
    ClipSpace = gl_Position;
    EmitVertex();
    
    EndPrimitive();
}