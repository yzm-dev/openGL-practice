#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

static std::string ReadShaderSourceFromFile(const std::string& filePath)
{
	std::ifstream file(filePath);
	if (!file.is_open()) {
		std::cerr << "Not opening this shader file!" << filePath << std::endl;
		return "";
	}
	std::stringstream buffer;
	buffer << file.rdbuf();
	return buffer.str();
}

static unsigned int CompileShader(GLenum shaderType, const std::string& source)
{
	unsigned int shader = glCreateShader(shaderType);
	const char* src = source.c_str();
	glShaderSource(shader, 1, &src, nullptr);
	glCompileShader(shader);

	int result;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &result);
	if (result == GL_FALSE)
	{
		int length;
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &length);
		char* infoLog = new char[length];
		glGetShaderInfoLog(shader, length, &length, infoLog);
		std::cerr << (shaderType == GL_VERTEX_SHADER ? "Vertex" : "Fragment") << "Shader compilation failed: " << infoLog << std::endl;
		delete[] infoLog;
		glDeleteShader(shader);
		return 0;
	}

	return shader;
}

// Update the viewport when the window's framebuffer size changes
static void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

float vertices[] = {
	-0.5f, -0.5f, 0.0f,
	 0.5f, -0.5f, 0.0f,
	 0.0f,  0.5f, 0.0f
};

unsigned int indices[] = {
	0, 1, 2
};

int main(void)
{
	GLFWwindow* window;

	/* Initialize the library */
	if (!glfwInit())
		return -1;

	/* Create a windowed mode window and its OpenGL context */
	window = glfwCreateWindow(1920, 1080, "Drawing a triangle", nullptr, nullptr);
	if (!window)
	{
		glfwTerminate();
		return -1;
	}

	/* Make the window's context current */
	glfwMakeContextCurrent(window);

	/* Initialize GLEW */
	if (glewInit() != GLEW_OK) {
		std::cerr << "GLEW init failed\n";
		return -1;
	}

    // Register a framebuffer size change callback to update the viewport when the window is resized
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	unsigned int VAO, VBO, EBO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	// create and compile the vertex shader and fragment shader
	std::string vertexShaderSource = ReadShaderSourceFromFile("resource/shaders/vertex_shader.glsl");
	std::string fragmentShaderSource = ReadShaderSourceFromFile("resource/shaders/fragment_shader.glsl");

	unsigned int vertexShader = CompileShader(GL_VERTEX_SHADER, vertexShaderSource);
	unsigned int fragmentShader = CompileShader(GL_FRAGMENT_SHADER, fragmentShaderSource);

	unsigned int shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);
	glLinkProgram(shaderProgram);

	glBindVertexArray(0);
	glBindBuffer(VBO, 0);
	glBindBuffer(EBO, 0);

	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);


	/* Loop until the user closes the window */
	while (!glfwWindowShouldClose(window))
	{
		/* Render here */
		glClear(GL_COLOR_BUFFER_BIT);

		glUseProgram(shaderProgram);
		glBindVertexArray(VAO);

		//glDrawArrays(GL_TRIANGLES, 0, 3);
		glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT,nullptr);

		/* Swap front and back buffers */
		glfwSwapBuffers(window);

		/* Poll for and process events */
		glfwPollEvents();
	}
	// Cleanup
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteProgram(shaderProgram);
	glfwDestroyWindow(window);

	glfwTerminate();
	return 0;
}