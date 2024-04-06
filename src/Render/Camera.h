#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <memory>
#include <vector>
#include "ShaderProgram.h"

namespace Render 
{
	class Camera
	{
	public:
		static void setScreenSize(const glm::vec2& size);
		static void setDirection(const glm::vec2 dir);

		static void addShader(std::shared_ptr<ShaderProgram> ptr);

		static void updatePositionMouse(const glm::vec2 pos);
		static void updatePositionCamera(float delta);
		static void updateFov(float deltaFov);
	private:
		static void updateProjection();
		static void updateView();

		static glm::vec2 screenSize;
		static glm::vec2 lastPosMouse;
		static glm::vec2 direction;

		static glm::vec3 cameraPos;
		static glm::vec3 cameraFront;
		static glm::vec3 cameraUp;

		static float fov;
		static float yaw;
		static float pitch;

		static std::vector<std::shared_ptr<ShaderProgram>> shaders;
	};
}