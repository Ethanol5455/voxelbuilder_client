#include "vbpch.h"
#include "ShaderStorageBuffer.h"

namespace Utils
{

static GLenum GFBufferUsageToGL(BufferUsage usage)
{
	switch (usage) {
	case BufferUsage::StreamDraw:
		return GL_STREAM_DRAW;
	case BufferUsage::StreamRead:
		return GL_STREAM_READ;
	case BufferUsage::StreamCopy:
		return GL_STREAM_COPY;
	case BufferUsage::StaticDraw:
		return GL_STATIC_DRAW;
	case BufferUsage::StaticRead:
		return GL_STATIC_READ;
	case BufferUsage::StaticCopy:
		return GL_STATIC_COPY;
	case BufferUsage::DynamicDraw:
		return GL_DYNAMIC_DRAW;
	case BufferUsage::DynamicRead:
		return GL_DYNAMIC_READ;
	case BufferUsage::DynamicCopy:
		return GL_DYNAMIC_COPY;
	}

	VB_ASSERT(false, "Unknown buffer usage");
	return 0;
}

static GLenum GFBufferAccessToGL(BufferAccess usage)
{
	switch (usage) {
	case BufferAccess::ReadOnly:
		return GL_READ_ONLY;
	case BufferAccess::WriteOnly:
		return GL_WRITE_ONLY;
	case BufferAccess::ReadWrite:
		return GL_READ_WRITE;
	}

	VB_ASSERT(false, "Unknown buffer access");
	return 0;
}

}

ShaderStorageBuffer::ShaderStorageBuffer(uint32_t dataSize, BufferUsage usage)
{
	glGenBuffers(1, &m_RendererID);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_RendererID);
	glBufferData(GL_SHADER_STORAGE_BUFFER, dataSize, nullptr,
		     Utils::GFBufferUsageToGL(usage));
}

ShaderStorageBuffer::ShaderStorageBuffer(const void *data, uint32_t dataSize,
					 BufferUsage usage)
{
	glGenBuffers(1, &m_RendererID);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_RendererID);
	glBufferData(GL_SHADER_STORAGE_BUFFER, dataSize, data,
		     Utils::GFBufferUsageToGL(usage));
}

void *ShaderStorageBuffer::MapBuffer(BufferAccess access)
{
	return glMapBuffer(GL_SHADER_STORAGE_BUFFER,
			   Utils::GFBufferAccessToGL(access));
}

void ShaderStorageBuffer::BindBase(uint32_t index)
{
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, index, m_RendererID);
}

void ShaderStorageBuffer::Bind()
{
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_RendererID);
}

void ShaderStorageBuffer::Unbind()
{
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}
