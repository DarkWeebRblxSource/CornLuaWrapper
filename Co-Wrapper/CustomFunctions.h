#pragma once
#include "xorstring.h"
#include "Base64Encryption.h"
#include "Wrapper.h"
#include "Stuffs.h"

HANDLE clip;
using rlua_CFunction = int(*)(DWORD);
auto rlua_tocfunction = [](DWORD rState, int idx) -> rlua_CFunction {
	TValue* o = (TValue*)rlua_index2adr(rState, idx);
	return (!iscfunction(o)) ? NULL : cast(rlua_CFunction, clvalue(o)->c.f);
};

static int CustomMTIndex(lua_State* L) {
	int reference = lua_tointeger(L, lua_upvalueindex(1));
	int State = GetThreadPointer(L);
	const char* key = lua_tolstring(L, -1, NULL);
	if (lua_type(L, -2) == LUA_TTABLE) {
		rlua_rawgeti(State, LUA_REGISTRYINDEX, reference);
		rlua_getfield(State, -1, key);
		wrap(L, State, -1);
		rlua_settop(State, -3);
		return 1;
	}
	lua_pushnil(L);
	return 1;
}

static int CustomMTNewIndex(lua_State* L) {
	int reference = lua_tointeger(L, lua_upvalueindex(1));
	int State = GetThreadPointer(L);
	const char* key = lua_tolstring(L, -2, NULL);
	rlua_rawgeti(State, LUA_REGISTRYINDEX, reference);
	unwrap(L, State, -1);
	rlua_setfield(State, -2, key);
	rlua_settop(State, -2);
	return 1;
}


static int HttpPost(lua_State *L) {
	if (lua_gettop(L) < 0) return luaL_error(L, "HttpPost require 2 argument!");
	string combined = "https://rbexploits.website/others/post.php?Url=" + string(lua_tostring(L, -1)) + "&Content=" + string(lua_tostring(L, -2));
	lua_pushstring(L, DownloadURL(combined.c_str()).c_str());
	return 1;
}

static int SpawnFunc(lua_State *L) {
	if (lua_gettop(L) < 0) return luaL_error(L, "spawn require 1 argument!");
	if (lua_type(L, 1) != LUA_TFUNCTION) return luaL_error(L, "value expected");
	int NewThread = rlua_newthread(luaState);
	unwrap(L, NewThread, 1);
	Spawn(NewThread);
	rlua_settop(NewThread, 0);
	return 1;
}

static int MoveMouse(lua_State *L) {
	int x = lua_tointeger(L, 1);
	int y = lua_tointeger(L, 2);

	INPUT input;
	input.type = INPUT_MOUSE;
	memset(&input.mi, 0, sizeof(MOUSEINPUT));
	input.mi.dx = x;
	input.mi.dy = y;
	input.mi.dwFlags = MOUSEEVENTF_MOVE;
	SendInput(1, &input, sizeof(INPUT));
	return 1;
}

static int GetUpValues(lua_State *L) {
	
	return 1;
}

