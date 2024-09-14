
Texture2D texture_main : register(t0);
SamplerState sampler_main : register(s0);

cbuffer DomeBufffer : register(b1)
{
    float2 dome_fovTan;
    float dome_pitch_cos; //camera
    float dome_pitch_sin;
    
    float dome_yaw; //radians
    float3 padding;
}

struct Vertex
{
    float2 pos : POSITION;
};

struct Pixel
{
    float4 pos : SV_POSITION;
    float3 uv : TEXCOORD;
};

Pixel vs_main(Vertex vertex)
{
    Pixel pixel;
    pixel.pos = float4(vertex.pos, 0, 1);
    
    pixel.uv.x = dome_fovTan.x * pixel.pos.x;
    pixel.uv.y = dome_pitch_sin + dome_fovTan.y * pixel.pos.y * dome_pitch_cos;
    pixel.uv.z = dome_pitch_cos - dome_fovTan.y * pixel.pos.y * dome_pitch_sin;
    
    return pixel;
}

static const float PI = 3.14159265f;
static const float INV_2PI = 1.f / (2 * PI);
static const float INV_HALF_PI = 2.f / PI;

float4 ps_main(Pixel pixel) : SV_TARGET
{
    pixel.uv = normalize(pixel.uv);
    
    float2 texturePos = float2(
        (atan2(pixel.uv.x, pixel.uv.z) + dome_yaw) * INV_2PI,
        1 - asin(pixel.uv.y) * INV_HALF_PI
    );
    
    float3 diffuse = (float3) texture_main.Sample(sampler_main, texturePos);
    return float4(diffuse, 1);
}