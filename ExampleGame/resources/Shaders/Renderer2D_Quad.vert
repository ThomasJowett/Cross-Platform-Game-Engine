#version 450 core

layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec4 a_Colour;
layout(location = 2) in vec2 a_TexCoord;
layout(location = 3) in float a_TexIndex;
layout(location = 4) in int a_EntityId;

layout(std140, binding = 0) uniform Camera
{
	mat4 u_ViewProjection;
};

struct VertexOutput
{
	vec4 Colour;
	vec2 TexCoord;
};

layout (location = 0) out VertexOutput Output;
layout (location = 2) out flat float v_TexIndex;
layout (location = 3) out flat int v_EntityId;

void main()
{
	Output.TexCoord = a_TexCoord;
	Output.Colour = a_Colour;
	v_TexIndex = a_TexIndex;
	v_EntityId = a_EntityId;
	gl_Position = u_ViewProjection * vec4(a_Position, 1.0);
}