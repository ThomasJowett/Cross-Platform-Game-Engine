struct VertexOutput {
	@builtin(position) position: vec4<f32>,
	@location(0) texCoord: vec2<f32>,
};

@vertex
fn vs_main(@location(0) position: vec3<f32>, @location(1) texCoord: vec2<f32>) -> VertexOutput {
	var output: VertexOutput;
	output.position = vec4<f32>(position, 1.0);
	output.texCoord = texCoord;
	return output;
}

@group(0) @binding(0) var u_Colour: texture_2d<f32>;
@group(0) @binding(1) var u_Sampler: sampler;

struct PostProcessBuffer {
	screenSize: vec2<f32>,
	time: f32,
	deltaTime: f32,

	customParam1: f32, // horizontal
	customParam2: f32, // strength
	customParam3: f32,
	customParam4: f32,
};

@group(0) @binding(3) var<uniform> u_PostProcess: PostProcessBuffer;

const M: i32 = 16;
const N: i32 = 2 * M + 1;

// sigma = 10
// naga only allows dynamic (non-constant) indexing into `var` arrays, not module-scope `const` ones.
var<private> coeffs: array<f32, 33> = array<f32, 33>(
	0.012318109844189502,
	0.014381474814203989,
	0.016623532195728208,
	0.019024086115486723,
	0.02155484948872149,
	0.02417948052890078,
	0.02685404941667096,
	0.0295279624870386,
	0.03214534135442581,
	0.03464682117793548,
	0.0369716985390341,
	0.039060328279673276,
	0.040856643282313365,
	0.04231065439216247,
	0.043380781642569775,
	0.044035873841196206,
	0.04425662519949865,
	0.044035873841196206,
	0.043380781642569775,
	0.04231065439216247,
	0.040856643282313365,
	0.039060328279673276,
	0.0369716985390341,
	0.03464682117793548,
	0.03214534135442581,
	0.0295279624870386,
	0.02685404941667096,
	0.02417948052890078,
	0.02155484948872149,
	0.019024086115486723,
	0.016623532195728208,
	0.014381474814203989,
	0.012318109844189502
);

@fragment
fn fs_main(input: VertexOutput) -> @location(0) vec4<f32> {
	var sum = vec4<f32>(0.0);

	for (var i: i32 = 0; i < N; i = i + 1) {
		let offset = f32(i - M);
		var offsetTexCoord = input.texCoord;

		if (u_PostProcess.customParam1 > 0.5) {
			offsetTexCoord.x = offsetTexCoord.x + offset * (1.0 / u_PostProcess.screenSize.x) * u_PostProcess.customParam2;
		} else {
			offsetTexCoord.y = offsetTexCoord.y + offset * (1.0 / u_PostProcess.screenSize.y) * u_PostProcess.customParam2;
		}

		sum = sum + coeffs[i] * textureSample(u_Colour, u_Sampler, offsetTexCoord);
	}

	return sum;
}
