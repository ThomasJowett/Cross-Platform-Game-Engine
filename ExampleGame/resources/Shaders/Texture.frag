#version 330 core
uniform sampler2D u_textures[32];

in vec4 v_colour;
in vec2 v_texCoord;
in float v_texIndex;

layout(location = 0) out vec4 frag_colour;

void main()
{
	frag_colour = texture(u_textures[int(v_texIndex)], v_texCoord) * v_colour;
}