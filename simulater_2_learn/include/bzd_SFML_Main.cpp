#include "bzd_SFML_Main.h"

namespace bzd_SFML_main {

    //图形元素（智能指针管理）
    std::unique_ptr<sf::Text> text;


    void Bzd_SFML_Ready() {
        //窗口
        auto& _window = AppInit::getWinodw();
        auto& _basepath = AppInit::getProgramDirectoryPath();
        auto& _basefont = AppInit::getBasefont();
        auto& backColor = AppInit::getBackColor();
        //调试
        auto& _debugMessages = AppInit::getDebugMessages();

        // SFML图形
        text = std::make_unique<sf::Text>(_basefont);
        //text->setFont(_basefont);
        text->setString(L"经典语句：Hello world");
        text->setCharacterSize(24);
        text->setPosition({ 200, 150 });
        text->setFillColor(sf::Color::Red);
    }

	void Bzd_SFML_Update() {
        //窗口
        auto& _window = AppInit::getWinodw();
        //鼠标
        auto& _globalMousePos = AppInit::getGlobalMousePos();
        auto& _windowMousePos = AppInit::getWindowMousePos();
        //颜色
        auto& backColor = AppInit::getBackColor();
        //调试
        auto& _debugMessages = AppInit::getDebugMessages();



	}

    void Bzd_SFML_draw() {
        //窗口
        auto& _window = AppInit::getWinodw();

        _window.draw(*text);
    }
}