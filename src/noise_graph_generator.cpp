/**************************************************************************

* Author: AllocBlock

* Create At: 2020-04-14

* Update At: 2020-04-16

* Description: Provide noise graph generate functions

**************************************************************************/

#include "../include/noise_graph_generator.h"


bool savePPMP2(GrayImage img, const char path[]) {
    std::ofstream f;
    f.open(path, std::ios::out | std::ios::trunc);
    if (!f.is_open()) {
        return false;
    }
    else {
        // 写入PPM
        f << "P2" << std::endl; // 魔数，文件头
        f << img.width() << ' ' << img.height() << std::endl; // 图像宽和搞
        f << 255 << std::endl; // 最大的颜色值
        for (int j = 0; j < img.height(); j++) {
            for (int i = 0; i < img.width(); i++) {
                f << ((int)img.pixel(i, j) & 0x000000ff) << ' ';
            }
            f << std::endl;
        }
        return true;
    }
}

// fade/ease curve
float fade(float t) {
    // w(t)=6*t^5 - 15*t^4 + 10*t^3
    return t * t * t * (t * (t * 6 - 15) + 10);
}

float lerp(float a, float b, float u) {
    return a + u * (b - a);
}

float lerp2D(float lt, float lb, float rt, float rb, float u, float v) {
    float topLerp = lerp(lt, rt, u);
    float bottomLerp = lerp(lb, rb, u);
    float res = lerp(topLerp, bottomLerp, v);
    return res;
}

GrayImage whiteNoise(int width, int height, int cellSize, int seed) {
    GrayImage img(width, height);
    Random random(seed);

    for (int j = 0; j < height; j += cellSize) {
        for (int i = 0; i < width; i += cellSize) {
            byte randGray = random.randByte();
            img.fill(i, j, std::min(cellSize, width - i), std::min(cellSize, height - j), randGray);
        }
    }
    return img;
}

GrayImage valueNoise(int width, int height, int cellSize, int seed) {
    GrayImage img(width, height);
    Random random(seed);

    int rowPointCount = width / cellSize + 1, colPointCount = height  / cellSize + 1;

    byte* cellColor = new byte[rowPointCount * colPointCount];
    for (int i = 0; i < rowPointCount * colPointCount; i++) {
        cellColor[i] = random.randByte();
    }

    for (int j = 0; j < colPointCount - 1; j++) {
        for (int i = 0; i < rowPointCount - 1; i++) {
            for (int v = 0; v < cellSize && j * cellSize + v < height; v++) {
                for (int u = 0; u < cellSize && i * cellSize + u < width; u++) {
                    // 计算距离向量
                    vec2 pos = { ((float)u + 0.5f) / cellSize, ((float)v + 0.5f) / cellSize }; // 取像素中心

                    // period boundary
                    /*byte cLeftTop = cellColor[j * rowPointCount + i];
                    byte cLeftBottom = cellColor[((j + 1) % (colPointCount - 1)) * rowPointCount + i];
                    byte cRightTop = cellColor[j * rowPointCount + ((i+1) % (rowPointCount - 1))];
                    byte cRightBottom = cellColor[((j + 1) % (colPointCount - 1)) * rowPointCount + ((i + 1) % (rowPointCount - 1))];*/
                    
                    // additional point
                    byte cLeftTop = cellColor[j * rowPointCount + i];
                    byte cLeftBottom = cellColor[(j + 1) * rowPointCount + i];
                    byte cRightTop = cellColor[j * rowPointCount + (i + 1)];
                    byte cRightBottom = cellColor[(j + 1)  * rowPointCount + (i + 1)];

                    float res = lerp2D(cLeftTop, cLeftBottom, cRightTop, cRightBottom, fade(pos.x), fade(pos.y));
                    img.pixel(i * cellSize + u, j * cellSize + v, res);
                }
            }

        }
    }


    delete[]cellColor;
    return img;
}

