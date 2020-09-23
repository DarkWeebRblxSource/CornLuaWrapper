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
#include "Stuffs.h"
#include "Rlua.h"

extern "C" {
#include "Lua\lua.h"
#include "Lua\lualib.h"
#include "Lua\lauxlib.h"
#include "Lua\luaconf.h"
#include "Lua\lapi.h"
#include "Lua\lualib.h"
#include "Lua\lobject.h"
#include "Lua\lstate.h"
#include "Lua\lgc.h"
}

std::unordered_map<void*, int> WrapList;
std::unordered_map<void*, int> UnWrapList;
std::unordered_map<void*, int> ReferenceListW;
std::unordered_map<void*, int> ReferenceListU;
static int WrapCallHandler(lua_State *L);
static int UnWrapCallHandler(int rL);
int Registry;
lua_State *Vanilla;
bool InstanceCache = false;
void wrap(lua_State *L, int rL, int idx);
void unwrap(lua_State *L, int rL, int idx);
static int GarbageCollector(lua_State *L) {
	void *UD = lua_touserdata(L, 1);
	lua_pushvalue(L, 1);
	lua_gettable(L, LUA_REGISTRYINDEX);
	if (!lua_isnil(L, -1)) {
		int Reference = lua_tointeger(L, -1);
		rlua_pushnil(luaState);
		rlua_rawseti(luaState, LUA_REGISTRYINDEX, Reference);
		rlua_settop(luaState, 0);
		writerf(1, "Garbage Collected!");
		return 1;
	}
	lua_pop(L, 1);
	return 0;
}

static int EQUserData(lua_State *Thread) {
	cout << "EQ: " << lua_touserdata(Thread, 1) << " =? " << lua_touserdata(Thread, 2) << endl;
	lua_pushboolean(Thread, (*(DWORD *)lua_touserdata(Thread, 1) == *(DWORD *)lua_touserdata(Thread, 2)));
	return 1;
}
static int GetThreadPointer(lua_State *L) {
	lua_getfield(L, -10000, "__thread");
	int Thread = lua_tointeger(L, -1);
	lua_pop(L, 1);
	return Thread;
}
static int DownloadString(lua_State *L) {
	if (lua_gettop(L) < 0) return luaL_error(L, "HttpGet require 1/2 argument!");
	if (lua_gettop(L) == 2) {
		
		lua_pushstring(L, DownloadURL(lua_tostring(L, -2)).c_str());
	}
	else {
		lua_pushstring(L, DownloadURL(lua_tostring(L, -1)).c_str());
	}
	return 1;
}


static int GetObjects(lua_State *L) {
	if (lua_gettop(L) < 1) return luaL_error(L, "GetObjects require 1 argument!");
	int State = GetThreadPointer(L);
	const char *id = lua_tostring(L, 1);
	lua_newtable(L);
	lua_pushnumber(L, 1);
	rlua_getglobal(State, "game");
	rlua_getfield(State, -1, "GetService");
	rlua_pushvalue(State, -2);
	rlua_pushstring(State, "InsertService");
	rlua_pcall(State, 2, 1, 0);
	rlua_getfield(State, -1, "LoadLocalAsset");
	rlua_pushvalue(State, -2);
	rlua_pushstring(State, id);
	rlua_pcall(State, 2, 1, 0);
	wrap(L, State, -1);
	lua_settable(L, -3);
	return 1;
}

