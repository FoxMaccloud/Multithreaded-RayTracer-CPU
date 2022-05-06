#include "Renderer.h"


Renderer::Renderer()
{
}

Renderer::~Renderer()
{
	// Stop jobs && Sync threads
}

void Renderer::set_image_size(float x, float y)
{
	m_viewPort = { x, y };
}

void Renderer::start(uint32_t n_threads)
{
	//m_state = RenderState::Running;

	//std::make_unique<ThreadPool>(n_threads);
	//
	//auto renderLine = [this](uint32_t lineCoord)
	//{
	//	if (m_state == RenderState::Stop)
	//		return;

	//	uint32_t lineCord;

	//	for (uint32_t x = 0; x < m_viewPort.x; ++x)
	//	{
	//		auto pixelCord = glm::uvec2{ x, lineCord };
	//		glm::vec3 pixelColor{ 0,0,0 };
	//		for (uint32_t sample = 0; sample < m_samplesPerPixel; ++sample)
	//		{
	//			//auto u = (static_cast<float>(pixelCord.x) + m_unifDistribution(m_rnGenerator)) / (m_viewPort.x - 1);
	//			//auto v = (static_cast<float>(pixelCord.y) + m_unifDistribution(m_rnGenerator)) / (m_viewPort.y - 1);
	//			auto u = (static_cast<float>(pixelCord.x)) / (m_viewPort.x - 1);
	//			auto v = (static_cast<float>(pixelCord.y)) / (m_viewPort.y - 1);
	//			Ray r = m_camera->NewRay(u, v);
	//			pixelColor += shoot_ray(r, m_maxRayDepth);
	//		}
	//		write_pix_to_buffer(pixelCord, m_samplesPerPixel, pixelColor);
	//	}
	//};

	//auto renderQuad = [this](const glm::uvec2 minCoo, const glm::uvec2 maxCoo)
	//{
	//	if (m_state == RenderState::Stop)
	//		return;

	//	for (uint32_t j = maxCoo.y; j > minCoo.y; --j) {
	//		for (uint32_t i = minCoo.x; i < maxCoo.x; ++i) {
	//			glm::vec3 pixelColor{ 0,0,0 };
	//			auto pixelCord = glm::uvec2{ i, j - 1 };
	//			for (uint32_t sample = 0; sample < m_samplesPerPixel; ++sample) {
	//				auto u = (static_cast<float>(pixelCord.x)) / (m_viewPort.x - 1);
	//				auto v = (static_cast<float>(pixelCord.y)) / (m_viewPort.y - 1);
	//				Ray r = m_camera->NewRay(u, v);
	//				pixelColor += shoot_ray(r, m_maxRayDepth);
	//			}
	//			write_pix_to_buffer(pixelCord, m_samplesPerPixel, pixelColor);
	//		}
	//	}
	//};

	//std::vector<std::future<void>> futures;

	//switch (m_strat)
	//{
	//case Renderer::Line:
	//	// Render per-line
	//	for (int line = m_viewPort.y - 1; line >= 0; --line)
	//		futures.push_back(threadpool->add_task(renderLine, line));
	//	break;
	//case Renderer::Quad:
	//	// Render per-quad
	//	//for (const auto& [minCoo, maxCoo] : SplitImage()) {
	//	//	futures.push_back(m_threadPool.AddTask(renderQuad, minCoo, maxCoo));
	//	//}
	//	break;
	//default:
	//	throw std::runtime_error("ERROR: No strategy selected!");
	//	break;
	//}
	//std::for_each(begin(futures), end(futures), [](auto& future) { future.wait(); });

	//threadpool.reset();
	//m_state = RenderState::Ready;
}

void Renderer::stop()
{
}

glm::vec3 Renderer::shoot_ray(Ray& ray, uint32_t depth)
{
	//if (depth == 0)
	//	return glm::vec3(0,0,0);

	//if (auto& [hitRecord, scatterResult] = m_scene.hit(ray, 0.001f, infinity); hitRecord)
	//{
	//	auto& [p, normal, t, front_face] = hitRecord.value();
	//	if (scatterResult)
	//	{
	//		const auto& [attenuation, scattered] = scatterResult.value();
	//		return attenuation * shoot_ray(scattered, depth - 1);
	//	}
	//	return glm::vec3{ 0,0,0 };
	//}

	//glm::vec3 white{ 1.0, 1.0, 1.0 };
	//glm::vec3 azure{ 0.5, 0.7, 1.0 };

	//float t = 0.5f * (ray.direction.y + 1.0f);
	//return (1.0f - t) * white + t * azure;
	return glm::vec3(0, 0, 0);
}

void Renderer::write_pix_to_buffer(glm::uvec2 pixelCords, uint32_t samples, glm::vec3 pixelColor)
{
	pixelColor /= samples;
	pixelColor = glm::sqrt(pixelColor);
	pixelColor = glm::clamp(pixelColor, 0.0f, 1.0f);

	uint32_t index = (pixelCords.x + (pixelCords.y * m_viewPort.x));

	uint32_t w_color = 0x000000FF;

	w_color |= static_cast<uint32_t>(255 * pixelColor.x) << 8;
	w_color |= static_cast<uint32_t>(255 * pixelColor.y) << 16;
	w_color |= static_cast<uint32_t>(255 *pixelColor.z) << 24;
}