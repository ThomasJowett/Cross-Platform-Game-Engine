#version 450 core

layout(location = 0)in vec3 a_Position;
layout(location = 1)in vec3 a_Normal;
layout(location = 2)in vec3 a_Tangent;
layout(location = 3)in vec2 a_TexCoord0;
layout(location = 4)in vec2 a_TexCoord1;
layout(location = 5)in vec4 a_Colour;

layout(std140, binding = 0)uniform ConstantBuffer
{
	uniform mat4 u_ViewProjection;
	uniform vec3 u_EyePosition;
};

layout(std140, binding = 1)uniform ModelBuffer
{
	uniform mat4 u_ModelMatrix;
	uniform vec4 u_Colour;
	uniform vec2 u_TextureOffset;
	uniform float u_TilingFactor;
	uniform int u_EntityId;
};

struct VertexOutput
{
	vec3 PosW;
	vec2 TexCoord0;
	vec2 TexCoord1;
	vec3 Tangent;
	vec3 Normal;
	vec4 Colour;
};

layout(location = 0)out VertexOutput Output;

void main()
{
	Output.Colour = a_Colour;
	
	vec4 posW = u_ModelMatrix * vec4(a_Position, 1.0f);
	Output.PosW = posW.xyz;
	gl_Position = u_ViewProjection * u_ModelMatrix * vec4(a_Position, 1.0f);
	Output.Normal = normalize((u_ModelMatrix * vec4(a_Normal, 0.0)).xyz);
	
	Output.Tangent = normalize((u_ModelMatrix * vec4(a_Tangent, 0.0)).xyz);
	
	Output.TexCoord0 = (a_TexCoord0 + u_TextureOffset) * u_TilingFactor;
	Output.TexCoord1 = a_TexCoord1;
}