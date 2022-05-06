#pragma once
#include <iostream>
#include <vector>
#include "ImGui/imgui.h"

#include "ThreadPool.h"
#include "Camera.h"
#include "Scene.cpp"

class Renderer
{
public:
	enum RenderState
	{
		Ready,
		Running,
		Stop
	};
	enum Strategy
	{
		Line,
		Quad
	};

	RenderState m_state = RenderState::Ready;
	Strategy m_strat = Strategy::Line;

	Renderer();
	~Renderer();


	void set_image_size(float x, float y);
	void set_new_buffer(std::vector<uint32_t> &buffer) { m_imageBuffer = buffer; }
	void set_samples_per_pixel(uint32_t nSamples) { m_samplesPerPixel = nSamples; }
	void set_max_ray_bounces(uint32_t nBounces) { m_maxRayDepth = nBounces; }
	//RenderState state() { return m_state; } Already public...
	void set_scene();

	void start(uint32_t n_threads);
	void stop();

private:
	ImVec2 m_viewPort{ 0,0 };

	// Copy of the buffer in Gui.
	std::vector<uint32_t> m_imageBuffer;

	uint32_t m_samplesPerPixel = 10;
	uint32_t m_maxRayDepth = 5;
	uint32_t m_nThreads;


	// Threads
	// TP + Main T
	std::unique_ptr<ThreadPool> threadpool;

	Objects m_scene;
	std::unique_ptr<Camera> m_camera;

	glm::vec3 shoot_ray(Ray& ray, uint32_t depth);
	void write_pix_to_buffer(glm::uvec2 pixelCords, uint32_t samples, glm::vec3 pixelColor);

	float aspect_ratio() { return (static_cast<float>(m_viewPort.x) / static_cast<float>(m_viewPort.y)); };
};