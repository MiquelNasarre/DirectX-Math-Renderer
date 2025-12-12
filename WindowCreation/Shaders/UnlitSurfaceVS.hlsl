#include "Perspective.hlsli"

cbuffer Cbuff1 : register(b1)
{
    float4 traslation;
    float4 quaternion;
};

struct VSOut
{
    float2 tex : TexCoord;
    float4 SCpos : SV_Position;
};

VSOut main(float3 pos : Position, float2 tex : TexCoord)
{
    VSOut vso;
    float4 R3Pos = Q2V(qRot(quaternion, V2Q(pos))) + traslation;

    vso.SCpos = R3toScreenPos(R3Pos);
    vso.tex = tex;
    return vso;
}