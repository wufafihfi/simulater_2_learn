#ifndef APPLYLUA
#define APPLYLUA

#include <filesystem>
#include <iostream>
#include <string>
#include <sstream>

#include <box2d/box2d.h>
#include <lua.hpp>

#include "appInit.h"
#include "LuaManager.h"
#include "PhysicsWorld.h"


namespace ApplyLua {
	void SetDataPoor(std::vector<bzd_Phy::bodyData>* _DataPoor);

	class LuaAP
	{
	public:
		// 脚本文件存储
		std::vector<std::string> LuaScriptFiles;
		std::vector<std::string> LuaScriptFilesName;
		// lua脚本管理器
		LuaManager::LuaManager manager;

		//box2d
		b2WorldId worldId;

	public:
		void initLuaAP(b2WorldId _worldId);
		bool findLuaScriptAuto();

		void UpDateLuaScript(float dt);

		void BodyLuaSetting(bzd_Phy::bodyData &data);

		void createInstanceOnBody(bzd_Phy::bodyData &data, const std::string scriptName);
		void destroyInstanceOnBody(bzd_Phy::bodyData &data);

		std::string getScriptFilePath(const std::string& scriptName) {
			for (size_t i = 0; i < LuaScriptFilesName.size(); ++i) {
				if (LuaScriptFilesName[i] == scriptName) {
					return LuaScriptFiles[i];
				}
			}
			return "";
		}

	private:
		
	};

	bool getFilesWithExtensionRecursive(std::vector<std::string>& fileVector, const std::string& path, const std::string& extension);
	bool getFilesNameWithExtensionRecursive(std::vector<std::string>& fileNameVector, const std::string& path, const std::string& extension);
}

#endif