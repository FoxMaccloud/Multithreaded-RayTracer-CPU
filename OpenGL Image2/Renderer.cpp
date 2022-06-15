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
	if (m_workThread.joinable())
		m_workThread.join();
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

void Renderer::render(uint32_t n_threads)
{
	auto startTime = std::chrono::system_clock::now();

	set_selected_scene(m_selectedScene);

	m_threadpool = std::make_unique<ThreadPool>(n_threads);
	
	auto renderLine = [this](const uint32_t lineCord)
	{
		for (uint32_t x = 0; x < m_viewPort.x; ++x)
		{
			if (m_state == RenderState::Stop)
				return;
			const auto pixelCord = glm::uvec2{ x, lineCord };
			glm::vec3 pixelColor{ 0,0,0 };
			for (uint32_t sample = 0; sample < m_samplesPerPixel; ++sample)
			{
				const auto u = (static_cast<float>(pixelCord.x)) / (m_viewPort.x - 1);
				const auto v = (static_cast<float>(pixelCord.y)) / (m_viewPort.y - 1);
				Ray r = m_camera->new_ray(u, v);
				pixelColor += shoot_ray(r, m_maxRayDepth);
			}
			write_pix_to_buffer(pixelCord, m_samplesPerPixel, pixelColor);
		}
	};

	auto renderQuad = [this](const glm::uvec2 minCoo, const glm::uvec2 maxCoo)
	{
		for (uint32_t j = maxCoo.y; j > minCoo.y; --j) {
			if (m_state == RenderState::Stop)
				return;
			for (uint32_t i = minCoo.x; i < maxCoo.x; ++i) {
				glm::vec3 pixelColor{ 0,0,0 };
				const auto pixelCord = glm::uvec2{ i, j - 1 };
				for (uint32_t sample = 0; sample < m_samplesPerPixel; ++sample) {
					const auto u = (static_cast<float>(pixelCord.x)) / (m_viewPort.x - 1);
					const auto v = (static_cast<float>(pixelCord.y)) / (m_viewPort.y - 1);
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
		for (int line = 0; line <= m_viewPort.y - 1; ++line)
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

	auto stopTime = std::chrono::system_clock::now();
	auto time = std::chrono::duration_cast<std::chrono::seconds>(stopTime - startTime);

	m_results->push_back(Results{ m_nThreads, m_samplesPerPixel, m_maxRayDepth, m_strat, m_selectedScene, time });

	m_scene.clear_objects();

	m_state = RenderState::Ready;
	m_threadpool.reset();
}

void Renderer::start()
{
	if (m_workThread.joinable())
	{
		switch (m_state)
		{
		case RenderState::Ready:
		case RenderState::Stop:
			m_workThread.join();
			break;
		default:
			break;
		}
	}
	m_state = RenderState::Running;

	m_workThread = std::thread{ &Renderer::render, this, m_nThreads };
}

void Renderer::stop()
{
	m_state = RenderState::Stop;
	//m_threadpool->emergency_stop();
	//m_threadpool.reset();
	//m_state = RenderState::Ready;
}

void Renderer::pause()
{
	m_pause = !m_pause;
	m_threadpool->pause(m_pause);
	if (m_pause)
		m_state = RenderState::Pause;
	else
		m_state = RenderState::Running;
}

glm::vec3 Renderer::shoot_ray(const Ray& ray, uint32_t depth)
{
	if (depth == 0)
		return glm::vec3(0,0,0);

	if (const auto& [HitRecord, Scattering] = m_scene.hit(ray, 0.001f, infinity); HitRecord)
	{
		const auto& [p, normal, t, front] = HitRecord.value();
		if (Scattering)
		{
			const auto& [fading, scattered] = Scattering.value();
			return fading * shoot_ray(scattered, depth - 1);
		}
		return glm::vec3{ 0,0,0 };
	}

	glm::vec3 white{ 1.0, 1.0, 1.0 };
	glm::vec3 azure{ 0.5, 0.7, 1.0 };

	float t = 0.5f * (ray.direction.y + 1.0f);
	return (1.0f - t) * white + t * azure;
}

void Renderer::write_pix_to_buffer(glm::uvec2 pixelCords, uint32_t samples, glm::vec3 pixelColor)
{
	pixelColor /= samples;
	pixelColor = glm::sqrt(pixelColor);
	pixelColor = glm::clamp(pixelColor, 0.0f, 1.0f);

	uint32_t index = (pixelCords.x + (pixelCords.y * m_viewPort.x));

	uint8_t w_color[4];

	w_color[0] = static_cast<uint8_t>(255 * pixelColor.x);
	w_color[1] = static_cast<uint8_t>(255 * pixelColor.y);
	w_color[2] = static_cast<uint8_t>(255 * pixelColor.z);
	w_color[3] = 0xFF;

	//w_color |= static_cast<uint8_t>(255 * 1) << 24;
	//w_color |= static_cast<uint8_t>(255 * pixelColor.x) << 16;
	//w_color |= static_cast<uint8_t>(255 * pixelColor.y) << 8;
	//w_color |= static_cast<uint8_t>(255 * pixelColor.z) << 0;

	(*m_imageBuffer)[index] = *(uint32_t*)(&w_color);
}