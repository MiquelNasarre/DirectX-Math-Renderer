
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

struct PSOut
{
    float4 accum : SV_Target0; // premultiplied color + alpha
    float reveal : SV_Target1; // alpha for revealage product
};

PSOut main(float4 color : Color,
           float3 pos : PointPos,
           float3 norm : Norm,
           bool front : SV_IsFrontFace)
{
    // Handle backfaces: flip normal if fragment is from back side
    if (!front)
        norm = -norm;
    
    float4 totalLight = float4(0.f, 0.f, 0.f, 0.f);
    float dist = 0.f;
    float exposure = 0.f;
    
    // Simple point lights
    [unroll]
    for (int i = 0; i < 8; i++)
    {
        // skip unused lights
        if (!lights[i].intensity.r && !lights[i].intensity.g)
            continue;
        
        dist = distance(lights[i].position, pos);
        float3 L = (lights[i].position - pos) / dist; // direction to light
        exposure = dot(norm, L);
    
        float light = lights[i].intensity.g / (dist * dist); // diffuse base
        if (exposure > 0.f)
            light += lights[i].intensity.r * exposure / (dist * dist); // directional term

        totalLight += light * lights[i].color;
    }
    
    // Apply lighting to base color
    float4 lit = color * totalLight;

    // Use incoming color alpha as transparency (you can change this)
    float alpha = saturate(color.a);

    PSOut outp;
    // Accumulation target: premultiplied color + alpha
    outp.accum = float4(lit.rgb * alpha, alpha);
    // Reveal target: just alpha (used to build product of (1 - alpha))
    outp.reveal = alpha;

    return outp;
}