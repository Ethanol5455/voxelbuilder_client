#pragma once

#include "glad/glad.h"

struct VertexBufferElement
{
	uint32_t type;
	uint32_t count;
	unsigned char normalized;

	static uint32_t GetSizeOfType(uint32_t type)
	{
		switch (type)
		{
		case GL_FLOAT:			return 4;
		case GL_UNSIGNED_INT:	return 4;
		case GL_UNSIGNED_BYTE:	return 1;
		}
		VB_ASSERT(false, "Incompatible Vertex Buffer Element");
		return 0;
	}
};

class VertexBufferLayout
{
private:
	std::vector<VertexBufferElement> m_Elements;
	uint32_t m_Stride;
public:
	VertexBufferLayout()
		: m_Stride(0) {}

	template<typename T>
	void Push(uint32_t count)
	{
		VB_DEBUGBREAK();
	}

#ifdef VB_PLATFORM_WINDOWS
	template<>
	void Push<float>(uint32_t count)
	{
		m_Elements.push_back({ GL_FLOAT, count, GL_FALSE });
		m_Stride += count * VertexBufferElement::GetSizeOfType(GL_FLOAT);
	}
	template<>
	void Push<uint32_t>(uint32_t count)
	{
		m_Elements.push_back({ GL_UNSIGNED_INT, count, GL_FALSE });
		m_Stride += count * VertexBufferElement::GetSizeOfType(GL_UNSIGNED_INT);
	}
	template<>
	void Push<uint8_t>(uint32_t count)
	{
		m_Elements.push_back({ GL_UNSIGNED_BYTE, count, GL_TRUE });
		m_Stride += count * VertexBufferElement::GetSizeOfType(GL_UNSIGNED_BYTE);
	}
#endif

	inline const std::vector<VertexBufferElement> GetElements() const { return m_Elements; }
	inline uint32_t GetStride() const { return m_Stride; }
};