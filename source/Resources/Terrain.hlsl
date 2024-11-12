//http://www.opengl-tutorial.org/intermediate-tutorials/tutorial-16-shadow-mapping/

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
SamplerComparisonState sampler_main : register(s0);

static const uint nrSamples = 4;
static const uint nrRandomOffsets = 16;
static const float edgeThickness = 1.f / 900; //how 'thick' the soft edge of the shadow is (random unit)
static const float2 randomOffsets[nrRandomOffsets] =
{
    float2(-0.16997, -0.82578),
    float2(-0.0168109, -0.0128592),
    float2(0.767212, 0.137829),
    float2(0.21882, 0.2482),
    float2(-0.62805, 0.58695),
    float2(-0.61265, -0.46394),
    float2(0.45748, -0.6795),
    float2(0.16878, -0.42929),
    float2(-0.3047, 0.20586),
    float2(0.66535, -0.25607),
    float2(-0.31625, -0.25222),
    float2(0.48058, 0.47146),
    float2(-0.71273, 0.10962),
    float2(0.38434, -0.05205),
    float2(-0.22771, 0.65239),
    float2(0.23037, 0.77172),
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
    
    const float maxShadowImpact = .3f;
    
    //where is my pixel's world pos inside the shadowSpace
    float4 shadowSpace = mul(float4(pixel.worldPos, 1), light_matrix);
    
    if (shadowSpace.x >= -shadowSpace.w && shadowSpace.x <= shadowSpace.w &&
        shadowSpace.z >= 0 && shadowSpace.z <= shadowSpace.w)
    {
        shadowSpace /= shadowSpace.w;
        shadowSpace.xy += float2(1, 1);
        shadowSpace.xy /= float2(2, 2);
        shadowSpace.y = 1 - shadowSpace.y;
        
        float4 random4 = float4(floor(pixel.worldPos * 1000), 0);
        float sum = 0;
        for (uint iSample = 0; iSample < nrSamples; iSample++)
        {
            random4.w = iSample * 111;
            
            float random = dot(random4, float4(42.98, 78.23, 45.14, 94.63));
            random = frac(sin(random / 16.f)) * 971.123;
            
            uint index = ((uint) (random * nrRandomOffsets)) % nrRandomOffsets;
            float2 coord = shadowSpace.xy + randomOffsets[index] * edgeThickness;
            
            sum += shadowMap.SampleCmpLevelZero(sampler_main, coord, shadowSpace.z).r;
        }
        sum /= nrSamples;
        brightness *= 1.0f - maxShadowImpact * sum;
    }
    
    return float4(pixel.color * brightness, 1);
}