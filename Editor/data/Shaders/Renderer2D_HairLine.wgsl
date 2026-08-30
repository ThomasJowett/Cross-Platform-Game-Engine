struct Camera {
    u_ViewProjection: mat4x4<f32>,
    u_EyePosition: vec3<f32>,
};

@group(0) @binding(0)
var<uniform> camera: Camera;

struct VertexInput {
    @location(0) position: vec3<f32>,
    @location(1) colour: vec4<f32>,
    @location(2) entityId: i32,
};

struct VertexOutput {
    @builtin(position) position: vec4<f32>,
    @location(0) colour: vec4<f32>,
    @location(1) @interpolate(flat) entityId: i32,
};

struct FragmentOutput {
    @location(0) frag_colour: vec4<f32>,
    @location(1) entityId: i32,
};

@vertex
fn vs_main(input: VertexInput) -> VertexOutput {
    var output: VertexOutput;
    output.colour = input.colour;
    output.entityId = input.entityId;
    output.position = camera.u_ViewProjection * vec4<f32>(input.position, 1.0);
    return output;
}

@fragment
fn fs_main(input: VertexOutput) -> FragmentOutput {
    var output: FragmentOutput;
    output.frag_colour = input.colour;
    output.entityId = input.entityId;
    return output;
}