#include "Lighting.hlsli"

cbuffer ConstantBuffer : register(b0)
{
    float4 surfaceColor;
}

cbuffer drawRect : register(b1)
{
    float xOffset;
    float yOffset;
    float rectWidth;
    float rectHeight;
    float imgWidth;
    float imgHeight;
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
    float4 albedoColor = Albedo.Sample(BasicSampler, float2(input.uv.x * (rectWidth / imgWidth) + xOffset, input.uv.y * (rectHeight / imgHeight) + yOffset));

    if (albedoColor.w < 0.01f)
        discard;

    return albedoColor;
}