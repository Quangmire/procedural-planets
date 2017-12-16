#ifndef PERLIN_HPP
#define PERLIN_HPP

extern int p[512];
float lerp(float v0, float v1, float w);
int hash(int x, int y, int z);
float grad(int hash, float x, float y, float z);
float fade(float t);
float perlin(float x, float y, float z);
float octave_perlin(float x, float y, float z, int octaves, float persistence);

#endif
