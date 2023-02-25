#version 460 core

layout(location = 0) out vec4 frag_colour;
layout(location = 1) out int entityId;

struct VertexOutput
{
    vec4 world;
    vec2 TexCoord;
};

layout(std140, binding = 0) uniform ConstantBuffer
{
	uniform mat4 u_ViewProjection;
    uniform vec3 u_EyePosition;
};

layout(std140, binding = 1) uniform ModelBuffer
{
    uniform mat4 u_ModelMatrix;
    uniform vec4 u_Colour;
    uniform vec2 u_TextureOffset;
    uniform float u_Radius;
    uniform int u_EntityId;
};

layout (location = 0) in VertexOutput Input;

void main()
{
    // Minor grid lines
    vec3 grid = abs(fract(Input.world.xyz - 0.5) - 0.5) / fwidth(Input.world.xyz);
    float dist = sqrt(dot(Input.world.xyz - u_EyePosition.xyz, Input.world.xyz - u_EyePosition.xyz));
    float size = min(min(grid.x, grid.y), grid.z);
    float val = 1.0 - min(size, 1.0);
    val *= clamp(1 - ((dist - u_Radius)/u_Radius), 0, 1);

    // Major grid lines
    const float extraTickness = 0.5;
    vec3 axes = (abs(Input.world.xyz)) / fwidth(Input.world.xyz) - extraTickness;
    size = min(min(axes.x, axes.y), axes.z);
    float axisVal = 1.0 - min(size, 1.0);
    axisVal *= clamp(1 - ((dist - u_Radius * 1.5)/(u_Radius * 1.5)), 0, 1);

    // combine, and drop transparent pixels
    val = max(val*0.6, axisVal);
    if(val <= 0)
        discard;

    frag_colour = vec4(vec3(u_Colour.xyz), val);
    entityId = u_EntityId;
}