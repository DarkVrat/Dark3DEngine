#include "Camera.h"

#include <glm/gtc/type_ptr.hpp>

glm::vec2 Render::Camera::screenSize = glm::vec2(1280.f, 720.f);
glm::vec2 Render::Camera::lastPosMouse = glm::vec2(640.f, 360.f);
glm::vec2 Render::Camera::direction = glm::vec2(0.f, 0.f);
glm::vec3 Render::Camera::cameraPos = glm::vec3(0.0f, 0.0f, 3.0f);
glm::vec3 Render::Camera::cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 Render::Camera::cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
float Render::Camera::fov = 90.f;
float Render::Camera::yaw = -90.f;
float Render::Camera::pitch = 0.f;
GLuint Render::Camera::m_matricesUBO = 0;

namespace Render
{
	void Camera::setScreenSize(const glm::vec2& size)
	{
		screenSize = size;
		updateProjection();
	}

	void Camera::setDirection(const glm::vec2 dir)
	{
		direction = dir;
	}

	void Camera::updateFov(float deltaFov)
	{
		fov -= (float)deltaFov;

		if (fov < 1.0f)
			fov = 1.0f;
		else if (fov > 120)
			fov = 120;

		updateProjection();
	}

	void Camera::updatePositionMouse(const glm::vec2 pos)
	{
		float xoffset = pos.x - lastPosMouse.x;
		float yoffset = lastPosMouse.y - pos.y;
		lastPosMouse.x = pos.x;
		lastPosMouse.y = pos.y;

		const float sensitivity = 0.1f;
		xoffset *= sensitivity;
		yoffset *= sensitivity;

		yaw += xoffset;
		pitch += yoffset;

		if (pitch > 89.0f)
			pitch = 89.0f;
		if (pitch < -89.0f)
			pitch = -89.0f;

		glm::vec3 direction;
		direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
		direction.y = sin(glm::radians(pitch));
		direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
		cameraFront = glm::normalize(direction);

		updateView();
	}

	void Camera::updatePositionCamera(float delta)
	{
		float cameraSpeed = 2.f * delta;
		cameraPos += cameraSpeed * cameraFront * direction.x;
		cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed * direction.y;
		updateView();
	}

	void Camera::initMatricesUBO()
	{
		glGenBuffers(1, &m_matricesUBO);
		glBindBuffer(GL_UNIFORM_BUFFER, m_matricesUBO);
		glBufferData(GL_UNIFORM_BUFFER, 2 * sizeof(glm::mat4), NULL, GL_STATIC_DRAW);
		glBindBuffer(GL_UNIFORM_BUFFER, 0);
		glBindBufferRange(GL_UNIFORM_BUFFER, 0, m_matricesUBO, 0, 2 * sizeof(glm::mat4));
	}

	void Camera::updateProjection()
	{
		glm::mat4 projection = glm::perspective(glm::radians(fov), screenSize.x / screenSize.y, 0.01f, 200.0f);
		glBindBuffer(GL_UNIFORM_BUFFER, m_matricesUBO);
		glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), glm::value_ptr(projection));
		glBindBuffer(GL_UNIFORM_BUFFER, 0);
	}

	void Camera::updateView()
	{
		glm::mat4 view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
		glBindBuffer(GL_UNIFORM_BUFFER, m_matricesUBO);
		glBufferSubData(GL_UNIFORM_BUFFER, sizeof(glm::mat4), sizeof(glm::mat4), glm::value_ptr(view));
		glBindBuffer(GL_UNIFORM_BUFFER, 0);
	}
}
