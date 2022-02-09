#version 450 core

layout(location = 0) out vec4 frag_colour;
layout(location = 1) out int entityId;

struct VertexOutput
{
	vec3 PosW;
	vec2 TexCoord;
	vec3 Tangent;
	vec3 Normal;
};

layout(std140, binding = 1) uniform ModelBuffer
{
	uniform mat4 u_ModelMatrix;
	uniform vec4 u_Colour;
	uniform float u_TilingFactor;
	uniform int u_EntityId;
};

layout (binding = 0) uniform sampler2D u_texture;

layout (location = 0) in VertexOutput Input;


void main()
{
	frag_colour = texture(u_texture, Input.TexCoord * u_TilingFactor) * u_Colour;
	entityId = u_EntityId;
}