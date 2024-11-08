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
#include "Render/PostProcessing.h"
#include "Render/ShadowMap.h"

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
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 4);
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

    glm::vec3 containersPositions[] = {
        glm::vec3(0.f,1.5f,0.f),
        glm::vec3(2.f,0.f,1.f),
        glm::vec3(-1.f,0.f,-2.f),
    };

    glm::vec3 containersScales[] = {
        glm::vec3(0.5f),
        glm::vec3(0.5f),
        glm::vec3(0.25f),
    };

    float rotateRadians[] = { 0.f, 0.f, 60.f };

    glm::vec3 containersRotates[] = {
        glm::vec3(1.f,0.f,0.f),
        glm::vec3(1.f,0.f,0.f),
        glm::vec3(1.f,0.f,1.f),
    };

    Managers::ResourceManager::loadResources("res/Resources.json");

    Camera::initMatricesUBO();
    Camera::setScreenSize(glm::vec2(windowSize.x, windowSize.y));
    Camera::updatePositionMouse(glm::vec2(windowSize.x / 2, windowSize.y / 2));

    std::shared_ptr<ShaderProgram> Shader = Managers::ResourceManager::getShader("shadow_shader");

    Shader->use();
    Shader->setFloat("material.shininess", 32.f);

    Shader->setVec3("dirLight.direction", glm::vec3(0.0f, -1.0f, 0.0f));
    Shader->setVec3("dirLight.ambient", glm::vec3(0.1f, 0.1f, 0.1f));
    Shader->setVec3("dirLight.diffuse", glm::vec3(0.9f, 0.9f, 0.9f));
    Shader->setVec3("dirLight.specular", glm::vec3(0.2f, 0.2f, 0.2f));

    float lastFrame = 0.0f;

    std::shared_ptr<Model> container = Managers::ResourceManager::getModel("container");
    std::shared_ptr<Model> floor = Managers::ResourceManager::getModel("floor");

    ShadowMap shadowMap;
    std::shared_ptr<ShaderProgram> depthShader = Managers::ResourceManager::getShader("shadow_depth");

    glm::vec3 lightPos(0.0f, 4.0f, 0.0f);

    float quadVertices[] = {
        // positions   // texture Coords
        -1.0f,  1.0f,  0.0f, 1.0f,  // Верхний левый угол
        -1.0f, -1.0f,  0.0f, 0.0f,  // Нижний левый угол
         1.0f,  1.0f,  1.0f, 1.0f,  // Верхний правый угол
         1.0f, -1.0f,  1.0f, 0.0f   // Нижний правый угол
    };

    // VBO и VAO для квадрата
    unsigned int quadVAO, quadVBO;
    glGenVertexArrays(1, &quadVAO);
    glGenBuffers(1, &quadVBO);
    glBindVertexArray(quadVAO);
    glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
    glBindVertexArray(0);

    std::shared_ptr<ShaderProgram> quadShader = Managers::ResourceManager::getShader("depth_debug");

    while (!glfwWindowShouldClose(window))
    {
        float currentFrame = static_cast<float>(glfwGetTime());
        float deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        
        processInput(window);

        glm::mat4 lightProjection, lightView;
        glm::mat4 lightSpaceMatrix;

        float near_plane = 0.0f, far_plane = 10.f;
        lightProjection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, near_plane, far_plane);

        glm::vec3 direction = glm::normalize(glm::vec3(sin(currentFrame / 8), -3.f, cos(currentFrame / 8)));
        glm::vec3 globalUp = glm::vec3(0.0f, 1.0f, 0.0f);
        glm::vec3 right = glm::normalize(glm::cross(globalUp, direction));
        glm::vec3 newUp = glm::normalize(glm::cross(direction, right));
        
        lightView = glm::lookAt(lightPos, lightPos+direction, newUp);
        lightSpaceMatrix = lightProjection * lightView;

        depthShader->use();
        depthShader->setMatrix4("lightSpaceMatrix", lightSpaceMatrix);

        shadowMap.bindDepthMap();

        {
            for (unsigned int i = 0; i < 3; i++)
            {
                glm::mat4 model = glm::mat4(1.0f);
                model = glm::translate(model, containersPositions[i]);
                model = glm::rotate(model, glm::radians(rotateRadians[i]), containersRotates[i]);
                model = glm::scale(model, containersScales[i]);
                depthShader->setMatrix4("model", model);
                container->Draw(depthShader);
            }

            glm::mat4 model = glm::mat4(1.0f);
            model = glm::translate(model, glm::vec3(0.f, -0.5f, 0.f));
            model = glm::rotate(model, glm::radians(90.f), glm::vec3(0, 0, 1));
            model = glm::scale(model, glm::vec3(4.f, 4.f, 4.f));
            depthShader->setMatrix4("model", model);
            floor->Draw(depthShader);
            shadowMap.unbindFramebuffer();
        }


        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
        glViewport(0, 0, windowSize.x, windowSize.y);

        Camera::updatePositionCamera(deltaTime);

        Shader->use();
        Shader->setVec3("dirLight.direction", direction);
        Shader->setMatrix4("lightSpaceMatrix", lightSpaceMatrix);
        Shader->setInt("shadowMap", 2);
        shadowMap.bindShadowMapTexture(2);


        for (unsigned int i = 0; i < 3; i++)
        {
            glm::mat4 model = glm::mat4(1.0f);
            model = glm::translate(model, containersPositions[i]);
            model = glm::rotate(model, glm::radians(rotateRadians[i]), containersRotates[i]);
            model = glm::scale(model, containersScales[i]);
            Shader->setMatrix4("model", model);
            container->Draw(Shader);
        }

        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.f, -0.5f, 0.f));
        model = glm::rotate(model, glm::radians(90.f), glm::vec3(0, 0, 1));
        model = glm::scale(model, glm::vec3(30.f));
        Shader->setMatrix4("model", model);
        floor->Draw(Shader);


        glViewport(0, 0, 200, 200);  // Размер карты теней на экране
        quadShader->use();
        quadShader->setInt("shadowMap", 1);
        glBindVertexArray(quadVAO);
        shadowMap.bindShadowMapTexture(1); // Привязка карты теней к текстурному блоку 1
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
        glBindVertexArray(0);

        // Вернуть нормальный вьюпорт для следующего рендера
        glViewport(0, 0, windowSize.x, windowSize.y);

        glfwSwapBuffers(window);
        glfwPollEvents();
    } 
    Managers::ResourceManager::clear();
    glfwTerminate();
    return 0;
}
