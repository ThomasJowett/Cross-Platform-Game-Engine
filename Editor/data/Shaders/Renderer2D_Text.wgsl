struct Camera {
    u_ViewProjection: mat4x4<f32>,
    u_EyePosition: vec3<f32>,
};

@group(0) @binding(0)
var<uniform> camera: Camera;

// Up to 8 font atlases, selected by index in the fragment shader. Share one sampler (binding 9).
@group(0) @binding(1) var u_FontAtlas0: texture_2d<f32>;
@group(0) @binding(2) var u_FontAtlas1: texture_2d<f32>;
@group(0) @binding(3) var u_FontAtlas2: texture_2d<f32>;
@group(0) @binding(4) var u_FontAtlas3: texture_2d<f32>;
@group(0) @binding(5) var u_FontAtlas4: texture_2d<f32>;
@group(0) @binding(6) var u_FontAtlas5: texture_2d<f32>;
@group(0) @binding(7) var u_FontAtlas6: texture_2d<f32>;
@group(0) @binding(8) var u_FontAtlas7: texture_2d<f32>;
@group(0) @binding(9) var u_FontAtlasSampler: sampler;

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

// TODO(texture-array-cleanup): 8-slot if-chain workaround for WGSL's lack of dynamic indexing -
// see the TODO on WebGPUPipeline::SetTextureArray.
fn sampleFontAtlas(index: i32, uv: vec2<f32>) -> vec3<f32> {
    if (index == 0) { return textureSample(u_FontAtlas0, u_FontAtlasSampler, uv).rgb; }
    else if (index == 1) { return textureSample(u_FontAtlas1, u_FontAtlasSampler, uv).rgb; }
    else if (index == 2) { return textureSample(u_FontAtlas2, u_FontAtlasSampler, uv).rgb; }
    else if (index == 3) { return textureSample(u_FontAtlas3, u_FontAtlasSampler, uv).rgb; }
    else if (index == 4) { return textureSample(u_FontAtlas4, u_FontAtlasSampler, uv).rgb; }
    else if (index == 5) { return textureSample(u_FontAtlas5, u_FontAtlasSampler, uv).rgb; }
    else if (index == 6) { return textureSample(u_FontAtlas6, u_FontAtlasSampler, uv).rgb; }
    else { return textureSample(u_FontAtlas7, u_FontAtlasSampler, uv).rgb; }
}

fn fontAtlasSize(index: i32) -> vec2<f32> {
    if (index == 0) { return vec2<f32>(textureDimensions(u_FontAtlas0)); }
    else if (index == 1) { return vec2<f32>(textureDimensions(u_FontAtlas1)); }
    else if (index == 2) { return vec2<f32>(textureDimensions(u_FontAtlas2)); }
    else if (index == 3) { return vec2<f32>(textureDimensions(u_FontAtlas3)); }
    else if (index == 4) { return vec2<f32>(textureDimensions(u_FontAtlas4)); }
    else if (index == 5) { return vec2<f32>(textureDimensions(u_FontAtlas5)); }
    else if (index == 6) { return vec2<f32>(textureDimensions(u_FontAtlas6)); }
    else { return vec2<f32>(textureDimensions(u_FontAtlas7)); }
}

fn median3(r: f32, g: f32, b: f32) -> f32 {
    return max(min(r, g), min(max(r, g), b));
}

fn screenPxRange(texIndex: i32, texCoord: vec2<f32>) -> f32 {
    let pxRange = 2.0;
    let unitRange = vec2<f32>(pxRange) / fontAtlasSize(texIndex);
    let screenTexSize = vec2<f32>(1.0) / fwidth(texCoord);
    return max(0.5 * dot(unitRange, screenTexSize), 1.0);
}

@fragment
fn fs_main(input: VertexOutput) -> FragmentOutput {
    var output: FragmentOutput;

    let texIndex = i32(input.texIndex);
    let msd = sampleFontAtlas(texIndex, input.texCoord);
    let sd = median3(msd.r, msd.g, msd.b);
    let screenPxDistance = screenPxRange(texIndex, input.texCoord) * (sd - 0.5);
    let opacity = clamp(screenPxDistance + 0.5, 0.0, 1.0);

    let frag_colour = vec4<f32>(input.colour.rgb, input.colour.a * opacity);
    if (frag_colour.a <= 0.0001) {
        discard;
    }

    output.frag_colour = frag_colour;
    output.entityId = input.entityId;
    return output;
}
