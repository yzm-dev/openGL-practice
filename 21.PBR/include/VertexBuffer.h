#pragma once

class VertexBuffer
{
private:
	unsigned int m_RendererID;
public:
	VertexBuffer(const void* data, unsigned int size);
	~VertexBuffer();
	void BindVertexBuffer() const;
	void UnBindVertexBuffer() const;
};

