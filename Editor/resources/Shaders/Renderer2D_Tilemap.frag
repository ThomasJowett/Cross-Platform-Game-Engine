#version 450

layout(location = 0) out vec4 frag_colour;
layout(location = 1) out int entityId;

layout(std140, binding = 1) uniform ModelBuffer
{
	uniform mat4 u_ModelMatrix;
	uniform vec4 u_Colour;
	uniform float u_TilingFactor;
	uniform int u_EntityId;
};

layout (binding = 0) uniform sampler2D u_Texture;

layout (location = 0) in vec2 v_TexCoord;

void main()
{
	frag_colour = texture(u_Texture, v_TexCoord) * u_Colour;
	if(frag_colour.a <= 0.0001)
		discard;
	entityId = u_EntityId;
}