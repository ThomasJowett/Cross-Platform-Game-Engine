#version 460 core

layout(location = 0)out vec4 frag_colour;
layout(location = 1)out int entityId;

struct VertexOutput
{
    vec4 world;
    vec2 TexCoord;
};

layout(std140, binding = 0)uniform ConstantBuffer
{
    uniform mat4 u_ViewProjection;
    uniform vec3 u_EyePosition;
};

layout(std140, binding = 1)uniform ModelBuffer
{
    uniform mat4 u_ModelMatrix;
    uniform vec4 u_Colour;
    uniform vec2 u_TextureOffset;
    // Doubles as the fade-to-horizon radius (Material::SetTilingFactor()) - there's no dedicated
    // per-material float slot in the shared ModelBuffer layout, and this needs to be tunable per
    // material/object (scale and position vary), not a fixed constant.
    uniform float u_TilingFactor;
    uniform int u_EntityId;
};

layout(location = 0)in VertexOutput Input;

void main()
{
    // The mesh is flat along one axis (world Y in 3D, Z in 2D), sitting at that axis's origin -
    // both fwidth() and the numerator below are exactly 0 there, so 0/epsilon wins every min()
    // instead of being ignored. mix() forces that axis to a large value so it's excluded, not
    // dominant.
    vec3 deriv = fwidth(Input.world.xyz);
    bvec3 isFlatAxis = lessThan(deriv, vec3(0.00001));
    vec3 derivEpsilon = vec3(0.0001);

    // See the u_TilingFactor field comment above - this is a per-material setting, not a constant.
    float fadeRadius = u_TilingFactor;

    // Minor grid lines
    vec3 grid = mix(abs(fract(Input.world.xyz - 0.5) - 0.5) / max(deriv, derivEpsilon), vec3(1000.0), isFlatAxis);
    float dist = sqrt(dot(Input.world.xyz - u_EyePosition.xyz, Input.world.xyz - u_EyePosition.xyz));
    float size = min(min(grid.x, grid.y), grid.z);
    float val = 1.0 - min(size, 1.0);
    val *= clamp(1 - ((dist - fadeRadius) / fadeRadius), 0, 1);

    // Major grid lines
    const float extraThickness = 0.5;
    vec3 axes = mix(abs(Input.world.xyz) / max(deriv, derivEpsilon) - extraThickness, vec3(1000.0), isFlatAxis);
    size = min(min(axes.x, axes.y), axes.z);
    float axisVal = 1.0 - min(size, 1.0);
    axisVal *= clamp(1 - ((dist - fadeRadius * 1.5) / (fadeRadius * 1.5)), 0, 1);

    // combine, and drop transparent pixels
    float finalAlpha = max(val * 0.6, axisVal);
    if (finalAlpha <= 0)
    discard;
    
    vec3 colour = u_Colour.xyz;
    if (axisVal > val * 0.6) {
        colour *= 0.3; // Darken major grid lines
    }
    
    frag_colour = vec4(vec3(colour), finalAlpha);
    entityId = u_EntityId;
}