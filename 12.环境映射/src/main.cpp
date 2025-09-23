#include <iostream>
#include <string>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <stb_image/stb_image.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "VertexArray.h"
#include "VertexBufferLayout.h"
#include "Shader.h"
#include "Camera.h"
#include "Model.h"
#include "Renderer.h"

float skyboxVertices[] = {
    // positions          
    -1.0f,  1.0f, -1.0f,
    -1.0f, -1.0f, -1.0f,
     1.0f, -1.0f, -1.0f,
     1.0f, -1.0f, -1.0f,
     1.0f,  1.0f, -1.0f,
    -1.0f,  1.0f, -1.0f,

    -1.0f, -1.0f,  1.0f,
    -1.0f, -1.0f, -1.0f,
    -1.0f,  1.0f, -1.0f,
    -1.0f,  1.0f, -1.0f,
    -1.0f,  1.0f,  1.0f,
    -1.0f, -1.0f,  1.0f,

     1.0f, -1.0f, -1.0f,
     1.0f, -1.0f,  1.0f,
     1.0f,  1.0f,  1.0f,
     1.0f,  1.0f,  1.0f,
     1.0f,  1.0f, -1.0f,
     1.0f, -1.0f, -1.0f,

    -1.0f, -1.0f,  1.0f,
    -1.0f,  1.0f,  1.0f,
     1.0f,  1.0f,  1.0f,
     1.0f,  1.0f,  1.0f,
     1.0f, -1.0f,  1.0f,
    -1.0f, -1.0f,  1.0f,

    -1.0f,  1.0f, -1.0f,
     1.0f,  1.0f, -1.0f,
     1.0f,  1.0f,  1.0f,
     1.0f,  1.0f,  1.0f,
    -1.0f,  1.0f,  1.0f,
    -1.0f,  1.0f, -1.0f,

    -1.0f, -1.0f, -1.0f,
    -1.0f, -1.0f,  1.0f,
     1.0f, -1.0f, -1.0f,
     1.0f, -1.0f, -1.0f,
    -1.0f, -1.0f,  1.0f,
     1.0f, -1.0f,  1.0f
};

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow* window);

unsigned int loadCubemap(std::vector<std::string> faces);

// settings
const unsigned int SCR_WIDTH = 1920;
const unsigned int SCR_HEIGHT = 1080;

// camera
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

// timing
float deltaTime = 0.0f;	// time between current frame and last frame
float lastFrame = 0.0f;

