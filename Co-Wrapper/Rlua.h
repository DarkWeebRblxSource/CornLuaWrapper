#pragma once
#include "Scanner.h"
#include "RetCheck.h"



DWORD unprotect(DWORD addr)
{
	BYTE* tAddr = (BYTE*)addr;
	do
	{
		tAddr += 16;
	} while (!(tAddr[0] == 0x55 && tAddr[1] == 0x8B && tAddr[2] == 0xEC));

	DWORD funcSz = tAddr - (BYTE*)addr;

	PVOID nFunc = VirtualAlloc(NULL, funcSz, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
	if (nFunc == NULL)
		return addr;

	memcpy(nFunc, (void*)addr, funcSz);

	BYTE* pos = (BYTE*)nFunc;
	BOOL valid = false;
	do
	{
		if (pos[0] == 0x72 && pos[2] == 0xA1 && pos[7] == 0x8B) {
			*(BYTE*)pos = 0xEB;

			DWORD cByte = (DWORD)nFunc;
			do
			{
				if (*(BYTE*)cByte == 0xE8)
				{
					DWORD oFuncPos = addr + (cByte - (DWORD)nFunc);
					DWORD oFuncAddr = (oFuncPos + *(DWORD*)(oFuncPos + 1)) + 5;

					if (oFuncAddr % 16 == 0)
					{
						DWORD relativeAddr = oFuncAddr - cByte - 5;
						*(DWORD*)(cByte + 1) = relativeAddr;

						cByte += 4;
					}
				}

				cByte += 1;
			} while (cByte - (DWORD)nFunc < funcSz);

			valid = true;
		}
		pos += 1;
	} while ((DWORD)pos < (DWORD)nFunc + funcSz);

	if (!valid)
	{
		VirtualFree(nFunc, funcSz, MEM_RELEASE);
		return addr;
	}

	return (DWORD)nFunc;
}


#define Declare(address, returnValue, callingConvention, ...) (returnValue(callingConvention*)(__VA_ARGS__))(unprotect(ScanAddress(address)))
#define DeclareX(address, returnValue, callingConvention, ...) (returnValue(callingConvention*)(__VA_ARGS__))(ReturnCheck::UnProtect((byte*)ScanAddress(address)))
#define rlua_gettop(rL) ((*(DWORD *)(rL + 24) - *(DWORD *)(rL + 16)) >> 4)
#define rlua_pop(rL, n) rlua_settop(rL, -(n) - 1)
#define rlua_getglobal(rL, k) rlua_getfield(rL, -10002, k)

#define RLUA_TNIL						 0
#define RLUA_TLIGHTUSERDATA              2
#define RLUA_TNUMBER					 3
#define RLUA_TBOOLEAN					 1
#define RLUA_TSTRING					 4
#define RLUA_TTHREAD                     8
#define RLUA_TFUNCTION					 5
#define RLUA_TTABLE						 7
#define RLUA_TUSERDATA					 6






int int3breakpoint, luaState, RobloxThread, int3breakpoint1, VerbState, lolx;

DWORD int3faker;

auto doVerb = (int(__thiscall*)(uintptr_t _this, int unk))ScanAddress(0xcfb220);

union r_Value {
	PVOID gc;
	PVOID p;
	double n;
	int b;
};




/* End Off the RetCheck!*/



enum Verbs {
	AdvTranslateTool,
	AdvRotateTool,
	AdvArrowTool,
	AdvScaleTool,
	MoveResizeJoinTool,
	FlatTool,
	GlueTool,
	WeldTool,
	StudsTool,
	InletTool,
	UniversalTool,
	HingeTool,
	RightMotorTool,
	LeftMotorTool,
	OscillateMotorTool,
	AnchorTool,
	LockTool,
	FillTool,
	MaterialTool,
	DropperTool,
	GameTool,
	GrabTool,
	CloneTool,
	HammerTool
};

struct r_TValue {
	r_Value value;
	int tt;
};

struct Userdata {
	int32_t reference;
	void* UD;
	int Key;
	int Key2;
};

// -- Lua C Functions -- \\

auto rlua_index2adr = Declare(0x7c02c0, r_TValue*, __cdecl, DWORD a1, int idx);
typedef void(__cdecl* rgetfield)(DWORD rL, int idx, const char* k);
rgetfield rlua_getfield = (rgetfield)unprotect(ScanAddress(0x7c80a0));
auto rlua_gettable = Declare(0x7c8620, void, __cdecl, int a1, int idx);
auto rlua_createtable = Declare(0x7c7c70, void, __cdecl, int a1, int narray, int nrec);
//auto rlua_touserdata = Declare(0x767620, int, __cdecl, int a1, int idx);

typedef void* (__cdecl* touserdata)(DWORD, int);
touserdata rlua_touserdata = (touserdata)(ReturnCheck::UnProtect((byte*)(ScanAddress(0x7cac40))));
//auto rlua_touserdata = (int(__cdecl*)(int, int))ScanAddress(0x767620);

auto rlua_getmetatable = Declare(0x7c8430, int, __fastcall, int a1, signed int a2);
auto rlua_toboolean = Declare(0x7ca910, int, __cdecl, int a1, signed int a2);
//do tuk stignah

typedef void(__stdcall* rpushcclosure)(DWORD rL, int fn, int non, int a3);
rpushcclosure rl_pushcclosure = (rpushcclosure)(ReturnCheck::UnProtect((byte*)(ScanAddress(0x7c9100))));//Updated(0x7b9F80))));//Updated



auto rlua_pushlightuserdata = Declare(0x7c93c0, void, __cdecl, DWORD a1, void* p);
auto rlua_pushstring = Declare(0x7c95e0, int, __fastcall, DWORD a1, const char* a2);
auto rlua_pushvalue = Declare(0x7c9730, int, __fastcall, DWORD a1, int idx);
auto rlua_pushnumber = Declare(0x7c9550, void, __stdcall, DWORD a1, double a2);
auto rlua_pushboolean = Declare(0x7c9080, void, __cdecl, DWORD a1, int a2);
auto rlua_pushnil = Declare(0x7c94e0, void, __cdecl, DWORD a1);
auto rlua_type = Declare(0x7caca0, int, __cdecl, DWORD a1, int idx);
typedef void(__stdcall* rsetfield)(DWORD rL, int idx, const char* k);
rsetfield rlua_setfield = (rsetfield)unprotect(ScanAddress(0x7ca250));//Updated
auto rlua_settable = Declare(0x7ca700, void, __cdecl, DWORD a1, int idx);
auto rlua_tolstring = Declare(0x7ca9d0, const char*, __cdecl, DWORD a1, int a2, std::size_t * a3);
auto rlua_setreadonly = Declare(0x7ca600, void, __cdecl, DWORD a1, int a2, int a3);
auto rlua_setmetatable = Declare(0x7ca4d0, int, __cdecl, DWORD a1, int a2);
auto rlua_newthread = Declare(0x7c8ab0, int, __cdecl, DWORD a1);
auto rlua_rawgeti = Declare(0x7c99e0, void, __cdecl, DWORD a1, int idx, int n);
auto rlua_rawseti = Declare(0x7c9b70, void, __cdecl, DWORD a1, int idx, int n);
auto rlua_settop = Declare(0x7ca790, void, __stdcall, DWORD a1, int idx);


typedef int(__cdecl* rrr)(DWORD a1, int a2, int a3, int a4);
rrr rlua_pcall = (rrr)(ReturnCheck::UnProtect((byte*)(ScanAddress(0x7c8fb0))));


auto rluaL_error = Declare(0x7c2e90, int, __cdecl, DWORD a1, const char* lol, ...); //it was this one
auto writerf = Declare(0x5efea0, int, __cdecl, int idx, const char* lol, ...); //print addy?

typedef int(__cdecl* spawnuwu)(int);
spawnuwu Spawn = (spawnuwu)unprotect(ScanAddress(0x728b70));


typedef int(__cdecl* sandboxthread)(int, int, int);
sandboxthread SandBoxThread = (sandboxthread)ScanAddress(0x7287f0);

void* rlua_newuserdata(DWORD rL, size_t size)//Updated
{
	return ((void* (__cdecl*)(DWORD rL, size_t size, int))unprotect(ScanAddress(0x7c8bf0)))(rL, size, 0);
}

double rlua_tonumber(DWORD rL, int idx)
{
	return ((double(__cdecl*)(DWORD, int, int))ScanAddress(0x7cab00))(rL, idx, 0);//Updated
}

typedef DWORD(__cdecl* rref)(DWORD, DWORD);
rref rluaL_ref = (rref)(ReturnCheck::UnProtect((BYTE*)(ScanAddress(0x7c3180)))); // luaL_ref offset

typedef DWORD(__cdecl* next2)(DWORD rL, int idx);
next2 rlua_next = (next2)(ReturnCheck::UnProtect((BYTE*)(ScanAddress(0x7c8d10))));;



void rlua_pushcfunction(int rL, int fn, int n, bool lol = false) {
	lolx = lol;
	if (lol) {
		rlua_pushlightuserdata(rL, (void*)fn);
		rl_pushcclosure(rL, int3breakpoint1, n, 0);
	}
	else {
		rlua_pushlightuserdata(rL, (void*)fn);
		rl_pushcclosure(rL, int3breakpoint1, n + 1, 0);
	}
}

int GettopHook(int rL) {
	luaState = rL;
	return rlua_gettop(rL);
}

void rlua_pushcclosure(int rL, int fn, int n) {
	int3faker = fn;
	rl_pushcclosure(rL, int3breakpoint1, n, 0);
}





// -- int3jumphandler & breakpoint handler -- \\

static int int3jumphandler(int CurrentState)
{
	if (lolx) {
		typedef int(*_handle)(DWORD);
		_handle handle = (_handle)rlua_touserdata(CurrentState, -10003);
		return handle(CurrentState);
	}
	else {
		typedef int(*_handle)(DWORD);
		_handle handle = (_handle)rlua_touserdata(CurrentState, -10004);
		return handle(CurrentState);
	}
	return 0;
}

LONG WINAPI int3cbreakpoint(PEXCEPTION_POINTERS ex)
{
	if (ex->ExceptionRecord->ExceptionCode == EXCEPTION_BREAKPOINT)
	{
		if (ex->ContextRecord->Eip == int3breakpoint) {
			ex->ContextRecord->Eip = int3faker;
			return EXCEPTION_CONTINUE_EXECUTION;
		}

		if (ex->ContextRecord->Eip == int3breakpoint1) {
			ex->ContextRecord->Eip = (DWORD)(int3jumphandler);
			return EXCEPTION_CONTINUE_EXECUTION;
		}
	}
	return EXCEPTION_CONTINUE_SEARCH;
}
