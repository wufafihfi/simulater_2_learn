#include "applyLua.h"

namespace ApplyLua {
    std::vector<bzd_Phy::bodyData>* DataPoor;
    void SetDataPoor(std::vector<bzd_Phy::bodyData>* _DataPoor) {
        DataPoor = _DataPoor;
    }

    // 获取文件夹目录下的目标文件的路径
    bool getFilesWithExtensionRecursive(std::vector<std::string> &fileVector ,const std::string& path, const std::string& extension) {
        
        try {
            std::string ext = extension;
            if (!ext.empty() && ext[0] != '.') {
                ext = "." + ext;
            }

            for (const auto& entry : std::filesystem::recursive_directory_iterator(path)) {
                if (entry.is_regular_file()) {
                    std::string fileExt = entry.path().extension().string();

                    // 不区分大小写比较
                    if (fileExt.size() == ext.size() &&
                        std::equal(fileExt.begin(), fileExt.end(), ext.begin(),
                            [](char a, char b) {
                                return std::tolower(a) == std::tolower(b);
                            })) {
                        fileVector.push_back(entry.path().string());
                    }
                }
            }
        }
        catch (const std::filesystem::filesystem_error& e) {
            std::cerr << "错误: " << e.what() << std::endl;
            return false;
        }

        return true;
    }
    // 获取文件夹目录下的目标文件的文件名
    bool getFilesNameWithExtensionRecursive(std::vector<std::string>& fileNameVector,const std::string& path,const std::string& extension) {

        try {
            std::string ext = extension;
            if (!ext.empty() && ext[0] != '.') {
                ext = "." + ext;
            }

            for (const auto& entry : std::filesystem::recursive_directory_iterator(path)) {
                if (entry.is_regular_file()) {
                    std::string fileExt = entry.path().extension().string();

                    // 不区分大小写比较扩展名
                    if (fileExt.size() == ext.size() &&
                        std::equal(fileExt.begin(), fileExt.end(), ext.begin(),
                            [](char a, char b) {
                                return std::tolower(a) == std::tolower(b);
                            })) {

                        // 获取文件名（不包括后缀）
                        std::string filename = entry.path().stem().string();
                        fileNameVector.push_back(filename);
                    }
                }
            }
        }
        catch (const std::filesystem::filesystem_error& e) {
            std::cerr << "错误: " << e.what() << std::endl;
            return false;
        }

        return true;
    }

    bool LuaAP::findLuaScriptAuto() {
        auto& _basepath = AppInit::getProgramDirectoryPath();

        std::string folderPath = _basepath.basepath_A + "\\Lua\\LuaScript";
        std::string extension = "lua";

        return getFilesWithExtensionRecursive(LuaScriptFiles, folderPath, extension) && getFilesNameWithExtensionRecursive(LuaScriptFilesName, folderPath, extension);
    }