int main(void)
{
    GLFWwindow* window;

    /* Initialize the library */
    if (!glfwInit())
        return -1;
    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Enviroment Mapping", nullptr, nullptr);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    /* Make the window's context current */
    glfwMakeContextCurrent(window);

    /* Initialize GLEW */
    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK) {
        std::cerr << "GLEW init failed\n";
        return -1;
    }

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);

    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

    // tell GLFW to capture our mouse
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    VertexArray skyboxVAO;
    VertexBuffer skyboxVBO(skyboxVertices, sizeof(skyboxVertices));
    VertexBufferLayout skyboxLayout;
    skyboxLayout.Push<float>(3); // position
    skyboxVAO.AddBuffer(skyboxVBO, skyboxLayout);

    // create and compile the vertex shader and fragment shader for cube and plane
    std::string vertexShaderSource = "resource/shaders/vertex_shader.glsl";
    std::string fragmentShaderSource = "resource/shaders/fragment_shader.glsl";

    std::string skyboxVertexShaderSource = "resource/shaders/skybox_vertex_shader.glsl";
    std::string skyboxFragmentShaderSource = "resource/shaders/skybox_fragment_shader.glsl";

    std::string reflectShaderSource = "resource/shaders/reflect_vertex_shader.glsl";
    std::string reflectFragmentShaderSource = "resource/shaders/reflect_fragment_shader.glsl";

    std::string refractShaderSource = "resource/shaders/refract_vertex_shader.glsl";
    std::string refractFragmentShaderSource = "resource/shaders/refract_fragment_shader.glsl";

    Shader shader = Shader(vertexShaderSource, fragmentShaderSource);
    Shader skyboxShader = Shader(skyboxVertexShaderSource, skyboxFragmentShaderSource);
    Shader reflectShader = Shader(reflectShaderSource, reflectFragmentShaderSource);
    Shader refractShader = Shader(refractShaderSource, refractFragmentShaderSource);

    // load models
    Model ourModel("assets/nanosuit_reflection/nanosuit.blend");

    // load textures
    std::vector<std::string> faces
    {
        "resource/textures/skybox/right.jpg", // +X (right)
        "resource/textures/skybox/left.jpg", // -X (left)
        "resource/textures/skybox/top.jpg", // +Y (top)
        "resource/textures/skybox/bottom.jpg", // -Y (bottom)
        "resource/textures/skybox/front.jpg", // +Z (front) 
        "resource/textures/skybox/back.jpg" // -Z (back)
    };
    unsigned int cubemapTexture = loadCubemap(faces);

    // shader configuration
    shader.BindShader();
    reflectShader.SetUniform1i("skybox", 2);

    reflectShader.BindShader();
    reflectShader.SetUniform1i("skybox", 2);

    refractShader.BindShader();
    refractShader.SetUniform1i("skybox", 2);

    skyboxShader.BindShader();
    skyboxShader.SetUniform1i("skybox", 2);

    skyboxVAO.UnBindVertexArray();
    skyboxVBO.UnBindVertexBuffer();
    shader.UnBindShader();
    reflectShader.UnBindShader();
    refractShader.UnBindShader();
    skyboxShader.UnBindShader();

    Renderer renderer;
    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {
        // per-frame time logic
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // input
        processInput(window);

        /* Render here */
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // render the loaded origin model 1
        shader.BindShader();
        reflectShader.SetUniform3fv("cameraPos", camera.Position);
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.0f, -10.0f, -15.0f)); // translate it down so it's at the center of the scene
        model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));	// it's a bit too big for our scene, so scale it down
        glm::mat4 view = camera.GetViewMatrix();
        glm::mat4 projection = glm::perspective(glm::radians(camera.Fov), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        shader.SetUniformMat4fv("model", model);
        shader.SetUniformMat4fv("view", view);
        shader.SetUniformMat4fv("projection", projection);
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture); // TEXTURE_CUBE_MAP
        ourModel.Draw(reflectShader);
        glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

        // render the loaded reflect model 2
        reflectShader.BindShader();
        reflectShader.SetUniform3fv("cameraPos", camera.Position);
        model = glm::translate(model, glm::vec3(10.0f, 0.0f, 0.0f));
        reflectShader.SetUniformMat4fv("model", model);
        reflectShader.SetUniformMat4fv("view", view);
        reflectShader.SetUniformMat4fv("projection", projection);
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture); // TEXTURE_CUBE_MAP
        ourModel.Draw(reflectShader);
        glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

        // render the loaded refract model 3
        refractShader.BindShader();
        refractShader.SetUniform3fv("cameraPos", camera.Position);
        model = glm::translate(model, glm::vec3(10.0f, 0.0f, 0.0f));
        refractShader.SetUniformMat4fv("model", model);
        refractShader.SetUniformMat4fv("view", view);
        refractShader.SetUniformMat4fv("projection", projection);
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture); // TEXTURE_CUBE_MAP
        ourModel.Draw(refractShader);
        glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

        // draw skybox as last
        glDepthFunc(GL_LEQUAL);  // change depth function so depth test passes when values are equal to depth buffer's content
        skyboxShader.BindShader();
        view = glm::mat4(glm::mat3(camera.GetViewMatrix())); // remove translation from the view matrix
        skyboxShader.SetUniformMat4fv("view", view);
        skyboxShader.SetUniformMat4fv("projection", projection);

        // skybox cube
        skyboxVAO.BindVertexArray();
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture); // TEXTURE_CUBE_MAP
        renderer.DrawArrays(GL_TRIANGLES, 0, 36);
        glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
        glDepthFunc(GL_LESS); // set depth function back to default

        /* Swap front and back buffers */
        glfwSwapBuffers(window);

        /* Poll for and process events */
        glfwPollEvents();
    }
    glfwDestroyWindow(window);

    glfwTerminate();
    return 0;
}

// loads a cubemap texture from 6 individual texture faces
unsigned int loadCubemap(std::vector<std::string> faces)
{
    unsigned int textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);
    int width, height, nrChannels;
    // cubemap loading should not be flipped
    stbi_set_flip_vertically_on_load(0);
    for (unsigned int i = 0; i < faces.size(); i++)
    {
        unsigned char* data = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 0);
        if (data)
        {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
            stbi_image_free(data);
        }
        else
        {
            std::cout << "Cubemap texture failed to load at path: " << faces[i] << std::endl;
            stbi_image_free(data);
        }
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    return textureID;
}

void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(RIGHT, deltaTime);
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}

// glfw: whenever the mouse moves, this callback is called
void mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
{
    float xpos = static_cast<float>(xposIn);
    float ypos = static_cast<float>(yposIn);

    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

    lastX = xpos;
    lastY = ypos;

    camera.ProcessMouseMovement(xoffset, yoffset);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    camera.ProcessMouseScroll(static_cast<float>(yoffset));
}