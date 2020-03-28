#version 330 core

layout(location = 0) in vec3 a_position;
layout(location = 1) in vec4 a_colour;
layout(location = 2) in vec2 a_texCoord;
layout(location = 3) in float a_texIndex;

out vec4 v_colour;
out vec2 v_texCoord;

uniform mat4 u_ViewProjection;

void main()
{
	v_texCoord = a_texCoord;
	v_colour = a_colour;
	gl_Position = u_ViewProjection *vec4(a_position, 1.0);
}