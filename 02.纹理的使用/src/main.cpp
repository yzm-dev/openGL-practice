#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <stb_image/stb_image.h>

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

float vertices[] = {
	// positions          // texture coords
	-0.5f, -0.5f, 0.0f,  0.0f, 0.0f,
	 0.5f, -0.5f, 0.0f,  1.0f, 0.0f,
	 0.5f,  0.5f, 0.0f,  1.0f, 1.0f,
	 -0.5f,  0.5f, 0.0f,  0.0f, 1.0f,
};

unsigned int indices[] = {
	0, 1, 2, // first triangle
	2, 3, 0  // second triangle
};

// Update the viewport when the window's framebuffer size changes
static void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

int main(void)
{
	GLFWwindow* window;

	/* Initialize the library */
	if (!glfwInit())
		return -1;

	/* Create a windowed mode window and its OpenGL context */
	window = glfwCreateWindow(1920, 1080, "Using Texture", nullptr, nullptr);
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

    // Set the initial viewport to the current framebuffer size (compatible with high DPI)
    int fbWidth = 0, fbHeight = 0;
    glfwGetFramebufferSize(window, &fbWidth, &fbHeight);
    glViewport(0, 0, fbWidth, fbHeight);
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
	// set vertex attributes pointers
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), nullptr);
	glEnableVertexAttribArray(0);
	// set texture coordinates attributes pointers
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	// create and compile the vertex shader and fragment shader
	std::string vertexShaderSource = ReadShaderSourceFromFile("resource/shaders/vertex_shader.glsl");
	std::string fragmentShaderSource = ReadShaderSourceFromFile("resource/shaders/fragment_shader.glsl");

	unsigned int vertexShader = CompileShader(GL_VERTEX_SHADER, vertexShaderSource);
	unsigned int fragmentShader = CompileShader(GL_FRAGMENT_SHADER, fragmentShaderSource);

	unsigned int shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);
	glLinkProgram(shaderProgram);
	

	std::string texturePath = "resource/textures/awesomeface.png";
	//std::string texturePath = "resource/textures/container.jpg";

	int width, height, nrChannels;
	stbi_set_flip_vertically_on_load(true); // flip the texture vertically
	unsigned char* textureData = stbi_load(texturePath.c_str(), &width, &height, &nrChannels, 0);

	// texture setup
	unsigned int texture;
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	int format = GL_RGB;
	if (nrChannels == 4)
		format = GL_RGBA;
	else if (nrChannels == 3)
		format = GL_RGB;
	else if (nrChannels == 1)
		format = GL_RED;

	glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, textureData);
	glGenerateMipmap(GL_TEXTURE_2D);

	if (textureData) stbi_image_free(textureData);

	
	glUseProgram(shaderProgram);
	unsigned int slot = 0; // texture unit 0
	glUniform1i(glGetUniformLocation(shaderProgram, "ourTexture"), slot); // set the texture unit to 0

	glActiveTexture(GL_TEXTURE0 + slot);
	glBindTexture(GL_TEXTURE_2D, texture);

	glBindVertexArray(0);
	glBindBuffer(VBO, 0);
	glBindBuffer(VBO, 0);

	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	/* Loop until the user closes the window */
	while (!glfwWindowShouldClose(window))
	{
		/* Render here */
		glClear(GL_COLOR_BUFFER_BIT);

		glUseProgram(shaderProgram);
		glBindVertexArray(VAO);
		glDrawElements(GL_TRIANGLES, 3 * 2, GL_UNSIGNED_INT, nullptr);

		/* Swap front and back buffers */
		glfwSwapBuffers(window);

		/* Poll for and process events */
		glfwPollEvents();
	}
	// Cleanup
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteTextures(1, &texture);
    glDeleteProgram(shaderProgram);
    glfwDestroyWindow(window);

	glfwTerminate();
	return 0;
}