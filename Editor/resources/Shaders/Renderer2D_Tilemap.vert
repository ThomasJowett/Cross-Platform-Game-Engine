#version 450

layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec2 a_TexCoord;

layout(std140, binding = 0) uniform Camera
{
	mat4 u_ViewProjection;
	vec3 u_EyePosition;
};

layout(std140, binding = 1) uniform ModelBuffer
{
	uniform mat4 u_ModelMatrix;
	uniform vec4 u_Colour;
	uniform vec2 u_TextureOffset;
	uniform float u_TilingFactor;
	uniform int u_EntityId;
};

layout (location = 0) out vec2 v_TexCoord;

void main()
{
	v_TexCoord = a_TexCoord;
	gl_Position = u_ViewProjection * u_ModelMatrix * vec4(a_Position, 1.0);
}