#pragma once
#include <optional>
#include <variant>
#include "Math.h"
#include "glm/glm/gtc/random.hpp"
#include "glm/glm/gtx/epsilon.hpp"
#include "Ray.h"

struct Scattering
{
	glm::vec3 fading{ 0, 0, 0 };
	Ray ray{};
};

struct HitRecord
{
	float t;
	glm::vec3 p;
	glm::vec3 normal;
	bool front;

	void set_face_normal(const Ray& r, const glm::vec3& outwardNormal)
	{
		front = glm::dot(r.direction, outwardNormal) < 0;
		normal = front ? outwardNormal : -outwardNormal;
	}
};

namespace Materials
{
	class Metal
	{
		friend bool operator==(const Metal& lhs, const Metal& rhs);

	public:
		Metal(glm::vec3 ratio, float softness);

		[[nodiscard]] std::optional<Scattering> scatter(const Ray& rIn, HitRecord& rec) const;


	private:
		glm::vec3 m_ratio;
		float m_softness{ 1.0f };
	};

	inline bool operator==(const Metal& lhs, const Metal& rhs)
	{
		return (lhs.m_ratio == rhs.m_ratio) && (lhs.m_softness && rhs.m_softness);
	}
}

using Material = std::variant<Materials::Metal>;
