#include "Renderer.h"
#include "Objects.h"

void Renderer::set_selected_scene(Scenes scene)
{
	switch (scene)
	{
	case Renderer::Scenes::test1:
	{
		Camera::CameraOrientation viewMatrix{ glm::vec3(13, 2, 3), glm::vec3(0,0,0), glm::vec3(0,1,0) };

		float distanceFocus = 10.0f;
		float aperture = 0.1f;

		m_camera = std::make_unique<Camera>(viewMatrix, 20.0f, aspect_ratio(), aperture, distanceFocus);

		m_scene.add(Shapes::Sphere(glm::vec3(0, -1000, 0), 3000.0f), Materials::Metal(glm::vec3(0.7, 0.6, 0.5), 0.0f));
		m_scene.add(Shapes::Sphere(glm::vec3(0, 1, 0), 1.0f), Materials::Metal(glm::vec3(0.5, 0.8, 0.3), 0.0f));
		m_scene.add(Shapes::Sphere(glm::vec3(4, 1, 0), 1.0f), Materials::Metal(glm::vec3(0.7, 0.6, 0.5), 0.0f));
	}
		break;
	case Renderer::Scenes::test2:
		throw new std::runtime_error("test2 has not been implemented yet!");
		break;
	default:
		break;
	}
}