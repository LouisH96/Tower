//--| CBUFFER |--
cbuffer CBuffer : register(b0)
{
    float4x4 camera_matrix;
    float3 camera_pos;
    float padding;
}

cbuffer LightBuffer : register(b1)
{
    float4x4 light_matrix;
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

Texture2D shadowMap : register(t1);
SamplerState sampler_main : register(s0);

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
    
    float lambert = dot(normalize(camera_pos - pixel.worldPos), normalize(pixel.norm));
    lambert *= rangeLam;
    lambert += minLam;
    
    //where is my pixel's world pos inside the shadowSpace
    float4 shadowSpace = mul(float4(pixel.worldPos, 1), light_matrix);
    shadowSpace /= shadowSpace.w;
    shadowSpace.xy += float2(1, 1);
    shadowSpace.xy /= float2(2, 2);
    shadowSpace.y = 1 - shadowSpace.y;
    const float shadowMapValue = shadowMap.Sample(sampler_main, shadowSpace.xy).r;
    
    if (
        shadowSpace.x >= 0 && shadowSpace.x <= 1
        && shadowSpace.y >= 0 && shadowSpace.y <= 1
        && shadowSpace.z >= 0 && shadowSpace.z <= 1
        )
    {
        if (shadowSpace.z > shadowMapValue)
            return float4(0, 1, 0, 1);
        else
            return float4(1, 0, 0, 1);
    }
    
    return float4(pixel.color * lambert, 1);
}