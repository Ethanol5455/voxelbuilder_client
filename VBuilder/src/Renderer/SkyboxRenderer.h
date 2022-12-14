#pragma once

#include "Shader.h"
#include "VertexArray.h"
#include "VertexBufferLayout.h"
#include "Cubemap.h"

class SkyboxRenderer
{
public:
	static void Init();
	static void Shutdown();

	static void BeginScene(const glm::mat4& view, const glm::mat4& proj);
	static void EndScene();

	static void RenderSkybox(Cubemap& cubemap);
};