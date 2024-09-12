
Texture2D texture_main : register(t0);
SamplerState sampler_main : register(s0);

cbuffer PanelInfo : register(b1)
{
    float2 panel_size;
    float panel_height;
    float padding;
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
    float2 uv;
    const float2 hitPos = (float2) (pixel.uv) * panel_size;
 
    const float panelPlaneAngle = atan2(hitPos.y, hitPos.x) + PI;
    const float panelPlaneAngleRatio = panelPlaneAngle / (2 * PI);
    uv.x = panelPlaneAngleRatio;
    
    const float panelPlaneDist = sqrt(hitPos.x * hitPos.x + hitPos.y * hitPos.y);
    const float depthPlaneAngle = atan(panelPlaneDist / panel_height);
    uv.y = depthPlaneAngle * 2 / PI;
    
    //return scalarColor(uv.y);
    
    const float3 diffuse = (float3) texture_main.Sample(sampler_main, uv);
    return float4(diffuse, 1);
}