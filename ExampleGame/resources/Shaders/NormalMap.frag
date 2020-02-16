#version 330 core

uniform vec4 u_colour;
uniform float u_tilingFactor;
uniform sampler2D u_texture;

in vec3 v_posW;
in vec2 v_texCoord;
in vec3 v_normal;

layout(location = 0) out vec4 frag_colour;

void main()
{
	frag_colour = texture(u_texture, v_texCoord * u_tilingFactor) * u_colour;
}