
cbuffer cBuff : register(b0)
{
    float4 color;
}

struct PSOut
{
    float4 accum : SV_Target0; // premultiplied color + alpha
    float reveal : SV_Target1; // alpha for revealage product
};

PSOut main(float dist : Distance)
{
    if (dist < 1e-5)
        dist = 1e-5;
    
    PSOut pso;
    
    // We do not multiply because we want a glow effect.
    pso.accum = float4(color.rgb, dist * dist);
    
    // No opacity added.
    pso.reveal = 1.f / (dist * dist);
    
    return pso;
}