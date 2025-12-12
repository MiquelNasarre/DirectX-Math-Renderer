#include "Perspective.hlsli"

cbuffer Cbuff1 : register(b1)
{
    float4 traslation;
    float4 quaternion;
};

float4 main(float3 pos : Position) : SV_Position
{
    float4 R3Pos = Q2V(qRot(quaternion, float4(0, pos))) + traslation;

    return R3toScreenPos(R3Pos);
}