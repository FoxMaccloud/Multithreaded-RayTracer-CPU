#pragma once
class IndexBuffer
{
private:
	unsigned int m_renderID;
	unsigned int m_count;

public:
	IndexBuffer(const unsigned int* data, unsigned int count);
	~IndexBuffer();

	void bind();
	void unbind();

	unsigned int get_count() const { return m_count; };
};