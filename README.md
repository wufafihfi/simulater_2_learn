# simulater_2_learn
## 不要在意这个储存库分支的名字
## 多线程不做了
- 1.我太菜了
- 2.我已经耗费了很多精力了,在这方面一直没能成功,累了
# Lua使用须知
- 这里的可能做的不够好,发布文件里有使用指北（.HTML）
## 格式
- 此程序的LUA脚本格式跟大多数游戏引擎所用的脚本基本格式差不多(Godot,Unity,虚幻C++没用过)
## 全局变量
- 在创建实例后程序会在其中创建一个全局变量"BodyID_int",这是挂载此脚本的物体的ID,类型为 int(c++)
- BodyID_int是涉及挂载此脚本物体的函数需要用到的,那些函数用此id查找对应的物体！！
## 入口函数
- function INIT(instanceId) 脚本初次执行时的初始化方法,你可以在这里编写初始化代码,此方法可以不写.instanceId为现在运行的实例的ID,类型为 string(c++)
- function UPDATE(dt,instanceId) 程序每一帧会调用一次此方法,你可以在这里编写更新代码.instanceId为现在运行的实例的ID,类型为string(c++);dt为程序在上一帧执行花费的时间,单位为 秒(s)
## 获取API函数表
```lua
local API --定义全局变量 名字叫"API"

function INIT(instanceId)
    --获取API函数表
    API = cpp_GETAPI() --此方法要在INIT或是UPDATE里面,因为在执行这两个方法之前cpp_GETAPI是未定义的
end
```
## 杂项
```lua
--这里的print是输出文本在所选物体属性的LUA调试输出栏里
API.print(string.format("zeroG run! Id:%s",instanceId ), BodyID_int) --这里BodyID_int
API.print(string.format("%d", BodyID_int), BodyID_int) --参数输出示例
--获取自程序运行以来的时间
local current_time = API.time_ms() --数值单位为 毫秒(ms)
```
## 物理交互
```lua
local Ps = API.getPosition(BodyID_int) --获取物体位置(几何中心点)
local Ag = API.getAngle(BodyID_int) --获取物体角度(弧度制)
local V = API.getVelocity(BodyID_int) --获取物体速度
local m = API.getMass(BodyID_int) --获取物体质量
local g =  API.getGravity(BodyID_int) --获取世界的重力加速度

API.ApplyForce(BodyID_int,{pidF_x,pidF_y-m*g.y},Ps) --给物体施加力,参数(物体ID,力[x,y],施力点[x,y]),这里的施力点是物体在物理世界的坐标
API.ApplyTorque(BodyID_int,m*g.y*2) --给物体应用扭矩,参数(物体ID,扭矩)
```
