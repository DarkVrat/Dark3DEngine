#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <memory>

#include "Render/VertexArray.h"
#include "Render/ShaderProgram.h"
#include "Render/Model.h"
#include "Render/Camera.h"
#include "Manager/ResourceManager.h"
#include "Manager/ConfigManager.h"

extern "C" {
    _declspec(dllexport) unsigned long NvOptimusEnablement = 0x00000001;
    _declspec(dllexport) int AmdPowerXpressRequestHighPerformance = 1;
}

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

    Managers::ConfigManager::loadConfig();
    glm::ivec2 windowSize = Managers::ConfigManager::getWindowSize();

    GLFWwindow* window = glfwCreateWindow(windowSize.x, windowSize.y, "Dark3DEngine", NULL, NULL);
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

    glViewport(0, 0, windowSize.x, windowSize.y);
    glfwSetFramebufferSizeCallback(window, sizeCallback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetScrollCallback(window, scroll_callback);
    glfwSwapInterval(Managers::ConfigManager::getVSync());
   
    glEnable(GL_MULTISAMPLE);
    glfwWindowHint(GLFW_SAMPLES, Managers::ConfigManager::getSamples()); 

    glClearColor(0.f, 0.f, 0.f, 1.0f);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_CULL_FACE);

    glm::vec3 pointLightPositions[] = {
        glm::vec3(1.f,  0.7f,  0.2f),
        glm::vec3(2.3f, -3.3f, -4.0f),
        glm::vec3(-4.0f,  2.0f, -12.0f),
        glm::vec3(0.0f,  0.0f, -3.0f)
    };

    glm::vec3 pointLightColor[] = {
        glm::vec3(1.0f, 0.1f, 0.1f),
        glm::vec3(1.0f, 1.0f, 1.0f),
        glm::vec3(1.0f, 1.0f, 1.0f),
        glm::vec3(1.0f, 1.0f, 1.0f)
    };

    Managers::ResourceManager::loadResources("res/Resources.json");

    Camera::setScreenSize(glm::vec2(windowSize.x, windowSize.y));
    Camera::updatePositionMouse(glm::vec2(windowSize.x / 2, windowSize.y / 2));

    std::shared_ptr<ShaderProgram> Shader = Managers::ResourceManager::getShader("basic_shader");

    Shader->use();
    Shader->setFloat("material.shininess", 32.f);

    Shader->setVec3("dirLight.direction", glm::vec3(0.0f, -1.0f, 0.0f));
    Shader->setVec3("dirLight.ambient", glm::vec3(0.05f, 0.05f, 0.05f));
    Shader->setVec3("dirLight.diffuse", glm::vec3(0.4f, 0.4f, 0.4f));
    Shader->setVec3("dirLight.specular", glm::vec3(0.5f, 0.5f, 0.5f));

    Shader->setVec3("pointLights[0].position", pointLightPositions[0]);
    Shader->setVec3("pointLights[0].ambient", glm::vec3(0.05f, 0.05f, 0.05f) * pointLightColor[0]);
    Shader->setVec3("pointLights[0].diffuse", glm::vec3(0.8f, 0.8f, 0.8f) * pointLightColor[0]);
    Shader->setVec3("pointLights[0].specular", glm::vec3(1.0f, 1.0f, 1.0f) * pointLightColor[0]);
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
    
    Shader->setVec3("flashLight.ambient", glm::vec3(0.0f, 0.0f, 0.0f));
    Shader->setVec3("flashLight.diffuse", glm::vec3(1.0f, 1.0f, 1.0f));
    Shader->setVec3("flashLight.specular", glm::vec3(1.0f, 1.0f, 1.0f));
    Shader->setFloat("flashLight.constant", 1.0f);
    Shader->setFloat("flashLight.linear", 0.09f);
    Shader->setFloat("flashLight.quadratic", 0.032f);
    Shader->setFloat("flashLight.cutOff", glm::cos(glm::radians(12.5f)));
    Shader->setFloat("flashLight.outerCutOff", glm::cos(glm::radians(15.0f)));

    float lastFrame = 0.0f;

    std::shared_ptr<Model> backpack = Managers::ResourceManager::getModel("backpack");
    std::shared_ptr<Model> container = Managers::ResourceManager::getModel("container");

    while (!glfwWindowShouldClose(window))
    {
        float currentFrame = static_cast<float>(glfwGetTime());
        float deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        
        processInput(window);

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

        Camera::updatePositionCamera(deltaTime);

        Shader->use();
        Shader->setVec3("flashLight.position", Camera::getPosition());
        Shader->setVec3("flashLight.front", Camera::getFront());

        for (unsigned int i = 0; i < 4; i++)
        {
            glm::mat4 model = glm::mat4(1.0f);
            model = glm::translate(model, pointLightPositions[i]);
            model = glm::scale(model, glm::vec3(0.1f));
            Shader->setMatrix4("model", model);
            container->Draw(Shader);
        }

        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
        model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
        Shader->setMatrix4("model", model);
        backpack->Draw(Shader);

        glfwSwapBuffers(window);
        glfwPollEvents();
    } 
    Managers::ResourceManager::clear();
    glfwTerminate();
    return 0;
}
