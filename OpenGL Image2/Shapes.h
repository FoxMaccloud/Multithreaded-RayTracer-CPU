#pragma once
#include <variant>
#include <optional>
#include "Math.h"
#include "Materials.h"

namespace Shapes {

	class Sphere
	{
	public:
		Sphere() = default;
		Sphere(glm::vec3 center, float radius) : m_center{ center }, m_radius{ radius }{};
		//~Sphere();

		float fast_hit(const Ray& r, const float tMin, const float tMax) const;
		HitRecord compute_hit(const Ray& r, const float t) const;
		std::optional<HitRecord> hit(const Ray& r, const float tMin, const float tMax) const;

	private:
		glm::vec3 m_center{ 0,0,0 };
		float m_radius;
	};
}

using Shape = std::variant<Shapes::Sphere>;