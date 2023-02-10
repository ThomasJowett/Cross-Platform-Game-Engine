#version 450 core

layout(location = 0) out vec4 frag_colour;
layout(location = 1) out int entityId;

struct VertexOutput
{
	vec4 Colour;
	vec2 TexCoord;
};

layout (location = 0) in VertexOutput Input;
layout (location = 2) in flat float v_TexIndex;
layout (location = 3) in flat int v_EntityId;

layout (binding = 0) uniform sampler2D u_Textures[32];

void main()
{
	frag_colour = texture(u_Textures[int(v_TexIndex)], Input.TexCoord) * Input.Colour;
	if(frag_colour.a <= 0.0001)
		discard;
	entityId = v_EntityId;
}