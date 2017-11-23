#ifndef PERLIN_H
#define PERLIN_H
// Permutation of array of values from 0 to 255
// Repeated twice to avoid overflow
int p[512] = {
    151,160,137,91,90,15,131,13,201,95,96,53,194,233,7,225,140,36,103,30,69,142,
    8,99,37,240,21,10,23,190,6,148,247,120,234,75,0,26,197,62,94,252,219,203,
    117,35,11,32,57,177,33,88,237,149,56,87,174,20,125,136,171,168,68,175,74,
    165,71,134,139,48,27,166,77,146,158,231,83,111,229,122,60,211,133,230,220,
    105,92,41,55,46,245,40,244,102,143,54,65,25,63,161,1,216,80,73,209,76,132,
    187,208,89,18,169,200,196,135,130,116,188,159,86,164,100,109,198,173,186,3,
    64,52,217,226,250,124,123,5,202,38,147,118,126,255,82,85,212,207,206,59,227,
    47,16,58,17,182,189,28,42,223,183,170,213,119,248,152,2,44,154,163,70,221,
    153,101,155,167,43,172,9,129,22,39,253,19,98,108,110,79,113,224,232,178,185,
    112,104,218,246,97,228,251,34,242,193,238,210,144,12,191,179,162,241,81,51,
    145,235,249,14,239,107,49,192,214,31,181,199,106,157,184,84,204,176,115,121,
    50,45,127,4,150,254,138,236,205,93,222,114,67,29,24,72,243,141,128,195,78,
    66,215,61,156,180
    ,
    151,160,137,91,90,15,131,13,201,95,96,53,194,233,7,225,140,36,103,30,69,142,
    8,99,37,240,21,10,23,190,6,148,247,120,234,75,0,26,197,62,94,252,219,203,
    117,35,11,32,57,177,33,88,237,149,56,87,174,20,125,136,171,168,68,175,74,
    165,71,134,139,48,27,166,77,146,158,231,83,111,229,122,60,211,133,230,220,
    105,92,41,55,46,245,40,244,102,143,54,65,25,63,161,1,216,80,73,209,76,132,
    187,208,89,18,169,200,196,135,130,116,188,159,86,164,100,109,198,173,186,3,
    64,52,217,226,250,124,123,5,202,38,147,118,126,255,82,85,212,207,206,59,227,
    47,16,58,17,182,189,28,42,223,183,170,213,119,248,152,2,44,154,163,70,221,
    153,101,155,167,43,172,9,129,22,39,253,19,98,108,110,79,113,224,232,178,185,
    112,104,218,246,97,228,251,34,242,193,238,210,144,12,191,179,162,241,81,51,
    145,235,249,14,239,107,49,192,214,31,181,199,106,157,184,84,204,176,115,121,
    50,45,127,4,150,254,138,236,205,93,222,114,67,29,24,72,243,141,128,195,78,
    66,215,61,156,180
};

float lerp(float v0, float v1, float w) {
    return v0 * w + v1 * (1 - w);
}

int hash(int x, int y, int z) {
    return p[p[p[x] + y] + z];
}

float grad(int hash, float x, float y, float z) {
    /*
        // Original by Ken Perlin
        int h = hash & 0xF;
        float u = h < 8 ? x : y;
        float v = h < 4 ? y : (h == 12 || h == 14 ? x : z);
        return (h & 1 ? -u : u) + (h & 2 ? -v : v);
    */
    // Human Readable Version
    switch(hash & 0xF) {
        case 0x0: return  x + y;
        case 0x1: return -x + y;
        case 0x2: return  x - y;
        case 0x3: return -x - y;
        case 0x4: return  x + z;
        case 0x5: return -x + z;
        case 0x6: return  x - z;
        case 0x7: return -x - z;
        case 0x8: return  y + z;
        case 0x9: return -y + z;
        case 0xa: return  y - z;
        case 0xb: return -y - z;
        case 0xc: return  y + x;
        case 0xd: return -y + z;
        case 0xe: return  y - x;
        case 0xf: return -y - z;
        default:  return 0;
    }
}

float fade(float t) {
    // 6t^5 - 15t^4 + 10t^3
    return t * t * t * (t * (t * 6 - 15) + 10);
}

float perlin(float x, float y, float z) {
    // Get grid points. Bind to [0, 255] so that it can access the p array
    // This means it will wrap every 256, but subsampling is fine
    int xi = ((int)x) & 255;
    int yi = ((int)y) & 255;
    int zi = ((int)z) & 255;

    // Get location inside of unit cube
    x -= (int)x;
    y -= (int)y;
    z -= (int)z;

    // Fading is basically a smoothing curve
    float u = fade(x);
    float v = fade(y);
    float w = fade(z);

    // Linearly interpolate the dot product of the gradient and the difference
    // between the location and each of the 8 unit cube locations
    float x1 = lerp(grad(hash(    xi,     yi, zi), x,     y,     z),
                    grad(hash(1 + xi,     yi, zi), x - 1, y,     z), 1-u);
    float x2 = lerp(grad(hash(    xi, 1 + yi, zi), x    , y - 1, z),
                    grad(hash(1 + xi, 1 + yi, zi), x - 1, y - 1, z), 1-u);
    float y1 = lerp(x1, x2, 1-v);
    x1 = lerp(grad(hash(    xi,     yi, 1 + zi), x,     y,     z - 1),
              grad(hash(1 + xi,     yi, 1 + zi), x - 1, y,     z - 1), 1-u);
    x2 = lerp(grad(hash(    xi, 1 + yi, 1 + zi), x    , y - 1, z - 1),
              grad(hash(1 + xi, 1 + yi, 1 + zi), x - 1, y - 1, z - 1), 1-u);
    float y2 = lerp(x1, x2, 1-v);
    // Linearly interpolate in the z direction + normalize [-1,1] -> [0,1]
    return (lerp(y1, y2, 1-w) + 1.0f) / 2.0f;
}

float octave_perlin(float x, float y, float z, int octaves, float persistence) {
    float total = 0;
    float frequency = 1;
    float amplitude = 1;
    float maxValue = 0;  // Used for normalizing result to 0.0 - 1.0
    for(int i=0;i<octaves;i++) {
        total += perlin(x * frequency, y * frequency, z * frequency) * amplitude;
        maxValue += amplitude;
        amplitude *= persistence;
        frequency *= 2;
    }
    return total/maxValue;
}

#endif
