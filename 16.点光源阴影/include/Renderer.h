#pragma once

#include "VertexArray.h"
#include "Shader.h"
#include "IndexBuffer.h"

#include <GL/glew.h>
#include <GLFW/glfw3.h>



#define ASSERT(x) if (!(x)) __debugbreak();
#define GLErrorLogCall(x) { GLClearError();\
    x;\
    ASSERT(!GLErrorLogCall_(#x, __FILE__, __LINE__))}

void GLClearError();

bool GLErrorLogCall_(const char* func, const char* file, int line);

//glEnable(GL_DEBUG_OUTPUT);
//glDebugMessageCallback(debugCallback, nullptr);
void APIENTRY debugCallback(GLenum source, GLenum type, GLuint id,GLenum severity, GLsizei length, const GLchar * message, const void* userParam);

class Renderer
{
public:
    void Clear() const;
    void DrawElements(const IndexBuffer& indexBuffer) const;
    void DrawArrays(const GLenum &mode, const GLint &first, const GLsizei &count);
};

