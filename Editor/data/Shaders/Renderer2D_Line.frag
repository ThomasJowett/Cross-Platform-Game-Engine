#version 330 core

layout(origin_upper_left)in vec4 gl_FragCoord;
layout(location = 0)out vec4 colour;

in vec4 v_colour;
in vec2 v_texcoord;
in float v_width;
in float v_length;

uniform int u_Caps;

float CalculateD(float dx, float dy)
{
	float d;
	switch(u_Caps)
	{
		case 0 : d=v_width; break;
		case 1 : d=dx > dy ? dx : dy; break;
		case 2 : d=sqrt(dx * dx + dy * dy); break;
		case 3 : d=dx + dy; break;
	}
	
	return d;
}

void main()
{
	if (v_texcoord.x < 0)
	{
		float dx =- v_texcoord.x;
		float dy = v_texcoord.y > 0.f ? v_texcoord.y :- v_texcoord.y;
		float d = CalculateD(dx, dy);
		if (d > 0.5f * v_width)discard;
	}
	else if (v_texcoord.x > v_length)
	{
		float dx = v_texcoord.x - v_length;
		float dy = v_texcoord.y > 0.f ? v_texcoord.y :- v_texcoord.y;
		float d = CalculateD(dx, dy);
		if (d > 0.5f * v_width)discard;
	}
	
	colour = v_colour;
}