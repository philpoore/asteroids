#pragma once

#include <cmath>
#include <stdint.h>

inline float deg2rad(float deg) {
    return deg * 0.0174533;
}

inline int random(int n) {
    return rand() % n;
}

inline float randomf(float n) {
    return ((float)rand() / INT32_MAX) * n;
}