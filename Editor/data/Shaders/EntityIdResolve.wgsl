struct VertexInput {
    @location(0) position: vec3<f32>,
    @location(1) texCoord: vec2<f32>,
};

struct VertexOutput {
    @builtin(position) position: vec4<f32>,
};

@vertex
fn vs_main(input: VertexInput) -> VertexOutput {
    var output: VertexOutput;
    output.position = vec4<f32>(input.position, 1.0);
    return output;
}

@group(0) @binding(0) var u_EntityIdMultisampled: texture_multisampled_2d<i32>;

struct FragmentOutput {
    @location(0) entityId: i32,
};

// Multisample resolve has no native meaning for integer data (averaging entity IDs is
// meaningless), so this picks one representative sample (index 0) instead.
@fragment
fn fs_main(@builtin(position) fragCoord: vec4<f32>) -> FragmentOutput {
    var output: FragmentOutput;
    output.entityId = textureLoad(u_EntityIdMultisampled, vec2<i32>(fragCoord.xy), 0).r;
    return output;
}
