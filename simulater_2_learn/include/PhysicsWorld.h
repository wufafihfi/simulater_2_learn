#ifndef PHYSICSWORLD
#define PHYSICSWORLD

#include <iostream>

#include <box2d/box2d.h>
#include <SFML/Graphics.hpp>

namespace bzd_Phy {
    class PhysicsWorld {
    private:
        sf::RenderWindow* window;
        sf::View view;  // 原来的视图，不是gameView

        b2WorldId worldId;
        std::vector<b2BodyId> bodyId_s;  // 存储创建的物体

        // 转换比例：1米 = 30像素
        const float SCALE = 30.0f;
        b2Vec2 cameraOffset;  // 摄像机偏移 世界坐标
        float cameraZoom = 1.0f;    // 摄像机缩放

    public:
        PhysicsWorld(b2WorldDef* _worldDef, b2Vec2 _gravity);
        ~PhysicsWorld();

        // 基础功能
        void setWindow(sf::RenderWindow* _window);
        void setView(sf::View _view);  // 恢复这个函数
        void Initialize(b2WorldDef* _worldDef);
        void Step(float& timeStep, int& subStepCount);
        b2WorldId& GetWorldId();

        // 创建物体
        b2BodyId CreateBodyPolygon(b2BodyDef* _BodyDef, b2Polygon* _Box, b2ShapeDef* _shapeDef);
        b2BodyId CreateBodyCircle(b2BodyDef* _BodyDef, b2Circle* _circle, b2ShapeDef* _shapeDef);

        // 渲染到SFML窗口
        void Render();

        // 工具函数
        b2Vec2 ToBox2D(sf::Vector2f screenPos);
        sf::Vector2f ToScreen(b2Vec2 worldPos);
        b2Vec2 ToBox2D_L(sf::Vector2f screenPos);
        sf::Vector2f ToScreen_L(b2Vec2 worldPos);

        b2Vec2 GetCameraOffset();
        float GetCameraZoom();
        void SetCameraOffset(b2Vec2 offset);
        void SetCameraZoom(float zoom);
        void MoveCamera(b2Vec2 delta);
        void ZoomCamera(float factor);
        void FollowBody(b2BodyId& body);

    private:
        // 辅助绘制函数
        void DrawBody(b2BodyId& body);
        void DrawCoordinateSystemGrid();
        // 绘制测试
        void DrawTest_CoordinateTransformation();
    };
}
#endif
