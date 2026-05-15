@group(0) @binding(0) var u_Texture : texture_2d<f32>;
@group(0) @binding(1) var u_Sampler : sampler;

@fragment
fn main(@location(0) texCoord : vec2<f32>) -> @location(0) vec4<f32> {
    return textureSample(u_Texture, u_Sampler, texCoord);
}
