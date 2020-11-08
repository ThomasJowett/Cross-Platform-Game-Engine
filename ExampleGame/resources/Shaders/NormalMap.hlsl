Texture2D u_Texture : register(t0);

sampler samLinear : register(s0);

struct VS_INPUT
{
    float3 a_Position : POSITION;
    float3 a_Normal : NORMAL;
    float3 a_Tangent : TANGENT;
    float2 a_TexCoord : TEXCOORD;
};

struct VS_OUTPUT
{
    float4 v_ScreenPosition : SV_POSITION;
    float3 v_WorldPosition : POSITION;
    float2 v_TexCoord : TEXCOORD;
    float3 v_Tangent : TANGENT;
    float3 v_Normal : NORMAL;
};


cbuffer ConstantBuffer : register(b0)
{
    matrix u_ViewProjection;
    matrix u_ModelMatrix;
}

VS_OUTPUT vertexShader(VS_INPUT input)
{
    VS_OUTPUT output = (VS_OUTPUT) 0;
    
    float4 posW = mul(float4(input.a_Position, 1.0f), u_ViewProjection);
    output.v_WorldPosition = posW.xyz;
    
    output.v_ScreenPosition = mul(u_ViewProjection, mul(u_ModelMatrix, float4(input.a_Position, 1.0f)));
    
    float3 normalW = mul(float4(input.a_Normal, 0.0f), u_ModelMatrix).xyz;
    output.v_Normal = normalize(normalW);
    
    float3 tangentW = mul(float4(input.a_Tangent, 0.0f), u_ModelMatrix).xyz;
    output.v_Tangent = normalize(tangentW);
    
    output.v_TexCoord = input.a_TexCoord;
    
    return output;
}

float4 pixelShader(VS_OUTPUT input) : SV_Target
{
    return u_Texture.Sample(samLinear, input.v_TexCoord);
}