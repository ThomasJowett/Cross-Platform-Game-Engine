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

@group(0) @binding(0) var u_Depth: texture_depth_2d;

struct FragmentOutput {
	@builtin(frag_depth) depth: f32,
};

@fragment
fn fs_main(@builtin(position) fragCoord: vec4<f32>) -> FragmentOutput {
	var output: FragmentOutput;
	output.depth = textureLoad(u_Depth, vec2<i32>(fragCoord.xy), 0);
	return output;
}
