struct ConstantBuffer {
	viewProjection: mat4x4<f32>,
	eyePosition: vec3<f32>,
};

struct ModelBuffer {
	modelMatrix: mat4x4<f32>,
	colour: vec4<f32>,
	textureOffset: vec2<f32>,
	tilingFactor: f32, // matches Renderer.cpp's ModelBuffer layout - unused here
	entityId: i32,
};

@group(0) @binding(0) var<uniform> u_Constants: ConstantBuffer;
@group(0) @binding(1) var<uniform> u_Model: ModelBuffer;
// Unused, but Renderer.cpp always binds a texture+sampler at 2/3, and WebGPU's auto layout only
// keeps bindings the shader actually references - fs_main below reads one texel at weight 0.
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
};

@vertex
fn vs_main(input: VertexInput) -> VertexOutput {
	var output: VertexOutput;
	let world = u_Model.modelMatrix * vec4<f32>(input.position, 1.0);
	output.worldPos = world.xyz;
	output.position = u_Constants.viewProjection * world;
	output.texCoord = input.texCoord;
	return output;
}

struct FragmentOutput {
	@location(0) frag_colour: vec4<f32>,
	@location(1) entityId: i32,
};

@fragment
fn fs_main(input: VertexOutput) -> FragmentOutput {
	var output: FragmentOutput;

	let coord = input.worldPos;
	let dist = length(coord - u_Constants.eyePosition);

	// The mesh is flat along one axis (world Y in 3D, Z in 2D), sitting at that axis's origin - both
	// fwidth() and the numerator below are exactly 0 there, so 0/epsilon wins every min() instead of
	// being ignored. select() forces that axis to a large value so it's excluded, not dominant.
	let deriv = fwidth(coord);
	let isFlatAxis = deriv < vec3<f32>(0.00001);
	let derivEpsilon = vec3<f32>(0.0001);

	// Fade-to-horizon distance - fixed to this mesh's own extent, not a material property.
	let fadeRadius = 500.0;

	// Minor grid lines
	let gridFrac = select(abs(fract(coord - 0.5) - 0.5) / max(deriv, derivEpsilon), vec3<f32>(1000.0), isFlatAxis);
	var size = min(min(gridFrac.x, gridFrac.y), gridFrac.z);
	var val = 1.0 - min(size, 1.0);
	val = val * clamp(1.0 - ((dist - fadeRadius) / fadeRadius), 0.0, 1.0);

	// Major grid lines (axes)
	let extraThickness = 0.5;
	let axes = select(abs(coord) / max(deriv, derivEpsilon) - extraThickness, vec3<f32>(1000.0), isFlatAxis);
	size = min(min(axes.x, axes.y), axes.z);
	var axisVal = 1.0 - min(size, 1.0);
	axisVal = axisVal * clamp(1.0 - ((dist - fadeRadius * 1.5) / (fadeRadius * 1.5)), 0.0, 1.0);

	// combine, and drop transparent pixels
	// (unused texel read - see the binding comment above for why this exists at all)
	let unusedAlbedo = textureSample(u_Albedo, u_Sampler, input.texCoord).r * 0.0;
	let finalAlpha = max(val * 0.6, axisVal) + unusedAlbedo;
	if (finalAlpha <= 0.0) {
		discard;
	}

	var colour = u_Model.colour.xyz;
	if (axisVal > val * 0.6) {
		colour = colour * 0.3; // Darken major grid lines
	}

	output.frag_colour = vec4<f32>(colour, finalAlpha);
	output.entityId = u_Model.entityId;
	return output;
}
