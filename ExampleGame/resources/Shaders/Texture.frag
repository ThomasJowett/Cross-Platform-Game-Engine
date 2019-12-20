#version 330 core

uniform sampler2D u_texture;

in vec2 v_texCoord;

layout(location = 0) out vec4 frag_colour;

void main()
{
	frag_colour = texture(u_texture,v_texCoord);
}