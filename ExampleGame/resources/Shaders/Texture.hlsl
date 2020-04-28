Texture3D u_textures : register(t0);

SamplerState samLinear : register(s0);

struct VS_INPUT
{
	float3 a_position : POSITION;
	float4 a_colour : COLOR;
	float2 a_texcoord : TEXCOORD;
	float a_texIndex : PSIZE;
};

struct VS_OUTPUT
{
	float4 v_position : SV_POSITION;
	float4 v_colour : COLOR;
	float2 v_texcoord: TEXCOORD;
	float v_texIndex : PSIZE;
};

VS_OUTPUT vertexShader(VS_INPUT input)
{
	VS_OUTPUT output = (VS_OUTPUT)0;

	output.v_colour = input.a_colour;
	output.v_texcoord = input.a_texcoord;
	output.v_texIndex = input.a_texIndex;

	output.v_position = float4(input.a_position, 1.0f);
	return output;
}

float4 pixelShader(VS_OUTPUT input) : SV_Target
{
	return u_textures.Sample(samLinear, float3(input.v_texcoord, input.v_texIndex)) * input.v_colour;
}