    //合适的PRINT
    static int custom_print(lua_State* L) {
        int argc = lua_gettop(L);
        if (argc != 2) {
            lua_pushstring(L, "need 2 parameters");
            lua_error(L);
            return 0;
        }

        // 获取第一个参数（字符串）
        if (!lua_isstring(L, 1)) {
            lua_pushstring(L, "(erro->string,int[bodyID_int])");
            lua_error(L);
            return 0;
        }
        const char* str = lua_tostring(L, 1);

        // 获取第二个参数（整数）
        if (!lua_isnumber(L, 2)) {
            lua_pushstring(L, "(string,erro->int[bodyID_int])");
            lua_error(L);
            return 0;
        }
        int number = (int)lua_tonumber(L, 2);

        std::string cpp_str(str);

        for (auto& data_ : *DataPoor) {
            if(data_.BodyId_int == number)
            {
                data_.debugMessages.push_back(cpp_str);
                break;
            }
        }
        return 0;
    }
    // API函数
    //自程序运行以来的时间戳
    static int cpp_uptime(lua_State* L) {
        static auto start_time = std::chrono::steady_clock::now();
        auto now = std::chrono::steady_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(
            now - start_time
        );
        lua_pushnumber(L, static_cast<lua_Number>(duration.count()));
        return 1;
    }
    //辅助
    bzd_Phy::bodyData isTargetBody(uint32_t targetBodyDataID_Int) {
        for (auto& data_ : *DataPoor) {
            if (data_.BodyId_int == targetBodyDataID_Int)
            {
                return data_;
            }
        }
        return bzd_Phy::GetNullBody();
    }
    //获取参数
    static int cpp_getPosition(lua_State* L) {
        int argc = lua_gettop(L);
        if (argc != 1) {
            lua_pushstring(L, "need 1 parameters");
            lua_error(L);
            return 0;
        }

        if (!lua_isnumber(L, 1)) {
            lua_pushstring(L, "(erro->int[bodyID_int])");
            lua_error(L);
            return 0;
        }
        int number = (int)lua_tonumber(L, 1);

        b2Vec2 position({ 0,0 });

        bzd_Phy::bodyData data_ = isTargetBody(number);

        if (data_.BodyId_int != bzd_Phy::GetNullBody().BodyId_int)
        {
            position = b2Body_GetPosition(data_.body_id);
        }

        lua_newtable(L);

        // 设置数组部分
        lua_pushnumber(L, position.x);
        lua_rawseti(L, -2, 1);

        lua_pushnumber(L, position.y);
        lua_rawseti(L, -2, 2);

        return 1;
    }
    static int cpp_getVelocity(lua_State* L) {
        int argc = lua_gettop(L);
        if (argc != 1) {
            lua_pushstring(L, "need 1 parameters");
            lua_error(L);
            return 0;
        }

        if (!lua_isnumber(L, 1)) {
            lua_pushstring(L, "(erro->int[bodyID_int])");
            lua_error(L);
            return 0;
        }
        int number = (int)lua_tonumber(L, 1);

        b2Vec2 position({ 0,0 });

        bzd_Phy::bodyData data_ = isTargetBody(number);

        if (data_.BodyId_int != bzd_Phy::GetNullBody().BodyId_int)
        {
            position = b2Body_GetLinearVelocity(data_.body_id);
        }

        lua_newtable(L);

        // 设置数组部分
        lua_pushnumber(L, position.x);
        lua_rawseti(L, -2, 1);

        lua_pushnumber(L, position.y);
        lua_rawseti(L, -2, 2);

        return 1;
    }
    static int cpp_getAngularVelocity(lua_State* L) {
        int argc = lua_gettop(L);
        if (argc != 1) {
            lua_pushstring(L, "need 1 parameters");
            lua_error(L);
            return 0;
        }

        if (!lua_isnumber(L, 1)) {
            lua_pushstring(L, "(erro->int[bodyID_int])");
            lua_error(L);
            return 0;
        }
        int number = (int)lua_tonumber(L, 1);

        float agV = 0.0f;

        bzd_Phy::bodyData data_ = isTargetBody(number);

        if (data_.BodyId_int != bzd_Phy::GetNullBody().BodyId_int)
        {
            agV = b2Body_GetAngularVelocity(data_.body_id);
        }

        lua_pushnumber(L, agV);

        return 1;
    }
    static int cpp_getMass(lua_State* L) {
        int argc = lua_gettop(L);
        if (argc != 1) {
            lua_pushstring(L, "need 1 parameters");
            lua_error(L);
            return 0;
        }

        if (!lua_isnumber(L, 1)) {
            lua_pushstring(L, "(erro->int[bodyID_int])");
            lua_error(L);
            return 0;
        }
        int number = (int)lua_tonumber(L, 1);

        float mass = 0.0f;

        bzd_Phy::bodyData data_ = isTargetBody(number);

        if (data_.BodyId_int != bzd_Phy::GetNullBody().BodyId_int)
        {
            mass = b2Body_GetMass(data_.body_id);
        }

        lua_pushnumber(L, mass);

        return 1;
    }
    static int cpp_getGravity(lua_State* L) {
        int argc = lua_gettop(L);
        if (argc != 1) {
            lua_pushstring(L, "need 1 parameters");
            lua_error(L);
            return 0;
        }

        if (!lua_isnumber(L, 1)) {
            lua_pushstring(L, "(erro->int[bodyID_int])");
            lua_error(L);
            return 0;
        }
        int number = (int)lua_tonumber(L, 1);

        b2Vec2 position({ 0,0 });

        bzd_Phy::bodyData data_ = isTargetBody(number);

        if (data_.BodyId_int != bzd_Phy::GetNullBody().BodyId_int)
        {
            position = b2World_GetGravity(*data_.worldId);
        }

        lua_newtable(L);

        // 设置数组部分
        lua_pushnumber(L, position.x);
        lua_rawseti(L, -2, 1);

        lua_pushnumber(L, position.y);
        lua_rawseti(L, -2, 2);

        return 1;
    }
    static int cpp_ApplyForce(lua_State* L) {
        int argc = lua_gettop(L);
        if (argc != 3) {
            lua_pushstring(L, "need 3 parameters: (bodyID, force_table, point_table)");
            lua_error(L);
            return 0;
        }

        // 第一个参数应该是body ID（数字）
        if (!lua_isinteger(L, 1) && !lua_isnumber(L, 1)) {
            lua_pushstring(L, "Parameter 1 error: expected int[bodyID_int]");
            lua_error(L);
            return 0;
        }
        int body_id = (int)lua_tonumber(L, 1);

        // 第二个参数是force表
        if (!lua_istable(L, 2)) {
            lua_pushstring(L, "Parameter 2 error: expected table[force]");
            lua_error(L);
            return 0;
        }

        // 从参数2获取force表的x,y
        lua_rawgeti(L, 2, 1);  // force[1] = x
        lua_rawgeti(L, 2, 2);  // force[2] = y
        float fx = luaL_optnumber(L, -2, 0);
        float fy = luaL_optnumber(L, -1, 0);
        lua_pop(L, 2);  // 清理栈

        // 第三个参数是point表
        if (!lua_istable(L, 3)) {
            lua_pushstring(L, "Parameter 3 error: expected table[point]");
            lua_error(L);
            return 0;
        }

        // 从参数3获取point表的x,y
        lua_rawgeti(L, 3, 1);  // point[1] = x
        lua_rawgeti(L, 3, 2);  // point[2] = y
        float px = luaL_optnumber(L, -2, 0);
        float py = luaL_optnumber(L, -1, 0);
        lua_pop(L, 2);  // 清理栈

        b2Vec2 force({ fx, fy });
        b2Vec2 point({ px, py });

        bzd_Phy::bodyData data_ = isTargetBody(body_id);

        if (data_.BodyId_int != bzd_Phy::GetNullBody().BodyId_int)
        {
            b2Body_ApplyForce(data_.body_id, force, point, true);
        }
        else
        {
            lua_pushstring(L, "Body not found or invalid body ID");
            lua_error(L);
        }

        return 0;
    }
    static int cpp_ApplyTorque(lua_State* L) {
        int argc = lua_gettop(L);
        if (argc != 2) {
            lua_pushstring(L, "need 2 parameters");
            lua_error(L);
            return 0;
        }

        if (!lua_isnumber(L, 1)) {
            lua_pushstring(L, "Parameter 1 error: expected int[bodyID_int]");
            lua_error(L);
            return 0;
        }
        int number = (int)lua_tonumber(L, 1);

        if (!lua_isnumber(L, 2)) {
            lua_pushstring(L, "Parameter 2 error: expected float[Torque]");
            lua_error(L);
            return 0;
        }
        float Torque = (float)lua_tonumber(L, 2);

        bzd_Phy::bodyData data_ = isTargetBody(number);

        if (data_.BodyId_int != bzd_Phy::GetNullBody().BodyId_int)
        {
            b2Body_ApplyTorque(data_.body_id, Torque,true);
        }
        else
        {
            lua_pushstring(L, "Body not found or invalid body ID");
            lua_error(L);
        }

        return 0;
    }
    // API注册  故障 ： 返回的table里啥也没有
    static int cpp_GETAPI(lua_State* L) {
        // 创建API table
        lua_createtable(L, 0, 2);

        lua_pushcfunction(L, cpp_uptime);
        lua_setfield(L, -2, "time_ms");

        return 1;
    }

