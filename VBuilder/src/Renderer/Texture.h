#pragma once

#include <GLFW/glfw3.h>
#include <glad/glad.h>
#include <glm/glm.hpp>

#include "Core/Timer.h"

struct TextureCoordinates
{
	glm::vec2 BL, BR, TL, TR;
};

static bool s_UseTextureCacheIfResized = false;

class Texture
{
private:
	uint32_t m_RendererID;
	std::string m_FilePath;
	int m_Width, m_Height, m_BPP;

	std::string m_Type;
public:
	Texture(uint32_t width, uint32_t height, const std::string& type = "");
	Texture(const std::string& path, const std::string& type = "");
	Texture(const std::string& path, uint32_t width, uint32_t height, const std::string& type = "");
	~Texture();

	void BindTexture(uint32_t slot = 0) const;
	void UnbindTexture() const;
	void BindImageTexture(bool allowRead, bool allowWrite, uint32_t slot = 0) const;

	inline int GetWidth() const { return m_Width; }
	inline int GetHeight() const { return m_Height; }
	inline uint32_t GetRendererID() { return m_RendererID; }
	inline const std::string& GetFilePath() { return m_FilePath; }
	inline std::string GetFileName() { return m_FilePath.substr(m_FilePath.find_last_of('/') + 1); }

	const std::string& GetType() { return m_Type; }

	static TextureCoordinates ExtractCoordinates(const glm::vec2& count, const glm::vec2& position);

private:
	bool LoadTexture(std::string path, uint32_t width, uint32_t height, const std::string& type = "");
};