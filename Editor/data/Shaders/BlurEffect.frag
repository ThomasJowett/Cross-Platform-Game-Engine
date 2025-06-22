#version 450 core

in vec2 v_TexCoord;
layout(location = 0)out vec4 frag_colour;

layout(binding = 0)uniform sampler2D u_Colour;

layout(std140, binding = 3)uniform PostProcessBuffer
{
    vec2 u_TextureSize;
    float u_Time;
    float u_DeltaTime;
    
    float u_CustomParam1; // horizontal
    float u_CustomParam2; // strength
    float u_CustomParam3;
    float u_CustomParam4;
};

const int M = 16;
const int N = 2 * M + 1;

// sigma = 10
const float coeffs[N] = float[](
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

void main()
{
    vec4 sum = vec4(0.0);
    
    for(int i = 0; i < N; ++ i)
    {
        int offset = i - M;
        vec2 offset_texcoord = v_TexCoord;
        
        if (u_CustomParam1 > 0.5) {
            offset_texcoord.x += offset * (1.0f / u_TextureSize.x) * u_CustomParam2;
        }
        else {
            offset_texcoord.y += offset * (1.0f / u_TextureSize.y) * u_CustomParam2;
        }
        
        sum += coeffs[i] * texture(u_Colour, offset_texcoord);
    }
    
    frag_colour = sum;
}