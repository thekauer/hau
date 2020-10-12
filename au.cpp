#include "au.h"
#include <TlHelp32.h>
#include <string>
#include <iostream>
#include <locale>
#include <codecvt>
AmongUs::AmongUs()
{
	pId = get_au_pid();
	dwGameAsm = get_gameasm_base(pId);
	hAu = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pId);
	dwInstanceBase = get_instance_base();

	std::ios_base::sync_with_stdio(false);

	std::locale utf8(std::locale(), new std::codecvt_utf8_utf16<wchar_t>);
	std::wcout.imbue(utf8);
}

AmongUs::~AmongUs() {
	CloseHandle(hAu);
}

DWORD AmongUs::get_au_pid() {
	DWORD pid = -1;
	auto ss = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (ss == INVALID_HANDLE_VALUE) {
		throw "invalid handle";
	}
	PROCESSENTRY32 pe;
	pe.dwSize = sizeof(pe);
	if (!Process32First(ss, &pe)) {
		throw "err1";
	}
	do {
		if (std::wstring(pe.szExeFile) == L"Among Us.exe") {
			pid = pe.th32ProcessID;
			break;
		}
	} while (Process32Next(ss, &pe));
	CloseHandle(ss);
	return pid;
}

DWORD AmongUs::get_gameasm_base(DWORD pid) {
	auto ss = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, pid);
	MODULEENTRY32 me;
	me.dwSize = sizeof(me);
	DWORD base;
	if (ss == INVALID_HANDLE_VALUE) {
		throw "invalid handle in module";
	}
	if (!Module32First(ss, &me)) {
		throw "invalid module";
	}
	do {
		if (std::wstring(me.szModule) == L"GameAssembly.dll") {
			base = (DWORD)me.modBaseAddr;
			break;
		}
	} while (Module32Next(ss, &me));
	CloseHandle(ss);
	return base;
}

DWORD AmongUs::get_instance_base()
{
	SIZE_T read;
	DWORD ptr;
	ReadProcessMemory(hAu, (LPCVOID)(dwGameAsm + 0x00D98B24), &ptr, sizeof(ptr), &read);
	ReadProcessMemory(hAu, (LPCVOID)(ptr + 0x684), &ptr, sizeof(ptr), &read);
	return ptr+0x200;		//0xac0
}

DWORD AmongUs::get_player_mylight(DWORD dwPlayer)
{
	DWORD mylight;
	SIZE_T read;
	ReadProcessMemory(hAu, (LPCVOID)(dwPlayer + 0x54), &mylight, sizeof(mylight), &read);
	return mylight;
}

DWORD AmongUs::get_nth_player(int n)
{
	return dwInstanceBase+n*0xa0;
}

Pos AmongUs::get_player_pos(DWORD dwPlayer)
{
	Pos p;
	DWORD ptr;
	SIZE_T read;
	ReadProcessMemory(hAu, (LPCVOID)(dwPlayer + 0x60), &ptr, sizeof(ptr), &read);
	ReadProcessMemory(hAu, (LPCVOID)(ptr + 0x3c), &p, sizeof(p), &read);
	return p;
}

Pos AmongUs::get_player_local_pos(DWORD dwPlayer)
{
	Pos p;
	DWORD ptr;
	SIZE_T read;
	ReadProcessMemory(hAu, (LPCVOID)(dwPlayer + 0x60), &ptr, sizeof(ptr), &read);
	ReadProcessMemory(hAu, (LPCVOID)(ptr + 0x50), &p, sizeof(p), &read);
	return p;
}

std::wstring AmongUs::get_player_name(DWORD dwPlayer)
{
	DWORD size,ptr;
	ReadProcessMemory(hAu, (LPCVOID)(dwPlayer + 0x4c), &ptr, sizeof(ptr), 0);
	ReadProcessMemory(hAu, (LPCVOID)(ptr+0x28), &ptr, sizeof(ptr), 0);
	ReadProcessMemory(hAu, (LPCVOID)(ptr + 0x8), &size, sizeof(size), 0);
	std::vector<wchar_t> buff(20);
	ReadProcessMemory(hAu, (LPCVOID)(ptr + 0xc), &buff[0], 20, 0);
	return std::wstring(&buff[0]);
}

std::wstring AmongUs::get_playerinfo_name(DWORD dwPlayerInfo)
{
	DWORD ptr;
	wchar_t buff[20] = { 0 };
	ReadProcessMemory(hAu, (LPCVOID)(dwPlayerInfo + 0xc), &ptr, sizeof(ptr), 0);
	ReadProcessMemory(hAu, (LPCVOID)(ptr + 0xc), &buff, 20, 0);
	return std::wstring(buff);
}

void AmongUs::set_playerinfo_name(DWORD dwPlayerInfo, std::wstring name)
{
	DWORD ptr;
	DWORD size = name.size();
	wchar_t nev[20] = {0};
	for (int i = 0; i < name.size();i++) {
		nev[i] = name[i];
	}
	ReadProcessMemory(hAu, (LPCVOID)(dwPlayerInfo + 0xc), &ptr, sizeof(ptr), 0);
	WriteProcessMemory(hAu, (LPVOID)(dwPlayerInfo + 0x8), &size, sizeof(size), 0);
	WriteProcessMemory(hAu, (LPVOID)(ptr + 0xc), &nev[0], 20, 0);
}

void AmongUs::set_player_pos(DWORD dwPlayer, const Pos& p)
{
	DWORD ptr;
	SIZE_T read;
	ReadProcessMemory(hAu, (LPCVOID)(dwPlayer + 0x60), &ptr, sizeof(ptr), &read);
	WriteProcessMemory(hAu, (LPVOID)(ptr + 0x3c), &p, sizeof(p), &read);
}

