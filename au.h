#pragma once
#include <Windows.h>
#include <vector>
#include <string>
struct Pos {
	float x, y;
	Pos(float x, float y) : x{ x }, y{ y }{};
	Pos() :x{ 0.0 }, y{ 0.0 }{};
};

class AmongUs {
public:
	AmongUs();
	~AmongUs();

	DWORD get_nth_player(int n);
	Pos get_player_pos(DWORD dwPlayer);
	Pos get_player_local_pos(DWORD dwPlayer);
	std::wstring get_player_name(DWORD dwPlayer);
	std::wstring get_playerinfo_name(DWORD dwPlayerInfo);
	void set_playerinfo_name(DWORD dwPlayerInfo, std::wstring name);
	void set_player_pos(DWORD dwPlayer, const Pos& p);
	void freez_kill_timer(DWORD dwPlayer);
	void print_all_player_pos();
	void kill(DWORD dwPlayerInfo);
	void ressurect(DWORD dwPlayerInfo);
	void make_crew(DWORD dwPlayerInfo);
	void make_impostor(DWORD dwPlayerInfo);
	DWORD get_local_player();
	void change_color(DWORD dwPlayer, unsigned char color);
	DWORD get_local_PlayerInfo();
	DWORD PlayerControl_from_PlayerInfo(DWORD dwPlayerInfo);
	DWORD PlayerInfo_from_PlayerControl(DWORD dwPlayerControl);
	bool is_impostor(DWORD dwPlayerInfo);
	std::vector<DWORD> get_impostots();

private:
	HANDLE hAu;
	DWORD pId;
	DWORD dwGameAsm;
	DWORD dwInstanceBase;

	DWORD get_au_pid();
	DWORD get_gameasm_base(DWORD pid);
	DWORD get_instance_base();
	DWORD get_player_mylight(DWORD dwPlayer);

	struct ColorArg {
		DWORD self;
		BYTE color;
		DWORD ret;
	};
};