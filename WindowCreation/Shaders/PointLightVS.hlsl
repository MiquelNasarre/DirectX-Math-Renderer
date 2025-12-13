#include "Perspective.hlsli"

struct VSOut
{
    float intensity : Intensity;
    float4 SCpos : SV_Position;
};

cbuffer cBuff1 : register(b1)
{
    float4 position;
    float4 rotation;
    float radius;
}

VSOut main(float2 pos : Reference, float intensity : Intensity)
{
    float4 e1 = normalize(float4(rotation.y, -rotation.x, 0.f, 0.f));
    float4 e2 = normalize(float4(-rotation.x * rotation.z, -rotation.z * rotation.y, rotation.y * rotation.y + rotation.x * rotation.x, 0.f));
    
    VSOut vso;
    vso.intensity = intensity;
    float4 obs = mul(position + (e1 * pos.x + e2 * pos.y) * radius - center, rotation);
    vso.SCpos = float4(obs.x, obs.y, z_trans(obs.z), 1.f);
    
    return vso;
}