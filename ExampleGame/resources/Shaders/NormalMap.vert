#version 330 core

layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec3 a_Normal;
layout(location = 2) in vec3 a_Tangent;
layout(location = 3) in vec2 a_TexCoord;

out VertexData {
vec3 posW;
vec2 texCoord;
vec3 normal;
vec3 tangent;
} VertexOut;

uniform mat4 u_ViewProjection;
uniform mat4 u_ModelMatrix;

void main()
{
	vec4 posW = u_ModelMatrix * vec4(a_Position, 1.0f);
	VertexOut.posW = posW.xyz;
	gl_Position = u_ViewProjection * u_ModelMatrix * vec4(a_Position, 1.0f);
	VertexOut.normal = normalize((u_ModelMatrix* vec4(a_Normal, 0.0)).xyz);

	VertexOut.tangent = normalize((u_ModelMatrix* vec4(a_Tangent, 0.0)).xyz);

	VertexOut.texCoord = a_TexCoord;
}