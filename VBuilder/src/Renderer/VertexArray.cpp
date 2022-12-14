#include "vbpch.h"
#include "VertexArray.h"

#include "VertexBufferLayout.h"



VertexArray::VertexArray(uint32_t verticiesSize, const VertexBufferLayout& layout, 
	const uint32_t* indicies, uint32_t indiciesCount)
{
	// Vertex Buffer
	glGenBuffers(1, &m_VBRendererID);
	glBindBuffer(GL_ARRAY_BUFFER, m_VBRendererID);
	glBufferData(GL_ARRAY_BUFFER, verticiesSize, nullptr, GL_DYNAMIC_DRAW);


	// Index Buffer
	VB_ASSERT(sizeof(uint32_t) == sizeof(GLuint), "Size of uint32_t != GLuint");

	glGenBuffers(1, &m_IBRendererID);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_IBRendererID);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indiciesCount * sizeof(uint32_t), indicies, GL_STATIC_DRAW);

	//Vertex Array
	glGenVertexArrays(1, &m_VARendererID);

	Bind();

	const auto& elements = layout.GetElements();
	uint32_t offset = 0;
	for (uint32_t i = 0; i < elements.size(); i++)
	{
		const auto& element = elements[i];
		glEnableVertexAttribArray(i);
		glVertexAttribPointer(i, element.count, element.type,
			element.normalized, layout.GetStride(), (const void*)offset);
		offset += element.count * VertexBufferElement::GetSizeOfType(element.type);
	}
}

VertexArray::VertexArray(const void* verticies, uint32_t verticiesSize, const VertexBufferLayout& layout, 
	const uint32_t* indicies, uint32_t indiciesCount)
	: m_IndexCount(indiciesCount)
{
	// Vertex Buffer
	glGenBuffers(1, &m_VBRendererID);
	glBindBuffer(GL_ARRAY_BUFFER, m_VBRendererID);
	glBufferData(GL_ARRAY_BUFFER, verticiesSize, verticies, GL_STATIC_DRAW);


	// Index Buffer
	VB_ASSERT(sizeof(uint32_t) == sizeof(GLuint), "Size of uint32_t != GLuint");

	glGenBuffers(1, &m_IBRendererID);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_IBRendererID);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indiciesCount * sizeof(uint32_t), indicies, GL_STATIC_DRAW);

	//Vertex Array
	glGenVertexArrays(1, &m_VARendererID);

	Bind();

	const auto& elements = layout.GetElements();
	uint32_t offset = 0;
	for (uint32_t i = 0; i < elements.size(); i++)
	{
		const auto& element = elements[i];
		glEnableVertexAttribArray(i);
		glVertexAttribPointer(i, element.count, element.type,
			element.normalized, layout.GetStride(), (const void*)offset);
		offset += element.count * VertexBufferElement::GetSizeOfType(element.type);
	}
}

VertexArray::~VertexArray()
{
	glDeleteBuffers(1, &m_VBRendererID);
	glDeleteBuffers(1, &m_IBRendererID);
	glDeleteVertexArrays(1, &m_VARendererID);
}

//void VertexArray::AddBuffer(const VertexBuffer& vb, const VertexBufferLayout& layout)
//{
//	Bind();
//	vb.Bind();
//	const auto& elements = layout.GetElements();
//	uint32_t offset = 0;
//	for (uint32_t i = 0; i < elements.size(); i++)
//	{
//		const auto& element = elements[i];
//		glEnableVertexAttribArray(i);
//		glVertexAttribPointer(i, element.count, element.type,
//			element.normalized, layout.GetStride(), (const void*)offset);
//		offset += element.count * VertexBufferElement::GetSizeOfType(element.type);
//	}
//}

void VertexArray::Bind(bool bindVB) const
{
	if (bindVB)
		glBindBuffer(GL_ARRAY_BUFFER, m_VBRendererID);
	glBindVertexArray(m_VARendererID);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_IBRendererID);
}

void VertexArray::Unbind() const
{
	glBindVertexArray(0);
}