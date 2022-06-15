#include "IndexBuffer.h"

#include "Image.h"

IndexBuffer::IndexBuffer(const unsigned int* data, unsigned int count)
	: m_count(count)
{
	GLCall(glGenBuffers(1, &m_renderID));
	GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_renderID));
	GLCall(glBufferData(GL_ELEMENT_ARRAY_BUFFER, count * sizeof(GLuint), data, GL_STATIC_DRAW));
}

IndexBuffer::~IndexBuffer()
{
	GLCall(glDeleteBuffers(1, &m_renderID));
}

void IndexBuffer::bind()
{
	GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_renderID));
}

void IndexBuffer::unbind()
{
	GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));
}
