#include "LuaManager.h"
#include <fstream>
#include <sstream>
#include <random>
#include <iostream>
#include <filesystem>

namespace fs = std::filesystem;

namespace LuaManager {

    // ==================== LuaInstance 实现 ====================

    LuaInstance::LuaInstance(const std::string& filepath, const std::string& id)
        : filepath(filepath),
        instanceId(id.empty() ? "instance_" + std::to_string(rand()) : id),
        hasUpdateFunc(false),
        hasInitFunc(false) {

        // 从文件路径提取脚本名称
        fs::path p(filepath);
        scriptName = p.stem().string();

        std::cout << "[LuaInstance] Creating instance: " << instanceId
            << " from: " << filepath << std::endl;
    }

    LuaInstance::~LuaInstance() {
        if (L) {
            lua_close(L);
        }
        std::cout << "[LuaInstance] Destroyed: " << instanceId << std::endl;
    }

    bool LuaInstance::load() {
        // 检查文件是否存在
        if (!fs::exists(filepath)) {
            std::cerr << "[LuaInstance] File not found: " << filepath << std::endl;
            return false;
        }

        // 创建Lua状态机
        L = luaL_newstate();
        if (!L) {
            std::cerr << "[LuaInstance] Failed to create Lua state" << std::endl;
            return false;
        }

        luaL_openlibs(L);
        std::cout << "[LuaInstance] Created Lua state for: " << instanceId << std::endl;

        // 加载脚本文件
        if (luaL_loadfile(L, filepath.c_str()) != LUA_OK) {
            std::string error = lua_tostring(L, -1);
            lua_pop(L, 1);
            std::cerr << "[LuaInstance] Failed to load script: " << error << std::endl;
            return false;
        }

        // 执行脚本（定义函数）
        if (lua_pcall(L, 0, 0, 0) != LUA_OK) {
            std::string error = lua_tostring(L, -1);
            lua_pop(L, 1);
            std::cerr << "[LuaInstance] Failed to execute script: " << error << std::endl;
            return false;
        }

        // 检查是否有UPDATE函数
        lua_getglobal(L, "UPDATE");
        hasUpdateFunc = lua_isfunction(L, -1);
        if (!hasUpdateFunc) {
            std::cerr << "[LuaInstance] Script has no UPDATE function: " << filepath << std::endl;
            lua_pop(L, 1);
            return false;
        }
        lua_pop(L, 1);

        // 检查是否有INIT函数
        lua_getglobal(L, "INIT");
        hasInitFunc = lua_isfunction(L, -1);
        if (hasInitFunc) {
            std::cout << "[LuaInstance] Script has INIT function" << std::endl;
        }
        lua_pop(L, 1);

        std::cout << "[LuaInstance] Successfully loaded: " << filepath
            << " (hasUpdate: " << hasUpdateFunc
            << ", hasInit: " << hasInitFunc << ")" << std::endl;

        return true;
    }

    bool LuaInstance::executeUpdate(float deltaTime) {
        if (!isValid()) return false;

        // 获取UPDATE函数
        lua_getglobal(L, "UPDATE");
        if (!lua_isfunction(L, -1)) {
            lua_pop(L, 1);
            return false;
        }

        // 压入参数
        lua_pushnumber(L, deltaTime);
        lua_pushstring(L, instanceId.c_str());

        // 执行函数
        int result = lua_pcall(L, 2, 0, 0);

        if (result != LUA_OK) {
            std::string error = lua_tostring(L, -1);
            lua_pop(L, 1);
            std::cerr << "[LuaInstance::executeUpdate] Error: " << error << std::endl;
            return false;
        }

        return true;
    }

    bool LuaInstance::executeInit(const std::vector<std::string>& args) {
        if (!L || !hasInitFunc) {
            return false;
        }

        // 获取INIT函数
        lua_getglobal(L, "INIT");
        if (!lua_isfunction(L, -1)) {
            lua_pop(L, 1);
            return false;
        }

        // 压入参数
        lua_pushstring(L, instanceId.c_str());
        for (const auto& arg : args) {
            lua_pushstring(L, arg.c_str());
        }

        // 执行函数
        int totalArgs = 1 + args.size();
        int result = lua_pcall(L, totalArgs, 0, 0);

        if (result != LUA_OK) {
            std::string error = lua_tostring(L, -1);
            lua_pop(L, 1);
            std::cerr << "[LuaInstance::executeInit] Error: " << error << std::endl;
            return false;
        }

        std::cout << "[LuaInstance] INIT executed for: " << instanceId << std::endl;
        return true;
    }

