
Texture2D texture_main : register(t0);
SamplerState sampler_main : register(s0);

struct Vertex
{
    float2 pos : POSITION;
    float2 uv : TEXCOORD;
};

struct Pixel
{
    float4 pos : SV_POSITION;
    float2 uv : TEXCOORD;
};

Pixel vs_main(Vertex vertex)
{
    Pixel pixel;
    pixel.pos = float4(vertex.pos, 0, 1);
    pixel.uv = vertex.uv;
    return pixel;
}

float4 ps_main(Pixel pixel) : SV_TARGET
{
    const float3 diffuse = (float3) texture_main.Sample(sampler_main, pixel.uv);
    return float4(diffuse, 1);
}