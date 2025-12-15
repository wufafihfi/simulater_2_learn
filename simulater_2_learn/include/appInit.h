#ifndef APPINIT
#define APPINIT

#include <SFML/Graphics.hpp>
#include "imgui.h"
#include "imgui-SFML.h"
#include <filesystem>
#include <iostream>
#include <memory>
#include <string>

#ifdef _WIN32
#include <windows.h>
#else
// Linux/macOS
#include <unistd.h>
#include <limits.h>
#endif

namespace AppInit {

	std::vector<std::string>& getDebugMessages();
	sf::RenderWindow& getWinodw();
	float& getK_size();

	struct BasePath {
		std::string basepath_A;
		std::string basepath_W;
	};

	sf::Vector2i& getGlobalMousePos();
	sf::Vector2i& getWindowMousePos();
	sf::Color& getBackColor();

	std::string getExecutablePath(int mode = 0);
	BasePath& getProgramDirectoryPath();

	void baseFontLoad_SF(sf::Font& font);
	void InitFile_IMGUI();

	sf::Font& getBasefont();

	bool IsWindows10OrGreaterSimple();

}

#endif