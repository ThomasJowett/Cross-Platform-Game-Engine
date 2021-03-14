#version 450 core

layout(location = 0) out vec4 frag_colour;

in vec4 v_Colour;
in vec2 v_TexCoord;
in flat float v_TexIndex;

uniform sampler2D u_Textures[32];

void main()
{
	frag_colour = texture(u_Textures[int(v_TexIndex)], v_TexCoord) * v_Colour;
}