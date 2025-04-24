//vs input struct for particles
struct VertexShaderInput_Particle
{
    float3 localPosition : POSITION; // XYZ position
    float2 uv : TEXCOORD; // UV coordinates
    float4 color : COLOR; // color
};

//vertex to pixel struct for particle
struct VertexToPixel_Particle
{
    float4 position : SV_POSITION; // XYZW position
    float2 uv : TEXCOORD; // UV coordinates
    float4 color : COLOR; // color
};

//constant buffer definition
cbuffer externalData : register(b0)
{
    //float4x4 world;
    float4x4 view;
    float4x4 projection;
};

//entry point for the vertex shader
VertexToPixel_Particle main(VertexShaderInput_Particle input)
{
    //set up output
    VertexToPixel_Particle output;
    
    //calculate the position
    matrix wvp = mul(projection, view);
    output.position = mul(wvp, float4(input.localPosition, 1.0f));
    
    //pass uv and color through
    output.uv = input.uv;
    output.color = input.color;
    
    return output;
}