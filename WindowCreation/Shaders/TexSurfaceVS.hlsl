#include "Perspective.hlsli"

cbuffer Cbuff1 : register(b1)
{
    float4 traslation;
    float4 quaternion;
};

struct VSOut
{
    float2 tex : TexCoord;
    float4 R3pos : PointPos;
    float4 norm : Norm;
    float4 SCpos : SV_Position;
};

VSOut main(float3 pos : Position, float3 norm : Normal, float2 tex : TexCoord)
{
    VSOut vso;
    
    vso.norm = Q2V(qRot(quaternion, float4(0, norm)));
    vso.R3pos = Q2V(qRot(quaternion, float4(0, pos))) + traslation;
    
    vso.SCpos = R3toScreenPos(vso.R3pos);
    vso.tex = tex;
    return vso;
}