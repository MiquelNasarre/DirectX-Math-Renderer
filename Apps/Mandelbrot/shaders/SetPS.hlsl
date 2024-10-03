
#define N 16

cbuffer cBuff : register(b0)
{
    float4 colors[N];
};

float4 main(float4 complex : Complex, float scale : Scale) : SV_TARGET
{
    float2 c = float2(complex.x, complex.y);
    int maxValue = 500;
    
    float2 z = float2(0.f, 0.f);
    
    int i;
    for (i = 0; i < maxValue; i++)
    {
        //z = float2(z.x * z.x * z.x - z.y * z.y * z.x - 2 * z.x * z.y * z.y + c.x, z.x * z.x * z.y - z.y * z.y * z.y + 2 * z.x * z.x * z.y + c.y);
        z = float2(z.x * z.x - z.y * z.y + c.x, 2 * z.x * z.y + c.y);
        if (z.x * z.x + z.y * z.y > 100)
            break;
    }
    
    if (i == maxValue)
        return colors[0];
    if (i > colors[1].w)
        return colors[1];
    
    for (int j = 2; j < N; j++)
    {
        if(i > colors[j].w)
            return colors[j] * (colors[j - 1].w - i) / (colors[j - 1].w - colors[j].w) + colors[j - 1] * (i - colors[j].w) / (colors[j - 1].w - colors[j].w);

    }
    return colors[N - 1];
}