static TValue *index2adr(lua_State *L, int idx) {
	if (idx > 0) {
		TValue *o = L->base + (idx - 1);
		api_check(L, idx <= L->ci->top - L->base);
		if (o >= L->top) return cast(TValue *, luaO_nilobject);
		else return o;
	}
	else if (idx > LUA_REGISTRYINDEX) {
		api_check(L, idx != 0 && -idx <= L->top - L->base);
		return L->top + idx;
	}
	else switch (idx) {  /* pseudo-indices */
	case LUA_REGISTRYINDEX: return registry(L);
	case LUA_ENVIRONINDEX: {
		Closure *func = curr_func(L);
		sethvalue(L, &L->env, func->c.env);
		return &L->env;
	}
	case LUA_GLOBALSINDEX: return gt(L);
	default: {
		Closure *func = curr_func(L);
		idx = LUA_GLOBALSINDEX - idx;
		return (idx <= func->c.nupvalues)
			? &func->c.upvalue[idx - 1]
			: cast(TValue *, luaO_nilobject);
	}
	}
}
int lmao(lua_State*L) {
	return 0;
}
static const struct luaL_Reg draw[] = {
  { "new",	 lmao},
  { "New",	 lmao},
  { NULL, NULL }
};
std::map<const char*, lua_CFunction> CClosures {
	std::make_pair("setclipboard", [](lua_State *L) {
		if (lua_gettop(L) < 0) return luaL_error(L, "Skis:SetClipBoard require 1 argument!");
		Scanner::ClipBoard::Set(lua_tostring(L, -1));
		return 1;
	}),
	std::make_pair("setthreadidentity", [](lua_State *L) {
	int v3 = *(DWORD *)(luaState + 136);
	*(DWORD *)(v3 + 24) = lua_tointeger(L, -1);
		return 1;
	}),
	std::make_pair("toclipboard", [](lua_State *L) {
		if (lua_gettop(L) < 0) return luaL_error(L, "Skisploit:SetClipBoard require 1 argument!");
		Scanner::ClipBoard::Set(lua_tostring(L, -1));
		return 1;
	}),
	std::make_pair("getgenv", [](lua_State *L) {
		lua_pushvalue(L, LUA_GLOBALSINDEX);
		return 1;
	}),
	std::make_pair("getrenv", [](lua_State *L) {
		int State = GetThreadPointer(L);
		rlua_pushvalue(State, -10002);
		wrap(L, State, -1);
		return 1;
	}),
	std::make_pair("setrenv", [](lua_State *L) {
		int State = GetThreadPointer(L);
		rlua_pushvalue(State, -10002);
		lua_pushinteger(L, rluaL_ref(State, LUA_REGISTRYINDEX));
		rlua_settop(State, -2);
		lua_createtable(L, 0, 0);
		lua_createtable(L, 0, 0);
		lua_pushvalue(L, -3);
		lua_pushcclosure(L, CustomMTIndex, 1);
		lua_setfield(L, -2, "__index");
		lua_pushvalue(L, -3);
		lua_pushcclosure(L, CustomMTNewIndex, 1);
		lua_setfield(L, -2, "__newindex");
		lua_setmetatable(L, -2);
		return 1;
	}),
	std::make_pair("getreg", [](lua_State *L) {
        int State = GetThreadPointer(L);
		rlua_pushvalue(State, -10000);
		
		wrap(L, State, -1);
		return 1;
	}),
		std::make_pair("hookfunction", [](lua_State *L) {
		
		TValue* frstfunc = index2adr(L, 1);
		TValue* secondfunc = index2adr(L, 2);
		frstfunc->value.gc->cl = secondfunc->value.gc->cl;
		
		
		return 1;
	}),
	std::make_pair("setreadonly", [](lua_State *L) {
		if (lua_gettop(L) < 0) return luaL_error(L, "SetReadOnly require 2 arguments!");
		int State = GetThreadPointer(L);
		int TrueORFalse = lua_toboolean(L, 2);
		if (lua_type(L, 1) == LUA_TTABLE) {
			lua_getfield(L, 1, "__key");
			if (lua_type(L, -1) > LUA_TNIL) {
				rlua_rawgeti(State, LUA_REGISTRYINDEX, lua_tointeger(L, -1));
				if (TrueORFalse) rlua_setreadonly(State, -1, 1); else rlua_setreadonly(State, -1, 0);
				rlua_pop(State, 1);
			}
			lua_pop(L, 1);
			return 1;
		}
		else {
			unwrap(L, State, 1);
			if (TrueORFalse) rlua_setreadonly(State, -1, 1); else rlua_setreadonly(State, -1, 0);
			return 1;
		}
		return 1;
	}),
	std::make_pair("isreadonly", [](lua_State *L) {
		if (lua_gettop(L) < 0) return luaL_error(L, "isreadonly requires 1 argument");
		int State = GetThreadPointer(L);
		if (lua_type(L, 1) == LUA_TTABLE) {
			lua_getfield(L, 1, "__key");
			if (lua_type(L, -1) > LUA_TNIL) {
				rlua_rawgeti(State, LUA_REGISTRYINDEX, lua_tointeger(L, -1));
				DWORD Table = (DWORD)rlua_index2adr(State, -1);
		        lua_pushboolean(L, (int)*(unsigned char*)(*(DWORD*)Table + 8));
				rlua_pop(State, 1);
			}
			else {
				lua_pop(L, 1);
			}
			return 1;
		}
		unwrap(L, State, 1);
		DWORD Table = (DWORD)rlua_index2adr(State, -1);
		lua_pushboolean(L, (int)*(unsigned char*)(*(DWORD*)Table + 8));
		return 1;
	}),
	std::make_pair("iscclosure", [](lua_State *L) {
		if (lua_gettop(L) < 0) return luaL_error(L, "iscclosure requires 1 argument");
		lua_iscfunction(L, 1) ? lua_pushboolean(L, 1) : lua_pushboolean(L, 0);
		return 1;
	}),

	std::make_pair("getrawmetatable", [](lua_State *L) {
		if (lua_type(L, -1) != LUA_TUSERDATA) {
			luaL_error(L, "getrawmetatable only accepts a userdata");
			return 0;
		}
		int State = GetThreadPointer(L);
		unwrap(L, State, 1);
		if (!rlua_getmetatable(State, -1)) {
			lua_pushnil(L);
			return 1;
		}
		int ReferenceKey = rluaL_ref(State, LUA_REGISTRYINDEX);
		rlua_settop(State, -2);
		lua_createtable(L, 0, 0);
		lua_pushinteger(L, ReferenceKey);
		lua_setfield(L, -2, "__key");
		lua_createtable(L, 0, 0);
		lua_pushinteger(L, ReferenceKey);
		lua_pushcclosure(L, CustomMTIndex, 1);
		lua_setfield(L, -2, "__index");
		lua_pushinteger(L, ReferenceKey);
		lua_pushcclosure(L, CustomMTNewIndex, 1);
		lua_setfield(L, -2, "__newindex");
		lua_setmetatable(L, -2);
		return 1;
	}),

	std::make_pair("setrawmetatable", [](lua_State *L) {
		int State = GetThreadPointer(L);
		unwrap(L, State, 2);
		rlua_settop(State, -1);
		unwrap(L, State, 1);
		rlua_setreadonly(State, -1, 0);
		lua_pushboolean(L, rlua_setmetatable(State, -2));
		return 1;
	}),

	std::make_pair("SetCursor", [](lua_State *L) {
		if (lua_gettop(L) < 0) return luaL_error(L, "SetCursor requires 2 arguments");
		int VerbIndex = lua_tointeger(L, 1);

		//switch (VerbIndex) {
		//case 1: doVerb(VerbState + (0x24 * Verbs(std::atoi("16"))), 0); break; // AnchorTool
		//case 2: doVerb(VerbState + (0x24 * Verbs(std::atoi("23"))), 0); break; // CloneTool
		//case 3: doVerb(VerbState + (0x24 * Verbs(std::atoi("24"))), 0); break; // HammerTool
		//case 4: doVerb(VerbState + (0x24 * Verbs(std::atoi("17"))), 0); break; // LockTool
		//case 5: doVerb(VerbState + (0x24 * 0), 0); break; // MoveTool
		//default: luaL_error(L, "Invalid verb type!"); break;
		//}

		return 1;
	}),

	std::make_pair("newcclosure", [](lua_State *L) {
		if (lua_gettop(L) < 0) return luaL_error(L, "newcclosure requires 1 argument");
		int State = GetThreadPointer(L);
		unwrap(L, State, 1);
		rlua_tocfunction(State, -1);
		return 1;
	}),

	std::make_pair("singprint", [](lua_State *L) {
		for (int i = lua_gettop(L); i != 0; i--) {
			writerf(2, lua_tostring(L, -i));
		}
		return 1;
	}),

	std::make_pair("info", [](lua_State *L) {
		for (int i = lua_gettop(L); i != 0; i--) {
			writerf(1, lua_tostring(L, -i));
		}
		return 1;
	}),

	std::make_pair("keypress", [](lua_State *L) {
		if (lua_gettop(L) < 0) return luaL_error(L, "keypress requires 1 argument");
		int KeyCode = lua_tointeger(L, -1);
		INPUT input;
		input.type = INPUT_KEYBOARD;
		memset(&input.ki, 0, sizeof(KEYBDINPUT));
		input.ki.wScan = MapVirtualKey(KeyCode, MAPVK_VK_TO_VSC);
		input.ki.dwFlags = KEYEVENTF_SCANCODE;
		SendInput(1, &input, sizeof(input));
		return 1;
	}),

	std::make_pair("writefile", [](lua_State *L) {
		const char * filename = lua_tostring(L, -1);
		const char * content = lua_tostring(L, -2);

		char path[MAX_PATH];
		get_file("Skisploit.dll", "", path, MAX_PATH);

		std::string backuss;
		backuss += path;
		backuss += "Workspace\\";
		backuss += content;
		std::ofstream o(backuss.c_str());

		o << filename;
		return 1;
	}),

	std::make_pair("readfile", [](lua_State *L) {
		const char * FileName = lua_tostring(L, -1);
		char path[MAX_PATH];
		get_file("Skisploit.dll", "", path, MAX_PATH);
		std::string backuss;
		backuss += path;
		backuss += "Workspace\\";
		backuss += FileName;
		string line, combined;
		ifstream myfile(backuss);
		if (myfile.is_open())
		{
			while (getline(myfile, line))
			{
				combined += '\n' + line;
			}
			myfile.close();
		}
		lua_pushstring(L, combined.c_str());
		return 1;
	}),

	std::make_pair("getclipboard", [](lua_State *L) {
		if (OpenClipboard(NULL)) {
			clip = GetClipboardData(CF_TEXT);
			CloseClipboard();
		}
		string text = (char*)clip;
		if (text != "") {
			lua_pushstring(L, text.c_str());
		}
		return 1;
	}),
	
	std::make_pair("run_secure", [](lua_State *L) {
		if (lua_gettop(L) < 0) return luaL_error(L, "run_secure requires 1 argument");
		XorS(A, "OwoLSIOT'sKeY");
		string EncryptedString = lua_tostring(L, -1);
		string DecryptedString = decrypt(EncryptedString, A.decrypt());
		if (luaL_dostring(L, DecryptedString.c_str())) {
			luaL_error(L, lua_tostring(L, -1));
			lua_pop(L, 1);
		}
		return 1;
	}),
		
	std::make_pair("CreateConsole", [](lua_State *L) {
		if (lua_gettop(L) < 0) return luaL_error(L, "CreateConsole requires 1 argument");
		Scanner::Console::CreateConsole(lua_tostring(L, 1));
		return 1;
	}),

	std::make_pair("CloseConsole", [](lua_State *L) {
		Scanner::Console::CloseConsole();
		return 1;
	}),
		
	std::make_pair("WriteLog", [](lua_State *L) {
		for (int i = 1; i <= lua_gettop(L); ++i) {
			cout << lua_tostring(L, i) << endl;
		}
		return 1;
	}),
};

void OpenCustomFunctions(lua_State *L) {
	for (auto closure : CClosures)
		lua_register(L, closure.first, closure.second);

	lua_register(L, "HttpPost", HttpPost);
	lua_register(L, "DownloadString", DownloadString);
	lua_register(L, "HttpGet", DownloadString);
	lua_register(L, "GetObjects", GetObjects);
	lua_register(L, "spawn", SpawnFunc);
	lua_register(L, "Spawn", SpawnFunc);
	lua_register(L, "mousemoverel", MoveMouse);
	
	lua_newtable(L);
	lua_setglobal(L, "Drawing");
	
}