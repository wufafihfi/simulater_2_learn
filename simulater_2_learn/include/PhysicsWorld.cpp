#include "PhysicsWorld.h"

namespace bzd_Phy {

    PhysicsWorld::PhysicsWorld(b2WorldDef* _worldDef, b2Vec2 _gravity) {
        _worldDef->gravity = _gravity;
        Initialize(_worldDef);
    }

    PhysicsWorld::~PhysicsWorld() {
        b2DestroyWorld(worldId);
    }

    void PhysicsWorld::setWindow(sf::RenderWindow* _window) {
        window = _window;
    }

    void PhysicsWorld::setView(sf::View _view) {
        view = _view;
    }

    void PhysicsWorld::Initialize(b2WorldDef* _worldDef) {
        worldId = b2CreateWorld(_worldDef);
    }

    void PhysicsWorld::Step(float& timeStep, int& subStepCount) {
        b2World_Step(worldId, timeStep, subStepCount);
    }

    b2BodyId PhysicsWorld::CreateBodyPolygon(b2BodyDef* _BodyDef, b2Polygon* _Box, b2ShapeDef* _shapeDef) {
        b2BodyId bodyId = b2CreateBody(worldId, _BodyDef);
        b2CreatePolygonShape(bodyId, _shapeDef, _Box);
        bodyId_s.push_back(bodyId);
        return bodyId;
    }

    b2BodyId PhysicsWorld::CreateBodyCircle(b2BodyDef* _BodyDef, b2Circle* _circle, b2ShapeDef* _shapeDef) {
        b2BodyId bodyId = b2CreateBody(worldId, _BodyDef);
        b2CreateCircleShape(bodyId, _shapeDef, _circle);
        bodyId_s.push_back(bodyId);
        return bodyId;
    }

    b2WorldId& PhysicsWorld::GetWorldId() {
        return worldId;
    }

    // 坐标转换 应用相机
    b2Vec2 PhysicsWorld::ToBox2D(sf::Vector2f screenPos) {
        // 2. 视图中心作为原点：从视图坐标转换到以视图中心为原点的坐标
        sf::Vector2f viewCenter = view.getCenter();
        sf::Vector2f centeredViewPos = screenPos - viewCenter;

        // 3. Y轴翻转（SFML Y向下，Box2D Y向上）
        centeredViewPos.y = -centeredViewPos.y;

        // 4. 应用相机变换
        b2Vec2 box2dPos;
        box2dPos.x = (centeredViewPos.x / (SCALE * cameraZoom)) + cameraOffset.x;
        box2dPos.y = (centeredViewPos.y / (SCALE * cameraZoom)) + cameraOffset.y;

        return box2dPos;
    }
    sf::Vector2f PhysicsWorld::ToScreen(b2Vec2 box2dPos) {
        // 1. 转换到视图中心的相对坐标（Box2D -> 视图相对坐标）
        sf::Vector2f centeredViewPos;
        centeredViewPos.x = (box2dPos.x - cameraOffset.x) * (SCALE * cameraZoom);
        centeredViewPos.y = (box2dPos.y - cameraOffset.y) * (SCALE * cameraZoom);

        // 2. Y轴翻转
        centeredViewPos.y = -centeredViewPos.y;

        // 3. 从中心相对坐标转换到视图绝对坐标
        sf::Vector2f viewCenter = view.getCenter();
        sf::Vector2f viewPos = centeredViewPos + viewCenter;

        // 4. 将视图坐标转换为窗口像素坐标
        return sf::Vector2f(viewPos);
    }
    // 坐标转换 非相机
    b2Vec2 PhysicsWorld::ToBox2D_L(sf::Vector2f screenPos) {
        // 2. 视图中心作为原点：从视图坐标转换到以视图中心为原点的坐标
        sf::Vector2f centeredViewPos = screenPos ;

        // 3. Y轴翻转（SFML Y向下，Box2D Y向上）
        centeredViewPos.y = -centeredViewPos.y;

        // 4. 应用相机变换
        b2Vec2 box2dPos;
        box2dPos.x = (centeredViewPos.x / (SCALE * cameraZoom));
        box2dPos.y = (centeredViewPos.y / (SCALE * cameraZoom));

        return box2dPos;
    }
    sf::Vector2f PhysicsWorld::ToScreen_L(b2Vec2 box2dPos) {
        // 1. 转换到视图中心的相对坐标（Box2D -> 视图相对坐标）
        sf::Vector2f centeredViewPos;
        centeredViewPos.x = (box2dPos.x) * (SCALE * cameraZoom);
        centeredViewPos.y = (box2dPos.y) * (SCALE * cameraZoom);

        // 2. Y轴翻转
        centeredViewPos.y = -centeredViewPos.y;

        // 3. 从中心相对坐标转换到视图绝对坐标
        sf::Vector2f viewPos = centeredViewPos;

        // 4. 将视图坐标转换为窗口像素坐标
        return sf::Vector2f(viewPos);
    }

