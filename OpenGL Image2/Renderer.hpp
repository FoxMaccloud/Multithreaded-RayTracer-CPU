#pragma once
#include <iostream>
#include <vector>
#include "ImGui/imgui.h"

#include "ThreadPool.hpp"
#include "Camera.hpp"
#include "Objects.hpp"
#include <random>
#include <chrono>
#include <string>
#include <utility>
#include <map>

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
		ThreeBalls,
		Test2,
		Random
	};

	struct AvailableScenes
	{
		std::string name;
		Scenes scene;
	};
	std::vector<AvailableScenes> m_availableScenes = {
	  AvailableScenes{ "Three Balls", Scenes::ThreeBalls },
	  AvailableScenes{ "Test2", Scenes::Test2 },
	  AvailableScenes{ "Random", Scenes::Random },
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

	void SetImageSize(float x, float y);
	void SetNewBuffer(std::vector<uint32_t>& buffer) { m_imageBuffer = &buffer; };
	void SetResultsBuffer(std::vector<Renderer::Results>& buffer) { m_results = &buffer; };
	void SetSamplesPerPixel(uint32_t nSamples) { m_samplesPerPixel = nSamples; };
	void SetMaxRayBounces(uint32_t nBounces) { m_maxRayDepth = nBounces; };
	void SetNumThreads(uint32_t nThreads) { m_nThreads = nThreads; };
	void SetScene(Scenes scene) { m_selectedScene = scene; };

	void Render(uint32_t n_threads);
	void Start();
	void Stop();
	void Pause();

private:
	ImVec2 m_viewPort{ 0,0 };

	// Copy of the buffer in Gui.
	std::vector<uint32_t>* m_imageBuffer = nullptr;
	// Copy of results buffer
	std::vector<Renderer::Results>* m_results = nullptr;

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
	void SetSelectedScene(Scenes scene);

	std::unique_ptr<Camera> m_camera;

	std::vector<Quad> SplitImage(uint32_t quadSize = 120); // Add quadSize to settings?

	glm::vec3 ShootRay(const Ray& ray, uint32_t depth);
	void WritePixelToBuffer(glm::uvec2 pixelCords, uint32_t samples, glm::vec3 pixelColor);

	float AspectRatio() const { return (static_cast<float>(m_viewPort.x) / static_cast<float>(m_viewPort.y)); };
};