#ifndef HLSL_INCLUDE_HOGE
#define HLSL_INCLUDE_HOGE

struct Particle
{
    int uuid;
    float size;
    float3 position;
    float3 velocity;
    float4 color;
};

float Noise(float x)
{
    float i = floor(x);
    float f = frac(x);
    float s = sign(frac(x / 2.0) - 0.5);

    float k = frac(i * .1731);

    return s * f * (f - 1.0) * ((16.0 * k - 4.0) * f * (f - 1.0) - 1.0);
}

float SdCircle(float2 p, float r)
{
    return length(p) - r;
}

float SdBox(float2 p, float2 b)
{
    float2 d = abs(p) - b;
    return length(max(d, 0.0)) + min(max(d.x, d.y), 0.0);
}

float SdTriangle(float2 p, float r)
{
    const float k = sqrt(3.0);
    p.x = abs(p.x) - r;
    p.y = p.y + r / k;
    if (p.x + k * p.y > 0.0) p = float2(p.x - k * p.y, -k * p.x - p.y) / 2.0;
    p.x -= clamp(p.x, -2.0 * r, 0.0);
    return -length(p) * sign(p.y);
}

float SdPentagon(float2 p, float r)
{
    const float3 k = float3(0.809016994, 0.587785252, 0.726542528);
    p.x = abs(p.x);
    p -= 2.0 * min(dot(float2(-k.x, k.y), p), 0.0) * float2(-k.x, k.y);
    p -= 2.0 * min(dot(float2(k.x, k.y), p), 0.0) * float2(k.x, k.y);
    p -= float2(clamp(p.x, -r * k.z, r * k.z), r);
    return length(p) * sign(p.y);
}

float2 Rotate(float2 p, float rad)
{
    float2 res;
    res.x = p.x * cos(rad) - p.y * sin(rad);
    res.y = p.x * sin(rad) + p.y * cos(rad);
    return res;
}


// Permuted Congruential Generator
// Original source code: https://www.shadertoy.com/view/XlGcRh

#define FLOAT_MAX float(0xffffffffu)
#define FLOAT_MAX_INVERT 1.0 / FLOAT_MAX

uint Pcg(uint v)
{
    uint state = v * 747796405u + 2891336453u;
    uint word = ((state >> ((state >> 28u) + 4u)) ^ state) * 277803737u;
    return (word >> 22u) ^ word;
}

uint2 Pcg2d(uint2 v)
{
    v = v * 1664525u + 1013904223u;

    v.x += v.y * 1664525u;
    v.y += v.x * 1664525u;

    v = v ^ (v >> 16u);

    v.x += v.y * 1664525u;
    v.y += v.x * 1664525u;

    v = v ^ (v >> 16u);

    return v;
}

uint3 Pcg3d(uint3 v)
{
    v = v * 1664525u + 1013904223u;

    v.x += v.y * v.z;
    v.y += v.z * v.x;
    v.z += v.x * v.y;

    v ^= v >> 16u;

    v.x += v.y * v.z;
    v.y += v.z * v.x;
    v.z += v.x * v.y;

    return v;
}

#endif
