struct ConstantBuffer {
	viewProjection: mat4x4<f32>,
	eyePosition: vec3<f32>,
};

struct ModelBuffer {
	modelMatrix: mat4x4<f32>,
	colour: vec4<f32>,
	textureOffset: vec2<f32>,
	radius: f32,
	entityId: i32,
};

@group(0) @binding(0) var<uniform> u_Constants: ConstantBuffer;
@group(0) @binding(1) var<uniform> u_Model: ModelBuffer;

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

	// Minor grid lines
	let gridFrac = abs(fract(coord - 0.5) - 0.5) / fwidth(coord);
	var size = min(min(gridFrac.x, gridFrac.y), gridFrac.z);
	var val = 1.0 - min(size, 1.0);
	val = val * clamp(1.0 - ((dist - u_Model.radius) / u_Model.radius), 0.0, 1.0);

	// Major grid lines (axes)
	let extraThickness = 0.5;
	let axes = abs(coord) / fwidth(coord) - extraThickness;
	size = min(min(axes.x, axes.y), axes.z);
	var axisVal = 1.0 - min(size, 1.0);
	axisVal = axisVal * clamp(1.0 - ((dist - u_Model.radius * 1.5) / (u_Model.radius * 1.5)), 0.0, 1.0);

	// combine, and drop transparent pixels
	let finalAlpha = max(val * 0.6, axisVal);
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
