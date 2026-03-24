cbuffer Camera : register(b0)
{
    float4x4 camera_view_projection;
    float3 camera_position;
    float padding1;
    float3 camera_forward;
    float padding2;
}