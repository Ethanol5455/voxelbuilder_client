#pragma once

#include "VertexArray.h"
#include "VertexBufferLayout.h"
#include "Texture.h"
#include "Shader.h"

struct Character {
	uint32_t m_RendererID;
	glm::ivec2 m_Size;
	glm::ivec2 m_Bearing;
	uint32_t m_Advance;
};

class TextRenderer {
    public:
	static void Init();
	static void Shutdown();

	static void SetTextSize(uint32_t textHeight);

	static void BeginScene(const glm::mat4 &view, const glm::mat4 &proj);
	//static void EndScene();

	static void RenderText(const std::string &text, float x, float y,
			       float scale = 1.0f,
			       const glm::vec3 &color = glm::vec3(1.0f));
};