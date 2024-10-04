
#define N 20

cbuffer cBuff : register(b0)
{
    float4 colors[N];
};

float4 main(float4 complex : Complex, float scale : Scale) : SV_TARGET
{
    float2 c = float2(complex.x, complex.y);
    
    float divergence = 100;
    int maxValue = 1000;
    
    float2 z = float2(0.f, 0.f);
    
    int i;
    float abs;
    
    for (i = 0; i < maxValue; i++)
    {
        z = float2(z.x * z.x - z.y * z.y + c.x, 2 * z.x * z.y + c.y);
        
        abs = z.x * z.x + z.y * z.y;
        if (abs > divergence)
            break;
    }
    
    if (i == maxValue)
        return colors[0];
    
    if (i > colors[1].w)
        return colors[1];
    
    for (int j = 2; j < N; j++)
    {
        if(i > colors[j].w)
        {
            float ip = i - sqrt((abs - divergence) / (divergence * (divergence - 1)));
            return colors[j] * (colors[j - 1].w - ip) / (colors[j - 1].w - colors[j].w) + colors[j - 1] * (ip - colors[j].w) / (colors[j - 1].w - colors[j].w);            
        }

    }
    return colors[N - 1];
}