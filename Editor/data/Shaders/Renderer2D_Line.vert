#version 330 core

layout(location = 0)in vec3 a_clipCoord;
layout(location = 1)in vec4 a_colour;
layout(location = 2)in vec2 a_texcoord;
layout(location = 3)in float a_width;
layout(location = 4)in float a_length;

out vec4 v_colour;
out vec2 v_texcoord;
out float v_width;
out float v_length;

void main()
{
	v_colour = a_colour;
	v_texcoord = a_texcoord;
	v_width = a_width;
	v_length = a_length;
	
	gl_Position = vec4(a_clipCoord, 1.f);
}