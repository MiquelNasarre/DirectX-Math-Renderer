
cbuffer Cbuff0 : register(b0)
{
    matrix projection;
    float4 center;
};

struct VSOut
{
    float4 ComplexPos : Complex;
    float4 scale : Scale;
    float4 SCpos : SV_Position;
};

VSOut main(float4 pos : Position)
{
    VSOut vso;
    
    vso.SCpos = pos;
    vso.scale = float4(projection._33, 0.f, 0.f, 0.f);
    vso.ComplexPos = float4(pos.x / projection._11 + center.x, pos.y / projection._22 + center.y, 0.f, 0.f);

    return vso;
}