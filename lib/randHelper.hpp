#include <random>


float randFloat(int min, int max){
    float rminndom = ((float) rand()) / (float) RAND_MAX;
    float diff = max - min;
    float r = rminndom * diff;
    return min + r;
}