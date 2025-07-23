#ifndef MATH
#define MATH

#include "common.cpp"

template <typename T>
const T& Min(const T& a, const T& b) {
    return (a <= b) ? a : b;
}

template <typename T>
const T& Max(const T& a, const T& b) {
    return (a >= b) ? a : b;
}

float RoundToDecimals(float& value, int& decimals) {
    float multiplier = pow(10.0, decimals);
    return round(value * multiplier) / multiplier;
}

glm::vec3 Lerp(const glm::vec3& a, const glm::vec3& b, float t) {
    return a + t * (b - a);
}

glm::quat Slerp(const glm::quat& a, const glm::quat& b, float t) {
    return glm::slerp(a, b, t);
}
#endif