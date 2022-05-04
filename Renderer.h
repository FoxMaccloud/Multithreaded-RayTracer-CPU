#pragma once
#include <iostream>
#include <vector>

class Renderer
{
public:
	enum RenderState
	{
		Ready,
		Running,
		Stop
	};
	RenderState m_state = RenderState::Ready;

	Renderer();
	~Renderer();


	void set_image_size(unsigned int x, unsigned int y);
	void set_new_buffer(std::vector<uint32_t> &buffer) { m_imageBuffer = buffer; }
	void set_samples_per_pixel(unsigned int nSamples) { m_samplesPerPixel = nSamples; }
	void set_max_ray_bounces(unsigned int nBounces) { m_maxRayDepth = nBounces; }
	RenderState State() { return m_state; }

	void start();
	void stop();

private:

	// Copy of the buffer in Gui.
	std::vector<uint32_t> m_imageBuffer;

	unsigned int m_samplesPerPixel = 10;
	unsigned int m_maxRayDepth = 5;

	// Threads

	// TODO: Make Camera std::unique_ptr<Camera>;

	void write_pix_to_buffer();
};