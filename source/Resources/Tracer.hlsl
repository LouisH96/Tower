#include "Shared.hlsl"

struct Vertex
{
    float3 pos : POSITION;
    float3 color : COLOR;
};

struct Pixel
{
    float4 pos : SV_POSITION;
    float3 color : COLOR;
};

Pixel vs_main(Vertex vertex)
{
    Pixel pixel;
    pixel.pos = mul(float4(vertex.pos, 1), camera_view_projection);
    pixel.color = vertex.color;
    return pixel;
}

float4 ps_main(Pixel pixel) : SV_TARGET
{
    return float4(pixel.color, 1);
}