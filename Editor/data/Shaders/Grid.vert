#version 410 core

layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec3 a_Normal;
layout(location = 2) in vec3 a_Tangent;
layout(location = 3) in vec2 a_TexCoord;

layout(std140) uniform ConstantBuffer
{
	uniform mat4 u_ViewProjection;
    uniform vec3 u_EyePosition;
};

layout(std140) uniform ModelBuffer
{
    uniform mat4 u_ModelMatrix;
    uniform vec4 u_Colour;
    uniform vec2 u_TextureOffset;
    uniform float u_Radius;
    uniform int u_EntityId;
};

struct VertexOutput
{
    vec4 world;
    vec2 TexCoord;
};

layout (location = 0) out VertexOutput Output;

void main()
{
    Output.world = u_ModelMatrix * vec4(a_Position, 1.0f);
    gl_Position = u_ViewProjection * u_ModelMatrix * vec4(a_Position, 1.0f);

    Output.TexCoord = a_TexCoord;
}