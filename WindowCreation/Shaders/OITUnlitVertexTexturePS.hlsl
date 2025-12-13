
// Texture and sampler in first register
Texture2D _texture : register(t0);
SamplerState _samp : register(s0);

struct PSOut
{
    float4 accum : SV_Target0; // premultiplied color + alpha
    float reveal : SV_Target1; // alpha for revealage product
};

PSOut main(float2 coord : Coord)
{
    // Sample from texture
    float4 color = _texture.Sample(_samp, coord);
    
    // Use incoming color alpha as transparency
    float alpha = saturate(color.a);

    PSOut outp;
    // Accumulation target: premultiplied color + alpha (C_src * C_dst, A_src + A_dst)
    outp.accum = float4(color.rgb * alpha, alpha);
    
    // Reveal target: alpha ((1 - A_src) * (1 - A_dst))
    outp.reveal = alpha;

    return outp;
}