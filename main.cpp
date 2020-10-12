#include <iostream>
#include "au.h"



int main() {
	AmongUs au;
	auto my_playerinfo = au.get_local_PlayerInfo();
	auto my_playercontrol = au.PlayerControl_from_PlayerInfo(my_playerinfo);
	auto impostors = au.get_impostots();
	std::cout << "impostors: " << std::endl;
	for (const auto impostor : impostors) {
		au.make_crew(impostor);
		std::wcout << au.get_playerinfo_name(impostor) << std::endl;
	}

	for (int i = 0; i <= 20; i++) {
		auto pc = au.get_nth_player(i);
		auto pi = au.PlayerInfo_from_PlayerControl(pc);
		au.set_playerinfo_name(pi, L"csicskám");
	}

	au.make_impostor(my_playerinfo);
	while(true)
	au.freez_kill_timer(my_playercontrol);
	Sleep(200);
	return 0;
}