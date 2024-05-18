#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <memory>
#include <vector>

namespace Render 
{
	class Camera
	{
	public:
		static void setScreenSize(const glm::vec2& size);
		static void setDirection(const glm::vec2 dir);

		static void updatePositionMouse(const glm::vec2 pos);
		static void updatePositionCamera(float delta);
		static void updateFov(float deltaFov);

		static glm::vec3 getPosition() { return cameraPos; }
		static glm::vec3 getFront() { return cameraFront; }
		static glm::vec3 getUp() { return cameraUp; }

		static void initMatricesUBO();
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

		static GLuint m_matricesUBO;
	};
}