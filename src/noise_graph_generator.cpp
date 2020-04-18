/**************************************************************************

* Author: AllocBlock

* Create At: 2020-04-14

* Update At: 2020-04-18

* Description: Provide noise graph generate functions

**************************************************************************/

#include "../include/noise_graph_generator.h"

byte intToByte(int v) {
    return (byte)(v & 0x000000ff);
}

bool savePPMP2(GrayImage img, const char path[]) {
    std::ofstream f;
    f.open(path, std::ios::out | std::ios::trunc);
    if (!f.is_open()) {
        return false;
    }
    else {
        // д��PPM
        f << "P2" << std::endl; // ħ�����ļ�ͷ
        f << img.width() << ' ' << img.height() << std::endl; // ͼ���͸�
        f << 255 << std::endl; // ������ɫֵ
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
                    // �����������
                    vec2 pos = { ((float)u + 0.5f) / cellSize, ((float)v + 0.5f) / cellSize }; // ȡ��������

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

    /* ȷ���̶��� */
    int rowPointCount = width / cellSize + 1; // +1��Ϊ�˱�Ե���ֵĴ���
    int colPointCount = height / cellSize + 1;
    /* ���ѡ���ݶ� */
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

    // ������ɫ
    for (int j = 0; j < colPointCount - 1; j++) {
        for (int i = 0; i < rowPointCount - 1; i++) {
             // ��ȡ�ݶ�
            vec2 gLeftTop = gList[gradient[j * rowPointCount + i]];
            vec2 gLeftBottom = gList[gradient[(j + 1) * rowPointCount + i]];
            vec2 gRightTop = gList[gradient[j * rowPointCount + (i + 1)]];
            vec2 gRightBottom = gList[gradient[(j + 1) * rowPointCount + (i + 1)]];

            for (int v = 0; v < cellSize && j * cellSize + v < width; v++) {
                for (int u = 0; u < cellSize && i * cellSize + u < height; u++) {
                    // �����������
                    vec2 pos = { ((float)u + 0.5f) / cellSize, ((float)v + 0.5f) / cellSize }; // ȡ��������

                    vec2 dLeftTop = pos;
                    vec2 dLeftBottom = pos - vec2(0.0, 1.0);
                    vec2 dRightTop = pos - vec2(1.0, 0.0);
                    vec2 dRightBottom = pos - vec2(1.0, 1.0);

                    // ����Ӱ��ֵ
                    float wLeftTop = vec2::dot(gLeftTop, dLeftTop);
                    float wLeftBottom = vec2::dot(gLeftBottom, dLeftBottom);
                    float wRightTop = vec2::dot(gRightTop, dRightTop);
                    float wRightBottom = vec2::dot(gRightBottom, dRightBottom);

                    // ��ֵ
                    float lerpVal = lerp2D(wLeftTop, wLeftBottom, wRightTop, wRightBottom, fade(pos.x), fade(pos.y));

                    // ������Χ [-1, 1] �� [0, 1]
                    float res = (lerpVal + 1) / 2;

                    byte gray = intToByte((int)(res * 255));
                    img.pixel(i * cellSize + u, j * cellSize + v, gray);
                }
            }

        }
    }

    delete[]gradient;
    return img;
}

GrayImage simplexNoise(int width, int height, int cellSize, int seed){
    // ע�⣺simplex�������ڳ��õİ汾�¸����Լ�������޸ĵģ����ܴ��ڴ���
    // Warning: this method is modified based on a general version. I implement this under my understanding, so it may have some errors in it
    GrayImage img(width, height);
    Random random(seed);
    
    // ȷ���̶���
    int rowPointCount = width / cellSize + 1; // +1��Ϊ�˱�Ե���ֵĴ���
    int colPointCount = height / cellSize + 1;
    // ���ѡ���ݶ�
    vec2 gList[4] = {
        {1, 1},
        {1, -1},
        {-1, 1},
        {-1, -1},
    };

    int gradCount = rowPointCount * colPointCount;
    int* gradient = new int[gradCount];
    for (int i = 0; i < gradCount; i++) {
        gradient[i] = random.randInt(4);
    } 


    // ������ɫ
    for (int j = 0; j < height; j++) {
        for (int i = 0; i < width; i++) {
            // б������任
            // ֱ������ӳ�䵽��������
            // ����ȽϺ���
            // x' = x*cos(15��) - y*sin(15��)
            // y' = y*cos(15��) - x*sin(15��)
            // �����������Ƴ��������굽ֱ������
            // x + y = ��2(x'+y')
            // x - y = ��6/3*(x'+y')
            // �ɵ� x = 0.5 * (��2 + ��6/3)x' + (��2 - ��6/3)y'
            // y = 0.5 * (��2 - ��6/3)x' + (��2 + ��6/3)y'
            // ȫ���ˡ�6/2��
            // x = (��3 + 1)/2*x' + (��3 - 1)/2*y'
            // y = (��3 - 1)/2*x' + (��3 + 1)/2*y'
            // �� s = (x' + y') * (��3 - 1) / 2, x = x' + s, y = y' + s
            // ???

            
            // ���οռ���ĵ� �� �����������ĵ�
            float c1 = 0.5 * (sqrt(3.0) + 1.0), c2 = 0.5 * (sqrt(3.0) - 1.0);
            int nX = c1 * i + c2 * j, nY = c1 * j + c2 * i;

            // �����ĺ���ԭ��
            int cellOriginSkewX = (nX / cellSize) * cellSize, cellOriginSkewY = (nY / cellSize) * cellSize; 
            
            // �����ĺ���ԭ�� �� ���οռ��ﳬ�����壨���Σ��Ķ���
            float G2 = (3.0 - sqrt(3.0)) / 6.0;
            float t = (float)(cellOriginSkewX + cellOriginSkewY) * G2;
            float cellOriginX = cellOriginSkewX - t; // Unskew the cell origin back to (x,y) space
            float cellOriginY = cellOriginSkewY - t;

            // ��õ㵽��������ԭ��(0,0)����Ծ���
            // ע�⣬�������߳���1��ɡ�3���Խ��ߣ��͡�3/��2�����ӱ߳�)��������Ҫ����???
            float x0 = (i - cellOriginX) / cellSize; // The x,y distances from the cell origin
            float y0 = (j - cellOriginY) / cellSize;

            // ��õ㵽�ڶ����������Ծ���
            // ����ȷ���˵����ĸ�����(cell)�����һ����������������(simplex)�������������ͬ
            float x1, y1;
            if (x0 > y0) { // �·��ĵ���(1,0)
                x1 = x0 - 1.0 + G2;
                y1 = y0 + G2;
            } 
            else { // �Ϸ��ĵ���(0,1)
                x1 = x0 + G2;
                y1 = y0 - 1.0 + G2;
            }

            // ��õ㵽�������������Ծ���
            float x2 = x0 - 1.0 + 2.0 * G2;
            float y2 = y0 - 1.0 + 2.0 * G2;

            // ����ݶ�
            int unitSeedX = cellOriginSkewX / cellSize * (long long)8204195 % gradCount;  // �˴���������ƽ���ֲ�
            int unitSeedY = cellOriginSkewY / cellSize * (long long)5798415 % gradCount;
            
            int gi0 = gradient[(unitSeedX + unitSeedY) % gradCount];
            int gi1;
            if (x0 > y0) { // �·��ĵ���
                gi1 = gradient[(unitSeedX + 8204195 + unitSeedY) % gradCount];
            }
            else { // �Ϸ��ĵ���
                gi1 = gradient[(unitSeedX + unitSeedY + 5798415) % gradCount];
            }
            int gi2 = gradient[(unitSeedX + 8204195 + 5798415 + unitSeedY) % gradCount];
            
            // ������������Ĺ���ֵ
            float n0, n1, n2; // ����ֵ

            float t0 = 0.5 - x0 * x0 - y0 * y0; // �뾶��2/2
            if (t0 < 0) n0 = 0.0; // �뾶���⣬�޹���
            else { // �뾶���ڣ�����˥��+����ͶӰ�õ�����ֵ
                float alpha = pow(t0, 4);
                n0 = alpha * vec2::dot(gList[gi0], { x0, y0 });
            }
            float t1 = 0.5 - x1 * x1 - y1 * y1;
            if (t1 < 0) n1 = 0.0;
            else {
                float alpha = pow(t1, 4);
                n1 = alpha * vec2::dot(gList[gi1], { x1, y1 });
            }
            float t2 = 0.5 - x2 * x2 - y2 * y2;
            if (t2 < 0) n2 = 0.0;
            else {
                float alpha = pow(t2, 4);
                n2 = alpha * vec2::dot(gList[gi2], { x2, y2 });
            }
            // ����ֵ�����ֵ��1/70���ң���Ҫ�Ŵ��÷�Χӳ�䵽��[-1, 1]�������ӳ�䵽[0, 255]
            byte res = intToByte((int)((((n0 + n1 + n2) * 70 + 1) / 2 * 255)));

            img.pixel(i, j, res);

        }
    }

    delete[]gradient;
    return img;
    
}