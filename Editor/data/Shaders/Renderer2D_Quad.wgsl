struct Camera {
    u_ViewProjection: mat4x4<f32>,
    u_EyePosition: vec3f,
};

@group(0) @binding(0)
var<uniform> camera: Camera;

@group(0) @binding(1)
var u_Textures: texture_2d<f32>;
@group(0) @binding(2)
var u_Sampler: sampler;

struct VertexInput {
    @location(0) position: vec3f,
    @location(1) colour: vec4f,
    @location(2) texCoord: vec2f,
    @location(3) texIndex: f32,
    @location(4) entityId: i32,
};

struct VertexOutput {
    @builtin(position) position: vec4f,
    @location(0) colour: vec4f,
    @location(1) texCoord: vec2f,
    @location(2) @interpolate(flat) texIndex: f32,
    @location(3) @interpolate(flat) entityId: i32,
};

struct FragmentOutput {
    @location(0) frag_colour: vec4f,
    @location(1) entityId: i32,
};

@vertex
fn vs_main(input: VertexInput) -> VertexOutput {
    var output: VertexOutput;
    output.position = camera.u_ViewProjection * vec4f(input.position, 1.0);
    output.colour = input.colour;
    output.texCoord = input.texCoord;
    output.texIndex = input.texIndex;
    output.entityId = input.entityId;
    return output;
}

@fragment
fn fs_main(input: VertexOutput) -> FragmentOutput {
    var output: FragmentOutput;
    let texIndex = f32(input.texIndex);
    output.frag_colour = textureSample(u_Textures, u_Sampler, vec3f(input.texCoord, texIndex)) * input.colour;

    if(output.frag_colour.a <= 0.0001) {
        discard;
    }

    output.entityId = input.entityId;
    return output;
}