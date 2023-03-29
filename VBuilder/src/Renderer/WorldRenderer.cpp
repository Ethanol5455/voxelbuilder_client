#include "vbpch.h"

#include "WorldRenderer.h"

#include <glm/gtc/matrix_transform.hpp>

struct RendererData {
	bool Initialized = false;

	//static const uint32_t MaxTextureSlots = 32; // TODO: RenderCaps
	Shader *BatchShader = nullptr;

	Texture *TextureAtlas;

	WorldRenderer::Statistics RenderStats;
};

static RendererData s_Data;

void WorldRenderer::Init(Shader *shader)
{
	VB_ASSERT(!s_Data.Initialized,
		  "Renderer: Init has been called twice without shutdown");

	s_Data.BatchShader = shader;
	s_Data.BatchShader->Bind();

	s_Data.BatchShader->SetUniform1i("u_Texture", 0);

	s_Data.Initialized = true;
}

void WorldRenderer::Shutdown()
{
	//delete s_Data.BatchShader;

	s_Data.Initialized = false;
}

void WorldRenderer::SetTexture(Texture *texture)
{
	s_Data.TextureAtlas = texture;
}

void WorldRenderer::BeginScene(const glm::mat4 &view, const glm::mat4 &proj)
{
	s_Data.BatchShader->Bind();
	s_Data.BatchShader->SetUniformMat4f("u_ViewProjection", proj * view);
}

void WorldRenderer::EndScene()
{
}

void WorldRenderer::DrawColumnVA(const VertexArray *va,
				 const glm::vec2 &transform)
{
	if (!va)
		return;
	s_Data.BatchShader->Bind();
	s_Data.BatchShader->SetUniformMat4f(
		"u_Transform",
		glm::translate(glm::mat4(1.0f),
			       glm::vec3(transform.x, 0, transform.y)));
	s_Data.TextureAtlas->BindTexture();
	s_Data.BatchShader->SetUniform1i("u_Texture", 0);
	va->Bind();
	glDrawElements(GL_TRIANGLES, va->GetIndexCount(), GL_UNSIGNED_INT,
		       nullptr);

	s_Data.RenderStats.DrawCalls++;
	s_Data.RenderStats.QuadCount += va->GetIndexCount() / 6 * 4;
}

void WorldRenderer::ResetStats()
{
	memset(&s_Data.RenderStats, 0, sizeof(Statistics));
}

WorldRenderer::Statistics WorldRenderer::GetStats()
{
	return s_Data.RenderStats;
}
