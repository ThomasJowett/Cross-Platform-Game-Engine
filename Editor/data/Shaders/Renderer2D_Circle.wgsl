struct Camera {
    u_ViewProjection: mat4x4<f32>,
    u_EyePosition: vec3<f32>,
};

@group(0) @binding(0)
var<uniform> camera: Camera;

struct VertexInput {
    @location(0) worldPosition: vec3<f32>,
    @location(1) localPosition: vec3<f32>,
    @location(2) colour: vec4<f32>,
    @location(3) thickness: f32,
    @location(4) fade: f32,
    @location(5) entityId: i32,
};

struct VertexOutput {
    @builtin(position) position: vec4<f32>,
    @location(0) localPosition: vec3<f32>,
    @location(1) colour: vec4<f32>,
    @location(2) @interpolate(flat) thickness: f32,
    @location(3) @interpolate(flat) fade: f32,
    @location(4) @interpolate(flat) entityId: i32,
};

struct FragmentOutput {
    @location(0) frag_colour: vec4<f32>,
    @location(1) entityId: i32,
}

@vertex
fn vs_main(input: VertexInput) -> VertexOutput {
    var output: VertexOutput;
    output.localPosition = input.localPosition;
    output.colour = input.colour;
    output.thickness = input.thickness;
    output.fade = input.fade;
    output.entityId = input.entityId;
    output.position = camera.u_ViewProjection * vec4f(input.worldPosition, 1.0);
    return output;
}

@fragment
fn fs_main(input: VertexOutput) -> FragmentOutput {
    var output: FragmentOutput;

    let dist = 1.0 - length(input.localPosition);
    var alpha = smoothstep(0.0, input.fade, dist);
    alpha = alpha * smoothstep(input.thickness + input.fade, input.thickness, dist);

    if (alpha <= 0.0001) {
        discard;
    }

    output.frag_colour = input.colour;
    output.frag_colour.a = output.frag_colour.a * alpha;
    output.entityId = input.entityId;

    return output;
}