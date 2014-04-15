#include "perlinnoise.h"

PerlinNoise::PerlinNoise(float p, int o)
{
    persistence = p;
    octaves = o;
}

float PerlinNoise::noise(int x1, int x2, int x3){
    int n = x1 + x2*57 + x3*61;
    n = (n << 13) ^ n;
    return (1.0f - ( (n * (n * n * 15731 + 789221) + 1376312589) & 0x7fffffff) / 1073741824.0f);
}

float PerlinNoise::noise2D(int x1, int x2){
    int n = x1 + x2*57;
    n = (n << 13) ^ n;
    return (1.0f - ( (n * (n * n * 15731 + 789221) + 1376312589) & 0x7fffffff) / 1073741824.0f);
}

float PerlinNoise::smoothNoise2D(int x, int y){
    float corners = noise2D(x + 1, y + 1) +
                    noise2D(x - 1, y + 1) +
                    noise2D(x - 1, y - 1) +
                    noise2D(x + 1, y - 1);

    float edges = noise2D(x + 1, y) +
                    noise2D(x - 1, y) +
                    noise2D(x, y + 1) +
                    noise2D(x, y - 1);

    float center = noise2D(x, y);
    return corners / 16 + edges / 8 + center / 4;
}

float PerlinNoise::interpolatedNoise2D(float x, float y){
    int int_x = (int) x;
    float frac_x = int_x - x;
    int int_y = (int) y;
    float frac_y = int_y - y;

    // Centers of the faces of the cube
    float v1 = smoothNoise2D(x, y);
    float v2 = smoothNoise2D(x + 1, y);
    float v3 = smoothNoise2D(x, y + 1);
    float v4 = smoothNoise2D(x + 1, y + 1);

    float i1 = interpolate(v1, v2, frac_x);
    float i2 = interpolate(v3, v4, frac_x);

    return interpolate(i1, i2, frac_y);
}

float PerlinNoise::smoothNoise(int x, int y, int z){
    // Averages the noise values on the cube centered on (x, y, z)
    // by average over the 8 subcubes that have (x, y, z) as a corner.

    // Corners, each counted once
    float corners = noise(x + 1, y + 1, z + 1) +
                    noise(x + 1, y + 1, z - 1) +
                    noise(x + 1, y - 1, z + 1) +
                    noise(x + 1, y - 1, z - 1) +
                    noise(x - 1, y + 1, z + 1) +
                    noise(x - 1, y + 1, z - 1) +
                    noise(x - 1, y - 1, z + 1) +
                    noise(x - 1, y - 1, z - 1);
    corners = corners / 64;

//    // Centers of edges, each counted twice
    float edges = 0;
//    float edges =   noise(x, y + 1, z + 1) +
//                    noise(x, y - 1, z + 1) +
//                    noise(x, y - 1, z - 1) +
//                    noise(x, y + 1, z - 1) +
//                    noise(x + 1, y, z + 1) +
//                    noise(x + 1, y, z - 1) +
//                    noise(x - 1, y, z - 1) +
//                    noise(x - 1, y, z + 1) +
//                    noise(x + 1, y + 1, z) +
//                    noise(x + 1, y - 1, z) +
//                    noise(x - 1, y - 1, z) +
//                    noise(x - 1, y + 1, z);

//    edges = edges / 32;

    // Centers of faces, each counted 4 times
    float faces =   noise(x, y, z + 1) +
                    noise(x, y, z - 1) +
                    noise(x, y - 1, z) +
                    noise(x, y + 1, z) +
                    noise(x + 1, y, z) +
                    noise(x + 1, y, z);
    faces = faces / 16;

    // Center of cube is counted 8 times
    float center = noise(x, y, z);
    center = center / 8;

    return corners + edges + faces + center;
}

// Cosine interpolation
float PerlinNoise::interpolate(float a, float b, float t){
    float ft = t * 3.14159f;
    float f = (1 - cos(ft)) * 0.5f;
    return a * (1 - f) + b * f;
}

float PerlinNoise::interpolatedNoise(float x, float y, float z){
    int int_x = (int) x;
    float frac_x = int_x - x;
    int int_y = (int) y;
    float frac_y = int_y - y;
    int int_z = (int) z;
    float frac_z = int_z - z;

    // Centers of the faces of the cube
    float v1 = smoothNoise(x, y, z);
    float v2 = smoothNoise(x + 1, y, z);
    float v3 = smoothNoise(x, y + 1, z);
    float v4 = smoothNoise(x + 1, y + 1, z);
    float v5 = smoothNoise(x, y, z + 1);
    float v6 = smoothNoise(x + 1, y, z + 1);
    float v7 = smoothNoise(x, y + 1, z + 1);
    float v8 = smoothNoise(x + 1, y + 1, z + 1);

    float i1 = interpolate(v1, v2, frac_x);
    float i2 = interpolate(v3, v4, frac_x);
    float i3 = interpolate(v5, v6, frac_x);
    float i4 = interpolate(v7, v8, frac_x);

    float i5 = interpolate(i1, i2, frac_y);
    float i6 = interpolate(i3, i4, frac_y);

    return interpolate(i5, i6, frac_z);
}

float PerlinNoise::perlin_noise2D(float x, float y){
    float total = 0;
    for (int i = 0; i < octaves; i++){
        float frequency = 1 << i; // 2^i
        float amplitude = pow(persistence, i);
        total += interpolatedNoise2D(x * frequency, y * frequency) * amplitude;
    }
    return total;
}

float PerlinNoise::perlin_noise(float x, float y, float z){
    float total = 0;
    for (int i = 0; i < octaves; i++){
        float frequency = 1 << i; // 2^i
        float amplitude = pow(persistence, i);
        total += interpolatedNoise(x * frequency, y * frequency, z * frequency) * amplitude;
    }
    return total;
}