    void PhysicsWorld::FollowBody(b2BodyId& body) {
        b2Vec2 targetPos = b2Body_GetPosition(body);
        b2Vec2 currentOffset = GetCameraOffset();
        b2Vec2 desiredOffset = currentOffset - targetPos;

        float followSpeed = 0.1f;
        b2Vec2 newOffset = currentOffset + (desiredOffset - currentOffset) * followSpeed;

        SetCameraOffset(newOffset);
    }

    // 获取摄像机参数
    b2Vec2 PhysicsWorld::GetCameraOffset() { return cameraOffset; }
    float PhysicsWorld::GetCameraZoom() { return cameraZoom; }

    void PhysicsWorld::SetCameraOffset(b2Vec2 offset) { cameraOffset = offset; }
    void PhysicsWorld::SetCameraZoom(float zoom) { cameraZoom = zoom; }
    void PhysicsWorld::MoveCamera(b2Vec2 delta) { cameraOffset += delta; }
    void PhysicsWorld::ZoomCamera(float factor) { cameraZoom *= factor; }

    // 渲染函数（保持简单，不修改视图）
    void PhysicsWorld::Render() {
        if (!window) return;

        //DrawTest_CoordinateTransformation();
        DrawCoordinateSystemGrid();

        // 遍历所有物体
        for (auto& bodyId : bodyId_s) {
            DrawBody(bodyId);
        }
    }

    void PhysicsWorld::DrawBody(b2BodyId& body) {
        b2Transform transform = b2Body_GetTransform(body);
        float _cos = transform.q.c;
        float _sin = transform.q.s;

        int32_t shapeCount = b2Body_GetShapeCount(body);
        b2ShapeId shapeId;
        if (shapeCount > 0) {
            b2Body_GetShapes(body, &shapeId, 1);
            b2ShapeType shapeType = b2Shape_GetType(shapeId);

            if (shapeType == b2_polygonShape) {
                b2Polygon _polygon = b2Shape_GetPolygon(shapeId);
                sf::ConvexShape polygon(_polygon.count);
                polygon.setOutlineThickness(-2.0f);

                // 修复顶点计算
                for (int i = 0; i < _polygon.count; i++) {
                    // 正确的旋转计算
                    b2Vec2 vertex = _polygon.vertices[i];
                    b2Vec2 rotated;
                    rotated.x = _cos * vertex.x - _sin * vertex.y;
                    rotated.y = _sin * vertex.x + _cos * vertex.y;

                    // 世界坐标
                    b2Vec2 worldVertex = transform.p + rotated;

                    // 转换到屏幕坐标
                    sf::Vector2f screenPos = ToScreen(worldVertex);
                    polygon.setPoint(i, screenPos);
                }

                if (b2Body_GetType(body) == b2_dynamicBody) {
                    polygon.setFillColor(sf::Color(200, 100, 100, 200));
                    polygon.setOutlineColor(sf::Color(220, 120, 120, 200));
                }
                else {
                    polygon.setFillColor(sf::Color(100, 100, 100, 200));
                    polygon.setOutlineColor(sf::Color(120, 120, 120, 200));
                }

                window->draw(polygon);
            }

            if (shapeType == b2_circleShape) {
                sf::Color lineColor = sf::Color(200, 100, 100, 200);
                sf::Color fillColor = sf::Color(220, 120, 120, 200);
                if (b2Body_GetType(body) != b2_dynamicBody) {
                    lineColor = sf::Color(100, 100, 100, 200);
                    fillColor = sf::Color(120, 120, 120, 200);
                }

                // 圆
                b2Circle _circle = b2Shape_GetCircle(shapeId);
                sf::Vector2f screenRadius = ToScreen_L({ _circle.radius,_circle.radius });
                sf::CircleShape circle(screenRadius.x);
                circle.setOrigin({ screenRadius.x,
                                   screenRadius.x });
                circle.setRotation(-sf::radians( atan2(ToScreen_L({ transform.q.s,0 }).x, ToScreen_L({ transform.q.c,0 }).x)));
                sf::Vector2f screenPos = ToScreen(transform.p);
                circle.setPosition(screenPos);
                circle.setFillColor(fillColor);
                circle.setOutlineColor(lineColor);
                circle.setOutlineThickness(-2.0f);
                window->draw(circle);

                // 方向线
                b2Vec2 lineEnd = b2Vec2({ _circle.radius, 0 });
                b2Vec2 lineEndRotated;
                lineEndRotated.x = _cos * lineEnd.x;
                lineEndRotated.y = _sin * lineEnd.x;

                sf::Vector2f lineStart = ToScreen(transform.p);
                sf::Vector2f lineEndScreen = ToScreen(transform.p + lineEndRotated);

                sf::Vertex line[] = {
                    sf::Vertex({lineStart, lineColor}),
                    sf::Vertex({lineEndScreen, lineColor })
                };
                window->draw(line, 2, sf::PrimitiveType::Lines);
            }
        }
    }

