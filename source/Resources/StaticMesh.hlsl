#include "Shadow.hlsl"
#include "Shared.hlsl"

//--| STRUCTS |---
struct Vertex
{
    float3 pos : POSITION;
    float3 norm : NORMAL;
    float2 uv : TEXCOORD;
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
    Pixel pixel = (Pixel) 0;
    pixel.worldPos = vertex.pos;
    pixel.pos = mul(float4(vertex.pos, 1), camera_view_projection);
    pixel.uv = vertex.uv;
    pixel.norm = vertex.norm;
    return pixel;
}

//--| PIXEL SHADER |--
float4 ps_main(Pixel pixel) : SV_TARGET
{
    const float minLam = .2f;
    const float maxLam = 1.f;
    const float rangeLam = maxLam - minLam;
    
    float brightness = dot(normalize(camera_position - pixel.worldPos), normalize(pixel.norm));
    brightness *= rangeLam;
    brightness += minLam;
    brightness *= GetShadowFactor(pixel.worldPos, normalize(pixel.norm));
    
    const float3 color = (float3) texture_main.Sample(sampler_main, pixel.uv);
    return float4(color * brightness, 1);
}