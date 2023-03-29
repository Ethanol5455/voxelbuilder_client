#include "vbpch.h"
#include "Cubemap.h"

#include "stb_image.h"

Cubemap::Cubemap(const std::string &faces)
{
	m_FilePaths.push_back(faces);

	glCreateTextures(GL_TEXTURE_CUBE_MAP, 1, &m_RendererID);
	glBindTexture(GL_TEXTURE_CUBE_MAP, m_RendererID);

	unsigned char *data;

	stbi_set_flip_vertically_on_load(0);
	data = stbi_load(faces.c_str(), &m_Width, &m_Height, &m_BPP, 0);
	if (data) {
		uint32_t fbID;
		glCreateFramebuffers(1, &fbID);
		glBindFramebuffer(GL_FRAMEBUFFER, fbID);

		uint32_t texID;
		glGenTextures(1, &texID);
		glBindTexture(GL_TEXTURE_2D, texID);
		auto format = GL_RGB;
		if (m_BPP == 4)
			format = GL_RGBA;
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, m_Width, m_Height, 0,
			     format, GL_UNSIGNED_BYTE, data);

		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
				       GL_TEXTURE_2D, texID, 0);

		VB_ASSERT(glCheckFramebufferStatus(GL_FRAMEBUFFER) ==
				  GL_FRAMEBUFFER_COMPLETE,
			  "Cubemap framebuffer not complete");

		glBindFramebuffer(GL_READ_FRAMEBUFFER, fbID);

		int xStep = m_Width / 4;
		int yStep = m_Height / 3;

		glCopyTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X, 0, GL_RGB,
				 xStep * 2, yStep * 1, xStep, yStep,
				 0); // Right Face

		glCopyTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_X, 0, GL_RGB,
				 xStep * 0, yStep * 1, xStep, yStep,
				 0); // Left Face

		glCopyTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Y, 0, GL_RGB,
				 xStep * 1, yStep * 0, xStep, yStep,
				 0); // Top Face

		glCopyTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, 0, GL_RGB,
				 xStep * 1, yStep * 2, xStep, yStep,
				 0); // Bottom Face

		glCopyTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Z, 0, GL_RGB,
				 xStep * 1, yStep * 1, xStep, yStep,
				 0); // Front Face

		glCopyTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, 0, GL_RGB,
				 xStep * 3, yStep * 1, xStep, yStep,
				 0); // Back Face

		glDeleteTextures(1, &texID);
		glDeleteFramebuffers(1, &fbID);

		m_IsLoaded = true;
	} else {
		VB_ERROR("Unable to load image: \"{0}\" for reason: {1}", faces,
			 stbi_failure_reason());
	}
	stbi_image_free(data);

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S,
			GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T,
			GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R,
			GL_CLAMP_TO_EDGE);
}

Cubemap::Cubemap(std::vector<std::string> faces)
	: m_FilePaths(faces)
{
	glCreateTextures(GL_TEXTURE_CUBE_MAP, 1, &m_RendererID);
	glBindTexture(GL_TEXTURE_CUBE_MAP, m_RendererID);

	unsigned char *data;

	stbi_set_flip_vertically_on_load(0);
	for (unsigned int i = 0; i < faces.size(); i++) {
		data = stbi_load(faces[i].c_str(), &m_Width, &m_Height, &m_BPP,
				 0);
		if (data) {
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0,
				     GL_RGB, m_Width, m_Height, 0, GL_RGB,
				     GL_UNSIGNED_BYTE, data);
		} else {
			VB_WARN("Unable to load cubemap at {0}", faces[i]);
		}
		stbi_image_free(data);
	}

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S,
			GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T,
			GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R,
			GL_CLAMP_TO_EDGE);
}

Cubemap::~Cubemap()
{
	glDeleteTextures(1, &m_RendererID);
}

void Cubemap::Bind(uint32_t slot) const
{
	glActiveTexture(GL_TEXTURE0 + slot);
	glBindTexture(GL_TEXTURE_CUBE_MAP, m_RendererID);
}

void Cubemap::Unbind()
{
	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
}
