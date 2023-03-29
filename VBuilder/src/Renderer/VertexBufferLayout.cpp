#include "vbpch.h"
#include "VertexBufferLayout.h"

#ifdef VB_PLATFORM_LINUX

template <> void VertexBufferLayout::Push<float>(uint32_t count)
{
	m_Elements.push_back({ GL_FLOAT, count, GL_FALSE });
	m_Stride += count * VertexBufferElement::GetSizeOfType(GL_FLOAT);
}
template <> void VertexBufferLayout::Push<uint32_t>(uint32_t count)
{
	m_Elements.push_back({ GL_UNSIGNED_INT, count, GL_FALSE });
	m_Stride += count * VertexBufferElement::GetSizeOfType(GL_UNSIGNED_INT);
}
template <> void VertexBufferLayout::Push<uint8_t>(uint32_t count)
{
	m_Elements.push_back({ GL_UNSIGNED_BYTE, count, GL_TRUE });
	m_Stride +=
		count * VertexBufferElement::GetSizeOfType(GL_UNSIGNED_BYTE);
}

#endif