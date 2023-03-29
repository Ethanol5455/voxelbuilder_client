#pragma once

#include <glad/glad.h>

class Cubemap {
    public:
	Cubemap(const std::string &faces);
	Cubemap(std::vector<std::string> faces);
	~Cubemap();

	void Bind(uint32_t slot = 0) const;
	void Unbind();

	const std::vector<std::string> &GetFilePaths()
	{
		return m_FilePaths;
	}
	bool IsLoaded()
	{
		return m_IsLoaded;
	}

    private:
	uint32_t m_RendererID;
	std::vector<std::string> m_FilePaths = {};
	bool m_IsLoaded = false;
	unsigned char *m_LocalBuffer;
	int m_Width, m_Height, m_BPP;
};