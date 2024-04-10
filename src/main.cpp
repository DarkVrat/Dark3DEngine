#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <memory>

#include "Render/VertexArray.h"
#include "Render/ShaderProgram.h"
#include "Render/Texture2D.h"
#include "Render/Camera.h"

#define WINDOW_WIDTH 1280
#define WINDOW_HEIGHT 720

using namespace Render;

void sizeCallback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
    Camera::setScreenSize(glm::vec2(width, height));
}

void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    glm::vec2 dir(0.f, 0.f);
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
    {
        dir.x = 1.f;
    }
    else if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
    {
        dir.x = -1.f;
    }

    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
    {
        dir.y = -1.f;
        dir = glm::normalize(dir);
    }
    else if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
    {
        dir.y = 1.f;
        dir = glm::normalize(dir);
    }
    Camera::setDirection(dir);
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    Camera::updatePositionMouse(glm::vec2(xpos, ypos));
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    Camera::updateFov(yoffset);
}

int main()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

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
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetScrollCallback(window, scroll_callback);
    glfwSwapInterval(0);
    glfwWindowHint(GLFW_SAMPLES, 4);

    glClearColor(0.f, 0.f, 0.f, 1.0f);
    glEnable(GL_DEPTH_TEST);

    float vertices[] = {
        // positions          // normals           // texture coords
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,
         0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 0.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,

        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 0.0f,
         0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 1.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 0.0f,

        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
        -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,

         0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
         0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
         0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
         0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
         0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,

        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f,
         0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 1.0f,
         0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,
         0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 0.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f,

        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 1.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 0.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f
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

    glm::vec3 pointLightPositions[] = {
        glm::vec3(0.7f,  0.2f,  2.0f),
        glm::vec3(2.3f, -3.3f, -4.0f),
        glm::vec3(-4.0f,  2.0f, -12.0f),
        glm::vec3(0.0f,  0.0f, -3.0f)
    };

    glm::vec3 pointLightColor[] = {
        glm::vec3(1.0f, 1.0f, 1.0f),
        glm::vec3(1.0f, 1.0f, 1.0f),
        glm::vec3(1.0f, 1.0f, 1.0f),
        glm::vec3(1.0f, 1.0f, 1.0f)
    };

    std::shared_ptr<ShaderProgram> Shader = std::make_shared<ShaderProgram>("res/Shaders/VertexShader.glsl", "res/Shaders/FragmentShader.glsl");
    std::shared_ptr<ShaderProgram> lightShader = std::make_shared<ShaderProgram>("res/Shaders/VertexShader.glsl", "res/Shaders/LightFragmentShader.glsl");

    Camera::addShader(Shader);
    Camera::addShader(lightShader);
    Camera::setScreenSize(glm::vec2(WINDOW_WIDTH, WINDOW_HEIGHT));
    Camera::updatePositionMouse(glm::vec2(WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2));

    VertexBuffer VBO;
    VBO.initStatic(vertices, sizeof(vertices));
    
    VertexArray VAO;
    VAO.addBuffer(VBO, 0, 3, 5);
    VAO.addBuffer(VBO, 1, 3, 5, GL_FLOAT, (void*)(3 * sizeof(float)));
    VAO.addBuffer(VBO, 2, 2, 6, GL_FLOAT, (void*)(6 * sizeof(float)));

    VertexArray lightVAO;
    lightVAO.addBuffer(VBO, 0, 3, 5); 
    lightVAO.addBuffer(VBO, 1, 3, 5, GL_FLOAT, (void*)(3 * sizeof(float)));
    lightVAO.addBuffer(VBO, 2, 2, 6, GL_FLOAT, (void*)(6 * sizeof(float)));

    Texture2D texture1("res/Textures/container2.png");
    texture1.setActiveId(0);
    texture1.bind();
    Texture2D texture2("res/Textures/container2_specular.png");
    texture1.setActiveId(1);
    texture1.bind();

    Shader->use(); 
    Shader->setInt("material.diffuse", 0);
    Shader->setInt("material.specular", 1);
    Shader->setFloat("material.shininess", 32.0f);

    Shader->setVec3("dirLight.direction", glm::vec3(-0.2f, -1.0f, -0.3f));
    Shader->setVec3("dirLight.ambient", glm::vec3(0.05f, 0.05f, 0.05f));
    Shader->setVec3("dirLight.diffuse", glm::vec3(0.4f, 0.4f, 0.4f));
    Shader->setVec3("dirLight.specular", glm::vec3(0.5f, 0.5f, 0.5f));

    Shader->setVec3("pointLights[0].position", pointLightPositions[0]);
    Shader->setVec3("pointLights[0].ambient", glm::vec3(0.05f, 0.05f, 0.05f));
    Shader->setVec3("pointLights[0].diffuse", glm::vec3(0.8f, 0.8f, 0.8f));
    Shader->setVec3("pointLights[0].specular", glm::vec3(1.0f, 1.0f, 1.0f));
    Shader->setFloat("pointLights[0].constant", 1.0f);
    Shader->setFloat("pointLights[0].linear", 0.09f);
    Shader->setFloat("pointLights[0].quadratic", 0.032f);

    Shader->setVec3("pointLights[1].position", pointLightPositions[1]);
    Shader->setVec3("pointLights[1].ambient", glm::vec3(0.05f, 0.05f, 0.05f));
    Shader->setVec3("pointLights[1].diffuse", glm::vec3(0.8f, 0.8f, 0.8f));
    Shader->setVec3("pointLights[1].specular", glm::vec3(1.0f, 1.0f, 1.0f));
    Shader->setFloat("pointLights[1].constant", 1.0f);
    Shader->setFloat("pointLights[1].linear", 0.09f);
    Shader->setFloat("pointLights[1].quadratic", 0.032f);

    Shader->setVec3("pointLights[2].position", pointLightPositions[0]);
    Shader->setVec3("pointLights[2].ambient", glm::vec3(0.05f, 0.05f, 0.05f));
    Shader->setVec3("pointLights[2].diffuse", glm::vec3(0.8f, 0.8f, 0.8f));
    Shader->setVec3("pointLights[2].specular", glm::vec3(1.0f, 1.0f, 1.0f));
    Shader->setFloat("pointLights[2].constant", 1.0f);
    Shader->setFloat("pointLights[2].linear", 0.09f);
    Shader->setFloat("pointLights[2].quadratic", 0.032f);

    Shader->setVec3("pointLights[3].position", pointLightPositions[0]);
    Shader->setVec3("pointLights[3].ambient", glm::vec3(0.05f, 0.05f, 0.05f));
    Shader->setVec3("pointLights[3].diffuse", glm::vec3(0.8f, 0.8f, 0.8f));
    Shader->setVec3("pointLights[3].specular", glm::vec3(1.0f, 1.0f, 1.0f));
    Shader->setFloat("pointLights[3].constant", 1.0f);
    Shader->setFloat("pointLights[3].linear", 0.09f);
    Shader->setFloat("pointLights[3].quadratic", 0.032f);
    
    Shader->setVec3("flashLight.ambient", glm::vec3(1.0f, 1.0f, 1.0f));
    Shader->setVec3("flashLight.diffuse", glm::vec3(1.0f, 1.0f, 1.0f));
    Shader->setVec3("flashLight.specular", glm::vec3(1.0f, 1.0f, 1.0f));
    Shader->setFloat("flashLight.constant", 1.0f);
    Shader->setFloat("flashLight.linear", 0.09f);
    Shader->setFloat("flashLight.quadratic", 0.032f);
    Shader->setFloat("flashLight.cutOff", glm::cos(glm::radians(12.5f)));
    Shader->setFloat("flashLight.outerCutOff", glm::cos(glm::radians(15.0f)));

    float lastFrame = 0.0f;

    while (!glfwWindowShouldClose(window))
    {
        float currentFrame = static_cast<float>(glfwGetTime());
        float deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        
        processInput(window);

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); 

        Camera::updatePositionCamera(deltaTime);
        
        Shader->use();
        Shader->setVec3("flashLight.position", Camera::getPosition());
        Shader->setVec3("flashLight.front", Camera::getFront());

        VAO.bind();
        for (unsigned int i = 0; i < 10; i++)
        {
            glm::mat4 model = glm::mat4(1.0f);
            model = glm::translate(model, cubePositions[i]);
            float angle = 20.0f * i;
            model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
            Shader->setMatrix4("model", model);

            glDrawArrays(GL_TRIANGLES, 0, 36);
        }

        lightShader->use();
        lightVAO.bind();
        for (unsigned int i = 0; i < 4; i++)
        {
            glm::mat4 model = glm::mat4(1.0f);
            model = glm::translate(model, pointLightPositions[i]);
            model = glm::scale(model, glm::vec3(0.2f));
            lightShader->setMatrix4("model", model);
            lightShader->setVec3("colorLight", pointLightColor[i]);
            glDrawArrays(GL_TRIANGLES, 0, 36);
        }

        

        glfwSwapBuffers(window);
        glfwPollEvents();
    } 

    glfwTerminate();
    return 0;
}