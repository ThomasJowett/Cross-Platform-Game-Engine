struct ConstantBuffer {
	viewProjection: mat4x4<f32>,
	eyePosition: vec3<f32>,
};

struct ModelBuffer {
	modelMatrix: mat4x4<f32>,
	colour: vec4<f32>,
	textureOffset: vec2<f32>,
	tilingFactor: f32,
	entityId: i32,
};

@group(0) @binding(0) var<uniform> u_Constants: ConstantBuffer;
@group(0) @binding(1) var<uniform> u_Model: ModelBuffer;
@group(0) @binding(2) var u_Albedo: texture_2d<f32>;
@group(0) @binding(3) var u_Sampler: sampler;

struct VertexInput {
	@location(0) position: vec3<f32>,
	@location(1) normal: vec3<f32>,
	@location(2) tangent: vec3<f32>,
	@location(3) texCoord: vec2<f32>,
};

struct VertexOutput {
	@builtin(position) position: vec4<f32>,
	@location(0) worldPos: vec3<f32>,
	@location(1) texCoord: vec2<f32>,
	@location(2) normal: vec3<f32>,
};

@vertex
fn vs_main(input: VertexInput) -> VertexOutput {
	var output: VertexOutput;
	let world = u_Model.modelMatrix * vec4<f32>(input.position, 1.0);
	output.worldPos = world.xyz;
	output.position = u_Constants.viewProjection * world;
	output.texCoord = (input.texCoord + u_Model.textureOffset) * u_Model.tilingFactor;
	output.normal = normalize((u_Model.modelMatrix * vec4<f32>(input.normal, 0.0)).xyz);
	return output;
}

struct FragmentOutput {
	@location(0) frag_colour: vec4<f32>,
	@location(1) entityId: i32,
};

@fragment
fn fs_main(input: VertexOutput) -> FragmentOutput {
	var output: FragmentOutput;

	output.frag_colour = textureSample(u_Albedo, u_Sampler, input.texCoord) * u_Model.colour;
	if (output.frag_colour.a <= 0.0001) {
		discard;
	}
	output.entityId = u_Model.entityId;
	return output;
}
