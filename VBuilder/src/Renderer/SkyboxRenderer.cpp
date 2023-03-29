#include "vbpch.h"

#include "SkyboxRenderer.h"

struct RendererData {
	VertexArray *VA = nullptr;

	Shader *shader = nullptr;
};

static RendererData s_Data;

void SkyboxRenderer::Init()
{
	float skyboxVertices[] = {
		// positions
		-1.0f, 1.0f,  -1.0f, //Back
		-1.0f, -1.0f, -1.0f, 1.0f,  -1.0f, -1.0f, 1.0f,	 -1.0f,
		-1.0f, 1.0f,  1.0f,  -1.0f, -1.0f, 1.0f,  -1.0f,

		-1.0f, -1.0f, 1.0f, //Left
		-1.0f, -1.0f, -1.0f, -1.0f, 1.0f,  -1.0f, -1.0f, 1.0f,
		-1.0f, -1.0f, 1.0f,  1.0f,  -1.0f, -1.0f, 1.0f,

		1.0f,  -1.0f, -1.0f, //Right
		1.0f,  -1.0f, 1.0f,  1.0f,  1.0f,  1.0f,  1.0f,	 1.0f,
		1.0f,  1.0f,  1.0f,  -1.0f, 1.0f,  -1.0f, -1.0f,

		-1.0f, -1.0f, 1.0f, //Front
		-1.0f, 1.0f,  1.0f,  1.0f,  1.0f,  1.0f,  1.0f,	 1.0f,
		1.0f,  1.0f,  -1.0f, 1.0f,  -1.0f, -1.0f, 1.0f,

		-1.0f, 1.0f,  -1.0f, //Up
		1.0f,  1.0f,  -1.0f, 1.0f,  1.0f,  1.0f,  1.0f,	 1.0f,
		1.0f,  -1.0f, 1.0f,  1.0f,  -1.0f, 1.0f,  -1.0f,

		-1.0f, -1.0f, -1.0f, //Down
		-1.0f, -1.0f, 1.0f,  1.0f,  -1.0f, -1.0f, 1.0f,	 -1.0f,
		-1.0f, -1.0f, -1.0f, 1.0f,  1.0f,  -1.0f, 1.0f
	};

	uint32_t indicies[6 * 6];
	for (int i = 0; i < 6 * 6; i++) {
		indicies[i] = i;
	}

	s_Data.shader = new Shader("res/shaders/Skybox.vert",
				   "res/shaders/Skybox.frag");
	s_Data.shader->Bind();
	s_Data.shader->SetUniform1i("u_Skybox", 0);

	VertexBufferLayout layout;
	layout.Push<float>(3);

	s_Data.VA = new VertexArray(&skyboxVertices[0], sizeof(skyboxVertices),
				    layout, &indicies[0], 6 * 6);
}

void SkyboxRenderer::Shutdown()
{
	delete s_Data.VA;
	delete s_Data.shader;
}

void SkyboxRenderer::BeginScene(const glm::mat4 &view, const glm::mat4 &proj)
{
	glDepthFunc(GL_LEQUAL);

	s_Data.shader->Bind();
	s_Data.shader->SetUniformMat4f("u_ViewProjection", proj * view);
}

void SkyboxRenderer::EndScene()
{
	glDepthFunc(GL_LESS);
}

void SkyboxRenderer::RenderSkybox(Cubemap &cubemap)
{
	s_Data.shader->Bind();
	s_Data.VA->Bind();
	cubemap.Bind();

	glDrawElements(GL_TRIANGLES, s_Data.VA->GetIndexCount(),
		       GL_UNSIGNED_INT, nullptr);
}
