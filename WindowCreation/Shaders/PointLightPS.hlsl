
cbuffer cBuff : register(b0)
{
    float4 color;
}

float4 main(float intensity : Intensity) : SV_Target
{
    return color * intensity;
}