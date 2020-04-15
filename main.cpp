#include <iostream>
#include "include/noise_graph_generator.h"

using namespace std;

void runTests() {
    GrayImage* img;
    int width, height;

    width = height = 512;
    
    // 白噪声，1x1
    img = &whiteNoise(width, height, 1);
    savePPMP2(*img, "white_noise_1x1.ppm");

    // 白噪声，16x16
    img = &whiteNoise(width, height, 16);
    savePPMP2(*img, "white_noise_16x16.ppm");
    
    // 连续噪声，16x16
    img = &valueNoise(width, height, 16);
    savePPMP2(*img, "value_noise_16x16.ppm");

    // 柏林噪声，16x16
    img = &perlinNoise(width, height, 16);
    savePPMP2(*img, "perlin_noise_16x16.ppm");

    // 分形
    img = new GrayImage(width, height);
    int startCellSize = 256;
    int iteration = 5;
    float tempScale = 0.5;
    while (startCellSize > 0 && iteration > 0) {
        GrayImage* t = &perlinNoise(width, height, startCellSize);
        t->mul(tempScale);
        img->add(*t);

        tempScale /= 2;
        startCellSize /= 2;
        iteration--;
    }
    savePPMP2(*img, "fabric_perlin_noise.ppm");

}

int main()
{
    runTests();
}
