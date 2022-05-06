#pragma once
#include "Objects.h"
#include "Math.h"
#include "glm/glm/gtc/random.hpp"
#include "glm/glm/gtx/epsilon.hpp"

struct Scattering
{
	glm::vec3 fading{ 0, 0, 0 };
	Ray ray{};
};

struct Hit
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
		friend bool operator==(Metal& lhs, Metal& rhs);

	public:
		Metal(glm::vec3 ratio, float softness);

		std::optional<Scattering> scatter(Ray& rIn, Hit& rec);


	private:
		glm::vec3 m_ratio;
		float m_softness{ 1.0f };
	};
	inline bool operator==(Metal& lhs, Metal& rhs)
	{
		return (lhs.m_ratio == rhs.m_ratio) && (lhs.m_softness && rhs.m_softness);
	}
}

using Material = std::variant<Materials::Metal>;
