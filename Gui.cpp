#include "Gui.h"

#include "Image.h"

Gui::Gui() : m_renderer{std::make_unique<Renderer>()}
{
	create_image_buffer();
	setup_main_window();
	m_renderer->set_image_size(m_windowSize.x, m_windowSize.y);
	m_renderer->set_new_buffer(m_imageBuffer);
}

Gui::~Gui()
{
	m_renderingThread.join();

	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

	glfwDestroyWindow(m_window);
	glfwTerminate();
}

void Gui::resize_image(ImVec2 newSize)
{
	m_windowSize = newSize;

	{
		m_topBar.x = m_windowSize.x;
		m_topBar.y = m_topbarHeight;

		m_settingsWindow.x = m_windowSize.x / m_settingsWidthScale;
		m_settingsWindow.y = m_windowSize.y - m_topbarHeight;

		m_viewport.x = m_windowSize.x - m_settingsWindow.x;
		m_viewport.y = m_windowSize.y - m_topbarHeight;
	}

	create_image_buffer();
	m_renderer->set_image_size(m_viewport.x, m_viewport.y);
	m_renderer->set_new_buffer(m_imageBuffer);
}

void Gui::create_image_buffer()
{
	m_imageBuffer.clear();
	m_imageBuffer.resize(m_viewport.x * m_viewport.y, 0x00000000);
}

void Gui::setup_main_window()
{
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.IniFilename = NULL;

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
	glfwSetWindowAttrib(m_window, GLFW_RESIZABLE, GLFW_TRUE);
}

void Gui::disable_window_resize()
{
	// Disable when rendering
	glfwSetWindowAttrib(m_window, GLFW_RESIZABLE, GLFW_FALSE);
}

void Gui::start_render(int nThreads)
{
	m_renderer->start(nThreads);
}

