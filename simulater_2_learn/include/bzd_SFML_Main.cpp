#include "bzd_SFML_Main.h"

namespace bzd_SFML_main {

	float valueChangeSmooth(float time, float value, float targetValue) {
		float _v = (targetValue - value) / time;
		return _v + value;
	}

	//BOX2D参数
	sf::Clock clock;
	float accumulator = 0.0f;
	const float timeStep = 1.0f / 60.0f;
	int subStepCount = 4;
	b2WorldDef worldDef = b2DefaultWorldDef();
	bzd_Phy::PhysicsWorld phyWorld_1(&worldDef,{ 0.0f, -10.0f });

    //图形元素（智能指针管理）
    std::unique_ptr<sf::Text> text;
	// TXT颜色
	float textColor[3] = { 0.0f, 1.0f, 0.0f };

	//物体ID
	b2BodyId ground_bodyId;
	b2BodyId ground_bodyId_1;
	b2BodyId body_1_bodyId;
	b2BodyId body_2_bodyId;
	b2BodyId body_3_bodyId;
	b2BodyId body_4_bodyId;

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
        text->setString(L"经典语句：Hello world");
        text->setCharacterSize(24);
        text->setPosition({ 200, 150 });
        text->setFillColor(sf::Color::Red);

		phyWorld_1.setView(_window.getView());
		// BOX2D
		phyWorld_1.setWindow(&_window);
		phyWorld_1.SetCameraOffset(b2Vec2{ 0.0f, 0.0f });
		phyWorld_1.SetCameraZoom(0.8f);
		// 物体
		//地板1
		{
			b2BodyDef ground_bdef = b2DefaultBodyDef();
			ground_bdef.position = b2Vec2({ -5.0f, 0.0f });
			b2Polygon ground_box = b2MakeBox(5.0f, 1.0f);
			b2ShapeDef ground_sdef = b2DefaultShapeDef();
			ground_sdef.material.friction = 0.6f;
			ground_bodyId = phyWorld_1.CreateBodyPolygon(&ground_bdef, &ground_box, &ground_sdef);
		}
		//地板2
		{
			b2BodyDef ground_bdef_1 = b2DefaultBodyDef();
			ground_bdef_1.position = b2Vec2({ 30.0f, -30.0f });
			b2Polygon ground_box_1 = b2MakeBox(200.0f, 1.0f);
			b2ShapeDef ground_sdef_1 = b2DefaultShapeDef();
			ground_sdef_1.material.friction = 0.5f;
			phyWorld_1.CreateBodyPolygon(&ground_bdef_1, &ground_box_1, &ground_sdef_1);
		}
		//地板3
		{
			b2BodyDef ground_bdef_1 = b2DefaultBodyDef();
			ground_bdef_1.position = b2Vec2({ -70.0f, -10.0f });
			b2Polygon ground_box_1 = b2MakeBox(1.0f, 100.0f);
			b2ShapeDef ground_sdef_1 = b2DefaultShapeDef();
			ground_sdef_1.material.friction = 0.5f;
			phyWorld_1.CreateBodyPolygon(&ground_bdef_1, &ground_box_1, &ground_sdef_1);
		}
		//物体1
		{
			b2BodyDef bodyDef_1 = b2DefaultBodyDef();
			bodyDef_1.type = b2_dynamicBody;
			bodyDef_1.position = b2Vec2({ 0.0f, 10.0f });
			b2Polygon dynamicBox_1 = b2MakeBox(2.0f, 2.0f);
			b2ShapeDef shapeDef_1 = b2DefaultShapeDef();
			shapeDef_1.density = 1.0f;
			shapeDef_1.material.friction = 0.2f;
			body_1_bodyId = phyWorld_1.CreateBodyPolygon(&bodyDef_1, &dynamicBox_1, &shapeDef_1);
		}
		//物体2
		{
			b2BodyDef bodyDef_2 = b2DefaultBodyDef();
			bodyDef_2.type = b2_dynamicBody;
			bodyDef_2.position = b2Vec2({ -2.0f, 16.0f });
			b2Polygon dynamicBox_2 = b2MakeBox(25.0f, 3.0f);
			b2ShapeDef shapeDef_2 = b2DefaultShapeDef();
			shapeDef_2.density = 1.0f;
			shapeDef_2.material.friction = 0.3f;
			body_2_bodyId = phyWorld_1.CreateBodyPolygon(&bodyDef_2, &dynamicBox_2, &shapeDef_2);
		}
		//物体3
		{
			b2BodyDef bodyDef_3 = b2DefaultBodyDef();
			bodyDef_3.type = b2_dynamicBody;
			bodyDef_3.position = b2Vec2({ 8.0f, 25.0f });
			b2Circle dynamicCircle_3;
			dynamicCircle_3.center = { 0,0 };
			dynamicCircle_3.radius = 4;
			b2ShapeDef shapeDef_3 = b2DefaultShapeDef();
			shapeDef_3.density = 1.0f;
			shapeDef_3.material.friction = 0.3f;
			body_3_bodyId = phyWorld_1.CreateBodyCircle(&bodyDef_3, &dynamicCircle_3, &shapeDef_3);
		}
		//物体4
		{
			b2BodyDef bodyDef = b2DefaultBodyDef();
			bodyDef.type = b2_dynamicBody;
			bodyDef.position = b2Vec2({ -33.0f, 3.0f });
			b2Polygon dynamicBox = b2MakeBox(3.0f, 23.0f);
			b2ShapeDef shapeDef = b2DefaultShapeDef();
			shapeDef.density = 2.0f;
			shapeDef.material.friction = 0.4f;
			body_4_bodyId = phyWorld_1.CreateBodyPolygon(&bodyDef, &dynamicBox, &shapeDef);
		}
		//物体组
		{
			b2BodyDef bodyDef = b2DefaultBodyDef();
			bodyDef.type = b2_dynamicBody;
			bodyDef.position = b2Vec2({ -50.0f, 1.0f });
			//b2Polygon dynamicBox = b2MakeBox(1.0f, 1.0f);
			b2Circle dynamicCircle;
			dynamicCircle.center = { 0,0 };
			dynamicCircle.radius = 1;
			b2ShapeDef shapeDef = b2DefaultShapeDef();
			shapeDef.density = 2.0f;
			shapeDef.material.friction = 0.4f;
			for (int g = 0; g <= 100; g++)
			{
				for (int i = 0; i <= 15; i++)
				{
					bodyDef.position = b2Vec2({ -67.0f + i * 2.0f, 1.0f + g * 2.0f });
					phyWorld_1.CreateBodyCircle(&bodyDef, &dynamicCircle, &shapeDef);
				}
			}
		}
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

