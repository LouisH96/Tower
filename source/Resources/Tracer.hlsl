#include "Shared.hlsl"

struct Vertex
{
    float3 pos : POSITION;
    float4 color : COLOR;
    float2 uv : TEXCOORD;
};

struct Pixel
{
    float4 pos : SV_POSITION;
    float4 color : COLOR;
    float2 uv : TEXCOORD;
};

Pixel vs_main(Vertex vertex)
{
    Pixel pixel;
    pixel.pos = mul(float4(vertex.pos, 1), camera_view_projection);
    pixel.color = vertex.color;
    pixel.uv = vertex.uv;
    
    return pixel;
}

float4 ps_main(Pixel pixel) : SV_TARGET
{
    float y = frac(pixel.uv.y);
    
    float width = .15f;
    float gradient = .15f;
    
    y = abs(y - .5f);
    
    float power = pixel.color.a;
    
    if (y < width)
        power *= 1;
    else if (y < width + gradient)
        power *= 1 - ((y - width) / gradient);
    else
        power = 0;
    
    float red = power;
    float green = 1 - power;
    
    float gray = .8f;
    //return float4(1, 0, 0, power * .2f);
    return float4(pixel.color.r, pixel.color.g, pixel.color.b, power);
    //return float4(gray, gray, gray, power);
    //return float4(1, 0, 0, power * .2f);
    
    //return float4(1, 0, 0, power);
    //return float4(red, green, 0, 1);
    
    
    //return float4(pixel.color, 1);
}