    void LuaInstance::setFunc(const std::string& name, lua_CFunction func) {
        if (!L) return;
        lua_pushcfunction(L, func);
        lua_setglobal(L, name.c_str());
    }

    void LuaInstance::setNumber(const std::string& name, double value) {
        if (!L) return;
        lua_pushnumber(L, value);
        lua_setglobal(L, name.c_str());
    }

    void LuaInstance::setInt(const std::string& name, int value) {
        if (!L) return;
        lua_pushinteger(L, value);
        lua_setglobal(L, name.c_str());
    }

    void LuaInstance::setUInt32(const std::string& name, uint32_t value) {
        if (!L) return;
        lua_pushinteger(L, static_cast<lua_Integer>(value));
        lua_setglobal(L, name.c_str());
    }

    void LuaInstance::setString(const std::string& name, const std::string& value) {
        if (!L) return;
        lua_pushstring(L, value.c_str());
        lua_setglobal(L, name.c_str());
    }

    double LuaInstance::getNumber(const std::string& name) const {
        if (!L) return 0.0;
        lua_getglobal(L, name.c_str());
        if (!lua_isnumber(L, -1)) {
            lua_pop(L, 1);
            return 0.0;
        }
        double value = lua_tonumber(L, -1);
        lua_pop(L, 1);
        return value;
    }

    std::string LuaInstance::getString(const std::string& name) const {
        if (!L) return "";
        lua_getglobal(L, name.c_str());
        if (!lua_isstring(L, -1)) {
            lua_pop(L, 1);
            return "";
        }
        const char* value = lua_tostring(L, -1);
        lua_pop(L, 1);
        return value ? value : "";
    }

    bool LuaInstance::callFunction(const std::string& funcName,
        const std::vector<lua_Number>& args) {
        if (!L) return false;

        lua_getglobal(L, funcName.c_str());
        if (!lua_isfunction(L, -1)) {
            lua_pop(L, 1);
            return false;
        }

        for (auto arg : args) {
            lua_pushnumber(L, arg);
        }

        int result = lua_pcall(L, args.size(), 0, 0);
        if (result != LUA_OK) {
            std::string error = lua_tostring(L, -1);
            lua_pop(L, 1);
            std::cerr << "[LuaInstance::callFunction] Error: " << error << std::endl;
            return false;
        }

        return true;
    }

    void LuaInstance::registerCFunction(const std::string& name, lua_CFunction func) {
        setFunc(name, func);
    }

    void LuaInstance::registerCFunctions(
        const std::vector<std::pair<std::string, lua_CFunction>>& funcs) {
        for (const auto& [name, func] : funcs) {
            registerCFunction(name, func);
        }
    }

    // ==================== LuaManager 实现 ====================

    LuaManager::LuaManager() {
        std::cout << "[LuaManager] Initialized" << std::endl;
    }

    LuaManager::~LuaManager() {
        destroyAll();
        std::cout << "[LuaManager] Destroyed" << std::endl;
    }

    LuaInstance* LuaManager::createInstance(const std::string& filepath,
        const std::string& instanceId) {

        std::string id = instanceId.empty() ? generateInstanceId() : instanceId;

        std::cout << "[LuaManager] Creating instance from: " << filepath
            << " with ID: " << id << std::endl;

        // 创建实例
        auto instance = std::make_unique<LuaInstance>(filepath, id);

        // 加载脚本
        if (!instance->load()) {
            std::cerr << "[LuaManager] Failed to load script for instance: " << id << std::endl;
            return nullptr;
        }

        // 应用全局函数
        applyGlobalFunctions(instance.get());

        // 执行INIT函数（如果存在）
        if (instance->hasInit()) {
            instance->executeInit();
        }

        // 保存实例
        instances[filepath].push_back(std::move(instance));
        auto* instancePtr = instances[filepath].back().get();

        std::cout << "[LuaManager] Instance created: " << id
            << " (file: " << filepath << ")" << std::endl;

        return instancePtr;
    }

