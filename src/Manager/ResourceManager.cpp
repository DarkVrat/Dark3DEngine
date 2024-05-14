#include "ResourceManager.h"

#include <fstream>
#include "../Render/Camera.h"
#include "TextureLoader.h"

std::map<std::string, std::shared_ptr<Render::ShaderProgram>> Managers::ResourceManager::m_shaders;
std::map<std::string, std::shared_ptr<Render::Model>> Managers::ResourceManager::m_models;
std::map<std::string, std::shared_ptr<Render::SkyboxRender>> Managers::ResourceManager::m_skyboxes;

namespace Managers
{
    inline std::string ResourceManager::getFullPath(const std::string& path)
    {
#ifdef NDEBUG
        return path;
#else
        return "C:/Users/DarkVrat/Desktop/Dark3DEngine/" + path;
#endif // #ifndef NDEBUG
    }

    std::shared_ptr<rapidjson::Document> ResourceManager::getDocument(const std::string& path)
    {
        std::ifstream file(getFullPath(path));
        if (!file.is_open()) {
            std::cerr << "Failed to open file: " << path << std::endl;
            return nullptr;
        }

        std::string jsonContent((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
        file.close();

        std::shared_ptr<rapidjson::Document> document = std::make_shared<rapidjson::Document>();
        document->Parse(jsonContent.c_str());
        if (document->HasParseError()) {
            std::cerr << "JSON parse error: " << document->GetParseError() << std::endl;
            return nullptr;
        }

        return document;
    }

    void ResourceManager::loadResources(const std::string& path)
    {
        std::shared_ptr<rapidjson::Document> document = getDocument(path);

        const auto& shaders = document->FindMember("shaders")->value;
        for (const auto& shader : shaders.GetArray())
            loadShader(shader);

        const auto& objects = document->FindMember("objects")->value;
        for (const auto& object : objects.GetArray())
            loadModel(object);

        const auto& skyboxes = document->FindMember("skyboxes")->value;
        for (const auto& skybox : skyboxes.GetArray())
            loadSkybox(skybox);
    }

    void ResourceManager::loadShader(const rapidjson::GenericValue<rapidjson::UTF8<>>& shader)
    {
        std::string name = shader["name"].GetString();
        std::string vertexPath = getFullPath(shader["vertex_path"].GetString());
        std::string fragmentPath = getFullPath(shader["fragment_path"].GetString());
        bool cameraFlag = shader["camera"].GetBool();
        
        std::shared_ptr<Render::ShaderProgram> shader_ptr = std::make_shared<Render::ShaderProgram>(vertexPath, fragmentPath);
        if (cameraFlag)
            Render::Camera::addShader(shader_ptr);

        m_shaders.emplace(name, shader_ptr);
    }

    std::shared_ptr<Render::ShaderProgram> ResourceManager::getShader(const std::string& name)
    {
        auto shader = m_shaders.find(name);
        if (shader == m_shaders.end())
            return nullptr;
        return shader->second;
    }

    void ResourceManager::loadModel(const rapidjson::GenericValue<rapidjson::UTF8<>>& model)
    {
        std::string name = model["name"].GetString();
        std::string objPath = getFullPath(model["obj_path"].GetString());
        bool mtlFlag = model["has_mtl"].GetBool();
        
        std::shared_ptr<Render::Model> model_ptr = std::make_shared<Render::Model>(objPath);
        m_models.emplace(name, model_ptr);

        if (!mtlFlag)
        {
            model_ptr->getSTexture().diffuse = TextureLoader::loadTexture(getFullPath(model["diffuse"].GetString()));

            if(model.HasMember("specular"))
                model_ptr->getSTexture().specular = TextureLoader::loadTexture(getFullPath(model["specular"].GetString()));

            if (model.HasMember("normal"))
                model_ptr->getSTexture().normal = TextureLoader::loadTexture(getFullPath(model["normal"].GetString()));
        }
    }

    std::shared_ptr<Render::Model> ResourceManager::getModel(const std::string& name)
    {
        auto model = m_models.find(name);
        if (model == m_models.end())
            return nullptr;
        return model->second;
    }

    void ResourceManager::loadSkybox(const rapidjson::GenericValue<rapidjson::UTF8<>>& skybox)
    {
        std::string name = skybox["name"].GetString();
        std::vector<std::string> vecPath;
        vecPath.reserve(6);
        vecPath.push_back(getFullPath(skybox["back"].GetString()));
        vecPath.push_back(getFullPath(skybox["bottom"].GetString()));
        vecPath.push_back(getFullPath(skybox["front"].GetString()));
        vecPath.push_back(getFullPath(skybox["left"].GetString()));
        vecPath.push_back(getFullPath(skybox["right"].GetString()));
        vecPath.push_back(getFullPath(skybox["top"].GetString()));

        std::shared_ptr<Render::SkyboxRender> skybox_ptr = std::make_shared<Render::SkyboxRender>(vecPath);

        m_skyboxes.emplace(name, skybox_ptr);
    }

    std::shared_ptr<Render::SkyboxRender> ResourceManager::getSkybox(const std::string& name)
    {
        auto skybox = m_skyboxes.find(name);
        if (skybox == m_skyboxes.end())
            return nullptr;
        return skybox->second;
    }

    void ResourceManager::clear()
    {
        m_models.clear();
        m_shaders.clear();
    }
}