void AmongUs::freez_kill_timer(DWORD dwPlayer)
{
	float zero=0;
	SIZE_T wrote;
	WriteProcessMemory(hAu, (LPVOID)(dwPlayer + 0x44), &zero, sizeof(zero), &wrote);
}

void AmongUs::print_all_player_pos()
{
	for (int i = 0; i <= 20; i++) {
		auto p = get_nth_player(i);
		auto pos = get_player_pos(p);
		auto name = get_player_name(p);
		std::wcout << "i: "<<i<<" name: " <<name <<std::flush<<" x: " << pos.x << " y: " << pos.y << std::endl;
	}
}

void AmongUs::kill(DWORD dwPlayerInfo)
{
	BYTE one = 1;
	WriteProcessMemory(hAu, (LPVOID)(dwPlayerInfo + 0x29), &one, sizeof(one), 0);
}

void AmongUs::ressurect(DWORD dwPlayerInfo)
{
	BYTE zero = 0;
	WriteProcessMemory(hAu, (LPVOID)(dwPlayerInfo + 0x29), &zero, sizeof(zero), 0);
}

void AmongUs::make_crew(DWORD dwPlayerInfo)
{
	BYTE zero = 0;
	WriteProcessMemory(hAu, (LPVOID)(dwPlayerInfo + 0x28), &zero, sizeof(zero), 0);
}

void AmongUs::make_impostor(DWORD dwPlayerInfo)
{
	BYTE one = 1;
	WriteProcessMemory(hAu, (LPVOID)(dwPlayerInfo + 0x28), &one, sizeof(one), 0);
}

DWORD AmongUs::get_local_player()
{
	for (int i = 0; i < 10; i++) {
		auto p = get_nth_player(i);
		auto light = get_player_mylight(p);
		if (light) return p;
	}
	return 0;
}

void AmongUs::change_color(DWORD dwPlayer, unsigned char color)
{
	const DWORD dwBuffSize=0x1000;
	DWORD dwSetColor = dwGameAsm + 0x211eb0;
	ColorArg arg = { dwPlayer,color };
	const auto arg_offset = 1;
	const auto func_offset = 12;
	const DWORD dwCodeSize = 0x200;
	char code[dwCodeSize] = {
		0xB8, 0x00, 0x00, 0x00, 0x00,					//mov eax,0			-Arg address
		0xFF, 0x30,										//push DWORD PTR [eax]
		0x8B,0x58,0x04,									//mov ebx,0			-this address
		0xB8, 0x0, 0x00, 0x00, 0x00,					//mov eax,0			-function address
		0xFF, 0xD0,										//call eax
		0x83, 0xC4, 0x08,								//add esp,4
		0xC3 };											//ret

	void* pMem = VirtualAllocEx(hAu, 0, dwBuffSize, MEM_RESERVE | MEM_COMMIT, PAGE_EXECUTE_READWRITE);
	if (!pMem) throw "failed to allocate memory";
	*(uintptr_t*)(&code[arg_offset]) = (uintptr_t)pMem;
	*(uintptr_t*)(&code[func_offset]) = (uintptr_t)dwSetColor;
	WriteProcessMemory(hAu, (LPVOID)pMem, &arg, sizeof(arg), 0);
	void* pCode = (void*)((uintptr_t)pMem + sizeof(arg));
	WriteProcessMemory(hAu, pCode, code, dwCodeSize, 0);
	HANDLE hThread = CreateRemoteThread(hAu, 0, 0, (LPTHREAD_START_ROUTINE)pCode, pMem, 0, 0);
	WaitForSingleObject(hThread, INFINITE);
	VirtualFreeEx(hAu, pMem, 0, MEM_RELEASE);
	CloseHandle(hThread);
}

DWORD AmongUs::get_local_PlayerInfo()
{
	DWORD ptr;
	ReadProcessMemory(hAu, (LPCVOID)(dwGameAsm + 0x00DA3C30), &ptr, sizeof(ptr), 0);
	ReadProcessMemory(hAu, (LPCVOID)(ptr + 0x5c), &ptr, sizeof(ptr), 0);
	ReadProcessMemory(hAu, (LPCVOID)(ptr + 0x10), &ptr, sizeof(ptr), 0);
	ReadProcessMemory(hAu, (LPCVOID)(ptr + 0x34), &ptr, sizeof(ptr), 0);
	return ptr + 0;
}

DWORD AmongUs::PlayerControl_from_PlayerInfo(DWORD dwPlayerInfo)
{
	DWORD ptr;
	ReadProcessMemory(hAu, (LPCVOID)(dwPlayerInfo + 0x2c), &ptr, sizeof(ptr), 0);
	return ptr;
}

DWORD AmongUs::PlayerInfo_from_PlayerControl(DWORD dwPlayerControl)
{
	DWORD ptr;
	ReadProcessMemory(hAu, (LPCVOID)(dwPlayerControl + 0x34), &ptr, sizeof(ptr), 0);
	return ptr;
}

bool AmongUs::is_impostor(DWORD dwPlayerInfo)
{
	BYTE imp;
	ReadProcessMemory(hAu, (LPCVOID)(dwPlayerInfo + 0x28), &imp, sizeof(imp), 0);
	return imp==1;
}

std::vector<DWORD> AmongUs::get_impostots()
{
	std::vector<DWORD> impostors;
	for (int i = 0; i <= 20; i++) {
		auto p = get_nth_player(i);
		auto pinfo = PlayerInfo_from_PlayerControl(p);
		if (is_impostor(pinfo)) {
			impostors.push_back(pinfo);
		}
	}

	return impostors;
}

