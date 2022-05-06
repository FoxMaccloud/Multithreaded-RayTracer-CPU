#pragma once

#include <iostream>
#include <vector>
#include "ImGui/imgui.h"
#include "ImGui/imgui_internal.h"
#include "ImGui/imgui_impl_glfw.h"
#include "ImGui/imgui_impl_opengl3.h"
#include <GL/gl3w.h>
#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>

#define WINDOW_TITLE "temp window title"

#include "Renderer.h"
#include "Image.h"

class Gui
{
public:
	Gui();
	~Gui();

	void run();
	void theme();
	GLFWwindow* get_window() { return m_window; };

private:
	GLFWwindow* m_window;
	ImVec2 m_windowSize;
	bool create = true;

	// Image Buffer to present the image
	std::vector<uint32_t> m_imageBuffer;

	// Our raytracer doing work
	std::unique_ptr<Renderer> m_renderer;

	void resize_image(ImVec2 newSize);
	void create_image_buffer();
	void setup_main_window();

	bool m_windowResize = false;
	void enable_window_resize();
	void disable_window_resize();
};
