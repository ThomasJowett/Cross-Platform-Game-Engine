#version 450 core

layout(location = 0) out vec4 frag_colour;
layout(location = 1) out int entityId;

struct VertexOutput
{
	vec3 LocalPosition;
	vec4 Colour;
	float Thickness;
	float Fade;
};

layout (location = 0) in VertexOutput Input;
layout (location = 4) in flat int v_EntityID;

void main()
{
	// Calculate distance and fill circle with white
	float dist = 1.0 - length(Input.LocalPosition);
	float alpha = smoothstep(0.0, Input.Fade, dist);
	alpha *= smoothstep(Input.Thickness + Input.Fade, Input.Thickness, dist);
	
	// Set output colour
	frag_colour = Input.Colour;
	frag_colour.a *= alpha;
	
	entityId = v_EntityID;
}