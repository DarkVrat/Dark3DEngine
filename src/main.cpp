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
#include "Render/ShadowPoint.h"

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

    glm::vec3 contanersPositions[] = {
        glm::vec3(4.f, -3.5f, 0.f),
        glm::vec3(2.f, 3.f, 1.f),
        glm::vec3(-3.f, -1.f, 0.f),
        glm::vec3(-1.5f, 1.f, 1.5f),
        glm::vec3(-1.5f, 2.f, -3.f)
    };

    glm::vec3 contanersScale[] = {
        glm::vec3(0.5f),
        glm::vec3(0.75f),
        glm::vec3(0.5f),
        glm::vec3(0.5f),
        glm::vec3(0.75f)
    };

    float contanersRotate[] = {
        0.f,
        0.f,
        0.f,
        0.f,
        glm::radians(60.0f)
    };

    glm::vec3 floorRotate[] = {
        glm::vec3(0, 0, 1),
        glm::vec3(0, 1, 0),
        glm::vec3(1, 0, 0),
    };

    Managers::ResourceManager::loadResources("res/Resources.json");

    Camera::initMatricesUBO();
    Camera::setScreenSize(glm::vec2(windowSize.x, windowSize.y));
    Camera::updatePositionMouse(glm::vec2(windowSize.x / 2, windowSize.y / 2));

    std::shared_ptr<ShaderProgram> Shader = Managers::ResourceManager::getShader("shadow_point_shader");
    std::shared_ptr<ShaderProgram> depthShader = Managers::ResourceManager::getShader("point_shadows_depth");

    glm::vec3 pointPos = glm::vec3(0.f, 0.f, 0.f);

    Shader->use();
    Shader->setFloat("material.shininess", 32.f);

    Shader->setVec3("pointLights[0].position", pointPos);
    Shader->setVec3("pointLights[0].ambient", glm::vec3(0.2f, 0.2f, 0.2f));
    Shader->setVec3("pointLights[0].diffuse", glm::vec3(0.8f, 0.8f, 0.8f));
    Shader->setVec3("pointLights[0].specular", glm::vec3(1.0f, 1.0f, 1.0f));
    Shader->setFloat("pointLights[0].constant", 1.0f);
    Shader->setFloat("pointLights[0].linear", 0.045f);
    Shader->setFloat("pointLights[0].quadratic", 0.0075f);

    float far_plane = 25.0f;

    Shader->setFloat("far_plane", far_plane);
    Shader->setInt("shadows", true);

    float lastFrame = 0.0f;

    std::shared_ptr<Model> container = Managers::ResourceManager::getModel("container");
    std::shared_ptr<Model> floor = Managers::ResourceManager::getModel("floor");
    std::shared_ptr<Model> modern = Managers::ResourceManager::getModel("modern-bloc");

    Render::PostProcessing::init();

    Render::ShadowPoint shadowPoint;

    float near_plane = 1.0f;
    glm::mat4 shadowProj = glm::perspective(glm::radians(90.0f), 1.0f, near_plane, far_plane);

    std::vector<glm::mat4> shadowTransforms;
    shadowTransforms.push_back(shadowProj * glm::lookAt(pointPos, pointPos + glm::vec3(1.0, 0.0, 0.0), glm::vec3(0.0, -1.0, 0.0)));
    shadowTransforms.push_back(shadowProj * glm::lookAt(pointPos, pointPos + glm::vec3(-1.0, 0.0, 0.0), glm::vec3(0.0, -1.0, 0.0)));
    shadowTransforms.push_back(shadowProj * glm::lookAt(pointPos, pointPos + glm::vec3(0.0, 1.0, 0.0), glm::vec3(0.0, 0.0, 1.0)));
    shadowTransforms.push_back(shadowProj * glm::lookAt(pointPos, pointPos + glm::vec3(0.0, -1.0, 0.0), glm::vec3(0.0, 0.0, -1.0)));
    shadowTransforms.push_back(shadowProj * glm::lookAt(pointPos, pointPos + glm::vec3(0.0, 0.0, 1.0), glm::vec3(0.0, -1.0, 0.0)));
    shadowTransforms.push_back(shadowProj * glm::lookAt(pointPos, pointPos + glm::vec3(0.0, 0.0, -1.0), glm::vec3(0.0, -1.0, 0.0)));

    depthShader->use();

    for (unsigned int i = 0; i < 6; ++i) {
        depthShader->setMatrix4("shadowMatrices[" + std::to_string(i) + "]", shadowTransforms[i]);
    }

    depthShader->setFloat("far_plane", far_plane);
    depthShader->setVec3("lightPos", pointPos);

    while (!glfwWindowShouldClose(window))
    {
        float currentFrame = static_cast<float>(glfwGetTime());
        float deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        processInput(window);

        Camera::updatePositionCamera(deltaTime);

        shadowPoint.bindDepthMap();

        depthShader->use(); 

        { // Scene
            for (unsigned int i = 0; i < 5; i++)
            { 
                glm::mat4 model = glm::mat4(1.0f);
                model = glm::translate(model, contanersPositions[i]);
                model = glm::rotate(model, contanersRotate[i]*currentFrame/3, glm::normalize(glm::vec3(1.0, 0.0, 1.0)));
                model = glm::scale(model, contanersScale[i]);
                depthShader->setMatrix4("model", model);
                container->Draw(depthShader);
            }

            for (unsigned int i = 0; i < 3; i++)
            {
                glm::mat4 model = glm::mat4(1.0f);
                model = glm::rotate(model, glm::radians(90.f), floorRotate[i]);
                model = glm::scale(model, glm::vec3(5.f));

                model = glm::translate(model, glm::vec3(-1.f, 0.f, 0.f));
                depthShader->setMatrix4("model", model);
                floor->Draw(depthShader);

                model = glm::translate(model, glm::vec3(2.f, 0.f, 0.f));
                depthShader->setMatrix4("model", model);
                floor->Draw(depthShader);
            }
        }
         
        shadowPoint.unbindFramebuffer();
        
        Render::PostProcessing::bind();

        Shader->use();
        shadowPoint.bindShadowMapTexture(15);
        Shader->setInt("depthMap", 15); 
        
        { // Scene
            glm::mat4 model = glm::mat4(1.0f);
            model = glm::translate(model, pointPos);
            model = glm::scale(model, glm::vec3(0.03f));
            Shader->setMatrix4("model", model);
            container->Draw(Shader);

            for (unsigned int i = 0; i < 5; i++)
            {
                glm::mat4 model = glm::mat4(1.0f);
                model = glm::translate(model, contanersPositions[i]);
                model = glm::rotate(model, contanersRotate[i] * currentFrame/3, glm::normalize(glm::vec3(1.0, 0.0, 1.0)));
                model = glm::scale(model, contanersScale[i]);
                Shader->setMatrix4("model", model);
                modern->Draw(Shader);
            }
             
            for (unsigned int i = 0; i < 3; i++)
            {
                glm::mat4 model = glm::mat4(1.0f);
                model = glm::rotate(model, glm::radians(90.f), floorRotate[i]);
                model = glm::scale(model, glm::vec3(5.f));
                 
                model = glm::translate(model, glm::vec3(-1.f, 0.f, 0.f));
                Shader->setMatrix4("model", model);
                floor->Draw(Shader);

                model = glm::translate(model, glm::vec3(2.f, 0.f, 0.f));
                Shader->setMatrix4("model", model);
                floor->Draw(Shader);
            }
        }

        Render::PostProcessing::render();

        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    Render::PostProcessing::terminate();
    Managers::ResourceManager::clear();
    glfwTerminate();
    return 0;
}
