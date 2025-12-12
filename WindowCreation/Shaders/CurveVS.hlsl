#include "Perspective.hlsli"

cbuffer Cbuff1 : register(b1)
{
    float4 traslation;
    float4 quaternion;
};

struct VSOut
{
    float4 color : Color;
    float4 SCpos : SV_Position;
};

VSOut main( float3 pos : Position, float4 color : Color)
{
    VSOut vso;
    vso.color = color;
    float4 R3Pos = Q2V(qRot(quaternion, float4(0, pos))) + traslation;
    
    vso.SCpos = R3toScreenPos(R3Pos);
	return vso;
}