GrayImage perlinNoise(int width, int height, int cellSize, int seed) {
    GrayImage img(width, height);
    Random random(seed);

    /* 确定固定点 */
    int rowPointCount = width / cellSize + 1; // +1，为了边缘部分的处理
    int colPointCount = height / cellSize + 1;
    /* 随机选择梯度 */
    vec2 gList[4] = {
        {1, 1},
        {1, -1},
        {-1, 1},
        {-1, -1},
    };
    
    int* gradient = new int[rowPointCount * colPointCount];
    for (int i = 0; i < rowPointCount * colPointCount; i++) {
        gradient[i] = random.randInt(4);
    }

    // 计算颜色
    for (int j = 0; j < colPointCount - 1; j++) {
        for (int i = 0; i < rowPointCount - 1; i++) {
             // 获取梯度
            vec2 gLeftTop = gList[gradient[j * rowPointCount + i]];
            vec2 gLeftBottom = gList[gradient[(j + 1) * rowPointCount + i]];
            vec2 gRightTop = gList[gradient[j * rowPointCount + (i + 1)]];
            vec2 gRightBottom = gList[gradient[(j + 1) * rowPointCount + (i + 1)]];

            for (int v = 0; v < cellSize && j * cellSize + v < width; v++) {
                for (int u = 0; u < cellSize && i * cellSize + u < height; u++) {
                    // 计算距离向量
                    vec2 pos = { ((float)u + 0.5f) / cellSize, ((float)v + 0.5f) / cellSize }; // 取像素中心

                    vec2 dLeftTop = pos;
                    vec2 dLeftBottom = pos - vec2(0.0, 1.0);
                    vec2 dRightTop = pos - vec2(1.0, 0.0);
                    vec2 dRightBottom = pos - vec2(1.0, 1.0);

                    // 计算影响值
                    float wLeftTop = vec2::dot(gLeftTop, dLeftTop);
                    float wLeftBottom = vec2::dot(gLeftBottom, dLeftBottom);
                    float wRightTop = vec2::dot(gRightTop, dRightTop);
                    float wRightBottom = vec2::dot(gRightBottom, dRightBottom);

                    // 插值
                    float lerpVal = lerp2D(wLeftTop, wLeftBottom, wRightTop, wRightBottom, fade(pos.x), fade(pos.y));

                    // 调整范围 [-1, 1] 到 [0, 1]
                    float res = (lerpVal + 1) / 2;

                    byte gray = (byte)((int)(res * 255) & 0x000000ff);
                    img.pixel(i * cellSize + u, j * cellSize + v, gray);
                }
            }

        }
    }

    delete[]gradient;
    return img;
}

GrayImage simplexNoise(int width, int height, int cellSize, int seed){
    GrayImage img(width, height);
    Random random(seed);

    /* 确定固定点 */
    int rowPointCount = width / cellSize + 1; // +1，为了边缘部分的处理
    int colPointCount = height / cellSize + 1;
    /* 随机选择梯度 */
    vec2 gList[4] = {
        {1, 1},
        {1, -1},
        {-1, 1},
        {-1, -1},
    };

    int* gradient = new int[rowPointCount * colPointCount];
    for (int i = 0; i < rowPointCount * colPointCount; i++) {
        gradient[i] = random.randInt(4);
    }

    // 计算颜色
    for (int j = 0; j < colPointCount - 1; j++) {
        for (int i = 0; i < rowPointCount - 1; i++) {
            // 获取梯度
            vec2 gLeftTop = gList[gradient[j * rowPointCount + i]];
            vec2 gLeftBottom = gList[gradient[(j + 1) * rowPointCount + i]];
            vec2 gRightTop = gList[gradient[j * rowPointCount + (i + 1)]];
            vec2 gRightBottom = gList[gradient[(j + 1) * rowPointCount + (i + 1)]];

            for (int v = 0; v < cellSize && j * cellSize + v < width; v++) {
                for (int u = 0; u < cellSize && i * cellSize + u < height; u++) {
                    // 计算距离向量
                    vec2 pos = { ((float)u + 0.5f) / cellSize, ((float)v + 0.5f) / cellSize }; // 取像素中心

                    vec2 dLeftTop = pos;
                    vec2 dLeftBottom = pos - vec2(0.0, 1.0);
                    vec2 dRightTop = pos - vec2(1.0, 0.0);
                    vec2 dRightBottom = pos - vec2(1.0, 1.0);

                    // 计算影响值
                    float wLeftTop = vec2::dot(gLeftTop, dLeftTop);
                    float wLeftBottom = vec2::dot(gLeftBottom, dLeftBottom);
                    float wRightTop = vec2::dot(gRightTop, dRightTop);
                    float wRightBottom = vec2::dot(gRightBottom, dRightBottom);

                    // 插值
                    float lerpVal = lerp2D(wLeftTop, wLeftBottom, wRightTop, wRightBottom, fade(pos.x), fade(pos.y));

                    // 调整范围 [-1, 1] 到 [0, 1]
                    float res = (lerpVal + 1) / 2;

                    byte gray = (byte)((int)(res * 255) & 0x000000ff);
                    img.pixel(i * cellSize + u, j * cellSize + v, gray);
                }
            }

        }
    }

    delete[]gradient;
    return img;
}