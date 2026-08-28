struct Camera {
    u_ViewProjection: mat4x4<f32>,
    u_EyePosition: vec3<f32>,
};

@group(0) @binding(0)
var<uniform> camera: Camera;

// Up to 8 textures, selected by index in the fragment shader. Share one sampler (binding 9).
@group(0) @binding(1) var u_Texture0: texture_2d<f32>;
@group(0) @binding(2) var u_Texture1: texture_2d<f32>;
@group(0) @binding(3) var u_Texture2: texture_2d<f32>;
@group(0) @binding(4) var u_Texture3: texture_2d<f32>;
@group(0) @binding(5) var u_Texture4: texture_2d<f32>;
@group(0) @binding(6) var u_Texture5: texture_2d<f32>;
@group(0) @binding(7) var u_Texture6: texture_2d<f32>;
@group(0) @binding(8) var u_Texture7: texture_2d<f32>;
@group(0) @binding(9) var u_Sampler: sampler;

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
    output.position = camera.u_ViewProjection * vec4<f32>(input.position, 1.0);
    output.colour = input.colour;
    output.texCoord = input.texCoord;
    output.texIndex = input.texIndex;
    output.entityId = input.entityId;
    return output;
}

// TODO(texture-array-cleanup): 8-slot if-chain workaround for WGSL's lack of dynamic indexing -
// see the TODO on WebGPUPipeline::SetTextureArray.
fn sampleQuadTexture(index: i32, uv: vec2<f32>) -> vec4<f32> {
    if (index == 0) { return textureSample(u_Texture0, u_Sampler, uv); }
    else if (index == 1) { return textureSample(u_Texture1, u_Sampler, uv); }
    else if (index == 2) { return textureSample(u_Texture2, u_Sampler, uv); }
    else if (index == 3) { return textureSample(u_Texture3, u_Sampler, uv); }
    else if (index == 4) { return textureSample(u_Texture4, u_Sampler, uv); }
    else if (index == 5) { return textureSample(u_Texture5, u_Sampler, uv); }
    else if (index == 6) { return textureSample(u_Texture6, u_Sampler, uv); }
    else { return textureSample(u_Texture7, u_Sampler, uv); }
}

@fragment
fn fs_main(input: VertexOutput) -> FragmentOutput {
    var output: FragmentOutput;
    output.frag_colour = sampleQuadTexture(i32(input.texIndex), input.texCoord) * input.colour;

    if(output.frag_colour.a <= 0.0001) {
        discard;
    }

    output.entityId = input.entityId;
    return output;
}
