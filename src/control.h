// CONTROL.H

#pragma once
#include "constants.h"

class Control {
	ifstream map_data_in;
	ofstream game_log;
public:
	unsigned int score = 0;
	string log_to_screen = "";
	SDL_Color color = { 255, 255, 255 };
	array<array<int, 28>, 31> map_data;
	Control();
	void log(string);
};