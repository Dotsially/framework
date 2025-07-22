#ifndef MATH
#define MATH

#include "common.cpp"

float Min(const float& a, const float& b) {
    return (a <= b) ? a : b;
}

float Max(const float& a, const float& b) {
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