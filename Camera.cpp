#include "glm/glm/gtc/random.hpp"
#include "Camera.h"

Camera::Camera(CameraOrientation orientation, float fov, float aspectRatio, float aperture, float focusDist) {
	auto theta = deg_to_rad(fov);
	auto h = std::tan(theta / 2);
	m_viewportHeight = 2.0f * h;
	m_viewportWidth = aspectRatio * m_viewportHeight;
	m_lensRadius = aperture / 2.0f;

	w = glm::normalize(orientation.lookingFrom - orientation.lookingAt);
	u = glm::normalize(glm::cross(orientation.vUp, w));
	v = glm::cross(w, u);

	m_origin = orientation.lookingFrom;
	m_horizontal = focusDist * m_viewportWidth * u;
	m_vertical = focusDist * m_viewportHeight * v;
	m_lowerLeftCorner = m_origin - m_horizontal / 2.0f - m_vertical / 2.0f - focusDist * w;
}

Ray Camera::NewRay(float s, float t)
{
	glm::vec2 randVec = glm::diskRand(m_lensRadius);
	glm::vec3 offset = u * randVec.x + v * randVec.y;

	return Ray(m_origin + offset, m_lowerLeftCorner + s * m_horizontal + t * m_vertical - m_origin - offset);
}