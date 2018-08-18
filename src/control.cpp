// CONTROL.CPP

#include "control.h"

Control::Control() {
	map_data_in.open("assets/map_binary.txt");
	game_log.open("game_log.txt");
	char c;
	int num;

	for (int i = 0; i < map_data.size(); i++) {
		for (int j = 0; j < map_data[i].size() && map_data_in.get(c); j++) {
			num = c - '0';
			map_data[i][j] = num;
		}
	}
}

void Control::log(string message) {
	log_to_screen = message;
	game_log << message << endl;
}