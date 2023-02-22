#version 450 core

layout(location = 0) out vec4 frag_colour;
layout(location = 1) out int entityId;

struct VertexOutput
{
	vec4 Colour;
	vec2 TexCoord;
};

layout (location = 0) in VertexOutput Input;
layout (location = 2) in flat float v_TexIndex;
layout (location = 3) in flat int v_EntityId;

layout (binding = 0) uniform sampler2D u_FontAtlases[32];

float median(float r, float g, float b)
{
	return max(min(r, g), min(max(r, g), b));
}

float screenPxRange()
{
	float pxRange = 2.0f;
	vec2 unitRange = vec2(pxRange) / vec2(textureSize(u_FontAtlases[int(v_TexIndex)], 0));
	vec2 screenTexSize = vec2(1.0/fwidth(Input.TexCoord));
	return max(0.5* dot(unitRange, screenTexSize), 1.0);
}
void main()
{
	vec4 bgColor = vec4(Input.Colour.rgb, 0.0);
	vec4 fgColor = Input.Colour;

	vec3 msd = texture(u_FontAtlases[int(v_TexIndex)], Input.TexCoord).rgb;
	float sd = median(msd.r, msd.g, msd.b);
	float screenPxDistance = screenPxRange() * (sd - 0.5f);
	float opacity = clamp(screenPxDistance + 0.5, 0.0, 1.0);

	frag_colour = mix(bgColor, fgColor, opacity);
	if(frag_colour.a <= 0.0001)
		discard;
	entityId = v_EntityId;
}