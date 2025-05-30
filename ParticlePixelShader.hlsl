//vertex to pixel struct for particle
struct VertexToPixel_Particle
{
    float4 position : SV_POSITION; // XYZW position
    float2 uv : TEXCOORD; // UV coordinates
    float4 color : COLOR; // color
};

Texture2D Particle : register(t0);
SamplerState BasicSampler : register(s0);

cbuffer externalData : register(b0)
{
    float3 colorTint;
};

//entry point for the pixel shader
float4 main(VertexToPixel_Particle input) : SV_TARGET
{
    //sample the texture
    float4 color = Particle.Sample(BasicSampler, input.uv) * input.color;
    color.rgb *= colorTint;
    
    //return the particle color
    return color;
}