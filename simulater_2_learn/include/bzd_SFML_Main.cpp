#include "bzd_SFML_Main.h"

namespace bzd_SFML_main {

	float valueChangeSmooth(float time, float value, float targetValue) {
		float _v = (targetValue - value) / time;
		return _v + value;
	}

    //图形元素（智能指针管理）
    std::unique_ptr<sf::Text> text;
	// TXT颜色
	float textColor[3] = { 0.0f, 1.0f, 0.0f };

	//sfml
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
        text->setString(L"经典语句：Hello world 还有 FMYSSMGAY");
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


        text->setFillColor(sf::Color(
            static_cast<uint8_t>(textColor[0] * 255),
            static_cast<uint8_t>(textColor[1] * 255),
            static_cast<uint8_t>(textColor[2] * 255)
        ));

	}

    void Bzd_SFML_draw() {
        //窗口
        auto& _window = AppInit::getWinodw();

        _window.draw(*text);
    }

	//IMGUI
	void UIready() {
		ImGui::StyleColorsDark();
		ImGui::GetStyle().Alpha = 0.5f;
	}

	void UIuodata() {
		//窗口
		auto& _window = AppInit::getWinodw();
		//鼠标
		auto& _globalMousePos = AppInit::getGlobalMousePos();
		auto& _windowMousePos = AppInit::getWindowMousePos();
		//调试
		auto& _debugMessages = AppInit::getDebugMessages();

		// 调试信息数量限制
		if (_debugMessages.size() > 100) {
			_debugMessages.erase(_debugMessages.begin());
		}

		// 调试窗口
		{
			static float windowAlpha = 0;
			static float maxAlpha = 0.8;
			static float minAlpha = 0.3;

			ImGui::PushStyleVar(ImGuiStyleVar_Alpha, windowAlpha);
			ImGui::Begin(u8"调试");

			//窗口动态透明度调整 平滑过渡
			bool bestHoverCheck = ImGui::IsWindowHovered(
				ImGuiHoveredFlags_ChildWindows |
				ImGuiHoveredFlags_AllowWhenBlockedByActiveItem);
			bool justActivated = ImGui::IsWindowAppearing();
			bool isFocused = ImGui::IsWindowFocused();
			if (bestHoverCheck || justActivated || isFocused)
			{
				windowAlpha = valueChangeSmooth(10, windowAlpha, maxAlpha);
			}
			else {
				windowAlpha = valueChangeSmooth(20, windowAlpha, minAlpha);
			}
			ImGui::Text(u8"窗口透明度:%0.3f", windowAlpha);
			if (ImGui::IsItemHovered()) {
				ImGui::SetTooltip(u8"窗口透明度动态响应调试信息");
			}
			ImGui::NewLine();

			ImGui::Text(u8"调试信息");
			ImGui::BeginChild("DebugScroll", ImVec2(0, 200), true, ImGuiWindowFlags_HorizontalScrollbar | ImGuiWindowFlags_AlwaysVerticalScrollbar);
			for (const auto& message : _debugMessages) {
				ImGui::TextUnformatted(message.c_str());
			}
			ImGui::Separator();
			if (ImGui::GetScrollY() >= ImGui::GetScrollMaxY()) {
				ImGui::SetScrollHereY(1.0f);
			}
			ImGui::EndChild();
			ImGui::TextColored(ImVec4(1, 1, 0, 1), u8"焦点状态:");
			ImGui::Text(u8"聚焦(Focused): %s", isFocused ? u8"是" : u8"否");
			ImGui::Text(u8"悬停(Hovered): %s", bestHoverCheck ? u8"是" : u8"否");
			ImGui::Text(u8"刚出现(Appearing): %s", justActivated ? u8"是" : u8"否");
			ImGui::NewLine();
			ImGui::Text(u8"全局鼠标坐标 X:%d Y:%d", _globalMousePos.x, _globalMousePos.y);
			ImGui::Text(u8"程序窗口鼠标坐标 X:%d Y:%d", _windowMousePos.x, _windowMousePos.y);
			ImGui::Text(u8"焦点是否为此窗口: %s", _window.hasFocus() ? u8"是" : u8"否");
			ImGui::Text(u8"程序窗口位置: (%d, %d)", _window.getPosition().x, _window.getPosition().y);
			ImGui::NewLine();
			ImGui::Text(u8"程序窗口大小 当前: (%d, %d)", _window.getSize().x, _window.getSize().y);
			static float k_size = 900.0f / 1700.0f;
			static int currentItem = 4;
			static const char* items[] = { u8"1700x900",u8"1500x794" ,u8"1300x688", u8"1000x529", u8"800x423" };
			ImGui::Combo(u8"程序窗口大小选择", &currentItem, items, IM_ARRAYSIZE(items));
			static bool windowSizeChange = false;
#ifdef _WIN32
			static sf::VideoMode desktopMode = sf::VideoMode(sf::Vector2u(GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN)));
			if (windowSizeChange) {
				sf::Vector2 windowSize = _window.getSize();
				_window.setPosition(sf::Vector2i(desktopMode.size.x / 2.0f - windowSize.x / 2.0f, desktopMode.size.y / 2.0f - windowSize.y / 2.0f));

				windowSizeChange = false;
			}
#else
			static sf::VideoMode desktopMode = sf::VideoMode(sf::Vector2u(9999, 9999));
#endif
			switch (currentItem)
			{
			case 0:
			{
				sf::Vector2u windowSIze = sf::Vector2u(1700, 1700 * k_size);
				if (desktopMode.size.x <= windowSIze.x || desktopMode.size.y <= windowSIze.y) {
					_debugMessages.push_back(u8"窗口大小设置失败,屏幕(虚拟)不够大");
					currentItem = 4;
					break;
				}
				if (!(_window.getSize().x == windowSIze.x && _window.getSize().y == windowSIze.y))
				{
					_window.setSize(windowSIze);
					std::cout << windowSIze.x << "x" << windowSIze.y << std::endl;
					_debugMessages.push_back(u8"窗口大小设置:1700x900");
					windowSizeChange = true;
				}
			}
			break;

			case 1:
			{
				sf::Vector2u windowSIze = sf::Vector2u(1500, 1500 * k_size);
				if (desktopMode.size.x <= windowSIze.x || desktopMode.size.y <= windowSIze.y) {
					_debugMessages.push_back(u8"窗口大小设置失败,屏幕(虚拟)不够大");
					currentItem = 4;
					break;
				}
				if (!(_window.getSize().x == windowSIze.x && _window.getSize().y == windowSIze.y))
				{
					_window.setSize(windowSIze);
					std::cout << windowSIze.x << "x" << windowSIze.y << std::endl;
					windowSizeChange = true;
					_debugMessages.push_back(u8"窗口大小设置:1500x794");
				}
			}
			break;

			case 2:
			{
				sf::Vector2u windowSIze = sf::Vector2u(1300, 1300 * k_size);
				if (desktopMode.size.x <= windowSIze.x || desktopMode.size.y <= windowSIze.y) {
					_debugMessages.push_back(u8"窗口大小设置失败,屏幕(虚拟)不够大");
					currentItem = 4;
					break;
				}
				if (!(_window.getSize().x == windowSIze.x && _window.getSize().y == windowSIze.y))
				{
					_window.setSize(windowSIze);
					std::cout << windowSIze.x << "x" << windowSIze.y << std::endl;
					windowSizeChange = true;
					_debugMessages.push_back(u8"窗口大小设置:1300x688");
				}
			}
			break;

			case 3:
			{
				sf::Vector2u windowSIze = sf::Vector2u(1000, 1000 * k_size);
				if (desktopMode.size.x <= windowSIze.x || desktopMode.size.y <= windowSIze.y) {
					_debugMessages.push_back(u8"窗口大小设置失败,屏幕(虚拟)不够大");
					currentItem = 4;
					break;
				}
				if (!(_window.getSize().x == windowSIze.x && _window.getSize().y == windowSIze.y))
				{
					_window.setSize(windowSIze);
					std::cout << windowSIze.x << "x" << windowSIze.y << std::endl;
					windowSizeChange = true;
					_debugMessages.push_back(u8"窗口大小设置:1000x529");
				}
			}
			break;

			case 4:
			{
				sf::Vector2u windowSIze = sf::Vector2u(800, 800 * k_size);
				if (desktopMode.size.x <= windowSIze.x || desktopMode.size.y <= windowSIze.y) {
					_debugMessages.push_back(u8"窗口大小设置失败,屏幕(虚拟)不够大");
					currentItem = 4;
					break;
				}
				if (!(_window.getSize().x == windowSIze.x && _window.getSize().y == windowSIze.y))
				{
					_window.setSize(windowSIze);
					std::cout << windowSIze.x << "x" << windowSIze.y << std::endl;
					windowSizeChange = true;
					_debugMessages.push_back(u8"窗口大小设置:800x423");
				}
			}
			break;

			default:
				break;
			}
			ImGui::NewLine();
			ImGui::DebugTextEncoding(u8"中文测试 HELLO");
			ImGui::End();

			ImGui::PopStyleVar(); // 透明度调整结尾
		}
		// 其他窗口
		{
			static float windowAlpha = 0;
			static float maxAlpha = 0.7;
			static float minAlpha = 0.3;

			ImGui::PushStyleVar(ImGuiStyleVar_Alpha, windowAlpha);
			ImGui::Begin(u8"第一个窗口");

			//窗口动态透明度调整 平滑过渡
			bool bestHoverCheck = ImGui::IsWindowHovered(
				ImGuiHoveredFlags_ChildWindows |
				ImGuiHoveredFlags_AllowWhenBlockedByActiveItem);
			bool isFocused = ImGui::IsWindowFocused();
			if (bestHoverCheck || isFocused)
			{
				windowAlpha = valueChangeSmooth(10, windowAlpha, maxAlpha);
			}
			else {
				windowAlpha = valueChangeSmooth(20, windowAlpha, minAlpha);
			}
			ImGui::NewLine();
			ImGui::ColorEdit3(u8"字的颜色", bzd_SFML_main::textColor);
			ImGui::End();

			ImGui::PopStyleVar(); // 透明度调整结尾
		}
		// 其他窗口
		{
			static float windowAlpha = 0;
			static float maxAlpha = 0.7;
			static float minAlpha = 0.3;

			ImGui::PushStyleVar(ImGuiStyleVar_Alpha, windowAlpha);
			ImGui::Begin(u8"第二个窗口");

			//窗口动态透明度调整 平滑过渡
			bool bestHoverCheck = ImGui::IsWindowHovered(
				ImGuiHoveredFlags_ChildWindows |
				ImGuiHoveredFlags_AllowWhenBlockedByActiveItem);
			bool isFocused = ImGui::IsWindowFocused();
			if (bestHoverCheck || isFocused)
			{
				windowAlpha = valueChangeSmooth(10, windowAlpha, maxAlpha);
			}
			else {
				windowAlpha = valueChangeSmooth(20, windowAlpha, minAlpha);
			}
			ImGui::TextColored(ImVec4(1, 0, 0, 1), u8"中文测试 FMYSSMGAY");
			ImGui::NewLine();
			ImGui::DebugTextEncoding(u8"中文测试 。//；【】；了【");
			ImGui::End();


			ImGui::PopStyleVar(); // 透明度调整结尾
		}
	}
}