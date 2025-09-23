#include "VertexBuffer.h"
#include "VertexBufferLayout.h"  
#include "VertexArray.h"  
#include <GL/glew.h>  

VertexArray::VertexArray()  
{  
    glGenVertexArrays(1, &m_RendererID);  
}  

VertexArray::~VertexArray()  
{  
    glDeleteVertexArrays(1, &m_RendererID);  
}  

void VertexArray::BindVertexArray() const  
{  
    glBindVertexArray(m_RendererID);  
}  

void VertexArray::UnBindVertexArray() const  
{  
    glBindVertexArray(0);  
}  

void VertexArray::AddBuffer(const VertexBuffer& vertexBuffer, const VertexBufferLayout& layout)  
{  
    BindVertexArray();  
    vertexBuffer.BindVertexBuffer();  
    const auto& elements = layout.GetElements();  
    unsigned int offset = 0;  
    for (unsigned int i = 0; i < elements.size(); i++)  
    {  
        const auto& element = elements[i];  
        glEnableVertexAttribArray(i);  
        glVertexAttribPointer(i, element.count, element.type, element.normalized, layout.GetStride(), (void*)offset);
        offset += element.count * VertexBufferElement::GetStrideOfType(element.type);  
    }  
}
