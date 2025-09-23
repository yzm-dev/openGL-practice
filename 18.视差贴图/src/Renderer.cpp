#include "Renderer.h"
#include <iostream>

void GLClearError()
{
	while (glGetError() != GL_NO_ERROR);
}

bool GLErrorLogCall_(const char* func, const char* file, int line)
{
	while (GLenum error = glGetError())
	{
		std::cout << "[OpenGL Error]: " << error << " in " << func << " at " << file << ": line " << line << std::endl;
		return true;
	}
	return false;
}

void APIENTRY debugCallback(GLenum source, GLenum type, GLuint id,
	GLenum severity, GLsizei length,
	const GLchar* message, const void* userParam) {
	std::cerr << "GL Debug: " << message << std::endl;
}

void Renderer::Clear() const
{

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Renderer::DrawElements(const IndexBuffer& indexBuffer) const
{
	glDrawElements(GL_TRIANGLES, indexBuffer.GetCount(), GL_UNSIGNED_INT, (void*)0);
}

void Renderer::DrawArrays(const GLenum &mode, const GLint &first, const GLsizei &count)
{
    glDrawArrays(mode, first, count);
}
