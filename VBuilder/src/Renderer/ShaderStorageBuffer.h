#pragma once

#include "glad/glad.h"

enum class BufferUsage {
	StreamDraw,
	StreamRead,
	StreamCopy,
	StaticDraw,
	StaticRead,
	StaticCopy,
	DynamicDraw,
	DynamicRead,
	DynamicCopy
};

enum class BufferAccess { ReadOnly, WriteOnly, ReadWrite };

class ShaderStorageBuffer {
    private:
	uint32_t m_RendererID;

    public:
	ShaderStorageBuffer(uint32_t dataSize, BufferUsage usage);
	ShaderStorageBuffer(const void *data, uint32_t dataSize,
			    BufferUsage usage);

	void *MapBuffer(BufferAccess access);

	void BindBase(uint32_t index = 0);
	void Bind();
	void Unbind();
};