    void LuaAP::initLuaAP(b2WorldId _worldId) {
        worldId = _worldId;

        manager.setErrorCallback([](const std::string& context, const std::string& error) {
            std::cout << "Lua Error [" << context << "]: " << error << std::endl;
            });

        if (findLuaScriptAuto()) {
            std::cout << "lua脚本读取成功" << std::endl;
        }
        else
        {
            std::cout << "lua脚本读取失败" << std::endl;
            return;
        }

        // ========== 移除了预加载循环 ==========
        // 不再预加载脚本，改为按需加载

        // 注册全局函数
        manager.registerGlobalCFunction("cpp_B_print", custom_print);
        manager.registerGlobalCFunction("cpp_time_ms", cpp_uptime);
        // box2d
        manager.registerGlobalCFunction("cpp_getPosition", cpp_getPosition);
        manager.registerGlobalCFunction("cpp_getVelocity", cpp_getVelocity);
        manager.registerGlobalCFunction("cpp_getAngularVelocity", cpp_getAngularVelocity);
        manager.registerGlobalCFunction("cpp_getMass", cpp_getMass);
        manager.registerGlobalCFunction("cpp_getGravity", cpp_getGravity);
        manager.registerGlobalCFunction("cpp_ApplyForce", cpp_ApplyForce);
        manager.registerGlobalCFunction("cpp_ApplyTorque", cpp_ApplyTorque);

        std::cout << "Lua API初始化完成（延迟加载模式）" << std::endl;
    }