		//窗口焦点
		const bool isThisWindowhasFocus = _window.hasFocus();

        text->setFillColor(sf::Color(
            static_cast<uint8_t>(textColor[0] * 255),
            static_cast<uint8_t>(textColor[1] * 255),
            static_cast<uint8_t>(textColor[2] * 255)
        ));

		float cameraMoveSpeed = 0.1f;
		//逻辑
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Scan::LShift)) { cameraMoveSpeed *= 3.0f * 1/phyWorld_1.GetCameraZoom(); }
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Scan::A) && isThisWindowhasFocus) { phyWorld_1.MoveCamera({ -cameraMoveSpeed,0 }); }
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Scan::D) && isThisWindowhasFocus) { phyWorld_1.MoveCamera({ cameraMoveSpeed,0 }); }
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Scan::W) && isThisWindowhasFocus) { phyWorld_1.MoveCamera({ 0,cameraMoveSpeed }); }
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Scan::S) && isThisWindowhasFocus) { phyWorld_1.MoveCamera({ 0,-cameraMoveSpeed }); }
		//phyWorld_1.FollowBody(ground_bodyId);



		// 物理更新
		float deltaTime = clock.restart().asSeconds();
		accumulator += deltaTime;
		while (accumulator >= timeStep) {
			b2WorldId worldId = phyWorld_1.GetWorldId();
			b2World_Step(worldId, timeStep, subStepCount);
			accumulator -= timeStep;
			std::cout << deltaTime << "|物理更新" << std::endl;
		}
	}

    void Bzd_SFML_draw() {
        //窗口
        auto& _window = AppInit::getWinodw();
		
		//BOX2D
		phyWorld_1.Render();

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
			bool isFocused = ImGui::IsWindowFocused();
			if (bestHoverCheck || isFocused)
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
			ImGui::NewLine();
			ImGui::Text(u8"全局鼠标坐标 X:%d Y:%d", _globalMousePos.x, _globalMousePos.y);
			ImGui::Text(u8"程序窗口鼠标坐标 X:%d Y:%d", _windowMousePos.x, _windowMousePos.y);
			ImGui::Text(u8"焦点是否为此窗口: %s", _window.hasFocus() ? u8"是" : u8"否");
			ImGui::Text(u8"程序窗口位置: (%d, %d)", _window.getPosition().x, _window.getPosition().y);
			ImGui::NewLine();
			ImGui::Text(u8"程序窗口大小 当前: (%d, %d)", _window.getSize().x, _window.getSize().y);
			float k_size = AppInit::getK_size();
			static int currentItem = 4;
			static const char* items[] = { u8"1700x900",u8"1500x794" ,u8"1300x688", u8"1000x529", u8"800x423(默认)" };
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
			ImGui::TextColored(ImVec4(1, 0, 0, 1), u8"性能:");
			float deltaTime = clock.restart().asSeconds();
			float fps = 1.0f / deltaTime;
			ImGui::Text(u8"帧率(SFML): %.1f FPS", fps);
			ImGui::Text(u8"帧率(IMGUI): %.1f FPS", ImGui::GetIO().Framerate);
			ImGui::NewLine();
			ImGui::TextColored(ImVec4(0, 1, 0, 1), u8"box2d调试:");
			b2Vec2 camera_CameraCenter = phyWorld_1.GetCameraOffset();
			ImGui::Text(u8"相机世界坐标: (%0.1f , %0.1f)", camera_CameraCenter.x, camera_CameraCenter.y);
			b2Vec2 body1Ps = b2Body_GetPosition(body_1_bodyId);
			static float w_Czoom = 0.5f;
			ImGui::SliderFloat(u8"相机缩放", &w_Czoom, 0.1f, 6.0f);
			phyWorld_1.SetCameraZoom(w_Czoom);
;			ImGui::Text(u8"物体1 坐标: (%0.1f , %0.1f)", body1Ps.x, body1Ps.y);
			b2Vec2 body2Ps = b2Body_GetPosition(body_2_bodyId);
			ImGui::Text(u8"物体2 坐标: (%0.1f , %0.1f)", body2Ps.x, body2Ps.y);
			b2Vec2 body3Ps = b2Body_GetPosition(body_3_bodyId);
			ImGui::Text(u8"物体3 坐标: (%0.1f , %0.1f)", body3Ps.x, body3Ps.y);
			ImGui::NewLine();

			ImGui::End();

			ImGui::PopStyleVar(); // 透明度调整结尾
		}
	}

	//程序结束
	void Bzd_End() {
		ImGui::SFML::Shutdown();
	}
}