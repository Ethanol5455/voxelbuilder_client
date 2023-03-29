#include "vbpch.h"
#include "Texture.h"

#include <filesystem>

#include "stb_image.h"
#include "stb_image_resize.h"
#include "stb_image_write.h"

Texture::Texture(uint32_t width, uint32_t height, const std::string &type)
	: m_RendererID(0)
	, m_FilePath("")
	, m_Width(width)
	, m_Height(height)
	, m_BPP(0)
	, m_Type(type)
{
	glActiveTexture(GL_TEXTURE1);
	glGenTextures(1, &m_RendererID);
	glBindTexture(GL_TEXTURE_2D, m_RendererID);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, m_Width, m_Height, 0, GL_RGBA,
		     GL_UNSIGNED_BYTE, nullptr);
}

Texture::Texture(const std::string &path, const std::string &type)
	: m_RendererID(0)
	, m_FilePath(path)
	, m_Width(0)
	, m_Height(0)
	, m_BPP(0)
	, m_Type(type)
{
	LoadTexture(path, 0, 0, type);
}

Texture::Texture(const std::string &path, uint32_t width, uint32_t height,
		 const std::string &type)
	: m_RendererID(0)
	, m_FilePath(path)
	, m_Width(0)
	, m_Height(0)
	, m_BPP(0)
	, m_Type(type)
{
	LoadTexture(path, width, height, type);
}

Texture::~Texture()
{
	glDeleteTextures(1, &m_RendererID);
#ifdef DISPLAY_MESSAGES
	VB_INFO("Unloaded texture {0}", GetFileName());
#endif
}

void Texture::BindTexture(uint32_t slot /*= 0*/) const
{
	glActiveTexture(GL_TEXTURE0 + slot);
	glBindTexture(GL_TEXTURE_2D, m_RendererID);
}

void Texture::UnbindTexture() const
{
	glBindTexture(GL_TEXTURE_2D, 0);
}

void Texture::BindImageTexture(bool allowRead, bool allowWrite,
			       uint32_t slot /*= 0*/) const
{
	GLenum access;
	if (allowRead && allowWrite)
		access = GL_READ_WRITE;
	else if (allowWrite)
		access = GL_WRITE_ONLY;
	else
		access = GL_READ_ONLY;

	glBindImageTexture(slot, m_RendererID, 0, GL_FALSE, 0, access,
			   GL_RGBA8);
}

TextureCoordinates Texture::ExtractCoordinates(const glm::vec2 &count,
					       const glm::vec2 &position)
{
	glm::vec2 segmentSize(1.0f / count.x, 1.0f / count.y);
	//glm::vec2 BL, BR, TL, TR;
	return { glm::vec2(segmentSize.x * position.x,
			   segmentSize.y * position.y),
		 glm::vec2(segmentSize.x * (position.x + 1.0f),
			   segmentSize.y * position.y),
		 glm::vec2(segmentSize.x * position.x,
			   segmentSize.y * (position.y + 1.0f)),
		 glm::vec2(segmentSize.x * (position.x + 1.0f),
			   segmentSize.y * (position.y + 1.0f)) };
}

bool Texture::LoadTexture(std::string path, uint32_t width, uint32_t height,
			  const std::string &type)
{
	bool doResize = (width != 0 && width != m_Width) ||
			(height != 0 && height != m_Height);

	bool writeToCacheAfterResize = false;
	const std::string cacheDir = "res/cache/textures/resized/";
	std::string cachePath = path;
	if (doResize) {
		if (s_UseTextureCacheIfResized) {
			std::replace(cachePath.begin(), cachePath.end(), '\\',
				     '_');
			std::replace(cachePath.begin(), cachePath.end(), '/',
				     '_');
			std::replace(cachePath.begin(), cachePath.end(), ':',
				     '_');
			std::replace(cachePath.begin(), cachePath.end(), '.',
				     '_');
			cachePath = cacheDir + cachePath +
				    std::to_string(width) +
				    std::to_string(height) + ".png";

			if (!std::filesystem::exists(cacheDir))
				std::filesystem::create_directories(cacheDir);
			if (std::filesystem::exists(cachePath)) {
				path = cachePath;
				doResize = false;
			} else {
				writeToCacheAfterResize = true;
			}
		}
	}

	unsigned char *localBuffer;

	stbi_set_flip_vertically_on_load(1);
	localBuffer = stbi_load(path.c_str(), &m_Width, &m_Height, &m_BPP, 4);

	if (!localBuffer) {
		VB_ERROR("Unable to load image: \"{0}\" for reason: {1}", path,
			 stbi_failure_reason());
		return false;
	}

	if (doResize) {
		unsigned char *newBuffer =
			new unsigned char[width * height * m_BPP];

		stbir_resize_uint8(localBuffer, m_Width, m_Height, 0, newBuffer,
				   width, height, 0, m_BPP);
		stbi_image_free(localBuffer);
		localBuffer = newBuffer;

		m_Width = width;
		m_Height = height;

		if (writeToCacheAfterResize) {
			stbi_flip_vertically_on_write(1);
			stbi_write_png(cachePath.c_str(), width, height, m_BPP,
				       localBuffer, m_Width * m_BPP);
		}
	}

	glGenTextures(1, &m_RendererID);
	glBindTexture(GL_TEXTURE_2D, m_RendererID);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, m_Width, m_Height, 0, GL_RGBA,
		     GL_UNSIGNED_BYTE, localBuffer);
	glBindTexture(GL_TEXTURE_2D, 0);

	if (localBuffer)
		stbi_image_free(localBuffer);

	return true;
}
