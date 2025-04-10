#include "Lighting.hlsli"

cbuffer ConstantBuffer : register(b0)
{
    float4 surfaceColor;
}

Texture2D Albedo : register(t0);
SamplerState BasicSampler : register(s0);

// --------------------------------------------------------
// The entry point (main method) for our pixel shader
// 
// - Input is the data coming down the pipeline (defined by the struct)
// - Output is a single color (float4)
// - Has a special semantic (SV_TARGET), which means 
//    "put the output of this into the current render target"
// - Named "main" because that's the default the shader compiler looks for
// --------------------------------------------------------
float4 main(VertexToPixel input) : SV_TARGET
{        
    float3 albedoColor = pow(Albedo.Sample(BasicSampler, input.uv/10).rgb, 2.2f);
 
    return float4(pow(surfaceColor.xyz * albedoColor, 1.0f / 2.2f), 1);
}