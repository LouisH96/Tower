//http://www.opengl-tutorial.org/intermediate-tutorials/tutorial-16-shadow-mapping/

static const uint nrSamples = 4;
static const uint nrRandomOffsets = 16;
static const float edgeThickness = 1.f / 900; //how 'thick' the soft edge of the shadow is (random unit)
static const float maxShadowImpact = .3f;
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

static const uint nrSlices = 3;
static const float sliceEnd[nrSlices] =
{
    5.f, 15.f, 50.f
};

cbuffer ShadowMap : register(b2)
{
    float4x4 shadow_map_view_projection[nrSlices];
    float3 shadow_forward;
    float shadow_padding[13];
}

Texture2D shadow_map_texture_near : register(t1);
Texture2D shadow_map_texture_mid : register(t2);
Texture2D shadow_map_texture_far : register(t3);
SamplerComparisonState shadow_sampler : register(s1);

float GetShadowFactor(float3 worldPos, float3 hitNormal)
{
    //return 1 means no shadow, 0 means full shadow
    float factor = 1;
    
    if (dot(shadow_forward, hitNormal) >= 0)
    {
        factor = 1.0f - maxShadowImpact;
    }
    else
    {
        uint slice = 0;
        for (; slice < nrSlices; ++slice)
        {
            float4 shadowSpace = mul(float4(worldPos, 1), shadow_map_view_projection[slice]);
            if (shadowSpace.x > -shadowSpace.w && shadowSpace.x < shadowSpace.w 
                && shadowSpace.y > -shadowSpace.w && shadowSpace.y < shadowSpace.w
                && shadowSpace.z > 0 && shadowSpace.z < shadowSpace.w)
            {
                shadowSpace /= shadowSpace.w;
                shadowSpace.xy += float2(1, 1);
                shadowSpace.xy /= float2(2, 2);
                shadowSpace.y = 1 - shadowSpace.y;
        
                float4 random4 = float4(floor(worldPos * 1000), 0);
                float sum = 0;
                for (uint iSample = 0; iSample < nrSamples; iSample++)
                {
                    random4.w = iSample * 111;
            
                    float random = dot(random4, float4(42.98, 78.23, 45.14, 94.63));
                    random = frac(sin(random / 16.f)) * 971.123;
            
                    uint index = ((uint) (random * nrRandomOffsets)) % nrRandomOffsets;
                    float2 coord = shadowSpace.xy + randomOffsets[index] * edgeThickness;
            
                    if (slice == 0)
                        sum += shadow_map_texture_near.SampleCmpLevelZero(shadow_sampler, coord, shadowSpace.z - 0.000f).r;
                    else if (slice == 1)
                        sum += shadow_map_texture_mid.SampleCmpLevelZero(shadow_sampler, coord, shadowSpace.z - 0.000f).r;
                    else
                        sum += shadow_map_texture_far.SampleCmpLevelZero(shadow_sampler, coord, shadowSpace.z - 0.000f).r;
                }
                sum /= nrSamples;
                factor = 1.0f - maxShadowImpact * sum;
                break;
            }
        }
    }
    return factor;
}