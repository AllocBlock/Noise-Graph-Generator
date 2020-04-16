#include <iostream>
#include "include/noise_graph_generator.h"


using namespace std;

void runTests() {
    GrayImage* img;
    int width, height;
    int seed = (int)time(0);

    width = height = 512;

    
    // 白噪声，1x1
    cout << "生成 1x1 白噪声(White Noise) 图像...";
    img = &whiteNoise(width, height, 1, seed);
    savePPMP2(*img, "white_noise_1x1.ppm");
    cout << "\r生成 1x1 白噪声(White Noise) 图像完成" << endl;

    // 白噪声，16x16
    cout << "生成 16x16 白噪声(White Noise) 图像...";
    img = &whiteNoise(width, height, 16, seed);
    savePPMP2(*img, "white_noise_16x16.ppm");
    cout << "\r生成 16x16 白噪声(White Noise) 图像完成" << endl;

    // 连续噪声，16x16
    cout << "生成 16x16 值噪声(Value Noise) 图像...";
    img = &valueNoise(width, height, 16, seed);
    savePPMP2(*img, "value_noise_16x16.ppm");
    cout << "\r生成 16x16 值噪声(Value Noise) 图像完成" << endl;

    // 柏林噪声，16x16
    cout << "生成 16x16 柏林噪声(Perlin Noise) 图像...";
    img = &perlinNoise(width, height, 16, seed);
    savePPMP2(*img, "perlin_noise_16x16.ppm");
    cout << "\r生成 16x16 柏林噪声(Perlin Noise) 图像完成" << endl;

    // 柏林噪声，16x16+量化
    cout << "生成 64x64 柏林噪声(Perlin Noise)+量化 图像...";
    img = &perlinNoise(width, height, 64, seed);
    img->quantify(10);
    savePPMP2(*img, "perlin_noise_64x64_quantify_10.ppm");
    cout << "\r生成 64x64 柏林噪声(Perlin Noise)+量化 图像完成" << endl;
    
    // 值噪声，分形
    cout << "生成 分形+值噪声 图像...";
    img = new GrayImage(width, height);
    int startCellSize = 64;
    int octave = 5;
    float tempScale = 0.5;
    while (startCellSize > 0 && octave > 0) {
        GrayImage* t = &valueNoise(width, height, startCellSize, seed);
        t->mul(tempScale);
        img->add(*t);

        tempScale /= 2;
        startCellSize /= 2;
        octave--;
    }
    savePPMP2(*img, "fabric_value_noise.ppm");
    cout << "\r生成 分形+值噪声 图像完成" << endl;

    // 柏林噪声，分形
    cout << "生成 分形+柏林噪声 图像...";
    img = new GrayImage(width, height);
    startCellSize = 64;
    octave = 5;
    tempScale = 0.5;
    while (startCellSize > 0 && octave > 0) {
        GrayImage* t = &perlinNoise(width, height, startCellSize, seed);
        t->mul(tempScale);
        img->add(*t);

        tempScale /= 2;
        startCellSize /= 2;
        octave--;
    }
    savePPMP2(*img, "fabric_perlin_noise.ppm");
    cout << "\r生成 分形+柏林噪声 图像完成" << endl;
}

int main()
{
    runTests();
}
