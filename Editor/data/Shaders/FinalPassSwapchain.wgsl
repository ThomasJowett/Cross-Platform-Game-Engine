struct VertexOutput {
    @builtin(position) position : vec4<f32>,
    @location(0) texCoord : vec2<f32>,
};

@vertex
fn vs_main(@location(0) position : vec3<f32>, @location(1) texCoord : vec2<f32>) -> VertexOutput {
    var output : VertexOutput;
    // This quad's own NDC positions never go through Renderer::BeginScene's Y-flip (that only
    // corrects the scene's projection matrix), and WebGPU's clip space still points the
    // opposite way to OpenGL's here too - without negating Y, this composite lands upside down
    // when presented directly to the swapchain (confirmed visually). The offscreen (Editor)
    // FinalPass pipeline doesn't hit this, since ImGui::Image's own texture sampling ends up
    // correcting for it before the user ever sees that path's output.
    output.position = vec4<f32>(position.x, -position.y, position.z, 1.0);
    output.texCoord = texCoord;
    return output;
}

@group(0) @binding(0)
var u_Texture : texture_2d<f32>;
@group(0) @binding(1)
var u_Sampler : sampler;

@fragment
fn fs_main(@location(0) texCoord : vec2<f32>) -> @location(0) vec4<f32> {
    return textureSample(u_Texture, u_Sampler, texCoord);
}
