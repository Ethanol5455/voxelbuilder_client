#pragma once

//#include "VertexBuffer.h"

class VertexBufferLayout;

class VertexArray
{
private:
	uint32_t m_VBRendererID;

	uint32_t m_IBRendererID;
	uint32_t m_IndexCount;

	uint32_t m_VARendererID;
public:
	VertexArray(uint32_t verticiesSize, const VertexBufferLayout& layout, const uint32_t* indicies, uint32_t indiciesCount);
	VertexArray(const void* verticies, uint32_t verticiesSize, const VertexBufferLayout& layout, const uint32_t* indicies, uint32_t indiciesCount);
	~VertexArray();

	void Bind(bool bindVB = false) const;
	void Unbind() const;

	inline uint32_t GetIndexCount() const { return m_IndexCount; };
};