#pragma once
#include <iostream>
#include <vector>
#include "ImGui/imgui.h"
#include <GL/gl3w.h>

#define ASSERT(x) if (!(x)) __debugbreak();
#define GLCall(x) GLClearError();\
	x;\
	ASSERT(GLLogCall(#x, __FILE__, __LINE__))

void GLClearError();
bool GLLogCall(const char* functiosn, const char* file, int line);
ImTextureID generate_texture(ImVec2 textureSize, std::vector<uint32_t> pixelData);