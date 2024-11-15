#include "Shadow.hlsl"

cbuffer Camera : register(b0)
{
    float4x4 camera_view_projection;
    float3 camera_position;
    float padding;
}

struct Vertex
{
    float3 localPos : POSITION;
    float3 normal : NORMAL;
    float2 uv : TEXCOORD;
    
    float4x4 modelMatrix : INST_MODEL;
};

struct Pixel
{
    float4 pos : SV_POSITION;
    float3 worldPos : POSITION2;
    float3 normal : NORMAL;
    float2 uv : TEXCOORD;
};

Texture2D diffuse : register(t0);
SamplerState default_sampler : register(s0);

Pixel vs_main(Vertex vertex)
{
    Pixel pixel;
    pixel.pos = mul(float4(vertex.localPos, 1), vertex.modelMatrix);
    pixel.worldPos = (float3) pixel.pos;
    pixel.pos = mul(pixel.pos, camera_view_projection);
    pixel.normal = mul(vertex.normal, (float3x3) vertex.modelMatrix);
    pixel.uv = vertex.uv;
    return pixel;
}

float4 ps_main(Pixel pixel) : SV_TARGET
{
    const float lambert = dot(normalize(camera_position - pixel.worldPos), normalize(pixel.normal));
    const float3 color = (float3) diffuse.Sample(default_sampler, pixel.uv);
    
    float brightness = lambert;
    brightness *= GetShadowFactor(pixel.worldPos);
    
    return float4(color * brightness, 1);
}