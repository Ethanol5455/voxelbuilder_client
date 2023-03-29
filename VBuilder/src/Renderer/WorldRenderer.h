#pragma once

#include "VertexArray.h"
#include "VertexBufferLayout.h"
#include "Texture.h"
#include "Shader.h"

struct QuadVertex {
	glm::vec3 Position;
	glm::vec2 TexCoord;
};

struct Mesh {
	QuadVertex *verticies = nullptr;
	uint32_t numVerticies = 0;
};

class WorldRenderer {
    public:
	static void Init(Shader *shader);
	static void Shutdown();

	static void SetTexture(Texture *texture);

	static void BeginScene(const glm::mat4 &view, const glm::mat4 &proj);
	static void EndScene();

	static void DrawColumnVA(const VertexArray *va,
				 const glm::vec2 &transform);

	// Stats
	struct Statistics {
		uint32_t DrawCalls = 0;
		uint32_t QuadCount = 0;

		uint32_t GetTotalVertexCount() const
		{
			return QuadCount * 4;
		}
		uint32_t GetTotalIndexCount() const
		{
			return QuadCount * 6;
		}
	};

	static void ResetStats();
	static Statistics GetStats();
};