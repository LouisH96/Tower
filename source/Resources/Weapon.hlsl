#include "Shared.hlsl"
#include <Skinning.hlsl>

cbuffer ModelBuffer : register(b1)
{
    float4x4 model_matrix;
}

//--| STRUCTS |---
struct Vertex
{
    float3 pos : POSITION;
    float3 norm : NORMAL;
    float2 uv : TEXCOORD;
    int4 boneIds : BLENDINDICES;
    float4 boneWeights : BLENDWEIGHT;
};

struct Pixel
{
    float4 pos : SV_POSITION;
    float3 norm : NORMAL; 
    float3 worldPos : POSITION2;
    float2 uv : TEXCOORD;
};

//--| Textures |--
Texture2D texture_main : register(t0);
SamplerState sampler_main : register(s0);

//--| VERTEX SHADER |--
Pixel vs_main(Vertex vertex)
{
    Pixel pixel;
    
    float4x4 bone = MakeBonesMatrix(vertex.boneIds, vertex.boneWeights);
    bone = mul(bone, model_matrix);
    
    pixel.worldPos = (float3) mul(float4(vertex.pos, 1), bone);
    pixel.pos = mul(float4(pixel.worldPos, 1), camera_view_projection);
    pixel.uv = vertex.uv;
    pixel.norm = mul(vertex.norm, (float3x3) bone);
    return pixel;
}

//--| PIXEL SHADER |--
float4 ps_main(Pixel pixel) : SV_TARGET
{
    const float lambert = dot(normalize(camera_position - pixel.worldPos), normalize(pixel.norm));
    const float3 color = (float3) texture_main.Sample(sampler_main, pixel.uv);
    return float4(color * lambert, 1);
}