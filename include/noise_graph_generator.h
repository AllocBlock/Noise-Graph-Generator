/**************************************************************************

* Author: AllocBlock

* Create At: 2020-04-14

* Update At: 2020-04-15

* Description: Provide noise graph generate functions

**************************************************************************/
#include <cstdlib>
#include <ctime>
#include <fstream>
#include <algorithm>


/* debug */
#include <iostream>
using namespace std;
/* debug end */



typedef unsigned char byte;

class Random {
private:
    int seed;

public:
    Random() {
        seed = (int)time(0);
        srand(seed);
    }

    Random(int seed) {
        this->seed = seed;
        srand(seed);
    }

    char randByte() {
        return (byte)(rand() % 255);
    }

    float randFloat() {
        return (float)rand() / RAND_MAX;
    }

    int randInt(int range) {
        return rand() % range;
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
    }

    int width() {
        return w;
    }

    int height() {
        return h;
    }

    char pixel(int x, int y) {
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

    void fill(int x, int y, int width, int height, char val) {
        if (x < 0 || x + width >  w || y < 0 || y + height > h) {
            throw("Image - fill - index out of range");
        }

        for (int i = 0; i < height; i++) {
            for (int j = 0; j < width; j++) {
                int index = i * w + j + y * w + x;
                data[index] = val;
            }
        }
    }

};

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

/*
 * ���ܣ�����ͼ��Ϊppm p2��ʽ��ascii grayscale)
 * ������
 *      img��ͼ����
 *      path�������·��
 * ���أ�bool���ɹ�Ϊtrue������false
*/
bool savePPMP2(GrayImage img, const char path[]);

GrayImage whiteNoise(int width, int height, int cellSize);
GrayImage valueNoise(int width, int height, int cellSize);
GrayImage perlinNoise(int width, int height, int cellSize);