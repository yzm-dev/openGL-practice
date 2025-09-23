#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <stb_image/stb_image.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

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

// Camera settings
glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 5.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
// Timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;

void processInput(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
	float cameraSpeed = static_cast<float>(3.5 * deltaTime);
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		cameraPos += cameraSpeed * cameraFront;
		//std::cout << "W pressed" << std::endl;
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		cameraPos -= cameraSpeed * cameraFront;
		//std::cout << "S pressed" << std::endl;
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
		//std::cout << "A pressed" << std::endl;
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
		//std::cout << "D pressed" << std::endl;
}

// Update the viewport when the window's framebuffer size changes
static void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

float vertices[] = {
	// positions          // texture coords
	-0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
	 0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
	 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
	 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
	-0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
	-0.5f, -0.5f, -0.5f,  0.0f, 0.0f,

	-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
	 0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
	 0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
	 0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
	-0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
	-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,

	-0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
	-0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
	-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
	-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
	-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
	-0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

	 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
	 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
	 0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
	 0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
	 0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
	 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

	-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
	 0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
	 0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
	 0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
	-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
	-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,

	-0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
	 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
	 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
	 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
	-0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
	-0.5f,  0.5f, -0.5f,  0.0f, 1.0f
};

glm::vec3 cubePositions[] = {
  glm::vec3(0.0f,  0.0f,  0.0f),
  glm::vec3(2.0f,  5.0f, -15.0f),
  glm::vec3(-1.5f, -2.2f, -2.5f),
  glm::vec3(-3.8f, -2.0f, -12.3f),
  glm::vec3(2.4f, -0.4f, -3.5f),
  glm::vec3(-1.7f,  3.0f, -7.5f),
  glm::vec3(1.3f, -2.0f, -2.5f),
  glm::vec3(1.5f,  2.0f, -2.5f),
  glm::vec3(1.5f,  0.2f, -1.5f),
  glm::vec3(-1.3f,  1.0f, -1.5f)
};

int main(void)
{
	GLFWwindow* window;

	/* Initialize the library */
	if (!glfwInit())
		return -1;
    int SCR_WIDTH = 1920;
    int SCR_HEIGHT = 1080;
    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Using Transform Coordinate System", nullptr, nullptr);
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

	glEnable(GL_DEPTH_TEST);

	unsigned int VAO, VBO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);

	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
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

	std::string texturePath_1 = "resource/textures/awesomeface.png";
	std::string texturePath_2 = "resource/textures/container.jpg";
	// Load the texture_1 from file
	int width_1, height_1, nrChannels_1;
	stbi_set_flip_vertically_on_load(true); // flip the texture vertically
	unsigned char* textureData_1 = stbi_load(texturePath_1.c_str(), &width_1, &height_1, &nrChannels_1, 0);

	unsigned int texture_1;
	glGenTextures(1, &texture_1);
	glBindTexture(GL_TEXTURE_2D, texture_1);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	int format_1 = GL_RGBA;
	if (nrChannels_1 == 4)
		format_1 = GL_RGBA;
	else if (nrChannels_1 == 3)
		format_1 = GL_RGB;
	else if (nrChannels_1 == 1)
		format_1 = GL_RED;

	glTexImage2D(GL_TEXTURE_2D, 0, format_1, width_1, height_1, 0, format_1, GL_UNSIGNED_BYTE, textureData_1);
	glGenerateMipmap(GL_TEXTURE_2D);

	if (textureData_1) stbi_image_free(textureData_1);

	// Load the texture_2 from file
	int width_2, height_2, nrChannels_2;
	stbi_set_flip_vertically_on_load(true); // flip the texture vertically
	unsigned char* textureData_2 = stbi_load(texturePath_2.c_str(), &width_2, &height_2, &nrChannels_2, 0);

	unsigned int texture_2;
	glGenTextures(1, &texture_2);
	glBindTexture(GL_TEXTURE_2D, texture_2);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	int format_2 = GL_RGB;
	if (nrChannels_2 == 4)
		format_2 = GL_RGBA;
	else if (nrChannels_2 == 3)
		format_2 = GL_RGB;
	else if (nrChannels_2 == 1)
		format_2 = GL_RED;

	glTexImage2D(GL_TEXTURE_2D, 0, format_2, width_2, height_2, 0, format_2, GL_UNSIGNED_BYTE, textureData_2);
	glGenerateMipmap(GL_TEXTURE_2D);

	if (textureData_2) stbi_image_free(textureData_2);

	glUseProgram(shaderProgram);
	unsigned int slot_1 = 0; // texture unit 0
	unsigned int slot_2 = 1; // texture unit 1
	glUniform1i(glGetUniformLocation(shaderProgram, "ourTexture1"), slot_1); // set the texture unit to 0
	glUniform1i(glGetUniformLocation(shaderProgram, "ourTexture2"), slot_2); // set the texture unit to 1

	glActiveTexture(GL_TEXTURE0 + slot_1);
	glBindTexture(GL_TEXTURE_2D, texture_1);
	glActiveTexture(GL_TEXTURE0 + slot_2);
	glBindTexture(GL_TEXTURE_2D, texture_2);

	glBindVertexArray(0);
	glBindBuffer(VBO, 0);
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	/* Loop until the user closes the window */
	while (!glfwWindowShouldClose(window))
	{
		float currentFrame = static_cast<float>(glfwGetTime());
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		// input
		processInput(window);

		/* Render here */
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glUseProgram(shaderProgram);
		glBindVertexArray(VAO);
		glm::mat4 model;
		for (unsigned int i = 0; i < 10; i++)
		{
			model = glm::translate(glm::mat4(1.0f), cubePositions[i]);
			float angle = 20.0f * i;
			model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
			glm::mat4 view = glm::lookAt(cameraPos, cameraFront, cameraUp);
			glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)SCR_WIDTH/ SCR_HEIGHT, 0.1f, 100.0f);

			glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));
			glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "view"), 1, GL_FALSE, glm::value_ptr(view));
			glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

			glDrawArrays(GL_TRIANGLES, 0, 36);
		}

		/* Swap front and back buffers */
		glfwSwapBuffers(window);

		/* Poll for and process events */
		glfwPollEvents();
	}
	// Cleanup
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
    glDeleteTextures(1, &texture_1);
    glDeleteTextures(1, &texture_2);
	glDeleteProgram(shaderProgram);
	glfwDestroyWindow(window);

	glfwTerminate();
	return 0;
}