#include "ShaderProgram.h"

#include <iostream>
#include <fstream>
#include <string>

namespace Render 
{
	ShaderProgram::ShaderProgram(const std::string& vertexShaderPath, const std::string& fragmentShaderPath)
	{
		GLuint vertexShader = compileShader(GL_VERTEX_SHADER, loadSource(vertexShaderPath).c_str());
		GLuint fragmentShader = compileShader(GL_FRAGMENT_SHADER, loadSource(fragmentShaderPath).c_str());

		m_id = glCreateProgram();
		glAttachShader(m_id, vertexShader);
		glAttachShader(m_id, fragmentShader);
		glLinkProgram(m_id);

		int  success;
		char infoLog[512];
		glGetProgramiv(m_id, GL_LINK_STATUS, &success);
		if (!success) {
			glGetProgramInfoLog(m_id, 512, NULL, infoLog);
			std::cout << "{E} ERROR::SHADER::LINKED_FAILED\n" << infoLog << std::endl;
			m_isCompiled = false;
		}

		glDeleteShader(vertexShader);
		glDeleteShader(fragmentShader);
	}

	ShaderProgram::ShaderProgram(const std::string& vertexShaderPath, const std::string& geometryShaderPath, const std::string& fragmentShaderPath)
	{
		GLuint vertexShader = compileShader(GL_VERTEX_SHADER, loadSource(vertexShaderPath).c_str());
		GLuint geometryShader = compileShader(GL_GEOMETRY_SHADER, loadSource(geometryShaderPath).c_str());
		GLuint fragmentShader = compileShader(GL_FRAGMENT_SHADER, loadSource(fragmentShaderPath).c_str());

		m_id = glCreateProgram();
		glAttachShader(m_id, vertexShader);
		glAttachShader(m_id, geometryShader);
		glAttachShader(m_id, fragmentShader);
		glLinkProgram(m_id);

		int  success;
		char infoLog[512];
		glGetProgramiv(m_id, GL_LINK_STATUS, &success);
		if (!success) {
			glGetProgramInfoLog(m_id, 512, NULL, infoLog);
			std::cout << "{E} ERROR::SHADER::LINKED_FAILED\n" << infoLog << std::endl;
			m_isCompiled = false;
		}

		glDeleteShader(vertexShader);
		glDeleteShader(geometryShader);
		glDeleteShader(fragmentShader);
	}

	ShaderProgram::ShaderProgram(ShaderProgram&& source) noexcept
	{
		m_id = source.m_id;
		m_isCompiled = source.m_isCompiled;
		source.m_id = 0;
		source.m_isCompiled = true;
	}

	ShaderProgram& ShaderProgram::operator=(ShaderProgram&& source) noexcept
	{
		if (source.m_id == m_id) return *this;

		m_id = source.m_id;
		m_isCompiled = source.m_isCompiled;
		source.m_id = 0;
		source.m_isCompiled = true;

		return *this;
	}

	ShaderProgram::~ShaderProgram() noexcept
	{
		glDeleteProgram(m_id);
	}

	void ShaderProgram::setUniformBlock(const std::string& name, int index)
	{
		unsigned int uniformBlockIndexRed = glGetUniformBlockIndex(m_id, name.c_str());
		glUniformBlockBinding(m_id, uniformBlockIndexRed, 0);
	}

	GLuint ShaderProgram::compileShader(GLenum type, const char* source) noexcept
	{
		GLuint result;
		result = glCreateShader(type);
		glShaderSource(result, 1, &source, NULL);
		glCompileShader(result);

		int  success;
		glGetShaderiv(result, GL_COMPILE_STATUS, &success);
		if (!success)
		{
			char infoLog[512];
			glGetShaderInfoLog(result, 512, NULL, infoLog);
			//GL_VERTEX_SHADER = 35633
			//GL_GEOMETRY_SHADER = 36313
			//GL_FRAGMENT_SHADER = 35632
			std::cout << "{E} ERROR::SHADER::COMPILATION_FAILED::"<< type << "\n" << infoLog << std::endl;
			m_isCompiled = false;
		}

		return result;
	}

	std::string ShaderProgram::loadSource(const std::string& filePath) noexcept
	{
		std::ifstream file(filePath);

		if (!file.is_open())
			std::cout << "{E} Failed open file "<< filePath <<  std::endl;		

		std::string text="", line;
		while (std::getline(file, line)) text += line + "\n";

		return text;
	}
}
