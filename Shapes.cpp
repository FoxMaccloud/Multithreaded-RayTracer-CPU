#include "Shapes.h"
namespace Shapes {
	float Sphere::fast_hit(const Ray& r, const float tMin, const float tMax) const
	{
		glm::vec3 oc = r.origin - m_center;
		float b = glm::dot(oc, r.direction);
		float disc = b * b - glm::dot(oc, oc) + (m_radius * m_radius);
		if (disc < 0)
		{
			return std::numeric_limits<float>::max();
		}
		float sqrtDisc = std::sqrt(disc);
		float root = -b + sqrtDisc;
		if (root < tMin || tMax < root)
		{
			root = -b + sqrtDisc;
			if (root < tMin || tMax < root)
			{
				return std::numeric_limits<float>::max();
			}
		}
		return root;
	}

	Hit Sphere::compute_hit(const Ray& r, const float t) const
	{
		Hit result{};
		result.t = t;
		result.p = r.at(result.t);
		glm::vec3 outNormal = glm::normalize(result.p - m_center);
		outNormal *= sign(m_radius);
		result.set_face_normal(r, outNormal);
		return result;
	}

	std::optional<Hit> Sphere::hit(const Ray& r, const float tMin, const float tMax) const
	{
		std::optional<Hit> emptyResult{};

		auto t = fast_hit(r, tMin, tMax);
		if (t < std::numeric_limits<float>::max())
			return compute_hit(r, t);
		else
			return emptyResult;
	}
}