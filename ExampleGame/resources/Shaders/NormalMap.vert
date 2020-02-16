#version 330 core

layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec3 a_Normal;
layout(location = 2) in vec3 a_Tangent;
layout(location = 3) in vec2 a_TexCoord;

out vec3 v_posW;
out vec2 v_texCoord;
out vec3 v_tangent;
out vec3 v_normal;

uniform mat4 u_ViewProjection;
uniform mat4 u_ModelMatrix;

void main()
{
	vec4 posW = u_ModelMatrix * vec4(a_Position, 1.0f);
	v_posW = posW.xyz;
	gl_Position = u_ViewProjection * u_ModelMatrix * vec4(a_Position, 1.0f);
	v_normal = normalize((u_ModelMatrix* vec4(a_Normal, 0.0)).xyz);

	v_tangent = normalize((u_ModelMatrix* vec4(a_Tangent, 0.0)).xyz);

	v_texCoord = a_TexCoord;
}