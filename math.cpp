#ifndef MATH
#define MATH

#include "common.cpp"

float Min(float a, float b){
    if (a <= b) return a;
    return b;
}

float RoundToDecimals(float value, int decimals) {
    float multiplier = pow(10.0, decimals);
    return round(value * multiplier) / multiplier;
}

#endif