#version 450 core

layout(location = 0)in vec3 a_Position;
layout(location = 1)in vec4 a_Colour;
layout(location = 2)in int a_EntityId;

layout(std140, binding = 0)uniform Camera
{
	mat4 u_ViewProjection;
	vec3 u_EyePosition;
};

layout(location = 0)out vec4 v_Colour;
layout(location = 1)out flat int v_EntityId;

void main()
{
	v_Colour = a_Colour;
	v_EntityId = a_EntityId;
	gl_Position = u_ViewProjection * vec4(a_Position, 1.0);
}