#version 450 core

layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec3 a_Normal;
layout(location = 2) in vec3 a_Tangent;
layout(location = 3) in vec2 a_TexCoord;

layout(std140, binding = 0) uniform ConstantBuffer
{
	uniform mat4 u_ViewProjection;
};

layout(std140, binding = 1) uniform ModelBuffer
{
	uniform mat4 u_ModelMatrix;
	uniform vec4 u_Colour;
	uniform float u_TilingFactor;
	uniform int u_EntityId;
};

struct VertexOutput
{
	vec3 PosW;
	vec2 TexCoord;
	vec3 Tangent;
	vec3 Normal;
};

layout (location = 0) out VertexOutput Output;

void main()
{
	vec4 posW = u_ModelMatrix * vec4(a_Position, 1.0f);
	Output.PosW = posW.xyz;
	gl_Position = u_ViewProjection * u_ModelMatrix * vec4(a_Position, 1.0f);
	Output.Normal = normalize((u_ModelMatrix* vec4(a_Normal, 0.0)).xyz);

	Output.Tangent = normalize((u_ModelMatrix* vec4(a_Tangent, 0.0)).xyz);

	Output.TexCoord = a_TexCoord;
}