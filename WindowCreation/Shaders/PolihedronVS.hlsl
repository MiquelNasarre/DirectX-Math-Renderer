#include "Perspective.hlsli"

cbuffer Cbuff1 : register(b1)
{
    float4 translation;         // World Translation
    float4 rotation;            // Object rotation (quaternion)
    float2 screenDisplacement;  // Screen displacement
};

struct VSOut
{
	float4 color : Color;
    float4 R3pos : PointPos;
    float4 norm : Norm;
	float4 SCpos : SV_Position;
};

VSOut main(float3 pos : Position, float3 norm : Normal, float4 color : Color)
{

	VSOut vso;
    
    vso.norm = Q2V(qRot(rotation, V2Q(norm)));
    vso.R3pos = Q2V(qRot(rotation, V2Q(pos))) + translation;
    
    vso.SCpos = R3toScreenPos(vso.R3pos);
    vso.SCpos.rg += screenDisplacement;
    
	vso.color = color;
    
	return vso;
}