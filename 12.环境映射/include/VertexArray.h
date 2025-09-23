#pragma once

#include "VertexBuffer.h"
#include "VertexBufferLayout.h"


class VertexArray
{
private:
	unsigned int m_RendererID;
public:
	VertexArray();
	~VertexArray();
	void BindVertexArray() const;
	void UnBindVertexArray() const;
	void AddBuffer(const VertexBuffer& vertexBuffer, const VertexBufferLayout& layout);
};
