#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>

namespace Render 
{
	class ShaderProgram
	{
	public:
		ShaderProgram(const std::string& vertexShaderPath, const std::string& fragmentShaderPath);
		ShaderProgram(const std::string& vertexShaderPath, const std::string& geometryShaderPath, const std::string& fragmentShaderPath);
		ShaderProgram(const ShaderProgram& source) = delete;
		ShaderProgram(ShaderProgram&& source) noexcept;
		ShaderProgram& operator=(const ShaderProgram& source) = delete;
		ShaderProgram& operator=(ShaderProgram&& source) noexcept;
		~ShaderProgram() noexcept;

		inline bool isCompiled() const noexcept { return m_isCompiled; }

		inline void use() const noexcept { glUseProgram(m_id); }

		void ShaderProgram::setInt(const std::string& name, const GLint& value)			{ glUniform1i(location(name), value); }
		void ShaderProgram::setFloat(const std::string& name, const GLfloat& value)		{ glUniform1f(location(name), value); }
		void ShaderProgram::setVec3(const std::string& name, const glm::vec3& vec3)		{ glUniform3f(location(name), vec3.x, vec3.y, vec3.z); }
		void ShaderProgram::setVec4(const std::string& name, const glm::vec4& vec4)		{ glUniform4f(location(name), vec4.x, vec4.y, vec4.z, vec4.w); }
		void ShaderProgram::setMatrix3(const std::string& name, const glm::mat3& matrix) { glUniformMatrix3fv(location(name), 1, GL_FALSE, glm::value_ptr(matrix)); }
		void ShaderProgram::setMatrix4(const std::string& name, const glm::mat4& matrix) { glUniformMatrix4fv(location(name), 1, GL_FALSE, glm::value_ptr(matrix)); }
		GLuint m_id = 0;
	private:
		
		bool m_isCompiled = true;

		GLuint compileShader(GLenum type, const char* source) noexcept;
		inline GLint location(const std::string& name) const noexcept { return glGetUniformLocation(m_id, name.c_str()); }
		static std::string loadSource(const std::string& filePath) noexcept;
	};
}
