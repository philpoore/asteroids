#pragma once
#include <cmath>

inline float deg2rad(float deg) {
    return deg * 0.0174533;
}

inline int random(int n) {
    return rand() % n;
}