inline void Gui::menu()
{
	ImVec2 size = ImGui::GetIO().DisplaySize;

	switch (m_renderer->m_state)
	{
	case (Renderer::RenderState::Running):
		disable_window_resize();
		break;
	case (Renderer::RenderState::Ready):
		enable_window_resize();
		break;
	default:
		break;
	}

	if (((m_windowSize.x != size.x) || (m_windowSize.y != size.y)) && m_renderer->m_state == Renderer::RenderState::Ready)
		if (m_windowSize.x > 0 && m_windowSize.y > 0)
			resize_image(size);

	// topbar
	{
		ImGui::Begin("##topbar", nullptr, ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoScrollbar);
		ImGui::SetWindowSize(m_topBar, NULL);
		ImGui::SetWindowPos(ImVec2(0, 0), NULL);

		if (ImGui::BeginMenuBar())
		{
			// File -> Save buffer as .PPM file
			if (ImGui::BeginMenu("File"))
			{
				if (ImGui::MenuItem("New")) {}
				//if (ImGui::MenuItem("Save", "Ctrl+S")) {}
				if (ImGui::MenuItem("Save As..")) {}
				ImGui::EndMenu();
			}
			if (ImGui::BeginMenu("Edit"))
			{
				if (ImGui::MenuItem("Cut", "CTRL+X")) {}
				if (ImGui::MenuItem("Copy", "CTRL+C")) {}
				if (ImGui::MenuItem("Paste", "CTRL+V")) {}
				ImGui::EndMenu();
			}
			if (ImGui::BeginMenu("Help"))
			{
				if (ImGui::MenuItem("About")) {}
				ImGui::EndMenu();
			}
			ImGui::EndMenuBar();
		}
		// TODO: Add [-][O][x] (minimize, maximize, exit)
		ImGui::End();
	}

	// settings menu
	{

		// ImGui doesn't take uints
		static int nThreads = 1;
		static int samplesPerPixel = 16;
		static int maxRayDepth = 10;


		const char* stratCopy[] = { "Line" , "Quad" };
		static int currentIndexStrat = 0;
		const char* showValueStrat = stratCopy[currentIndexStrat];

		const char* scenes[] = { "test1", "test2" };
		static int currentIndexScene = 0;
		const char* showValueScenes = scenes[currentIndexScene];



		ImGui::Begin("Settings", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoMove);
		ImGui::SetWindowPos(ImVec2((m_windowSize.x - m_settingsWindow.x), m_topbarHeight), NULL);
		ImGui::SetWindowSize(m_settingsWindow, NULL);
		{
			ImGui::Text("Numer of threads");
			ImGui::InputInt("##nThreads", &nThreads);
			if (nThreads < 1)
				nThreads = 1;

			ImGui::Text("Samples per Pixel"); 
			ImGui::InputInt("##sampels", &samplesPerPixel);
			if (samplesPerPixel < 1)
				samplesPerPixel = 1;

			ImGui::Text("Max Ray Depth");
			ImGui::InputInt("##depth", &maxRayDepth);
			if (maxRayDepth < 1)
				maxRayDepth = 1;

			ImGui::Text("Threading Strategy");
			if (ImGui::BeginCombo("##strategy", showValueStrat, NULL))
			{
				for (int n = 0; n < IM_ARRAYSIZE(stratCopy); n++)
				{
					const bool isSelected = (currentIndexStrat == n);
					if (ImGui::Selectable(stratCopy[n], isSelected))
						currentIndexStrat = n;

					if (isSelected)
						ImGui::SetItemDefaultFocus();
				}
				ImGui::EndCombo();
			}

			ImGui::Text("Scene");
			if (ImGui::BeginCombo("##scene", showValueScenes, NULL))
			{
				for (int n = 0; n < IM_ARRAYSIZE(scenes); n++)
				{
					const bool isSelected = (currentIndexScene == n);
					if (ImGui::Selectable(scenes[n], isSelected))
						currentIndexScene = n;

					if (isSelected)
						ImGui::SetItemDefaultFocus();
				}
				ImGui::EndCombo();
			}

			if (ImGui::Button("START"))
			{
				m_renderer->set_samples_per_pixel(samplesPerPixel);
				m_renderer->set_max_ray_bounces(maxRayDepth);
				m_renderer->set_nThreads(nThreads);

				switch (currentIndexStrat)
				{
				case (0):
					m_renderer->m_strat = Renderer::Strategy::Line;
					break;
				case(1):
					m_renderer->m_strat = Renderer::Strategy::Quad;
					break;
				default:
					throw std::runtime_error("Error: Invalid strategy!");
					break;
				}

				switch (currentIndexScene)
				{
				case (0):
					m_renderer->set_scene(Renderer::Scenes::test1);
					break;
				case(1):
					m_renderer->set_scene(Renderer::Scenes::test2);
					break;
				default:
					throw std::runtime_error("Error: Invalid Scene!");
					break;
				}

				m_renderingThread = std::thread{ &Gui::start_render, this, nThreads };
			}
			// TODO: Disable when Ready. Swap pause for restart on pause.
			ImGui::SameLine(); if (ImGui::Button("PAUSE")) { m_renderer->pause(); };
			ImGui::SameLine(); if (ImGui::Button("ABORT")) { m_renderer->stop(); };
		}
		ImGui::End();
	}
}

void Gui::run()
{
	// Create a image from the pixeldata

	static bool done = false;
	while (!done)
	{
		if (glfwWindowShouldClose(m_window))
		{
			done = true;
		}

		glfwPollEvents();
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		menu();

		GLuint texture;
		GLCall(glGenTextures(1, &texture));
		GLCall(glBindTexture(GL_TEXTURE_2D, texture));
		GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
		GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));
		GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE));
		GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST));
		GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
		ImTextureID m_imageTexture = (ImTextureID)(uintptr_t)texture;

		if (m_imageTexture)
		{
			ImGui::Begin("##viewportRender", nullptr, ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoScrollbar);
			ImGui::SetWindowSize(m_viewport, NULL);
			ImGui::SetWindowPos(ImVec2(0, m_topbarHeight), NULL);
			{
				ImGui::Image((void*)(intptr_t)m_imageTexture, m_viewport);
			}
			ImGui::End();
		}

		ImGui::Render();
		int display_w, display_h;
		glfwGetFramebufferSize(m_window, &display_w, &display_h);
		glViewport(0, 0, display_w, display_h);
		glClear(GL_COLOR_BUFFER_BIT);
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
		glfwSwapBuffers(m_window);

		// Delete the texture so we don't leak memory!
		GLCall(glDeleteTextures(1, &texture));
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