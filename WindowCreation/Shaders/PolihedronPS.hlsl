
struct Lightsource
{
    float2 intensity;
    //<-- theres a hidden float2 in here just for dramatic purpose
    float4 color;
    float3 position;
    //<-- and a float here as well!!
};

cbuffer cBuff : register(b0)
{
    Lightsource lights[8];
};

float4 main(float4 color : Color, float3 pos : PointPos, float3 norm : Norm, bool front : SV_IsFrontFace) : SV_Target
{
    if (!front)
        norm = -norm;
    
    float4 totalLight = float4(0.f, 0.f, 0.f, 0.f);
    float dist = 0;
    float exposure = 0;
    
    for (int i = 0; i < 8; i++)
    {
        if (!lights[i].intensity.r && !lights[i].intensity.g)
            continue;
        
        dist = distance(lights[i].position, pos);
        exposure = dot(norm, (lights[i].position -  pos) / dist);
    
        float light = lights[i].intensity.g / dist / dist;
        if (exposure > 0)
            light += lights[i].intensity.r * exposure / dist / dist;
        totalLight += light * lights[i].color;
    }
    
    float4 col = color * totalLight;
    return float4(col.r, col.g, col.b, color.a);
}