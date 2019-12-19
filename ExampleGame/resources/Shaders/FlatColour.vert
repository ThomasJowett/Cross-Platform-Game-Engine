#version 330 core

layout(location = 0) in vec3 a_position;

out vec4 v_colour;

uniform vec4 u_colour;
uniform mat4 u_ViewProjection;
uniform mat4 u_ModelMatrix;

void main()
{
	v_colour = u_colour;
	gl_Position = u_ViewProjection * u_ModelMatrix * vec4(a_position , 1.0f);
}