#pragma once
#include "Math.hpp"

class Ray
{
public:
	constexpr Ray() = default;
	Ray(const glm::vec3 origin, const glm::vec3 direction) : origin{ origin }, direction{ glm::normalize(direction) } {}
	//~Ray();

	glm::vec3 origin{ 0, 0, 0 };
	glm::vec3 direction{ 0, 0, 0 };

	[[nodiscard]] glm::vec3 At(const float t) const { return origin + direction * t; }
};