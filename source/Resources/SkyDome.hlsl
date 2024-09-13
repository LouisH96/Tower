
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
    float2 uv : TEXCOORD;
};

Pixel vs_main(Vertex vertex)
{
    Pixel pixel;
    pixel.pos = float4(vertex.pos, 0, 1);
    pixel.uv = float2(vertex.pos);
    return pixel;
}

static const float PI = 3.14159265f;

static const float4 red = float4(1, 0, 0, 1);
static const float4 green = float4(0, 1, 0, 1);
static const float4 blue = float4(0, 0, 1, 1);

float4 scalarColor(float value)
{
    return float4(value, value, value, 1);
}

float4 ps_main(Pixel pixel) : SV_TARGET
{
    float3 position = float3(
         1,
        dome_fovTan.y * pixel.uv.y,
        dome_fovTan.x * pixel.uv.x
    );
    
    position = normalize(float3(
    position.x * dome_pitch_cos - position.y * dome_pitch_sin,
    position.x * dome_pitch_sin + position.y * dome_pitch_cos,
    position.z
    ));
    
    float2 texturePos = float2(
        (atan2(position.z, position.x) - dome_yaw) / (2 * PI),
        1 - asin(position.y) / (PI / 2)
    );
    
    float3 diffuse = (float3) texture_main.Sample(sampler_main, texturePos);
    return float4(diffuse, 1);
}