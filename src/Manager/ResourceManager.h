#pragma once

#include <iostream>
#include <map>
#include <memory>
#include "rapidjson/document.h"
#include "../Render/Model.h"
#include "../Render/Texture2D.h"
#include "../Render/ShaderProgram.h"
#include "../Render/SkyboxRender.h"

namespace Managers
{
	class ResourceManager
	{
	public:
		static std::string getFullPath(const std::string& path);
		static std::shared_ptr<rapidjson::Document> getDocument(const std::string& path);

		static void loadResources(const std::string& path);

		static void loadShader(const rapidjson::GenericValue<rapidjson::UTF8<>>& shader);
		static std::shared_ptr<Render::ShaderProgram> getShader(const std::string& name);
		static void loadModel(const rapidjson::GenericValue<rapidjson::UTF8<>>& model);
		static std::shared_ptr<Render::Model> getModel(const std::string& name);
		static void loadSkybox(const rapidjson::GenericValue<rapidjson::UTF8<>>& skybox);
		static std::shared_ptr<Render::SkyboxRender> getSkybox(const std::string& name);

		static void clear();
	private:
		static std::map<std::string, std::shared_ptr<Render::ShaderProgram>> m_shaders;
		static std::map<std::string, std::shared_ptr<Render::Model>> m_models;
		static std::map<std::string, std::shared_ptr<Render::SkyboxRender>> m_skyboxes;
	};
}