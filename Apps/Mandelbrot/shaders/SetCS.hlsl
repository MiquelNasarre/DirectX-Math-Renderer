// MandelbrotCS.hlsl
RWTexture2D<float4> outputTexture : register(u0);

// Constants
cbuffer Constants : register(b0)
{
    float2 initialPosition;
    float2 pixelDistance;
    uint maxIterations;
    float4 palette[256]; // Assume a palette of 256 colors
};

[numthreads(16, 16, 1)]
void main(uint3 dispatchID : SV_DispatchThreadID)
{
    uint x = dispatchID.x;
    uint y = dispatchID.y;

    // Compute complex number c for this pixel
    float2 c;
    c.x = initialPosition.x + x * pixelDistance.x;
    c.y = initialPosition.y + y * pixelDistance.y;

    // Initialize z = 0
    float2 z = { 0.0f, 0.0f };
    uint iterations = 0;

    // Mandelbrot iteration
    while (z.x * z.x + z.y * z.y <= 4.0f && iterations < maxIterations)
    {
        float temp = z.x * z.x - z.y * z.y + c.x;
        z.y = 2.0f * z.x * z.y + c.y;
        z.x = temp;
        iterations++;
    }

    // Choose color based on iterations
    float4 color = (iterations == maxIterations) ? float4(0, 0, 0, 1) : palette[iterations % 256];

    // Write to texture
    outputTexture[uint2(x, y)] = color;
}