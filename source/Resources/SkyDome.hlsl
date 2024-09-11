
struct Vertex
{
    float2 pos : POSITION;
};

struct Pixel
{
    float4 pos : SV_POSITION;
};

Pixel vs_main(Vertex vertex)
{
    Pixel pixel;
    pixel.pos = float4(vertex.pos, 0, 1);
    return pixel;
}

float4 ps_main(Pixel pixel) : SV_TARGET
{
    return float4(1, 0, 0, 1);
}