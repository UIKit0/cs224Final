#ifndef PERLINNOISE_H
#define PERLINNOISE_H


/**
  * This class is no longer used. I didn't realize glm had noise functions!
  */

#include <math.h>

class PerlinNoise
{
public:
    PerlinNoise(float p, int o);

    float perlin_noise(float x, float y, float z);
    float perlin_noise2D(float x, float y);

private:
    float noise(int x1, int x2, int x3);
    float smoothNoise(int x, int y, int z);
    float interpolatedNoise(float x, float y, float z);
    float interpolate(float a, float b, float t);

    float noise2D(int x1, int x2);
    float smoothNoise2D(int x, int y);
    float interpolatedNoise2D(float x, float y);

    float persistence;
    int octaves;
};

#endif // PERLINNOISE_H
