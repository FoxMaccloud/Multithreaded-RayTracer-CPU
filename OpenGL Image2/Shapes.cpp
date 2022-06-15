#include "Shapes.h"
#include "Objects.h"

namespace Shapes {
	float Sphere::fast_hit(const Ray& r, const float tMin, const float tMax) const
	{
		const glm::vec3 oc = r.origin - m_center;
		const float b = glm::dot(oc, r.direction);
		const float disc = b * b - glm::dot(oc, oc) + (m_radius * m_radius);
		if (disc < 0)
		{
			return maxFloat;
		}
		float sqrtDisc = std::sqrt(disc);
		float root = -b - sqrtDisc;
		if (root < tMin || tMax < root)
		{
			root = -b + sqrtDisc;
			if (root < tMin || tMax < root)
			{
				return maxFloat;
			}
		}
		return root;
	}

	HitRecord Sphere::compute_hit(const Ray& r, const float t) const
	{
		HitRecord result{};
		result.t = t;
		result.p = r.at(result.t);
		glm::vec3 outNormal = glm::normalize(result.p - m_center);
		outNormal *= sign(m_radius);
		result.set_face_normal(r, outNormal);
		return result;
	}

	std::optional<HitRecord> Sphere::hit(const Ray& r, const float tMin, const float tMax) const
	{
		std::optional<HitRecord> emptyResult{};

		auto t = fast_hit(r, tMin, tMax);
		if (t < std::numeric_limits<float>::max())
			return compute_hit(r, t);
		else
			return emptyResult;
	}
}