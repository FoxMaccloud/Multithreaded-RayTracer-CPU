#pragma once
#include <variant>
#include <optional>
#include "Math.hpp"
#include "Materials.hpp"

namespace Shapes {

	class Sphere
	{
	public:
		Sphere() = default;
		Sphere(glm::vec3 center, float radius) : m_center{ center }, m_radius{ radius }{};
		//~Sphere();

		float FastHit(const Ray& r, const float tMin, const float tMax) const;
		HitRecord ComputeHit(const Ray& r, const float t) const;
		std::optional<HitRecord> Hit(const Ray& r, const float tMin, const float tMax) const;

	private:
		glm::vec3 m_center{ 0,0,0 };
		float m_radius;
	};
}

using Shape = std::variant<Shapes::Sphere>;