    // 网格绘制
    void PhysicsWorld::DrawCoordinateSystemGrid() {
        // 获取当前视图参数
        sf::Vector2f viewCenter = view.getCenter();
        sf::Vector2f viewSize = view.getSize();

        // 计算屏幕可见区域的四个角（屏幕坐标）
        sf::Vector2f screenTopLeft(0, 0);
        sf::Vector2f screenTopRight(viewSize.x, 0);
        sf::Vector2f screenBottomLeft(0, viewSize.y);
        sf::Vector2f screenBottomRight(viewSize.x, viewSize.y);

        // 转换为Box2D世界坐标（使用带摄像机的转换）
        b2Vec2 worldTopLeft = ToBox2D(screenTopLeft);
        b2Vec2 worldTopRight = ToBox2D(screenTopRight);
        b2Vec2 worldBottomLeft = ToBox2D(screenBottomLeft);
        b2Vec2 worldBottomRight = ToBox2D(screenBottomRight);

        // 计算可见网格范围
        float left = std::min(std::min(worldTopLeft.x, worldTopRight.x),
            std::min(worldBottomLeft.x, worldBottomRight.x));
        float right = std::max(std::max(worldTopLeft.x, worldTopRight.x),
            std::max(worldBottomLeft.x, worldBottomRight.x));
        float top = std::max(std::max(worldTopLeft.y, worldTopRight.y),
            std::max(worldBottomLeft.y, worldBottomRight.y));
        float bottom = std::min(std::min(worldTopLeft.y, worldTopRight.y),
            std::min(worldBottomLeft.y, worldBottomRight.y));

        // 网格参数
        float gridSize = 10.0f;  // 网格大小（Box2D单位）
        sf::Color gridColor(50, 50, 50, 100);  // 半透明灰色

        // 对齐到网格
        float startX = floor(left / gridSize) * gridSize;
        float startY = floor(bottom / gridSize) * gridSize;
        float endX = ceil(right / gridSize) * gridSize;
        float endY = ceil(top / gridSize) * gridSize;

        // 绘制垂直线
        for (float x = startX; x <= endX; x += gridSize) {
            sf::Vector2f screenStart = ToScreen(b2Vec2({ x, bottom }));
            sf::Vector2f screenEnd = ToScreen(b2Vec2({ x, top }));

            sf::Vertex line[] = {
                sf::Vertex({screenStart, gridColor}),
                sf::Vertex({screenEnd, gridColor})
            };
            window->draw(line, 2, sf::PrimitiveType::Lines);
        }

        // 绘制水平线
        for (float y = startY; y <= endY; y += gridSize) {
            sf::Vector2f screenStart = ToScreen(b2Vec2({ left, y }));
            sf::Vector2f screenEnd = ToScreen(b2Vec2({ right, y }));

            sf::Vertex line[] = {
                sf::Vertex({screenStart, gridColor}),
                sf::Vertex({screenEnd, gridColor})
            };
            window->draw(line, 2, sf::PrimitiveType::Lines);
        }

        // X轴（红色）- 绘制在可见区域的中间
        float yForXAxis = (bottom + top) / 2;
        sf::Vector2f xStart = ToScreen(b2Vec2({ left, 0 }));
        sf::Vector2f xEnd = ToScreen(b2Vec2({ right, 0 }));
        sf::Vertex xAxis[] = {
            sf::Vertex({xStart, sf::Color::Red}),
            sf::Vertex({xEnd, sf::Color::Red})
        };
        window->draw(xAxis, 2, sf::PrimitiveType::Lines);

        // Y轴（绿色）- 绘制在可见区域的中间
        float xForYAxis = (left + right) / 2;
        sf::Vector2f yStart = ToScreen(b2Vec2({ 0, bottom }));
        sf::Vector2f yEnd = ToScreen(b2Vec2({ 0, top }));
        sf::Vertex yAxis[] = {
            sf::Vertex({yStart, sf::Color::Green}),
            sf::Vertex({yEnd, sf::Color::Green})
        };
        window->draw(yAxis, 2, sf::PrimitiveType::Lines);
    }

    // 坐标转换测试函数
    void PhysicsWorld::DrawTest_CoordinateTransformation() {
        sf::Vector2f lineStart = {0,0};
        sf::Vector2f lineEndScreen = view.getSize();
        sf::Vertex line[] = {
            sf::Vertex({lineStart, sf::Color::Green}),
            sf::Vertex({lineEndScreen, sf::Color::Green })
        };
        window->draw(line, 4, sf::PrimitiveType::Lines);

        sf::Vector2f lineStart1 = ToScreen({ 0,0 });
        sf::Vector2f lineEndScreen1 = ToScreen(ToBox2D(view.getSize()));
        sf::Vertex line1[] = {
            sf::Vertex({lineStart1, sf::Color::Red}),
            sf::Vertex({lineEndScreen1, sf::Color::Red })
        };
        window->draw(line1, 5, sf::PrimitiveType::Lines);
    }
}