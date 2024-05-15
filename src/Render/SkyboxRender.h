#pragma once

#include <vector>
#include <iostream>
#include <memory>
#include "ShaderProgram.h"
#include "VertexArray.h"

namespace Render
{
	class SkyboxRender
	{
	public:
		SkyboxRender(const std::vector<std::string>& paths);
		SkyboxRender(const SkyboxRender& right) = delete;
		SkyboxRender(SkyboxRender&& right) noexcept;
		SkyboxRender& operator=(const SkyboxRender& right) = delete;
		SkyboxRender& operator=(SkyboxRender&& right) noexcept;
		~SkyboxRender() noexcept = default;

		void setShader(std::shared_ptr<ShaderProgram> shader) { m_shader = shader; m_shader->use(); m_shader->setInt("skybox", 0); };
		void render();
		void bind();
	private:
		std::shared_ptr<ShaderProgram> m_shader;
		VertexArray m_VAO;
		VertexBuffer m_VBO;
		GLuint m_textureID;
	};
}