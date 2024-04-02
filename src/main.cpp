#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "Render/VertexArray.h"
#include "Render/ShaderProgram.h"
#include "Render/Texture2D.h"

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 800

using namespace Render;

void sizeCallback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

void closeInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_F11) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

int main()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_SAMPLES, 8);

    GLFWwindow* window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Dark3DEngine", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "{E} Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "{E} Failed to initialize GLAD" << std::endl;
        return -1;
    }

    glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
    glfwSetFramebufferSizeCallback(window, sizeCallback);

    float vertices[] = {
        // positions        // texture coords
         0.5f,  0.5f, 0.0f, 1.0f, 1.0f,   // top right
         0.5f, -0.5f, 0.0f, 1.0f, 0.0f,   // bottom right
        -0.5f, -0.5f, 0.0f, 0.0f, 0.0f,   // bottom left
        -0.5f,  0.5f, 0.0f, 0.0f, 1.0f    // top left 
    };

    unsigned int indices[] = {  // note that we start from 0!
        0, 1, 3,   // first triangle
        1, 2, 3    // second triangle
    };

    ShaderProgram Shader("res/Shaders/VertexShader.glsl", "res/Shaders/FragmentShader.glsl");
    Shader.use();
    Shader.setInt("texture1", 0);
    Shader.setInt("texture2", 1);

    VertexBuffer VBO;
    VBO.initStatic(vertices, sizeof(vertices));

    VertexBuffer EBO(GL_ELEMENT_ARRAY_BUFFER);
    EBO.initStatic(indices, sizeof(indices));
    
    VertexArray VAO;
    VAO.addBuffer(VBO, 0, 3, 2);
    VAO.addBuffer(VBO, 1, 2, 3, GL_FLOAT, (void*)(sizeof(float) * 3));

    EBO.bind();

    Texture2D texture1("res/Textures/container.png");
    texture1.setActiveId(0);
    Texture2D texture2("res/Textures/awesomeface.png");
    texture2.setActiveId(1);

    texture1.bind();
    texture2.bind();

    while (!glfwWindowShouldClose(window))
    {
        glm::mat4 trans = glm::mat4(1.0f);
        trans = glm::translate(trans, glm::vec3(0.5f, -0.5f, 0.0f));
        trans = glm::rotate(trans, (float)glfwGetTime(), glm::vec3(0.0f, 0.0f, 1.0f));
        Shader.setMatrix4("transform", trans);

        closeInput(window);

        glClearColor(0.1f, 0.2f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT); 
        
        
        Shader.use();
        VAO.bind();
        
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0); 

        glfwSwapBuffers(window);
        glfwPollEvents();
    } 

    glfwTerminate();
    return 0;
}