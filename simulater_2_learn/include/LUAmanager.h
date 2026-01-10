#ifndef LUAMANAGER_H
#define LUAMANAGER_H

#include <string>
#include <vector>
#include <unordered_map>
#include <memory>
#include <functional>
#include <variant>
#include <iostream>

extern "C" {
#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>
}

namespace LuaManager {

    // 脚本实例类 - 每个实例独立加载脚本
    class LuaInstance {
    private:
        lua_State* L;
        std::string filepath;   // 脚本文件路径
        std::string instanceId; // 实例唯一标识
        bool hasUpdateFunc;     // 是否有UPDATE函数
        bool hasInitFunc;       // 是否有INIT函数

    public:
        LuaInstance(const std::string& filepath, const std::string& id = "");
        ~LuaInstance();

        bool load();  // 加载脚本文件
        bool executeUpdate(float deltaTime);
        bool executeInit(const std::vector<std::string>& args = {});

        const std::string& getId() const { return instanceId; }
        const std::string& getFilepath() const { return filepath; }
        const std::string& getScriptName() const { return scriptName; }
        bool isValid() const { return L != nullptr && hasUpdateFunc; }
        bool hasInit() const { return hasInitFunc; }

        // 设置实例特定数据
        void setFunc(const std::string& name, lua_CFunction func);
        void setNumber(const std::string& name, double value);
        void setInt(const std::string& name, int value);
        void setUInt32(const std::string& name, uint32_t value);
        void setString(const std::string& name, const std::string& value);
        double getNumber(const std::string& name) const;
        std::string getString(const std::string& name) const;

        // 添加自定义API调用方法
        bool callFunction(const std::string& funcName,
            const std::vector<lua_Number>& args = {});

        // 注册C++函数到Lua
        void registerCFunction(const std::string& name, lua_CFunction func);

        // 批量注册C++函数
        void registerCFunctions(const std::vector<std::pair<std::string, lua_CFunction>>& funcs);

        // 获取Lua状态机
        lua_State* getLuaState() const { return L; }

    private:
        std::string scriptName;  // 脚本名称（文件名）
    };

    // 主管理器类
    class LuaManager {
    private:
        // 运行中的实例（按脚本文件路径分组）
        std::unordered_map<std::string, std::vector<std::unique_ptr<LuaInstance>>> instances;

        // 全局函数注册表（供所有实例使用）
        std::vector<std::pair<std::string, lua_CFunction>> globalFunctions;

        // 错误回调
        std::function<void(const std::string&, const std::string&)> errorCallback;

    public:
        LuaManager();
        ~LuaManager();

        // 实例管理 - 直接创建实例并加载脚本
        LuaInstance* createInstance(const std::string& filepath,
            const std::string& instanceId = "");

        void destroyInstance(const std::string& instanceId);
        void destroyAllInstances(const std::string& filepath);
        void destroyAll();

        // 批量更新
        void updateAll(float deltaTime);
        void updateInstancesFromFile(const std::string& filepath, float deltaTime);

        // 注册全局C++函数（所有新实例自动获得）
        void registerGlobalCFunction(const std::string& name, lua_CFunction func);

        // 注册到特定实例
        void registerInstanceCFunction(LuaInstance* instance,
            const std::string& funcName,
            lua_CFunction func);

        // 查询
        LuaInstance* getInstance(const std::string& instanceId) const;
        std::vector<LuaInstance*> getInstancesFromFile(const std::string& filepath) const;
        size_t getInstanceCount() const;
        size_t getInstanceCount(const std::string& filepath) const;

        // 错误处理
        void setErrorCallback(std::function<void(const std::string&, const std::string&)> callback);

    private:
        void handleError(const std::string& context, const std::string& error);
        std::string generateInstanceId() const;

        // 向实例注册全局函数
        void applyGlobalFunctions(LuaInstance* instance);
    };

} // namespace LuaManager

#endif // LUAMANAGER_H