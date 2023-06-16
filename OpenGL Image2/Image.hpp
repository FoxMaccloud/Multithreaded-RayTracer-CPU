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
ImTextureID GenerateTexture(ImVec2 textureSize, std::vector<uint32_t> pixelData);


// TODO:
class OGLImage
{
public:
	OGLImage(ImVec2& textureSize, std::vector<uint32_t>& pixelData) : m_imageSize(textureSize), m_image(pixelData){};
	//ImTextureID GenerateTexture();
	//void updateTexture();
	
	GLuint TextureId() const { return m_textureId; }

private:
	GLuint m_textureId = 0;
	std::vector<uint32_t> m_image;
	ImVec2 m_imageSize{ 0,0 };
};