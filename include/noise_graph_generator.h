/**************************************************************************

* Author: AllocBlock

* Create At: 2020-04-14

* Update At: 2020-04-16

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



//class Random {
//private:
//    int seed;
//
//public:
//    Random() {
//        seed = (int)time(0);
//        srand(seed);
//    }
//
//    Random(int seed) {
//        this->seed = seed;
//        srand(seed);
//    }
//
//    byte randByte() {
//        return (byte)(rand() % 255);
//    }
//
//    float randFloat() {
//        return (float)rand() / RAND_MAX;
//    }
//
//    int randInt(int range) {
//        return rand() % range;
//    }
//};


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

    static float dot(vec2 a, vec2 b) {
        return a.x * b.x + a.y * b.y;
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
        // �����ϵ����£���0��ʼ��
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
                byte res = (byte)(newVal & 0x000000ff);
                pixel(i, j, res);
            }
        }
    }

    void add(byte v) {
        for (int j = 0; j < h; j++) {
            for (int i = 0; i < w; i++) {
                int newVal = min(((int)pixel(i, j) + (int)v), 255);
                byte res = (byte)(newVal & 0x000000ff);
                pixel(i, j, res);
            }
        }
    }

    void mul(float v) {
        for (int j = 0; j < h; j++) {
            for (int i = 0; i < w; i++) {
                int newVal = min((int)(pixel(i, j) * v), 255);
                byte res = (byte)(newVal & 0x000000ff);
                pixel(i, j, res);
            }
        }
    }

    void quantify(int unit) {
        for (int j = 0; j < h; j++) {
            for (int i = 0; i < w; i++) {
                int level = (int)((float)pixel(i, j) / 255 * unit);
                int newVal = ((float)level / unit) * 255;
                byte res = (byte)(newVal & 0x000000ff);
                pixel(i, j, res);
            }
        }
    }
};


/*
 * ���ܣ�����ͼ��Ϊppm p2��ʽ��ascii grayscale)
 * ������
 *      img��ͼ����
 *      path�������·��
 * ���أ�bool���ɹ�Ϊtrue������false
*/
bool savePPMP2(GrayImage img, const char path[]);

GrayImage whiteNoise(int width, int height, int cellSize, int seed);
GrayImage valueNoise(int width, int height, int cellSize, int seed);
GrayImage perlinNoise(int width, int height, int cellSize, int seed);
GrayImage simplexNoise(int width, int height, int cellSize, int seed);