int Index(lua_State*L) {
	const char* key = lua_tostring(L, -1);

	
		//printf("Index Key: %s\n", key);
		unwrap(L, luaState, 1);
		rlua_getfield(luaState, -1, key);
		wrap(L, luaState, -1);
	
	return 1;
}
 int newindex(lua_State* L) {
		const char* Key = lua_tostring(L, -2);
		unwrap(L, luaState,-3);
		unwrap(L, luaState, -1);
		rlua_setfield(luaState, -2, Key);
		return 0;

}
void wrap(lua_State *L, int rL, int idx) {
	try {
		//printf("LUA: %d\n", rlua_type(rL, idx));
		switch (rlua_type(rL, idx))
		{
		case RLUA_TLIGHTUSERDATA:
			lua_pushlightuserdata(L, rlua_touserdata(rL, idx));
			break;
		case RLUA_TNIL:
			lua_pushnil(L);
			break;
		case RLUA_TNUMBER:
			lua_pushnumber(L, rlua_tonumber(rL, idx));
			break;
		case RLUA_TBOOLEAN:
			lua_pushboolean(L, rlua_toboolean(rL, idx));
			break;
		case RLUA_TSTRING:
			lua_pushstring(L, rlua_tolstring(rL, idx, 0));
			break;
		case RLUA_TTHREAD:
			lua_newthread(L);
			break;
		case RLUA_TFUNCTION:
			rlua_pushvalue(rL, idx);
			lua_pushinteger(L, rluaL_ref(rL, LUA_REGISTRYINDEX));
			lua_pushcclosure(L, WrapCallHandler, 1);
			break;
		case RLUA_TTABLE:
			rlua_pushvalue(rL, idx);
			lua_newtable(L);
			rlua_pushnil(rL);
			while (rlua_next(rL, -2) != RLUA_TNIL)
			{
				wrap(L, rL, -2);
				wrap(L, rL, -1);
				lua_settable(L, -3);
				rlua_pop(rL, 1);
			}
			rlua_pop(rL, 1);
			break;
		case RLUA_TUSERDATA: {
			const auto Key = rlua_touserdata(rL, idx);
			const auto iterator = WrapList.find(Key);
			if (iterator == WrapList.cend()) { // if not found
				//cout << "Not Found at wrap: " << Key << endl;
				rlua_pushvalue(rL, idx);
				lua_newuserdata(L, 0);
				lua_pushvalue(L, -1);
				lua_pushinteger(L, rluaL_ref(rL, LUA_REGISTRYINDEX));
				lua_settable(L, LUA_REGISTRYINDEX);

				rlua_getmetatable(rL, idx);
				wrap(L, rL, -1);

				/*lua_pushcfunction(L, GarbageCollector);
				lua_setfield(L, -2, "__gc");

				lua_pushcfunction(L, EQUserData);
				lua_setfield(L, -2, "__eq");
*/
				/*lua_pushcfunction(L, newindex);
				lua_setfield(L, -2, "__newindex");*/
				
				lua_pushcfunction(L, Index);
			    lua_setfield(L, -2, "__index");

				lua_setmetatable(L, -2);

				lua_pushvalue(L, -1);
				lua_rawseti(L, LUA_REGISTRYINDEX, ++Registry);
				WrapList[Key] = Registry;
				rlua_pop(rL, 1);
			}
			else {
				//cout << "Found at wrap: " << iterator->second << endl;
				lua_rawgeti(L, LUA_REGISTRYINDEX, iterator->second);
			}
			break;
		}
		default: break;
		}
	}
	catch (exception e) {
		MessageBox(0, e.what(), "ERROR", MB_TOPMOST || MB_OK);
	}
}

void unwrap(lua_State *L, int rL, int idx) {
	try {
		//printf("ROBLOX: %d\n", lua_type(L, idx));
		switch (lua_type(L, idx))
		{
		case LUA_TLIGHTUSERDATA:
			rlua_pushlightuserdata(rL, lua_touserdata(L, idx));
			break;
		case LUA_TNIL:
			rlua_pushnil(rL);
			break;
		case LUA_TNUMBER:
			rlua_pushnumber(rL, lua_tonumber(L, idx));
			break;
		case LUA_TBOOLEAN:
			rlua_pushboolean(rL, lua_toboolean(L, idx));
			break;
		case LUA_TSTRING:
			rlua_pushstring(rL, lua_tostring(L, idx));
			break;
		case LUA_TTHREAD:
			rlua_newthread(rL);
			break;
		case LUA_TFUNCTION: {
		//	printf("TFUNCTION!!\n");
			lua_pushvalue(L, idx);
		//	printf("TFUNCTION 1 !!\n");
			rlua_pushnumber(rL, luaL_ref(L, LUA_REGISTRYINDEX));
		//	printf("TFUNCTION 2!!\n");
			rlua_pushcfunction(rL, (DWORD)UnWrapCallHandler, 1);
	//		printf("TFUNCTION 3!!\n");
			break;
		}
		case LUA_TTABLE:
			lua_pushvalue(L, idx);
			rlua_createtable(rL, 0, 0);
			lua_pushnil(L);
			while (lua_next(L, -2) != LUA_TNIL)
			{
				unwrap(L, rL, -2);
				unwrap(L, rL, -1);
				rlua_settable(rL, -3);
				lua_pop(L, 1);
			}
			lua_pop(L, 1);
			break;
		case LUA_TUSERDATA: {
			lua_pushvalue(L, idx);
			lua_gettable(L, LUA_REGISTRYINDEX);
			if (!lua_isnil(L, -1)) rlua_rawgeti(rL, LUA_REGISTRYINDEX, lua_tointeger(L, -1)); else rlua_newuserdata(rL, lua_objlen(L, idx));
			lua_pop(L, 1);
			break;
		}
		default: break;
		}
	}
	catch (exception e) {
		MessageBox(0, e.what(), "ERROR", MB_TOPMOST || MB_OK);
	}
}

