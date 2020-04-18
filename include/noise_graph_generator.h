/**************************************************************************

* Author: AllocBlock

* Create At: 2020-04-14

* Update At: 2020-04-18

* Description: Provide noise graph generate functions

**************************************************************************/
#include <cstdlib>
#include <ctime>
#include <fstream>
#include <algorithm>

#include "random.h"

/* debug */
#include <iostream>
using namespace std;
/* debug end */

class vec2 {
public:
    float x, y;
    vec2() {
        x = y = 0;
    }
    vec2(float x, float y) {
        this->x = x;
        this->y = y;
    }

    float len() {
        return sqrt(x * x + y * y);
    }

    static float distance(vec2 a, vec2 b) {
        return (a - b).len();
    }

    static float dot(vec2 a, vec2 b) {
        return a.x * b.x + a.y * b.y;
    }

    static vec2 normalize(vec2 v) {
        float length = v.len();
        return v / length;
    }

    vec2 operator/(float val) {
        return { this->x / val, this->y / val };
    }

    vec2 operator+(vec2 v) {
        return { this->x + v.x, this->y + v.y };
    }

    vec2 operator-(vec2 v) {
        return { this->x - v.x, this->y - v.y };
    }

};


byte intToByte(int v);

class GrayImage {
private:
    byte* data;
    int w, h;

public:
    GrayImage(int w, int h) {
        this->w = w;
        this->h = h;
        data = new byte[w * h];
        memset(data, 0, sizeof(byte) * w * h);
    }

    int width() {
        return w;
    }

    int height() {
        return h;
    }

    byte pixel(int x, int y) {
        // 从左上到右下，从0开始数
        if (x < 0 || x >= w || y < 0 || y >= h) {
            throw("Image - getPixel - index out of range");
        }
        int index = y * w + x;
        return data[index];
    }

    void pixel(int x, int y, byte val) {
        if (x < 0 || x >= w || y < 0 || y >= h) {
            throw("Image - setPixel - index out of range");
        }
        int index = y * w + x;
        data[index] = val;
    }

    void fill(int x, int y, int width, int height, byte val) {
        if (x < 0 || x + width >  w || y < 0 || y + height > h) {
            throw("Image - fill - index out of range");
        }

        for (int j = 0; j < height; j++) {
            for (int i = 0; i < width; i++) {
                pixel(i + x, j + y, val);
            }
        }
    }

    void add(GrayImage v) {
        if (w != v.w || h != v.h) {
            throw("GrayImage - add - width or height doesn't match");
        }
        for (int j = 0; j < h; j++) {
            for (int i = 0; i < w; i++) {
                int newVal = min(((int)pixel(i, j) + (int)v.pixel(i, j)), 255);
                byte res = intToByte(newVal);
                pixel(i, j, res);
            }
        }
    }

    void add(byte v) {
        for (int j = 0; j < h; j++) {
            for (int i = 0; i < w; i++) {
                int newVal = min(((int)pixel(i, j) + (int)v), 255);
                byte res = intToByte(newVal);
                pixel(i, j, res);
            }
        }
    }

    void mul(GrayImage v) {
        if (w != v.w || h != v.h) {
            throw("GrayImage - add - width or height doesn't match");
        }
        for (int j = 0; j < h; j++) {
            for (int i = 0; i < w; i++) {
                int newVal = min((int)(pixel(i, j) * (int)v.pixel(i, j)), 255);
                byte res = intToByte(newVal);
                pixel(i, j, res);
            }
        }
    }

    void mul(float v) {
        for (int j = 0; j < h; j++) {
            for (int i = 0; i < w; i++) {
                int newVal = min((int)(pixel(i, j) * v), 255);
                byte res = intToByte(newVal);
                pixel(i, j, res);
            }
        }
    }

    void quantify(int unit) {
        for (int j = 0; j < h; j++) {
            for (int i = 0; i < w; i++) {
                int level = (int)((float)pixel(i, j) / 255 * unit);
                int newVal = ((float)level / unit) * 255;
                byte res = intToByte(newVal);
                pixel(i, j, res);
            }
        }
    }
};



/*
 * 功能：保存图像为ppm p2格式（ascii grayscale)
 * 参数：
 *      img：图像类
 *      path：保存的路径
 * 返回：bool，成功为true，否则false
*/
bool savePPMP2(GrayImage img, const char path[]);

GrayImage whiteNoise(int width, int height, int cellSize, int seed);
GrayImage valueNoise(int width, int height, int cellSize, int seed);
GrayImage perlinNoise(int width, int height, int cellSize, int seed);
GrayImage simplexNoise(int width, int height, int cellSize, int seed);
GrayImage worleyNoise(int width, int height, int cellSize, int seed);
