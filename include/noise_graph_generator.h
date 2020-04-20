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
#include <complex>

#include "random.h"
#define PI 3.14159265358979

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

    complex<float> omega(int k, int n) {
        float base = (float)k / n * 2 * PI;
        return complex<float>(cos(base), sin(base));
    }

    void fftRecur(complex<float>* v, int n) {
        if (n == 1) {
            return;
        }
        // 切换顺序
        int m = n / 2;
        complex<float>* buf = new complex<float>[n];
        for (int i = 0; i < m; i++) {
            buf[i] = v[2 * i];
            buf[i + m] = v[2 * i + 1];
        }
        // 递归计算
        fftRecur(buf, m);
        fftRecur(buf + m, n - m);
        // 并归
        for (int k = 0; k < n / 2; k++) {
            complex<float> A1k = buf[k];
            complex<float> A2k = buf[k + m];
            buf[k] = A1k + omega(k, n) * A2k;
            buf[k + m] = A1k - omega(k, n) * A2k;
        }
        // 填入
        for (int i = 0; i < n; i++) {
            v[i] = buf[i];
        }
    }

    GrayImage spectrum() {
        complex<float>* result = new complex<float>[w * h];

        for (int j = 0; j < h; j++) { // 先对每行
            // 填入
            complex<float>* v = new complex<float>[w];
            for (int i = 0; i < w; i++) {
                v[i] = complex<float>((float)pixel(i, j) / 255, 0.0f); // [0,1]
            }
            // 计算
            fftRecur(v, w);
            // 保存结果
            for (int i = 0; i < w; i++) {
                result[j * w + i] = v[i];
            }
        }
        for (int i = 0; i < w; i++) { // 再对每列
            // 填入
            complex<float>* v = new complex<float>[h];
            for (int j = 0; j < h; j++) {
                v[j] = result[j * w + i];
            }
            // 计算
            fftRecur(v, h);
            
            // 保存结果
            for (int j = 0; j < h; j++) {
                result[j * w + i]  = v[j];
            }
        }
        
        // 计算长度，并映射到[0,1]
        float* resultLen = new float[w * h];
        float maxFreqValue = 0;
        for (int i = 0; i < w*h; i++) {
            float len = sqrt(result[i].real() * result[i].real() + result[i].imag() * result[i].imag());
            len = log(len) + 1; // 0Hz频率会很高，是因为取值为[0,1]，为减小差距取log，图像更直观
            maxFreqValue = max(maxFreqValue, len);
            resultLen[i] = max(0.0f, len);
        }
        for (int i = 0; i < w * h; i++) {
            resultLen[i] /= maxFreqValue; // 缩放
        }
        // 填入
        GrayImage resImg(w, h);
        for (int j = 0; j < h; j++) {
            for (int i = 0; i < w; i++) {
                byte color = intToByte((int)(resultLen[j * w + i] * 255));
                resImg.pixel((i + w/2) % w, (j + h / 2) % h, color); // 向右下方循环平移半张图片
            }
        }
        return resImg;
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
