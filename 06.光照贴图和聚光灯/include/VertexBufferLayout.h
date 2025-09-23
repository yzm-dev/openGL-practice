#pragma once

#include <vector>

struct VertexBufferElement
{
	unsigned int type;
	unsigned int count;
	unsigned char normalized;
	static unsigned int GetStrideOfType(unsigned int type);
};

class VertexBufferLayout
{
private:
	std::vector<VertexBufferElement> m_Elements;
	unsigned int m_Stride;
public:
	VertexBufferLayout();
	~VertexBufferLayout();

    template<typename T>
    void Push(unsigned int count)
    {
        static_assert(std::is_same<T, void>::value, "Unsupported type for VertexArrayLayout::Push");
    }

    template<>
    void Push<float>(unsigned int count);

    template<>
    void Push<unsigned int>(unsigned int count);

    template<>
    void Push<unsigned char>(unsigned int count);

	const std::vector<VertexBufferElement>& GetElements() const;
	unsigned int GetStride() const;
};