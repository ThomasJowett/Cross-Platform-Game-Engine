struct VertexOutput {
    @builtin(position) position : vec4<f32>,
    @location(0) texCoord : vec2<f32>,
};

@vertex
fn vs_main(@location(0) position : vec3<f32>, @location(1) texCoord : vec2<f32>) -> VertexOutput {
    var output : VertexOutput;
    output.position = vec4<f32>(position, 1.0);
    output.texCoord = texCoord;
    return output;
}

@group(0) @binding(0) var u_Texture : texture_2d<f32>;
@group(0) @binding(1) var u_Sampler : sampler;
@group(0) @binding(2) var u_EntityId : texture_2d<i32>;

struct FragmentOutput {
    @location(0) frag_colour : vec4<f32>,
    @location(1) entityId : i32,
};

@fragment
fn fs_main(@location(0) texCoord : vec2<f32>) -> FragmentOutput {
    var output : FragmentOutput;
    output.frag_colour = textureSample(u_Texture, u_Sampler, texCoord);
    output.entityId = textureLoad(u_EntityId, vec2<i32>(texCoord * vec2<f32>(textureDimensions(u_EntityId))), 0).r;
    return output;
}
