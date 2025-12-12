#include "Perspective.hlsli"

cbuffer cBuff1 : register(b1)
{
    float4 position;
    float4 quaternion;
    float radius;
}

float4 main(float4 norm : Normal) : SV_Position
{
    float4 R3Pos = Q2V(qRot(quaternion, V2Q(position + norm * radius / scaling.b - center)));

    return R3toScreenPos(R3Pos);
}