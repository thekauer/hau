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
	au.make_impostor(my_playerinfo);
	while(true)
	au.freez_kill_timer(my_playercontrol);
	Sleep(200);
	return 0;
}