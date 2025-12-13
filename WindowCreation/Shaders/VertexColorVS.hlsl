#include "Perspective.hlsli"

cbuffer Cbuff1 : register(b1)
{
    float4 rotation;            // Object rotation (quaternion)    
    float4 translation;         // World Translation
    float2 screenDisplacement;  // Screen displacement
};

struct VSOut
{
	float4 color : Color;
    float4 R3pos : PointPos;
    float4 norm : Norm;
	float4 SCpos : SV_Position;
};

VSOut main(float4 pos : Position, float4 norm : Normal, float4 color : Color)
{
	VSOut vso;
    
    // Transform the normal vector and position with the objects rotation
    vso.norm = Q2V(qRot(rotation, V2Q(norm))); 
    vso.R3pos = Q2V(qRot(rotation, V2Q(pos))) + translation;
    
    // Default method to transform from R3 to screen position
    vso.SCpos = R3toScreenPos(vso.R3pos);
    
    // Add screen displacement
    vso.SCpos.rg += screenDisplacement;
    
    // Forward color
	vso.color = color;
    
	return vso;
}