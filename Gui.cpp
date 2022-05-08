#include "Gui.h"

#include "VertexBuffer.h"
#include "IndexBuffer.h"

Gui::Gui() : m_renderer{std::make_unique<Renderer>()}
{
	create_image_buffer();
	setup_main_window();
	m_renderer->set_image_size(m_windowSize.x, m_windowSize.y);
	m_renderer->set_new_buffer(m_imageBuffer);
}

Gui::~Gui()
{
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

	glfwDestroyWindow(m_window);
	glfwTerminate();
}

void Gui::resize_image(ImVec2 newSize)
{
	m_windowSize = newSize;
	create_image_buffer();
	m_renderer->set_image_size(m_windowSize.x, m_windowSize.y);
	m_renderer->set_new_buffer(m_imageBuffer);
}

void Gui::create_image_buffer()
{
	m_imageBuffer.clear();
	m_imageBuffer.resize(m_windowSize.x * m_windowSize.y, 0);
}

void Gui::setup_main_window()
{
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;

	// These values gets overwritten by GLFW_MAXIMIZED. This is just to fall back on...
	this->m_windowSize = { 1280, 720 };

	glfwInit();

	GLFWmonitor* monitor = glfwGetPrimaryMonitor();
	int monitorWidth = (int)(glfwGetVideoMode(monitor)->width);
	int monitorHeight = (int)(glfwGetVideoMode(monitor)->height);

	glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
	glfwWindowHint(GLFW_MAXIMIZED, GLFW_TRUE);
	//glfwWindowHint(GLFW_DECORATED, GLFW_FALSE);

	m_window = glfwCreateWindow(m_windowSize.x, m_windowSize.y, WINDOW_TITLE, NULL, NULL);

	if (!m_window)
		glfwTerminate();

	/* Make the window's context current */
	glfwMakeContextCurrent(m_window);
	glfwSwapInterval(1); // Enable vsync

	if (gl3wInit() != 0)
		throw std::runtime_error("ERROR: Failed to initialize OpenGL loader!");

	theme();

	ImGui_ImplGlfw_InitForOpenGL(m_window, true);
	ImGui_ImplOpenGL3_Init("#version 140");
}

void Gui::enable_window_resize()
{
	// Enable when not rendering
}

void Gui::disable_window_resize()
{
	// Disable when rendering
}

void Gui::run()
{
	// Create a image from the pixeldata
	std::vector<uint32_t> test;
	test.resize(m_windowSize.x * m_windowSize.y, 0);
	for (uint32_t i = 0; i < m_windowSize.x * m_windowSize.y; i++)
	{
		test[i] = 0xFFAC8919; // ABGR
	}

	ImTextureID render_img = generate_texture(m_windowSize, test);

	bool done = false;
	while (!done)
	{
		if (m_windowResize)
		{
			// Recreate shader/buffer/etc.
			m_windowResize = false;
		}

		if (glfwWindowShouldClose(m_window))
		{
			done = true;
		}

		glfwPollEvents();
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();


		{
			ImGui::Begin("Settings", nullptr, ImGuiWindowFlags_NoCollapse);
			ImGui::Text("Hi!");
			ImGui::End();
			// Put renderer::start on a new thread.
		}
		{
			ImGui::Begin("##", nullptr, ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoScrollbar);
			if (render_img)
			{
				ImGui::Image((void*)(intptr_t)render_img, m_windowSize);
			}
			ImGui::End();
		}

		// Handle States for renderer; Ready/Running/Stop/Pause
		// Disable resize if running

		ImGui::Render();
		int display_w, display_h;
		glfwGetFramebufferSize(m_window, &display_w, &display_h);
		glViewport(0, 0, display_w, display_h);
		glClear(GL_COLOR_BUFFER_BIT);
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
		glfwSwapBuffers(m_window);
	}
}

void Gui::theme()
{
	ImGuiStyle* style = &ImGui::GetStyle();

	style->ChildBorderSize = 1;
	style->WindowTitleAlign = ImVec2(0.5, 0.5);
	style->AntiAliasedFill = true;

	style->WindowRounding = 0;
	style->ChildRounding = 0;
	style->FrameRounding = 0;
	style->ScrollbarRounding = 0;
	style->TabRounding = 0;
	style->GrabRounding = 0;
	style->ScrollbarSize = 1;

	style->Colors[ImGuiCol_Text] = ImColor(255, 255, 255, 255);

	style->Colors[ImGuiCol_TitleBg] = ImColor(25, 137, 172, 255);
	style->Colors[ImGuiCol_TitleBgActive] = ImColor(25, 137, 172, 255);
	style->Colors[ImGuiCol_TitleBgCollapsed] = ImColor(25, 137, 172, 255);

	style->Colors[ImGuiCol_WindowBg] = ImColor(15, 15, 15, 230);
	style->Colors[ImGuiCol_CheckMark] = ImColor(255, 255, 255, 255);
	style->Colors[ImGuiCol_ChildBg] = ImColor(20, 20, 20, 255);

	style->Colors[ImGuiCol_FrameBg] = ImColor(25, 25, 25, 230);
	style->Colors[ImGuiCol_FrameBgActive] = ImColor(25, 25, 25, 230);
	style->Colors[ImGuiCol_FrameBgHovered] = ImColor(25, 25, 25, 230);

	style->Colors[ImGuiCol_Header] = ImColor(35, 35, 35, 230);
	style->Colors[ImGuiCol_HeaderActive] = ImColor(25, 137, 172, 255);
	style->Colors[ImGuiCol_HeaderHovered] = ImColor(25, 137, 172, 230);

	style->Colors[ImGuiCol_ResizeGrip] = ImColor(35, 35, 35, 255);
	style->Colors[ImGuiCol_ResizeGripActive] = ImColor(35, 35, 35, 255);
	style->Colors[ImGuiCol_ResizeGripHovered] = ImColor(35, 35, 35, 255);

	style->Colors[ImGuiCol_Button] = ImColor(37, 37, 37, 255);
	style->Colors[ImGuiCol_ButtonActive] = ImColor(41, 41, 41, 255);
	style->Colors[ImGuiCol_ButtonHovered] = ImColor(25, 137, 172, 255);

	style->Colors[ImGuiCol_Tab] = ImColor(136, 0, 20, 140);
	style->Colors[ImGuiCol_TabHovered] = ImColor(136, 0, 20, 160);
	style->Colors[ImGuiCol_TabActive] = ImColor(136, 0, 20, 220);

	style->Colors[ImGuiCol_SliderGrab] = ImColor(255, 255, 255, 255);
	style->Colors[ImGuiCol_SliderGrabActive] = ImColor(255, 255, 255, 255);
}
//(25, 137, 172, 255)