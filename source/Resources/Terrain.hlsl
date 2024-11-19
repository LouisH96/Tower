#include "Shadow.hlsl"

//--| CBUFFER |--
cbuffer CBuffer : register(b0)
{
    float4x4 camera_matrix;
    float3 camera_pos;
    float padding;
}

//--| STRUCTS |---
struct Vertex
{
    float3 pos : POSITION;
    float3 norm : NORMAL;
    float3 color : COLOR;
};

struct Pixel
{
    float4 pos : SV_POSITION;
    float3 norm : NORMAL;
    float3 worldPos : POSITION2;
    float3 color : COLOR;
};

//--| VERTEX SHADER |--
Pixel vs_main(Vertex vertex)
{
    Pixel pixel = (Pixel) 0;
    pixel.worldPos = vertex.pos;
    pixel.pos = mul(float4(vertex.pos, 1), camera_matrix);
    pixel.color = vertex.color;
    pixel.norm = vertex.norm;
    return pixel;
}

//--| PIXEL SHADER |--
float4 ps_main(Pixel pixel) : SV_TARGET
{
    const float minLam = .2f;
    const float maxLam = 1.f;
    const float rangeLam = maxLam - minLam;
    
    float brightness = dot(normalize(camera_pos - pixel.worldPos), normalize(pixel.norm));
    brightness *= rangeLam;
    brightness += minLam;
    brightness *= GetShadowFactor(pixel.worldPos, normalize(pixel.norm));
    
    return float4(pixel.color * brightness, 1);
}