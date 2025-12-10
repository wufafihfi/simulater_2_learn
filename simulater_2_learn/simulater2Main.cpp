#include <iostream>

#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <SFML/Window.hpp>
#include "imgui.h"
#include "imgui-SFML.h"

#include <Windows.h>
#include <shellscalingapi.h>

#include "include/S2L_includeAll.hpp"

int main() {
    // 初始变量
    //路径
    auto& _basepath = AppInit::getProgramDirectoryPath();
    //鼠标
    auto& _globalMousePos = AppInit::getGlobalMousePos();
    auto& _windowMousePos = AppInit::getWindowMousePos();
    //颜色
    auto& backColor = AppInit::getBackColor();

    // DPI
    //SetProcessDpiAwareness(PROCESS_DPI_UNAWARE);
    // SFML
    sf::Vector2u windowSize = sf::Vector2u({ 1700 ,900 });
    //sf::RenderWindow window;
    auto& window = AppInit::getWinodw();
    window.create(sf::VideoMode(windowSize),
        L"SFML&IMGUI 进阶学习项目",
        sf::Style::Titlebar | sf::Style::Close);

    //sf::VideoMode desktopMode = sf::VideoMode::getDesktopMode();
#ifdef _WIN32
    sf::VideoMode desktopMode = sf::VideoMode(sf::Vector2u(GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN)));
#endif
    window.setPosition(sf::Vector2i(desktopMode.size.x / 2.0f - windowSize.x / 2.0f, desktopMode.size.y / 2.0f - windowSize.y / 2.0f));

    if (!window.isOpen()) {
        MessageBoxA(NULL, "窗口创建失败", "错误", MB_ICONERROR | MB_OK);
        return -1;
    }
    window.setActive(true);
    window.setFramerateLimit(120);
     
    // IMGUI
    if (!ImGui::SFML::Init(window)) {
        MessageBoxA(NULL, "初始化 ImGui 失败,程序退出", "错误", MB_ICONERROR | MB_OK);
        return -1;
    }

    // 字体
    AppInit::InitFile_IMGUI();
    auto& _basefont = AppInit::getBasefont();

    // IMGUI
    bzd_SFML_main::UIready();

    // SFML图形
    bzd_SFML_main::Bzd_SFML_Ready();
    //sf::Text text(_basefont);
    //text.setString(L"经典语句：Hello world");
    //text.setCharacterSize(24);
    //text.setPosition({ 200, 150 });
    //text.setFillColor(sf::Color::Red);

    // 进入主循环
    sf::Clock deltaClock;
    while (window.isOpen()) {
        _globalMousePos = sf::Mouse::getPosition();
        _windowMousePos = sf::Mouse::getPosition(window);
        // 事件处理
        for (auto event = window.pollEvent(); event.has_value(); event = window.pollEvent()) {

            if (event->is<sf::Event::Closed>()) {
                window.close();
            }

            // 将事件传递给 ImGui - 使用正确的命名空间
            ImGui::SFML::ProcessEvent(window, *event);
        }
        ImGui::SFML::Update(window, deltaClock.restart());

        //SFML
        bzd_SFML_main::Bzd_SFML_Update();
        //IMGUI
        bzd_SFML_main::UIuodata();

        // 渲染
        //刷新
        window.clear(backColor);
        //SFML
        bzd_SFML_main::Bzd_SFML_draw();
        //window.draw(text);
        //ImGui
        ImGui::SFML::Render(window);
        ////
        window.display();
    }
    ImGui::SFML::Shutdown();
}