struct Camera {
    u_ViewProjection: mat4x4<f32>,
    u_EyePosition: vec3<f32>,
};

@group(0) @binding(0)
var<uniform> camera: Camera;

@group(0) @binding(1)
var u_FontAtlas: texture_2d<f32>;
@group(0) @binding(2)
var u_Sampler: sampler;

struct VertexInput {
    @location(0) position: vec3<f32>,
    @location(1) colour: vec4<f32>,
    @location(2) texCoord: vec2<f32>,
    @location(3) texIndex: f32,
    @location(4) entityId: i32,
};

struct VertexOutput {
    @builtin(position) position: vec4<f32>,
    @location(0) colour: vec4<f32>,
    @location(1) texCoord: vec2<f32>,
    @location(2) @interpolate(flat) texIndex: f32,
    @location(3) @interpolate(flat) entityId: i32,
};

struct FragmentOutput {
    @location(0) frag_colour: vec4<f32>,
    @location(1) entityId: i32,
};

@vertex
fn vs_main(input: VertexInput) -> VertexOutput {
    var output: VertexOutput;
    output.colour = input.colour;
    output.texCoord = input.texCoord;
    output.texIndex = input.texIndex;
    output.entityId = input.entityId;
    output.position = camera.u_ViewProjection * vec4<f32>(input.position, 1.0);
    return output;
}

@fragment
fn fs_main(input: VertexOutput) -> FragmentOutput {
    var output: FragmentOutput;
    // Just a basic pass-through for now, should do MSDF sampling
    output.frag_colour = input.colour;
    output.entityId = input.entityId;
    return output;
}
