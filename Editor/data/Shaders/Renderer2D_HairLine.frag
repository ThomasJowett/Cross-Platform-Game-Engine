#version 450 core

layout(location = 0)out vec4 frag_colour;
layout(location = 1)out int entityId;

layout(location = 0)in vec4 v_Colour;
layout(location = 1)in flat int v_EntityId;

void main()
{
	frag_colour = v_Colour;
	entityId = v_EntityId;
}