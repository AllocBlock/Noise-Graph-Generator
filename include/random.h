/**************************************************************************

* Author: AllocBlock

* Create At: 2020-04-16

* Update At: 2020-04-16

* Description: Generate random number

**************************************************************************/

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

    byte randByte() {
        return (byte)(rand() % 255);
    }

    float randFloat() {
        return (float)rand() / RAND_MAX;
    }

    int randInt(int range) {
        return rand() % range;
    }
};