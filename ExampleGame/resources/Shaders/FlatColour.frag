#version 330 core

in vec4 v_colour;

layout(location = 0) out vec4 frag_colour;

void main()
{
	frag_colour = v_colour;
}