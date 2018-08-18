// TEXT.H

#pragma once
#include "constants.h"
#include "graphics.h"
#include "entities.h"
#include "control.h"

class Text {
	SDL_Renderer* renderer;
	Control* level;
	TTF_Font* font_1;
	TTF_Font* font_2;
	SDL_Color color = { 255, 255, 255 };
	plot dimensions;
	stringstream score_ss;
	stringstream window_debug_ss, fullscreen_debug_ss, player_debug_ss, player_debug_2_ss, time_debug_ss, log_debug_ss;
	string score_str, start_str;
	string window_debug_str, fullscreen_debug_str, player_debug_str, player_debug_2_str, time_debug_str, log_debug_str;
	unsigned int init_window_width;
	unsigned int init_window_height;
public:
	Text(SDL_Renderer*, Control &, CoUnInt, CoUnInt, char*, char*);
	~Text();
	void update();
	void update(const bool, vector<Player*> &, CoUnInt, CoUnInt, CoUnInt);
	void render();
	void render_text_copy(SDL_Renderer* &, SDL_Texture* &, SDL_Rect &, SDL_Rect &, CoUnInt, CoUnInt, CoUnInt);
	plot position;
};