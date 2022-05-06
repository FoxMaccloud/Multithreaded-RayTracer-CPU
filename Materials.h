#pragma once
#include "Objects.h"
#include "Math.h"
#include "glm/glm/gtc/random.hpp"

using Material = std::variant<Materials::Metal>;

namespace Materials
{
	class Metal
	{
		friend bool operator==(Metal& lhs, Metal& rhs);

	public:
		Metal(glm::vec3 ratio, float softness);

		std::optional<Scattering> Scatter(Ray& rIn, Hit& rec);

	private:
		glm::vec3 m_ratio;
		float m_softness{ 1.0f };
	};
	inline bool operator==(Metal& lhs, Metal& rhs)
	{
		return (lhs.m_ratio == rhs.m_ratio) && (lhs.m_softness && rhs.m_softness);
	}
}
