#version 450 core

layout(location = 0)in vec3 a_WorldPosition;
layout(location = 1)in vec3 a_LocalPosition;
layout(location = 2)in vec4 a_Colour;
layout(location = 3)in float a_Thickness;
layout(location = 4)in float a_Fade;
layout(location = 5)in int a_EntityId;

layout(std140, binding = 0)uniform Camera
{
	mat4 u_ViewProjection;
	vec3 u_EyePosition;
};

struct VertexOutput
{
	vec3 LocalPosition;
	vec4 Colour;
	float Thickness;
	float Fade;
};

layout(location = 0)out VertexOutput Output;
layout(location = 4)out flat int v_EntityId;

void main()
{
	Output.Colour = a_Colour;
	Output.Thickness = a_Thickness;
	Output.Fade = a_Fade;
	Output.LocalPosition = a_LocalPosition;
	v_EntityId = a_EntityId;
	
	gl_Position = u_ViewProjection * vec4(a_WorldPosition, 1.0);
}