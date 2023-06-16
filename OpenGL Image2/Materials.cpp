#include "Materials.hpp"


namespace Materials
{
	Metal::Metal(glm::vec3 ratio, float softness)
	{
		this->m_ratio = ratio;
		if (softness > 1.0f)
			softness = 1.0f;
		this->m_softness = softness;
	}

	std::optional<Scattering> Metal::Scatter(const Ray& rIn, HitRecord& rec) const
	{
		std::optional<Scattering> emptyResult{};
		if (glm::vec3 newDirection = glm::reflect(rIn.direction, rec.normal) + m_softness * glm::sphericalRand(1.0f); glm::dot(newDirection, rec.normal) > 0)
			return Scattering{ m_ratio, Ray{rec.p, newDirection} };
		return emptyResult;
	}
}