    void LuaManager::destroyInstance(const std::string& instanceId) {
        for (auto& [filepath, instanceList] : instances) {
            for (auto it = instanceList.begin(); it != instanceList.end(); ++it) {
                if ((*it)->getId() == instanceId) {
                    std::cout << "[LuaManager] Destroying instance: " << instanceId << std::endl;
                    instanceList.erase(it);

                    // 如果这个文件没有实例了，清理条目
                    if (instanceList.empty()) {
                        instances.erase(filepath);
                    }
                    return;
                }
            }
        }

        std::cerr << "[LuaManager] Instance not found: " << instanceId << std::endl;
    }

    void LuaManager::destroyAllInstances(const std::string& filepath) {
        auto it = instances.find(filepath);
        if (it != instances.end()) {
            size_t count = it->second.size();
            it->second.clear();
            instances.erase(it);
            std::cout << "[LuaManager] Destroyed " << count
                << " instances from file: " << filepath << std::endl;
        }
    }

    void LuaManager::destroyAll() {
        size_t totalInstances = 0;
        for (auto& [filepath, instanceList] : instances) {
            totalInstances += instanceList.size();
            instanceList.clear();
        }
        instances.clear();
        std::cout << "[LuaManager] Destroyed all " << totalInstances << " instances" << std::endl;
    }

    void LuaManager::updateAll(float deltaTime) {
        for (auto& [filepath, instanceList] : instances) {
            for (auto& instance : instanceList) {
                if (instance && instance->isValid()) {
                    try {
                        instance->executeUpdate(deltaTime);
                    }
                    catch (const std::exception& e) {
                        handleError(instance->getId(), e.what());
                    }
                }
            }
        }
    }

    void LuaManager::updateInstancesFromFile(const std::string& filepath, float deltaTime) {
        auto it = instances.find(filepath);
        if (it == instances.end()) return;

        for (auto& instance : it->second) {
            if (instance && instance->isValid()) {
                try {
                    instance->executeUpdate(deltaTime);
                }
                catch (const std::exception& e) {
                    handleError(instance->getId(), e.what());
                }
            }
        }
    }

    void LuaManager::registerGlobalCFunction(const std::string& name, lua_CFunction func) {
        // 添加到全局函数列表
        globalFunctions.push_back({ name, func });
        std::cout << "[LuaManager] Registered global function: " << name << std::endl;

        // 应用到现有实例
        for (auto& [filepath, instanceList] : instances) {
            for (auto& instance : instanceList) {
                if (instance && instance->getLuaState()) {
                    instance->registerCFunction(name, func);
                }
            }
        }
    }

    void LuaManager::registerInstanceCFunction(LuaInstance* instance,
        const std::string& funcName,
        lua_CFunction func) {
        if (instance) {
            instance->registerCFunction(funcName, func);
        }
    }

    LuaInstance* LuaManager::getInstance(const std::string& instanceId) const {
        for (const auto& [filepath, instanceList] : instances) {
            for (const auto& instance : instanceList) {
                if (instance->getId() == instanceId) {
                    return instance.get();
                }
            }
        }
        return nullptr;
    }

    std::vector<LuaInstance*> LuaManager::getInstancesFromFile(const std::string& filepath) const {
        std::vector<LuaInstance*> result;
        auto it = instances.find(filepath);
        if (it != instances.end()) {
            for (const auto& instance : it->second) {
                result.push_back(instance.get());
            }
        }
        return result;
    }

    size_t LuaManager::getInstanceCount() const {
        size_t count = 0;
        for (const auto& [filepath, instanceList] : instances) {
            count += instanceList.size();
        }
        return count;
    }

    size_t LuaManager::getInstanceCount(const std::string& filepath) const {
        auto it = instances.find(filepath);
        return it != instances.end() ? it->second.size() : 0;
    }

    void LuaManager::setErrorCallback(std::function<void(const std::string&,
        const std::string&)> callback) {
        errorCallback = callback;
    }

    void LuaManager::handleError(const std::string& context, const std::string& error) {
        if (errorCallback) {
            errorCallback(context, error);
        }
        std::cerr << "[LuaManager] Error [" << context << "]: " << error << std::endl;
    }

    std::string LuaManager::generateInstanceId() const {
        static std::random_device rd;
        static std::mt19937 gen(rd());
        static std::uniform_int_distribution<> dis(1000, 9999);
        return "inst_" + std::to_string(dis(gen));
    }

    void LuaManager::applyGlobalFunctions(LuaInstance* instance) {
        if (!instance) return;

        for (const auto& [name, func] : globalFunctions) {
            instance->registerCFunction(name, func);
        }
    }

} // namespace LuaManager