    void LuaAP::UpDateLuaScript(float dt) {
        // 更新所有实例
        manager.updateAll(dt);
    }

    void LuaAP::createInstanceOnBody(bzd_Phy::bodyData& data, const std::string scriptName)
    {
        // 查找对应的脚本文件路径
        std::string filepath = "";
        for (size_t i = 0; i < LuaScriptFilesName.size(); i++) {
            if (LuaScriptFilesName[i] == scriptName) {
                filepath = LuaScriptFiles[i];
                break;
            }
        }

        if (filepath.empty()) {
            std::cout << "找不到脚本: " << scriptName << std::endl;
            return;
        }

        data.isCreateInstance = true;

        for (auto& data_ : *DataPoor) {
            if (data_.BodyId_int == data.BodyId_int) {
                std::string _str = "_";
                std::string instanceId = scriptName + _str + std::to_string(data.BodyId_int);

                // ========== 关键改动：直接创建实例（会自动加载） ==========
                auto* body = manager.createInstance(filepath, instanceId);
                if (!body) {
                    std::cout << "创建实例失败: " << instanceId << std::endl;
                    return;
                }

                data_.InstanceName = scriptName;
                data_.InstanceId = instanceId;
                data_.isCreateInstance = true;

                std::cout << "为物体 " << data.BodyId_int << " 创建Lua实例: " << instanceId << std::endl;
                return;
            }
        }
    }

    void LuaAP::destroyInstanceOnBody(bzd_Phy::bodyData& data)
    {
        for (auto& data_ : *DataPoor) {
            if (data_.BodyId_int == data.BodyId_int) {
                // ========== 关键改动：使用新的销毁函数 ==========
                manager.destroyInstance(data_.InstanceId);
                data.isCreateInstance = false;
                data_.isCreateInstance = false;
                data_.debugMessages.clear();
                return;
            }
        }
    }

    static const char* scriptNames[500] = { 0 };  // 初始化数组
    void LuaAP::BodyLuaSetting(bzd_Phy::bodyData &data) {
        int g = 0;
        for (std::string& scriptFileName : LuaScriptFilesName)
        {
            if (g >= 499)
            {
                break;
            }
            if(scriptNames[g] != scriptFileName.c_str())
            {
                scriptNames[g] = scriptFileName.c_str();
            }
            g++;
        }

        static int current_country = 0;
        static bool country_filter_active = false;
        static char country_filter[64] = "";

        if(data.isCreateInstance == false)
        {
            ImGui::Combo(u8"选择脚本", &current_country, scriptNames, IM_ARRAYSIZE(scriptNames));
            ImGui::Text(u8"当前选择:", scriptNames[current_country]);
            if (ImGui::Button(u8"运行脚本"))
            {
                createInstanceOnBody(data, scriptNames[current_country]);
            }
        }
        else
        {
            for (auto& data_ : *DataPoor) {
                // 调试信息数量限制
                while (data_.debugMessages.size() > 25)
                {
                    data_.debugMessages.erase(data_.debugMessages.begin());
                }
                if (data_.BodyId_int == data.BodyId_int) {
                    ImGui::Text(u8"Lua调试信息");
                    ImGui::BeginChild("DebugScroll", ImVec2(0, 200), true, ImGuiWindowFlags_HorizontalScrollbar | ImGuiWindowFlags_AlwaysVerticalScrollbar);
                    for (const auto& message : data_.debugMessages) {
                        ImGui::TextUnformatted(message.c_str());
                    }
                    ImGui::Separator();
                    if (ImGui::GetScrollY() >= ImGui::GetScrollMaxY()) {
                        ImGui::SetScrollHereY(1.0f);
                    }
                    ImGui::EndChild();
                }
            }
            if (ImGui::Button(u8"停止脚本"))
            {
                destroyInstanceOnBody(data);
            }
        }
    }
}