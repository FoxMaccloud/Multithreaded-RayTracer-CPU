#pragma once
#include "glm/glm/glm.hpp"
#include <cmath>
#include <iostream>

constexpr float maxFloat = std::numeric_limits<float>::max();
constexpr float infinity = std::numeric_limits<float>::infinity();
constexpr float pi = 3.1415926535897932384626433832795f;

inline float DegToRad(float degrees) { return degrees * pi / 180.0f; }
inline float RadToDeg(float radians) { return radians * 180.0f / pi; }

template <typename T>
int Sign(T val) { return (T(0) < val) - (val < T(0)); }