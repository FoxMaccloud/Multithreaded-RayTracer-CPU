#pragma once
#include <iostream>
#include <vector>
#include "ImGui/imgui.h"

#include "ThreadPool.h"
#include "Camera.h"
#include "Objects.h"
#include <random>
#include <chrono>
#include <string>

class Renderer
{
public:
	enum class RenderState
	{
		Ready,
		Running,
		Pause,
		Stop
	};
	enum class Strategy
	{
		Line,
		Quad
	};
	enum class Scenes
	{
		threeBalls,
		test2,
		random
	};

	struct Results
	{
		uint32_t nThreads;
		uint32_t samples;
		uint32_t depth;
		Strategy strat;
		Scenes scene;
		std::chrono::seconds time;
	};

	struct Quad
	{
		Quad(glm::uvec2 min, glm::uvec2 max) : minCoo{ min }, maxCoo{ max }{};
		glm::uvec2 minCoo;
		glm::uvec2 maxCoo;
	};

	RenderState m_state = RenderState::Ready;
	Strategy m_strat = Strategy::Line;

	Renderer();
	~Renderer();

	void set_image_size(float x, float y);
	void set_new_buffer(std::vector<uint32_t>& buffer) { m_imageBuffer = &buffer; };
	void set_results_buffer(std::vector<Renderer::Results>& buffer) { m_results = &buffer; };
	void set_samples_per_pixel(uint32_t nSamples) { m_samplesPerPixel = nSamples; };
	void set_max_ray_bounces(uint32_t nBounces) { m_maxRayDepth = nBounces; };
	void set_nThreads(uint32_t nThreads) { m_nThreads = nThreads; };
	void set_scene(Scenes scene) { m_selectedScene = scene; };

	void render(uint32_t n_threads);
	void start();
	void stop();
	void pause();

private:
	ImVec2 m_viewPort{ 0,0 };

	// Copy of the buffer in Gui.
	std::vector<uint32_t>* m_imageBuffer;
	// Copy of results buffer
	std::vector<Renderer::Results>* m_results;

	uint32_t m_samplesPerPixel = 0;
	uint32_t m_maxRayDepth = 0;
	uint32_t m_nThreads = 0;

	bool m_pause = false;

	std::mt19937 m_rng{};
	std::uniform_real_distribution<float> m_unifDist{ 0.0f, 1.0f };
	std::thread m_workThread;
	std::unique_ptr<ThreadPool> m_threadpool;

	Scenes m_selectedScene;
	Objects m_scene;
	void set_selected_scene(Scenes scene);

	std::unique_ptr<Camera> m_camera;

	std::vector<Quad> split_image(uint32_t quadSize = 120); // Add quadSize to settings?

	glm::vec3 shoot_ray(const Ray& ray, uint32_t depth);
	void write_pix_to_buffer(glm::uvec2 pixelCords, uint32_t samples, glm::vec3 pixelColor);

	float aspect_ratio() const { return (static_cast<float>(m_viewPort.x) / static_cast<float>(m_viewPort.y)); };
};