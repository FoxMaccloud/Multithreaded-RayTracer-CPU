#include "Renderer.h"
#include "Objects.h"

void Renderer::set_selected_scene(Scenes scene)
{
	switch (scene)
	{
	case Renderer::Scenes::threeBalls:
	{
		Camera::ViewMatrix viewmatrix{ glm::vec3(13, 2, 3), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0) };

		constexpr auto distToFocus = 10.0f;
		constexpr auto aperture = 0.1f;

		m_camera = std::make_unique<Camera>(viewmatrix, 20.0f, aspect_ratio(), aperture, distToFocus);

		m_scene.add(Shapes::Sphere(glm::vec3(1, 0, 2), 1.4f), Materials::Metal(Colors::Steel, 0.0f));
		m_scene.add(Shapes::Sphere(glm::vec3(1, 1, -1.7), 1.0f), Materials::Metal(Colors::Brass, 0.0f));
		m_scene.add(Shapes::Sphere(glm::vec3(-1, -1, -1.3), 0.5f), Materials::Metal(Colors::Gold, 0.0f));
	}
		break;
	case Renderer::Scenes::test2:
		throw new std::runtime_error("test2 has not been implemented yet!");
		break;
	case Renderer::Scenes::random:
	{
		Camera::ViewMatrix viewmatrix{ glm::vec3(13, 2, 3), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0) };

		constexpr auto distToFocus = 10.0f;
		constexpr auto aperture = 0.1f;

		m_camera = std::make_unique<Camera>(viewmatrix, 20.0f, aspect_ratio(), aperture, distToFocus);

		for (int j = -5; j < 5; j++)
		{
			for (int i = -5; i < 5; i++)
			{
				glm::vec3 place(i + 0.9 * m_unifDist(m_rng), i * 2 * m_unifDist(m_rng), j + 0.9 * m_unifDist(m_rng));
				glm::vec3 rngCol{ m_unifDist(m_rng), m_unifDist(m_rng), m_unifDist(m_rng) };
				
				auto size = 0.5f * m_unifDist(m_rng);
				if (size < 0.1f)
					size = 0.1f;
				//auto soft = 0.1f * m_unifDist(m_rng);
				auto soft = 0.0f;
				m_scene.add(Shapes::Sphere(place, size), Materials::Metal(rngCol, soft));
			}
		}
	}
		break;
	default:
		break;
	}
}