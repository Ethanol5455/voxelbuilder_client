#include "vbpch.h"

#include "TextRenderer.h"

struct RendererData {
	std::map<char, Character> *Characters;

	VertexArray *VA = nullptr;

	Shader *shader = nullptr;
};

static RendererData s_Data;

void TextRenderer::Init()
{
	s_Data.Characters = new std::map<char, Character>();

	SetTextSize(50);

	VertexBufferLayout layout;
	layout.Push<float>(2);
	layout.Push<float>(2);

	uint32_t indicies[] = { 0, 1, 2, 3, 4, 5 };

	s_Data.VA =
		new VertexArray(6 * 4 * sizeof(float), layout, indicies, 2 * 3);
	s_Data.shader = new Shader("res/shaders/TextVertex.glsl",
				   "res/shaders/TextFragment.glsl");
}

void TextRenderer::Shutdown()
{
	for (auto &ch : *s_Data.Characters) {
		glDeleteTextures(1, &ch.second.m_RendererID);
	}
	delete s_Data.Characters;
	delete s_Data.VA;
	delete s_Data.shader;
}

void TextRenderer::SetTextSize(uint32_t textHeight)
{
	for (auto &ch : *s_Data.Characters) {
		glDeleteTextures(1, &ch.second.m_RendererID);
	}
	s_Data.Characters->clear();

	FT_Library ft;
	if (FT_Init_FreeType(&ft)) {
		VB_ERROR("Unable to load FreeType Library");
	}

	FT_Face face;
	if (FT_New_Face(ft, "res/fonts/arial.ttf", 0, &face)) {
		VB_ERROR("Failed to load font");
	}

	FT_Set_Pixel_Sizes(face, 0, textHeight);

	glPixelStorei(GL_UNPACK_ALIGNMENT, GL_TRUE);

	for (unsigned char c = 0; c < 128; c++) {
		if (FT_Load_Char(face, c, FT_LOAD_RENDER)) {
			VB_ERROR("Failed to load Glyph");
			continue;
		}

		uint32_t texture;
		glGenTextures(1, &texture);
		glBindTexture(GL_TEXTURE_2D, texture);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RED,
			     face->glyph->bitmap.width,
			     face->glyph->bitmap.rows, 0, GL_RED,
			     GL_UNSIGNED_BYTE, face->glyph->bitmap.buffer);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,
				GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T,
				GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
				GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER,
				GL_LINEAR);

		Character character = { texture,
					glm::ivec2(face->glyph->bitmap.width,
						   face->glyph->bitmap.rows),
					glm::ivec2(face->glyph->bitmap_left,
						   face->glyph->bitmap_top),
					(uint32_t)face->glyph->advance.x };
		s_Data.Characters->insert(
			std::pair<char, Character>(c, character));
	}

	FT_Done_Face(face);
	FT_Done_FreeType(ft);
}

void TextRenderer::BeginScene(const glm::mat4 &view, const glm::mat4 &proj)
{
	glDisable(GL_DEPTH_TEST);
	s_Data.shader->Bind();
	s_Data.shader->SetUniformMat4f("u_ViewProjection", proj * view);
}

void TextRenderer::RenderText(const std::string &text, float x, float y,
			      float scale, const glm::vec3 &color)
{
	s_Data.shader->Bind();
	s_Data.shader->SetUniform3f("u_TextColor", color);
	glActiveTexture(GL_TEXTURE0);
	s_Data.VA->Bind(true);

	float origX = x;

	for (auto &c : text) {
		Character &ch = s_Data.Characters->at(c);

		if (c == '\n') {
			x = origX;
			float xScale, yScale;
			glfwGetWindowContentScale(glfwGetCurrentContext(),
						  &xScale, &yScale);
			y -= ch.m_Size.y * 1.5f;
			continue;
		}

		float xpos = x + ch.m_Bearing.x * scale;
		float ypos = y - (ch.m_Size.y - ch.m_Bearing.y) * scale;

		float w = ch.m_Size.x * scale;
		float h = ch.m_Size.y * scale;
		// update VBO for each character
		float verticies[6][4] = { { xpos, ypos + h, 0.0f, 0.0f },
					  { xpos, ypos, 0.0f, 1.0f },
					  { xpos + w, ypos, 1.0f, 1.0f },

					  { xpos, ypos + h, 0.0f, 0.0f },
					  { xpos + w, ypos, 1.0f, 1.0f },
					  { xpos + w, ypos + h, 1.0f, 0.0f } };

		glBindTexture(GL_TEXTURE_2D, ch.m_RendererID);

		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(verticies),
				verticies);

		glDrawArrays(GL_TRIANGLES, 0, 6);

		x += (ch.m_Advance >> 6) * scale;
	}

	glBindVertexArray(0);
	glBindTexture(GL_TEXTURE_2D, 0);
}