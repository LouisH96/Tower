#include "Shared.hlsl"
#include "Shadow.hlsl"

cbuffer ModelBuffer : register(b1)
{
    float4x4 model_matrix;
}

static const uint MAX_BONES = 1024;
static const uint BONES_PER_VERTEX = 4;
cbuffer BonesBuffer : register(b3)
{
    float4x4 bone_transforms[MAX_BONES];
}

//--| Structs |---
struct Vertex
{
    float3 localPos : POSITION;
    float3 normal : NORMAL;
    float2 uv : TEXCOORD;
    int4 boneIds : BLENDINDICES;
    float4 boneWeights : BLENDWEIGHTS;
    
    float4x4 modelWorld : INST_WORLD;
    uint boneIdOffset : INST_BONE_OFFSET;
};

struct Pixel
{
    float4 pos : SV_POSITION;
    float3 normal : NORMAL;
    float3 worldPos : POSITION2;
    float2 uv : TEXCOORD;
};

//--| Textures |--
Texture2D diffuse : register(t0);
SamplerState default_sampler : register(s0);

//---| Shaders |---
Pixel vs_main(Vertex vertex)
{
    //Skinning
    float4x4 bone = float4x4(
        0, 0, 0, 0,
        0, 0, 0, 0,
        0, 0, 0, 0,
        0, 0, 0, 0);
    
    for (uint iBone = 0; iBone < BONES_PER_VERTEX; ++iBone)
    {
        if (vertex.boneIds[iBone] < 0)
            break;

        const uint boneId = vertex.boneIds[iBone] + vertex.boneIdOffset;
        bone += bone_transforms[boneId] * vertex.boneWeights[iBone];
    }
    bone = mul(bone, vertex.modelWorld);
    
    //
    Pixel pixel;
    pixel.worldPos = (float3) mul(float4(vertex.localPos, 1), bone);
    pixel.pos = mul(float4(pixel.worldPos, 1), camera_view_projection);
    pixel.uv = vertex.uv;
    pixel.normal = mul(vertex.normal, (float3x3) bone);
    
    return pixel;
}

float4 ps_main(Pixel pixel) : SV_TARGET
{
    const float lambert = dot(normalize(camera_position - pixel.worldPos), normalize(pixel.normal));
    const float3 color = (float3) diffuse.Sample(default_sampler, pixel.uv);
    
    float brightness = lambert;
    brightness *= GetShadowFactor(pixel.worldPos, normalize(pixel.normal));
    
    return float4(color * brightness, 1);
}