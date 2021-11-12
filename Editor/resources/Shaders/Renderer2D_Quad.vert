#version 450 core

layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec4 a_Colour;
layout(location = 2) in vec2 a_TexCoord;
layout(location = 3) in float a_TexIndex;
layout(location = 4) in int a_EntityId;

out vec4 v_Colour;
out vec2 v_TexCoord;
out flat float v_TexIndex;

uniform mat4 u_ViewProjection;

void main()
{
	v_TexCoord = a_TexCoord;
	v_Colour = a_Colour;
	v_TexIndex = a_TexIndex;
	gl_Position = u_ViewProjection * vec4(a_Position, 1.0);
}