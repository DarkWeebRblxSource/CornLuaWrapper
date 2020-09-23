#pragma once
#include <vector>
#include <sstream>
#include <fstream>
#include <istream>
#include <iterator>
#include <algorithm>
#include <string>
#include <windows.h> 
#include <iostream> 
#include <process.h>
#include <Shlwapi.h>
#include <ctime>
#include <cstdlib>
#include <chrono>
#include <unordered_map>

#include "Rlua.h"
#include "Converter.h"
#include "Stuffs.h"

extern "C" {
#include "Lua\lua.h"
#include "Lua\lualib.h"
#include "Lua\lauxlib.h"
#include "Lua\luaconf.h"
#include "Lua\lapi.h"
#include "Lua\lualib.h"
#include "Lua\lobject.h"
#include "Lua\lstate.h"

}

using namespace std;

void GetMetatableFromRoblox(lua_State *L, const char *Name) {
	rlua_getglobal(luaState, "game");
	rlua_getmetatable(luaState, -1);
	rlua_getfield(luaState, -1, Name);
	wrap(L, luaState, -1);
	lua_setfield(L, -2, Name);
	rlua_pop(luaState, 3);
}

void SetThreadPointer(lua_State *L, int State) {
	lua_pushinteger(L, State);
	lua_setfield(L, LUA_REGISTRYINDEX, "__thread");
	lua_settop(L, 0);
}



static int GlobalIndexTable(lua_State *L) {
	if (lua_type(L, 2) != LUA_TSTRING) return luaL_error(L, "invalid string index");
	rlua_getfield(luaState, -10002, lua_tostring(L, 2));
	if (rlua_type(luaState, -1) > LUA_TNIL) {
		wrap(Vanilla, luaState, -1);
		rlua_pop(luaState, 1);
		return 1;
	}
	return 0;
}

void pushglobal(DWORD rL, lua_State* L, const char* s)
{
	rlua_getglobal(rL, s);
	wrap(L, rL, -1);
	lua_setglobal(L, s);
	rlua_pop(rL, 1);
	std::cout << "GLOBAL: " << s << std::endl;
}


void GetGlobals() {
	std::vector<std::string> rbxfuncs{ "print", "warn", "error", "printidentity",
		"DockWidgetPluginGuiInfo", "UDim", "NumberSequence", "Color3", "Enum", "Stats",
		"NumberRange", "elapsedTime", "PhysicalProperties", "PluginManager", "PluginDrag"
		"NumberSequenceKeypoint", "Version", "version", "game", "Game", "workspace", "Workspace",
		"delay", "Delay", "stats", "wait", "Wait", "CellId", "LoadLibrary", "typeof", "spawn", "Spawn",
		"_VERSION", "settings", "UDim2", "TweenInfo", "require", "Vector3", "Vector3int16",
		"Region3int16", "Vector2int16", "newproxy", "Random", "Region3", "utf8", "Faces", "ypcall", "ray", "Ray",
		"PathWaypoint", "Vector2", "tostring", "tonumber", "ColorSequenceKeypoint", "CFrame", "tick",
		"ColorSequence", "Instance", "ElapsedTime", "select", "Rect", "BrickColor", "Axes","pcall" };

	for (auto func : rbxfuncs) {
		rlua_getglobal(luaState, func.c_str());
		wrap(Vanilla, luaState, -1);
		lua_setfield(Vanilla, LUA_GLOBALSINDEX, func.c_str());
		rlua_pop(luaState, 1);
	}

}
void SetKoaxyDentity(int rls, int identity) {
	int v3 = *(DWORD *)(rls + 128);
	*(DWORD *)(v3 + 24) = identity;
}

int SpawnHandler(int CurrentState) {
	SetThreadPointer(Vanilla, CurrentState);
	//SetKoaxyDentity(CurrentState, 6);
	//SetKoaxyDentity(CurrentState, 6);
	//const char *rndmchar = (std::string)Scanner::RandomString::GenerateString(10);

	const char *Script = rlua_tolstring(CurrentState, lua_upvalueindex(1), 0);
	rlua_pop(CurrentState, 1);
	std::string buff;
	buff = "spawn(function() script = Instance.new'LocalScript' script.Disabled = true script.Parent = nil\r\n";
	buff += Script;
	buff += "\r\nend)";
	if (luaL_loadbuffer(Vanilla, buff.c_str(), buff.length(),"@Fluxus") || lua_pcall(Vanilla, 0, 0, 0)) {
		printf("error %s",lua_tostring(Vanilla, -1));
		lua_pop(Vanilla, 1);
	}

	return 1;
}

void ExecuteScript(const char *Script) {
	int NewThread = rlua_newthread(luaState);
	rlua_pushstring(NewThread, Script);
	rlua_pushcfunction(NewThread, (int)SpawnHandler, 1);
	Spawn(NewThread);
	rlua_settop(NewThread, 0);
}

#include "Utils.h"
void AutoExecute() {
	char AutoExec[MAX_PATH];// add the util to project ff 
	Files::GetFile("Skisploit.dll", "AutoExecute\\", AutoExec, MAX_PATH);
	std::string ReturnPath = AutoExec;
	std::vector<std::string> FilesToExecute;
	Files::GetFilesInDirectory(FilesToExecute, ReturnPath, 0);
	for (std::vector<std::string>::iterator CurrentFile = FilesToExecute.begin(); CurrentFile != FilesToExecute.end(); CurrentFile++) {
		std::string FinalPath = ReturnPath + *CurrentFile;
		std::string ReadData;
		if (Files::ReadFile(FinalPath, ReadData, 0)) {
			ExecuteScript(ReadData.c_str());
		}
	}
}