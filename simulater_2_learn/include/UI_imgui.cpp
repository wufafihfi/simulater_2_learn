#include "UI_imgui.h"

namespace UI_Imgui {

	void UImain() {
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
		ImGui::Begin(u8"调试");
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
		ImGui::Text(u8"全局鼠标坐标 X:%d Y:%d", _globalMousePos.x, _globalMousePos.y);
		ImGui::Text(u8"窗口鼠标坐标 X:%d Y:%d", _windowMousePos.x, _windowMousePos.y);
		ImGui::Text(u8"焦点是否为此窗口: %s", _window.hasFocus() ? u8"是" : u8"否");
		ImGui::Text(u8"窗口位置: (%d, %d)", _window.getPosition().x, _window.getPosition().y);
		ImGui::NewLine();
		ImGui::Text(u8"窗口大小 当前: (%d, %d)", _window.getSize().x, _window.getSize().y);
		static float k_size = 900.0f/1700.0f;
		static int currentItem = 3;
		static const char* items[] = { u8"1700x900",u8"1500x794" ,u8"1300x688", u8"1000x529", u8"800x423" };
		ImGui::Combo(u8"窗口大小选择", &currentItem, items, IM_ARRAYSIZE(items));
		switch (currentItem)
		{
		case 0:
		{
			sf::Vector2u windowSIze = sf::Vector2u(1700, 1700 *k_size);
			if (!(_window.getSize().x == windowSIze.x && _window.getSize().y == windowSIze.y))
			{
				_window.setSize(windowSIze);
				std::cout << windowSIze.x << "x" << windowSIze.y << std::endl;
			}
		}
			break;

		case 1:
		{
			sf::Vector2u windowSIze = sf::Vector2u(1500, 1500 * k_size);
			if (!(_window.getSize().x == windowSIze.x && _window.getSize().y == windowSIze.y))
			{
				_window.setSize(windowSIze);
				std::cout << windowSIze.x << "x" << windowSIze.y << std::endl;
			}
		}
			break;

		case 2:
		{
			sf::Vector2u windowSIze = sf::Vector2u(1300, 1300 * k_size);
			if (!(_window.getSize().x == windowSIze.x && _window.getSize().y == windowSIze.y))
			{
				_window.setSize(windowSIze);
				std::cout << windowSIze.x << "x" << windowSIze.y << std::endl;
			}
		}
			break;

		case 3:
		{
			sf::Vector2u windowSIze = sf::Vector2u(1000, 1000 * k_size);
			if (!(_window.getSize().x == windowSIze.x && _window.getSize().y == windowSIze.y))
			{
				_window.setSize(windowSIze);
				std::cout << windowSIze.x << "x" << windowSIze.y << std::endl;
			}
		}
			break;

		case 4:
		{
			sf::Vector2u windowSIze = sf::Vector2u(800, 800 * k_size);
			if (!(_window.getSize().x == windowSIze.x && _window.getSize().y == windowSIze.y))
			{
				_window.setSize(windowSIze);
				std::cout << windowSIze.x << "x" << windowSIze.y << std::endl;
			}
		}
		break;

		default:
			break;
		}
		ImGui::NewLine();
		ImGui::DebugTextEncoding(u8"中文测试 HELLO");
		ImGui::End();
	}
}