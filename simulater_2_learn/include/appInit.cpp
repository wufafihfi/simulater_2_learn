#include "appInit.h"

namespace AppInit {
    // 调试信息存储
    std::vector<std::string> debugMessages;
    std::vector<std::string>& getDebugMessages() {
        return debugMessages;
    }

    // SFML窗口类指针
    sf::RenderWindow window;
    sf::RenderWindow& getWinodw() {
        return window;
    }

    // 程序目录路径
    BasePath basePath;
    BasePath& getProgramDirectoryPath() {
        if (basePath.basepath_A.empty())
        {
            basePath.basepath_A = getExecutablePath(0);
        }
        if (basePath.basepath_W.empty())
        {
            basePath.basepath_W = getExecutablePath(1);
        }
        return basePath;
    }
    // SFML字体  基础
    sf::Font basefont;
    sf::Font& getBasefont() {
        if (basefont.getInfo().family.empty())
        {
            baseFontLoad_SF(basefont);
        }
        return basefont;
    }

    // 鼠标
    sf::Vector2i globalMousePos = { 0, 0 };
    sf::Vector2i windowMousePos = { 0, 0 };
    sf::Vector2i& getGlobalMousePos() {
        return globalMousePos;
    }
    sf::Vector2i& getWindowMousePos() {
        return windowMousePos;
    }

    // 背景
    sf::Color backgroundColor = sf::Color({0,0,0,0});
    sf::Color& getBackColor() {
        return backgroundColor;
    }

    // 路径
#ifdef _WIN32
    std::string getExecutablePath(int mode) {
        if(mode == 1)// Unicode
        {
            wchar_t szFilePath[MAX_PATH + 1] = { 0 };
            GetModuleFileNameW(NULL, szFilePath, MAX_PATH);

            // 直接转换为 UTF-8
            int utf8Size = WideCharToMultiByte(CP_UTF8, 0, szFilePath, -1, nullptr, 0, nullptr, nullptr);
            if (utf8Size == 0) {
                return "";
            }

            std::string utf8Path(utf8Size, 0);
            WideCharToMultiByte(CP_UTF8, 0, szFilePath, -1, &utf8Path[0], utf8Size, nullptr, nullptr);

            // 移除文件名，只保留目录路径
            size_t lastSlash = utf8Path.find_last_of("\\/");
            if (lastSlash != std::string::npos) {
                utf8Path = utf8Path.substr(0, lastSlash);
            }

            return utf8Path;
        }
        else
        {
            char szFilePath[MAX_PATH + 1] = { 0 };
            GetModuleFileNameA(NULL, szFilePath, MAX_PATH);
            (strrchr(szFilePath, '\\'))[0] = 0;
            std::string path = szFilePath;
            return path;
        }
    }
#else
    std::string getExecutablePath(int mode) {
        // Linux/macOS
        char path[PATH_MAX];
        ssize_t count = readlink("/proc/self/exe", path, PATH_MAX);
        return std::string(path, count > 0 ? count : 0);
    }
#endif

    // SFML字体
    void baseFontLoad_SF(sf::Font &font) {
        auto& _basepath = getProgramDirectoryPath();
        std::string ttfPath = _basepath.basepath_A + u8"\\fonts\\SimHei.ttf";
        if (font.openFromFile(ttfPath.c_str())) {
            std::cout << "SFML 字体加载成功!" << std::endl;
            debugMessages.push_back(u8"SFML 字体加载成功!");
        }
        else
        {
            std::cout << "SFML 字体加载失败!" << std::endl;
            debugMessages.push_back(u8"SFML 字体加载失败!");
        }
    }
    // IMGUI初始化集合
    void InitFile_IMGUI() {
        ImGuiIO& io = ImGui::GetIO();

        auto& _basepath = getProgramDirectoryPath();
        static std::string layoutiniPath = _basepath.basepath_W + u8"\\config\\imgui_layout.ini";
        io.IniFilename = layoutiniPath.c_str();

        // 清除默认字体
        io.Fonts->Clear();
        std::string ttfPath = _basepath.basepath_W + u8"\\fonts\\SimHei.ttf";
        ImFont* chineseFont = io.Fonts->AddFontFromFileTTF(
            ttfPath.c_str(),           // 字体文件路径
            13.0f /* dpiScale*/,                     // 字体大小
            nullptr,                   // 字体配置
            io.Fonts->GetGlyphRangesChineseFull()  // 中文字符集
        );

        if (chineseFont != nullptr) {
            std::cout << "ImGui 字体加载成功!" << std::endl;
            debugMessages.push_back(u8"ImGui 字体加载成功!");
        }
        else {
            std::cout << "ImGui 字体加载失败，使用默认字体" << std::endl;
            debugMessages.push_back(u8"ImGui 字体加载失败，使用默认字体");
            io.Fonts->AddFontDefault();
        }

        // 更新字体纹理
        ImGui::SFML::UpdateFontTexture(); 
        io.Fonts->AddFontDefault();
    }
}