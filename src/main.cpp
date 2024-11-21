#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <memory>
#include <cstdlib>
#include <ctime>   

#include "Render/VertexArray.h"
#include "Render/ShaderProgram.h"
#include "Render/Model.h"
#include "Render/Camera.h"
#include "Manager/ResourceManager.h"
#include "Manager/ConfigManager.h"
#include "Render/PostProcessing.h"
#include "Render/ShadowPoint.h"

#include "Render/DeferredShading.h"

extern "C" {
    _declspec(dllexport) unsigned long NvOptimusEnablement = 0x00000001;
    _declspec(dllexport) int AmdPowerXpressRequestHighPerformance = 1;
}

using namespace Render;

float randomFloat(float min, float max) {
    return min + static_cast<float>(rand()) / (static_cast<float>(RAND_MAX / (max - min)));
}

std::vector<glm::vec3> generateCubePositions() {
    std::vector<glm::vec3> positions;
    for (int x = -10; x < 10; ++x) { 
        for (int z = -10; z < 10; ++z) {
            float height = randomFloat(-1.0f, 1.0f);
            positions.emplace_back(x * 2.0f, height, z * 2.0f);
        }
    }
    return positions;
}

std::vector<glm::vec3> generateLightPositions(int count) {
    std::vector<glm::vec3> positions;
    for (int i = 0; i < count; ++i) {
        positions.emplace_back(
            randomFloat(-21.0f, 21.0f), 
            randomFloat(2.0f, 3.0f),    
            randomFloat(-21.0f, 21.0f)
        );
    }
    return positions;
}

std::vector<glm::vec3> generateLightColors(int count) {
    std::vector<glm::vec3> colors;
    for (int i = 0; i < count; ++i) {
        colors.emplace_back(
            randomFloat(0.2f, 30.0f), // R
            randomFloat(0.2f, 30.0f), // G
            randomFloat(0.2f, 30.0f)  // B
        );
    }
    return colors;
}

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

    srand(static_cast<unsigned>(time(0)));

    auto cubePositions = generateCubePositions();
    auto lightPositions = generateLightPositions(100);
    auto lightColors = generateLightColors(100);

    Managers::ResourceManager::loadResources("res/Resources.json");

    Camera::initMatricesUBO();
    Camera::setScreenSize(glm::vec2(windowSize.x, windowSize.y));
    Camera::updatePositionMouse(glm::vec2(windowSize.x / 2, windowSize.y / 2));


    std::shared_ptr<ShaderProgram> light_for_DF = Managers::ResourceManager::getShader("light_for_DF");
    light_for_DF->use();
    for (size_t i = 0; i < lightPositions.size(); ++i) {
        std::string base = "lights[" + std::to_string(i) + "].";
        light_for_DF->setVec3(base + "Position", lightPositions[i]);
        light_for_DF->setVec3(base + "Color", lightColors[i]);
        light_for_DF->setFloat(base + "Linear", 0.7f);
        light_for_DF->setFloat(base + "Quadratic", 1.8f);
        light_for_DF->setFloat(base + "Radius", 7.f);
    }

    float lastFrame = 0.0f;

    std::shared_ptr<Model> container = Managers::ResourceManager::getModel("modern-bloc");

    Render::PostProcessing::init();
    Render::DeferredShading::init();
    std::shared_ptr<ShaderProgram> scene_for_DF = Managers::ResourceManager::getShader("scene_for_DF");

    while (!glfwWindowShouldClose(window))
    {
        float currentFrame = static_cast<float>(glfwGetTime());
        float deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        processInput(window);

        Camera::updatePositionCamera(deltaTime);

        Render::DeferredShading::bind();

        scene_for_DF->use();
        for (const auto& pos : cubePositions) {
            glm::mat4 model = glm::mat4(1.0f);
            model = glm::translate(model, pos);
            model = glm::scale(model, glm::vec3(0.75f));
            scene_for_DF->setMatrix4("model", model);
            container->Draw(scene_for_DF);
        }

        Render::PostProcessing::bind();

        Render::DeferredShading::Render();

        Render::PostProcessing::render();

        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    Render::DeferredShading::terminate();
    Render::PostProcessing::terminate();
    Managers::ResourceManager::clear();
    glfwTerminate();
    return 0;
}
