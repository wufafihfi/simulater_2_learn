#include "PhysicsWorld.h"

namespace bzd_Phy {
    bodyData bzdPhyNullBody = { -1, u8"$NULL$" ,b2_nullBodyId ,false ,false ,false };
    bodyData& GetNullBody() {
        return bzdPhyNullBody;
    }

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
        if (drawBuffer.resize(sf::Vector2u(_view.getSize()))) {
            std::cout << "绘制缓冲区设置成功!" << std::endl;
        }
        else {
            std::cout << "绘制缓冲区设置失败!" << std::endl;
        }
    }

    void PhysicsWorld::Initialize(b2WorldDef* _worldDef) {
        worldId = b2CreateWorld(_worldDef);
    }

    void PhysicsWorld::Step(float& timeStep, int& subStepCount) {
        b2World_Step(worldId, timeStep, subStepCount);
    }

    b2BodyId PhysicsWorld::CreateBodyPolygon(b2BodyDef* _BodyDef, b2Polygon* _Box, b2ShapeDef* _shapeDef, bodyData Data) {
        DataPoor.push_back(Data);
        //_BodyDef->userData = &DataPoor.back();
        b2BodyId bodyId = b2CreateBody(worldId, _BodyDef);
        b2CreatePolygonShape(bodyId, _shapeDef, _Box);
        DataPoor.back().body_id = bodyId;
        DataPoor.back().BodyId_int = IdFuther;
        DataPoor.back().worldId = &worldId;
        IdFuther++;
        return bodyId;
    }

    b2BodyId PhysicsWorld::CreateBodyCircle(b2BodyDef* _BodyDef, b2Circle* _circle, b2ShapeDef* _shapeDef, bodyData Data) {
        DataPoor.push_back(Data);
        //_BodyDef->userData = &DataPoor.back();
        b2BodyId bodyId = b2CreateBody(worldId, _BodyDef);
        b2CreateCircleShape(bodyId, _shapeDef, _circle);
        DataPoor.back().body_id = bodyId;
        DataPoor.back().BodyId_int = IdFuther;
        DataPoor.back().worldId = &worldId;
        IdFuther++;
        return bodyId;
    }

    b2WorldId& PhysicsWorld::GetWorldId() {
        return worldId;
    }

    long long PhysicsWorld::GetDataPoorSize() {
        return DataPoor.size();
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
        //sf::Vector2f viewCenter = view.getCenter();
        //sf::Vector2f centeredViewPos = viewCenter - screenPos;

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
        //sf::Vector2f viewCenter = view.getCenter();
        //sf::Vector2f viewPos = centeredViewPos + viewCenter;
        sf::Vector2f viewPos = centeredViewPos;

        // 4. 将视图坐标转换为窗口像素坐标
        return sf::Vector2f(viewPos);
    }

    void PhysicsWorld::FollowBody(b2BodyId& body) {
        b2Vec2 targetPos = b2Body_GetPosition(body);
        b2Vec2 currentOffset = GetCameraOffset();
        b2Vec2 desiredOffset = targetPos - currentOffset;

        float followSpeed = 0.1f;
        b2Vec2 newOffset = currentOffset + desiredOffset * followSpeed;

        SetCameraOffset(newOffset);
    }

    // 多边形顶点计算
    sf::ConvexShape PhysicsWorld::PointConput(bodyData& bodyData) {
        b2Transform transform = b2Body_GetTransform(bodyData.body_id);
        float _cos = transform.q.c;
        float _sin = transform.q.s;

        b2ShapeId shapeId;
        b2Body_GetShapes(bodyData.body_id, &shapeId, 1);
        b2Polygon _polygon = b2Shape_GetPolygon(shapeId);
        sf::ConvexShape polygon(_polygon.count);

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

        return polygon;
    }
    /*   //// 有BUG的函数 现已有更好的方法替代  ////
    // 多边形顶转换 B2->SF
    sf::ConvexShape PhysicsWorld::PointConput_B2SF(bodyData& bodyData) {
        if (!b2Body_IsValid(bodyData.body_id)) {
            return sf::ConvexShape();  // 返回空形状
        }

        b2Transform transform = b2Body_GetTransform(bodyData.body_id);
        float _cos = transform.q.c;
        float _sin = transform.q.s;

        b2ShapeId shapeId;
        b2Body_GetShapes(bodyData.body_id, &shapeId, 1);
        b2Polygon _polygon = b2Shape_GetPolygon(shapeId);

        sf::ConvexShape polygon(_polygon.count);

        // 修复顶点计算
        for (int i = 0; i < _polygon.count; i++) {
            // 正确的旋转计算
            b2Vec2 vertex = _polygon.vertices[i];
            b2Vec2 rotated;
            rotated.x = _cos * vertex.x - _sin * vertex.y;
            rotated.y = _sin * vertex.x + _cos * vertex.y;

            // 世界坐标
            b2Vec2 worldVertex = rotated;

            // 转换到屏幕坐标
            sf::Vector2f screenPos = ToScreen_L(worldVertex);
            polygon.setPoint(i, screenPos);
        }
        polygon.setPosition(ToScreen(transform.p));

        return polygon;
    }
    */

    // 获取摄像机参数
    b2Vec2 PhysicsWorld::GetCameraOffset() { return cameraOffset; }
    float PhysicsWorld::GetCameraZoom() { return cameraZoom; }

    void PhysicsWorld::SetCameraOffset(b2Vec2 offset) { cameraOffset = offset; }
    void PhysicsWorld::SetCameraZoom(float zoom) { cameraZoom = zoom; }
    void PhysicsWorld::MoveCamera(b2Vec2 delta) { cameraOffset += delta; }
    void PhysicsWorld::ZoomCamera(float factor) { cameraZoom *= factor; }

    void PhysicsWorld::drawbegin() {
        drawBuffer.clear();
    }

    // 渲染
    void PhysicsWorld::Render() {
        //DrawTest_CoordinateTransformation();
        DrawCameraCenter();
        DrawCoordinateSystemGrid();

        // 检查
        DataPoor.erase(
            std::remove_if(DataPoor.begin(), DataPoor.end(),
                [](bodyData data) { return !b2Body_IsValid(data.body_id); }),
            DataPoor.end()
        );
        // 遍历所有物体
        for (auto& data : DataPoor) {
            DrawBody(data);
        }
    }

    void PhysicsWorld::Display() {
        sf::Sprite sprite(drawBuffer.getTexture());
        sf::FloatRect textureRect({ 0, 0 },
            { static_cast<float>(drawBuffer.getSize().x),
             static_cast<float>(drawBuffer.getSize().y) });

        // 将纹理矩形上下翻转
        sprite.setTextureRect(sf::IntRect(
            { 0,
            static_cast<int>(textureRect.size.y) },  // 从底部开始
            { static_cast<int>(textureRect.size.x),
            static_cast<int>(-textureRect.size.y) }  // 负高度表示翻转
        ));
        window->draw(sprite);
    }

    bool PhysicsWorld::bodyIdStatu(b2BodyId& body) {
        if (b2Body_IsValid(body)) {
            return true;
        }
        return false;
    }

    void PhysicsWorld::DrawBody(bodyData& bodyData) {
        if (!b2Body_IsValid(bodyData.body_id)) {
            return;
        }

        b2Transform transform = b2Body_GetTransform(bodyData.body_id);
        float _cos = transform.q.c;
        float _sin = transform.q.s;

        int32_t shapeCount = b2Body_GetShapeCount(bodyData.body_id);
        b2ShapeId shapeId;
        if (shapeCount > 0) {
            b2Body_GetShapes(bodyData.body_id, &shapeId, 1);
            b2ShapeType shapeType = b2Shape_GetType(shapeId);

            if (shapeType == b2_polygonShape) {
                sf::ConvexShape polygon = PointConput(bodyData);
                polygon.setOutlineThickness(-2.0f);

                if (b2Body_GetType(bodyData.body_id) == b2_dynamicBody) {
                    polygon.setFillColor(sf::Color(200, 100, 100, 200));
                    polygon.setOutlineColor(sf::Color(220, 120, 120, 200));
                }
                else {
                    polygon.setFillColor(sf::Color(100, 100, 100, 200));
                    polygon.setOutlineColor(sf::Color(120, 120, 120, 200));
                }

                drawBuffer.draw(polygon);
            }

            if (shapeType == b2_circleShape) {
                sf::Color lineColor = sf::Color(200, 100, 100, 200);
                sf::Color fillColor = sf::Color(220, 120, 120, 200);
                if (b2Body_GetType(bodyData.body_id) != b2_dynamicBody) {
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
                drawBuffer.draw(circle);

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
                drawBuffer.draw(line, 2, sf::PrimitiveType::Lines);
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
            drawBuffer.draw(line, 2, sf::PrimitiveType::Lines);
        }

        // 绘制水平线
        for (float y = startY; y <= endY; y += gridSize) {
            sf::Vector2f screenStart = ToScreen(b2Vec2({ left, y }));
            sf::Vector2f screenEnd = ToScreen(b2Vec2({ right, y }));

            sf::Vertex line[] = {
                sf::Vertex({screenStart, gridColor}),
                sf::Vertex({screenEnd, gridColor})
            };
            drawBuffer.draw(line, 2, sf::PrimitiveType::Lines);
        }

        // X轴（红色）- 绘制在可见区域的中间
        float yForXAxis = (bottom + top) / 2;
        sf::Vector2f xStart = ToScreen(b2Vec2({ left, 0 }));
        sf::Vector2f xEnd = ToScreen(b2Vec2({ right, 0 }));
        sf::Vertex xAxis[] = {
            sf::Vertex({xStart, sf::Color::Red}),
            sf::Vertex({xEnd, sf::Color::Red})
        };
        drawBuffer.draw(xAxis, 2, sf::PrimitiveType::Lines);

        // Y轴（绿色）- 绘制在可见区域的中间
        float xForYAxis = (left + right) / 2;
        sf::Vector2f yStart = ToScreen(b2Vec2({ 0, bottom }));
        sf::Vector2f yEnd = ToScreen(b2Vec2({ 0, top }));
        sf::Vertex yAxis[] = {
            sf::Vertex({yStart, sf::Color::Green}),
            sf::Vertex({yEnd, sf::Color::Green})
        };
        drawBuffer.draw(yAxis, 2, sf::PrimitiveType::Lines);
    }

    void PhysicsWorld::DrawCameraCenter() {
        sf::Vector2f viewSize = view.getSize();
        sf::Vector2f viewCenter = view.getCenter();

        sf::Vertex VAxis[] = {
            sf::Vertex({{viewCenter.x,0}, sf::Color::White }),
            sf::Vertex({{viewCenter.x,viewSize.y}, sf::Color::White})
        };
        drawBuffer.draw(VAxis, 2, sf::PrimitiveType::Lines);

        sf::Vertex HAxis[] = {
             sf::Vertex({{0,viewCenter.y}, sf::Color::White }),
            sf::Vertex({{viewSize.x,viewCenter.y}, sf::Color::White})
        };
        drawBuffer.draw(HAxis, 2, sf::PrimitiveType::Lines);
    }

    void PhysicsWorld::DrawBodyVelosity(b2BodyId& body) {
        if (!b2Body_IsValid(body)) {
            return;
        }

        b2Transform transform = b2Body_GetTransform(body);
        b2Vec2 velosity = b2Body_GetLinearVelocity(body);

        sf::Vector2f lineStart = ToScreen(transform.p);
        sf::Vector2f lineEndScreen = ToScreen(transform.p + velosity);
        sf::Vertex Line_v[] = {
        sf::Vertex({lineStart, sf::Color::White }),
        sf::Vertex({lineEndScreen, sf::Color::White})
        };
        drawBuffer.draw(Line_v, 2, sf::PrimitiveType::Lines);
    }

    // 坐标转换测试函数
    void PhysicsWorld::DrawTest_CoordinateTransformation() {
        sf::Vector2f lineStart = {0,0};
        sf::Vector2f lineEndScreen = view.getSize();
        sf::Vertex line[] = {
            sf::Vertex({lineStart, sf::Color::Green}),
            sf::Vertex({lineEndScreen, sf::Color::Green })
        };
        drawBuffer.draw(line, 4, sf::PrimitiveType::Lines);

        sf::Vector2f lineStart1 = ToScreen({ 0,0 });
        sf::Vector2f lineEndScreen1 = ToScreen(ToBox2D(view.getSize()));
        sf::Vertex line1[] = {
            sf::Vertex({lineStart1, sf::Color::Red}),
            sf::Vertex({lineEndScreen1, sf::Color::Red })
        };
        drawBuffer.draw(line1, 5, sf::PrimitiveType::Lines);
    }

    void PhysicsWorld::DrawTest_PointShow_WINDOW(sf::Vector2f p) {
        /*
        sf::Vector2f lineStart1 = p;
        sf::Vector2f lineEndScreen1 = ToScreen(ToBox2D(view.getSize()));
        sf::Vertex line1[] = {
            sf::Vertex({lineStart1, sf::Color::Red}),
            sf::Vertex({lineEndScreen1, sf::Color::Red })
        };
        drawBuffer.draw(line1, 5, sf::PrimitiveType::Lines);
        */

        sf::CircleShape circle(10);
        circle.setOrigin({ 10,10 });
        circle.setPosition(p);
        circle.setFillColor(sf::Color(200, 200, 0, 100));
        circle.setOutlineColor(sf::Color(255, 255, 0, 100));
        circle.setOutlineThickness(-2.0f);
        //std::cout << " s" << std::endl;
        window->draw(circle);
    }
    void PhysicsWorld::DrawTest_PointShow_DrawBuffer(sf::Vector2f p) {
        /*
        sf::Vector2f lineStart1 = p;
        sf::Vector2f lineEndScreen1 = ToScreen(ToBox2D(view.getSize()));
        sf::Vertex line1[] = {
            sf::Vertex({lineStart1, sf::Color::Red}),
            sf::Vertex({lineEndScreen1, sf::Color::Red })
        };
        drawBuffer.draw(line1, 5, sf::PrimitiveType::Lines);
        */

        sf::CircleShape circle(30);
        circle.setOrigin({ 30,30 });
        circle.setPosition(p);
        circle.setFillColor(sf::Color(200, 200, 0, 100));
        circle.setOutlineColor(sf::Color(255, 255, 0, 100));
        circle.setOutlineThickness(-2.0f);
        //std::cout << " s" << std::endl;
        drawBuffer.draw(circle);
    }

    // 叉积辅助函数
    float crossProduct(sf::Vector2f& a, sf::Vector2f& b, sf::Vector2f& p) {
        return (b.x - a.x) * (p.y - a.y) - (b.y - a.y) * (p.x - a.x);
    }
    // 矩形检测
    bool pointInConvexPolygon(sf::ConvexShape& shape, sf::Vector2f& point) {
        std::size_t pointCount = shape.getPointCount();
        if (pointCount < 3) return false;

        sf::Transform transform = shape.getTransform();

        // 获取第一个变换后的顶点作为参考
        sf::Vector2f p0 = transform.transformPoint(shape.getPoint(0));
        sf::Vector2f p1, p2;

        // 预先计算第一个边的方向作为参考符号
        p1 = transform.transformPoint(shape.getPoint(1));
        float referenceSign = crossProduct(p0, p1, point);

        // 检查所有边是否保持相同的方向（符号）
        for (std::size_t i = 1; i < pointCount; ++i) {
            std::size_t next = (i + 1) % pointCount;
            p1 = transform.transformPoint(shape.getPoint(i));
            p2 = transform.transformPoint(shape.getPoint(next));

            float currentSign = crossProduct(p1, p2, point);

            // 如果符号不同，点在多边形外
            if (referenceSign * currentSign < 0) {
                return false;
            }
        }

        return true;
    }

    // 屏幕物体选取
    //获取鼠标光标下的物体
    bodyData PhysicsWorld::screenBodySelect_1(sf::Vector2f Point) {
        bodyData returnData = bzdPhyNullBody;
        //int i = 1;
        //DrawTest_PointShow_DrawBuffer(Point);
        for (auto& data : DataPoor) {
            if (!b2Body_IsValid(data.body_id)) {
                continue;
            }

            b2Transform transform = b2Body_GetTransform(data.body_id);
            int32_t shapeCount = b2Body_GetShapeCount(data.body_id);
            b2ShapeId shapeId;

            if (shapeCount > 0) {
                b2Body_GetShapes(data.body_id, &shapeId, 1);
                b2ShapeType shapeType = b2Shape_GetType(shapeId);



                if (shapeType == b2_polygonShape) {
                    sf::ConvexShape polygon = PointConput(data);
                    if(pointInConvexPolygon(polygon, Point))
                    {
                        DrawTest_PointShow_DrawBuffer(ToScreen(transform.p));
                        data.isHovering = true;
                        //std::cout << "C!!" << i << std::endl;
                        //i++;
                        returnData = data;
                    }
                    else
                    {
                        data.isHovering = false;
                    }
                }

                if (shapeType == b2_circleShape) {
                    b2Circle _circle = b2Shape_GetCircle(shapeId);

                    sf::Vector2f centerP = ToScreen(transform.p);
                    sf::Vector2f screenRadius = ToScreen_L({ _circle.radius,_circle.radius });

                    sf::Vector2f delta_P = Point - centerP;

                    if (delta_P.length() <= screenRadius.x) {
                        DrawTest_PointShow_DrawBuffer(ToScreen(transform.p));
                        //std::cout << "C!!" << i << std::endl;
                        //i++;
                        data.isHovering = true;
                        returnData = data;
                    }
                    else
                    {
                        data.isHovering = false;
                    }
                }
            }
        }
        
        return returnData;
    }
    //选取物品,不做选取触发逻辑
    bodyData PhysicsWorld::screenBodySelect_2(bodyData data) {
        for (auto& data : DataPoor) {
            if (!b2Body_IsValid(data.body_id)) {
                continue;
            }
            if (!data.isProminent)
            {
                continue;
            }
            data.isProminent = false;
        }

        DataPoor[data.BodyId_int].isProminent = true;
        data.isProminent = true;
        return data;
    }
    //绘制被选取状态指示图形
    void PhysicsWorld::drawSelectedBody() {
        //int i = 1;
        for (auto& data : DataPoor) {
            if (!b2Body_IsValid(data.body_id)) {
                continue;
            }
            /*
            if (data.isHovering)
            {
                std::cout << "h!!" << i << std::endl;
                i++;
            }
            */
            if (!data.isProminent)
            {
                continue;
            }
            b2Transform transform = b2Body_GetTransform(data.body_id);
            DrawTest_PointShow_DrawBuffer(ToScreen(transform.p));
        }
    }
}