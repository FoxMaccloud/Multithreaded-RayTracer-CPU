#include "Renderer.h"


Renderer::Renderer()
{
	m_state = RenderState::Ready;
}

Renderer::~Renderer()
{
	// Stop jobs && Sync threads
	switch (m_state)
	{
	case Renderer::RenderState::Ready:
		break;
	case Renderer::RenderState::Running:
		m_state = RenderState::Stop;
		m_threadpool->emergency_stop();
		break;
	case Renderer::RenderState::Stop:
		m_threadpool->emergency_stop();
		break;
	default:
		break;
	}
}

void Renderer::set_image_size(float x, float y)
{
	m_viewPort = { x, y };
}

std::vector<Renderer::Quad> Renderer::split_image(uint32_t quadSize)
{
	glm::uvec2 u_viewPort{ m_viewPort.x, m_viewPort.y };
	std::vector<Quad> result;
	uint32_t numX = static_cast<uint32_t>(std::ceil(m_viewPort.x / static_cast<float>(quadSize)));
	uint32_t numY = static_cast<uint32_t>(std::ceil(m_viewPort.y / static_cast<float>(quadSize)));
	for (uint32_t y = numY; y > 0; --y)
	{
		for (uint32_t x = 0; x < numX; ++x)
		{
			result.emplace_back(glm::uvec2{ x * quadSize, (y - 1) * quadSize }, glm::min(glm::uvec2{ (x + 1) * quadSize, y * quadSize }, u_viewPort));
		}
	}
	return result;
}

void Renderer::start(uint32_t n_threads)
{
	m_state = RenderState::Running;

	set_selected_scene(m_selectedScene);

	m_threadpool = std::make_unique<ThreadPool>(n_threads);
	
	auto renderLine = [this](const uint32_t lineCord)
	{
		if (m_state == RenderState::Stop)
			return;

		for (uint32_t x = 0; x < m_viewPort.x; ++x)
		{
			const auto pixelCord = glm::uvec2{ x, lineCord };
			glm::vec3 pixelColor{ 0,0,0 };
			for (uint32_t sample = 0; sample < m_samplesPerPixel; ++sample)
			{
				const auto u = (static_cast<float>(pixelCord.x) + m_unifDist(m_rng)) / (m_viewPort.x - 1);
				const auto v = (static_cast<float>(pixelCord.y) + m_unifDist(m_rng)) / (m_viewPort.y - 1);
				Ray r = m_camera->new_ray(u, v);
				pixelColor += shoot_ray(r, m_maxRayDepth);
			}
			write_pix_to_buffer(pixelCord, m_samplesPerPixel, pixelColor);
		}
	};

	auto renderQuad = [this](const glm::uvec2 minCoo, const glm::uvec2 maxCoo)
	{
		if (m_state == RenderState::Stop)
			return;

		for (uint32_t j = maxCoo.y; j > minCoo.y; --j) {
			for (uint32_t i = minCoo.x; i < maxCoo.x; ++i) {
				glm::vec3 pixelColor{ 0,0,0 };
				const auto pixelCord = glm::uvec2{ i, j - 1 };
				for (uint32_t sample = 0; sample < m_samplesPerPixel; ++sample) {
					const auto u = (static_cast<float>(pixelCord.x) + m_unifDist(m_rng)) / (m_viewPort.x - 1);
					const auto v = (static_cast<float>(pixelCord.y) + m_unifDist(m_rng)) / (m_viewPort.y - 1);
					Ray r = m_camera->new_ray(u, v);
					pixelColor += shoot_ray(r, m_maxRayDepth);
				}
				write_pix_to_buffer(pixelCord, m_samplesPerPixel, pixelColor);
			}
		}
	};

	std::vector<std::future<void>> futures;

	switch (m_strat)
	{
	case Strategy::Line:
		// Render per-line
		for (int line = m_viewPort.y - 1; line >= 0; --line)
			futures.push_back(m_threadpool->add_task(renderLine, line));
		break;
	case Strategy::Quad:
		// Render per-quad
		for (const auto& [minCoo, maxCoo] : split_image())
			futures.push_back(m_threadpool->add_task(renderQuad, minCoo, maxCoo));
		break;
	default:
		throw std::runtime_error("ERROR: No strategy selected!");
		break;
	}
	std::for_each(begin(futures), end(futures), [](auto& future) { future.wait(); });

	m_threadpool.reset();
	m_state = RenderState::Ready;
}

void Renderer::stop()
{
}

glm::vec3 Renderer::shoot_ray(const Ray& ray, uint32_t depth)
{
	if (depth == 0)
		return glm::vec3(0,0,0);

	if (const auto& [hitRecord, scatterResult] = m_scene.hit(ray, 0.001f, infinity); hitRecord)
	{
		const auto& [p, normal, t, front_face] = hitRecord.value();
		if (scatterResult)
		{
			const auto& [attenuation, scattered] = scatterResult.value();
			return attenuation * shoot_ray(scattered, depth - 1);
		}
		return glm::vec3{ 0,0,0 };
	}

	glm::vec3 white{ 1.0, 1.0, 1.0 };
	glm::vec3 azure{ 0.5, 0.7, 1.0 };

	float t = 0.5f * (ray.direction.y + 1.0f);
	return (1.0f - t) * white + t * azure;
	return glm::vec3(0, 0, 0);
}

void Renderer::write_pix_to_buffer(glm::uvec2 pixelCords, uint32_t samples, glm::vec3 pixelColor)
{
	pixelColor /= samples;
	pixelColor = glm::sqrt(pixelColor);
	pixelColor = glm::clamp(pixelColor, 0.0f, 1.0f);

	uint32_t index = (pixelCords.x + (pixelCords.y * m_viewPort.x));

	uint32_t w_color = 0xFF000000;

	w_color |= static_cast<uint32_t>(255 * pixelColor.x) << 16;
	w_color |= static_cast<uint32_t>(255 * pixelColor.y) << 8;
	w_color |= static_cast<uint32_t>(255 * pixelColor.z) << 0;

	m_imageBuffer[index] = w_color;
}