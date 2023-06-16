#pragma once
#include "Ray.hpp"
#include "Math.hpp"

class Camera
{
public:
	struct ViewMatrix
	{
		glm::vec3 lookingFrom;
		glm::vec3 lookingAt;
		glm::vec3 vUp;
	};

	Camera(ViewMatrix viewmatrix, float fov, float aspectRatio, float aperture, float focusDist);
	//~Camera();

	Ray NewRay(float s, float t) const;

private:
	float m_viewportHeight{ 2.0f };
	float m_viewportWidth{ 0.0f };
	float m_lensRadius{ 0.5f };

	glm::vec3 m_origin{ 0, 0, 0 };
	glm::vec3 m_horizontal;
	glm::vec3 m_vertical;
	glm::vec3 u, w, v;
	glm::vec3 m_lowerLeftCorner;
};
