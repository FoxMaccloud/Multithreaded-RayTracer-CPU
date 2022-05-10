#include "Renderer.h"
#include "Objects.h"

// Brass color; (0.7, 0.5, 0.2)
void Renderer::set_selected_scene(Scenes scene)
{
	switch (scene)
	{
	case Renderer::Scenes::test1:
	{
		Camera::ViewMatrix viewmatrix{ glm::vec3(13, 2, 3), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0) };

		constexpr auto dist_to_focus = 10.0f;
		constexpr auto aperture = 0.1f;

		m_camera = std::make_unique<Camera>(viewmatrix, 20.0f, aspect_ratio(), aperture, dist_to_focus);

		m_scene.add(Shapes::Sphere(glm::vec3(0, 0, -2), 1.0f), Materials::Metal(Colors::Steel, 0.0f));
		m_scene.add(Shapes::Sphere(glm::vec3(0, 0, 0), 0.5f), Materials::Metal(Colors::Copper, 0.0f));
		m_scene.add(Shapes::Sphere(glm::vec3(0, 0, 2), 1.0f), Materials::Metal(Colors::Steel, 0.0f));
		//m_scene.add(Shapes::Sphere(glm::vec3(0, 1000, 0), 1000.0f), Materials::Metal(Colors::Steel, 0.0f));
	}
		break;
	case Renderer::Scenes::test2:
		throw new std::runtime_error("test2 has not been implemented yet!");
		break;
	default:
		break;
	}
}