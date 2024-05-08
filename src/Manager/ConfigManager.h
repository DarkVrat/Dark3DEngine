#pragma once

#include <string>
#include <rapidjson/document.h>
#include <glm/vec2.hpp>

namespace Managers
{
	class ConfigManager
	{
	public:
		static void loadConfig();
		static void saveConfig();

		static void setWindowSize(const int& x = 800, const int& y = 600);
		static void setDisplayNumber(const unsigned& monitorNumber = 0);
		static void setFullScreen(const bool& fullScreen = true);
		static void setSamples(const unsigned& sample = 1);
		static void setVSync(const unsigned& VSync = 0);

		static glm::ivec2 getWindowSize();
		static unsigned getDisplayNumber();
		static bool getFullScreen();
		static unsigned getSamples();
		static unsigned getVSync();
	private:
		static std::shared_ptr<rapidjson::Document> m_configDoc;
	};
}