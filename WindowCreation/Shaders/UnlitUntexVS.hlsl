#include "Perspective.hlsli"

cbuffer Cbuff1 : register(b1)
{
    float4 translation;
    float4 quaternion;
};

float4 main(float3 pos : Position) : SV_Position
{
    float4 R3Pos = Q2V(qRot(quaternion, float4(0, pos))) + translation;

    return R3toScreenPos(R3Pos);
}