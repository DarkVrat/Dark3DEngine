#include "ConfigManager.h"

#include <fstream>
#include <rapidjson/error/en.h>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/writer.h>
#include "ResourceManager.h"

std::shared_ptr<rapidjson::Document> Managers::ConfigManager::m_configDoc = nullptr;

namespace Managers
{
	void ConfigManager::loadConfig()
	{
		m_configDoc = ResourceManager::getDocument("res/config.json");
	}

	void ConfigManager::saveConfig()
	{
		std::ofstream f;
		f.open(ResourceManager::getFullPath("res/config.json"));
		if (!f.is_open()) {
			std::cerr << "(!) Failed to open: res/config.json" << std::endl;
			return;
		}

		rapidjson::StringBuffer buffer;
		rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
		m_configDoc->Accept(writer);
		f << buffer.GetString();
	}

	void ConfigManager::setWindowSize(const int& x, const int& y) {
		m_configDoc->FindMember("window size")->value["width"].SetInt(x);
		m_configDoc->FindMember("window size")->value["height"].SetInt(y);
	}
	void ConfigManager::setFullScreen(const bool& fullScreen)			{ m_configDoc->FindMember("full screen")->value.SetBool(fullScreen); }
	void ConfigManager::setDisplayNumber(const unsigned& monitorNumber) { m_configDoc->FindMember("display")->value.SetInt(monitorNumber); }
	void ConfigManager::setSamples(const unsigned& sample)				{ m_configDoc->FindMember("samples")->value.SetInt(sample); }
	void ConfigManager::setVSync(const unsigned& VSync)					{ m_configDoc->FindMember("VSync")->value.SetBool(VSync); }

	glm::ivec2 ConfigManager::getWindowSize() {
		int x = m_configDoc->FindMember("window size")->value["width"].GetInt();
		int y = m_configDoc->FindMember("window size")->value["height"].GetInt();
		return glm::ivec2(x, y);
	}
	bool ConfigManager::getFullScreen()			{ return m_configDoc->FindMember("full screen")->value.GetBool(); }
	unsigned ConfigManager::getDisplayNumber()	{ return m_configDoc->FindMember("display")->value.GetInt(); }
	unsigned ConfigManager::getSamples()		{ return m_configDoc->FindMember("samples")->value.GetInt(); }
	unsigned ConfigManager::getVSync()			{ return m_configDoc->FindMember("VSync")->value.GetBool(); }
}