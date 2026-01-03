#ifndef PHYSICSWORLD
#define PHYSICSWORLD

#include <iostream>

#include <box2d/box2d.h>
#include <SFML/Graphics.hpp>

namespace bzd_Phy {
    struct bodyData {
    public:
        std::string bodyName = u8"$NULL$";
        b2BodyId body_id = b2_nullBodyId;

        bool isProminent = false;
    };

    class PhysicsWorld {
    private:
        sf::RenderWindow* window;
        sf::RenderTexture drawBuffer;
        sf::View view;

        b2WorldId worldId;
        std::vector<bodyData> DataPoor;

        // 转换比例：1米 = 30像素
        const float SCALE = 30.0f;
        b2Vec2 cameraOffset;  // 摄像机偏移 世界坐标
        float cameraZoom = 1.0f;    // 摄像机缩放

    public:
        PhysicsWorld(b2WorldDef* _worldDef, b2Vec2 _gravity);
        ~PhysicsWorld();

        // 基础功能
        void setWindow(sf::RenderWindow* _window);
        void setView(sf::View _view);
        void Initialize(b2WorldDef* _worldDef);
        void Step(float& timeStep, int& subStepCount);
        b2WorldId& GetWorldId();
        long long GetDataPoorSize();

        // 创建物体
        b2BodyId CreateBodyPolygon(b2BodyDef* _BodyDef, b2Polygon* _Box, b2ShapeDef* _shapeDef, bodyData Data);
        b2BodyId CreateBodyCircle(b2BodyDef* _BodyDef, b2Circle* _circle, b2ShapeDef* _shapeDef, bodyData Data);

        // 渲染到SFML窗口
        void drawbegin();
        void Render();
        void Display();

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
        void DrawBodyVelosity(b2BodyId& body);
        bodyData screenBodySelect(sf::Vector2f Point);
        bool bodyIdStatu(b2BodyId& body);

        void DrawTest_PointShow_WINDOW(sf::Vector2f p);
        void DrawTest_PointShow_DrawBuffer(sf::Vector2f p);

    private:
        // 辅助绘制函数
        void DrawBody(bodyData& bodyData);
        void DrawCoordinateSystemGrid();
        void DrawCameraCenter();
        // 绘制测试
        void DrawTest_CoordinateTransformation();
        // 顶点计算
        sf::ConvexShape PointConput(bodyData& bodyData);
        sf::ConvexShape PointConput_B2SF(bodyData& bodyData);
    };
}
#endif