static int resume(lua_State* rL)
{
	lua_State* To = lua_tothread(rL, lua_upvalueindex(1));
	const int nargs = lua_gettop(rL);
	lua_xmove(rL, To, nargs);
	return lua_resume(To, nargs);
	lua_newtable(To);
	lua_pushstring(To, "This metatable is locked");
	lua_setfield(To, -2, "__metatable");
	lua_close(To);
}

static int WrapCallHandler(lua_State *L) {
	DWORD rL = rlua_newthread(luaState);
	rlua_setfield(luaState, LUA_REGISTRYINDEX, to_string(++Registry).c_str()); //rlua_setfield(luaState, LUA_REGISTRYINDEX, "Thread_Roblox");// lol its prob here
	int Args = lua_gettop(L);
	rlua_rawgeti(rL, LUA_REGISTRYINDEX, lua_tointeger(L, lua_upvalueindex(1)));

	for (int i = 1; i <= Args; ++i)
		unwrap(L, rL, i);

	if (rlua_pcall(rL, Args, LUA_MULTRET, 0))
	{
		string Message = rlua_tolstring(rL, -1, 0);
		
		if (Message.empty()) Message = "Error occoured, no output from Lua\n";
		if (Message == "attempt to yield across metamethod/C-call boundary")
		{
			rlua_pop(rL, 1);
			lua_pushthread(L);
			lua_pushcclosure(L, &resume, 1);
			unwrap(L, rL, -1);
			return lua_yield(L, 0);
		}
		return luaL_error(L, Message.c_str());
	}

	int Return = rlua_gettop(rL);
	for (int i = 1; i <= Return; ++i)
		wrap(L, rL, i);

	rlua_settop(rL, 0);
	rlua_pushnil(luaState);
    rlua_setfield(luaState, LUA_REGISTRYINDEX, to_string(Registry).c_str()); // rlua_setfield(luaState, LUA_REGISTRYINDEX, "Thread_Roblox");// 
	return Return;
	lua_close(L);
}

static int UnWrapCallHandler(int rL) {
	

	lua_State *L = lua_newthread(Vanilla);
	;

	lua_setfield(Vanilla, LUA_REGISTRYINDEX, to_string(++Registry).c_str()); //lua_setfield(Vanilla, LUA_REGISTRYINDEX, "Thread_Vanilla");//

	int Args = rlua_gettop(rL);

	lua_rawgeti(L, LUA_REGISTRYINDEX, cast_int(rlua_tonumber(rL, lua_upvalueindex(1))));

	for (int i = 1; i <= Args; ++i)
		wrap(L, rL, i);

	if (lua_pcall(L, Args, LUA_MULTRET, 0)) {
		
		return rluaL_error(rL, luaL_checkstring(L, -1));
	}

	int Return = lua_gettop(L);
	
	for (int i = 1; i <= Return; ++i)
		
		unwrap(L, rL, i);


	lua_settop(L, 0);
	
	lua_pushnil(Vanilla);

	lua_setfield(Vanilla, LUA_REGISTRYINDEX, to_string(Registry).c_str()); //lua_setfield(Vanilla, LUA_REGISTRYINDEX, "Thread_Vanilla");//
	
	return Return;
	lua_close(L);
}