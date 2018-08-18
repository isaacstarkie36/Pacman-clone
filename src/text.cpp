// TEXT.CPP

#include "text.h"
#include "entities.h"

Text::Text(SDL_Renderer* game_renderer, Control &control, CoUnInt init_width, CoUnInt init_height, char* arial, char* emulogic) {
	renderer = game_renderer;
	level = &control;
	dimensions = { 0, 20 };
	position = { 30, 100 };
	init_window_width = init_width;
	init_window_height = init_height;

	font_1 = TTF_OpenFont(arial, 10);
	if (!font_1) {
		level->log_to_screen = "Error opening font";
	}
	font_2 = TTF_OpenFont(emulogic, 14);
	if (!font_2) {
		level->log_to_screen = "Error opening font";
	}
}

Text::~Text() {
	font_1 = NULL;
	font_2 = NULL;
}

void Text::update(const bool fullscreen, vector<Player*> &players, CoUnInt window_width, CoUnInt window_height, CoUnInt TIME_PASSED) {
	score_ss.str("");
	score_ss << level->score;
	score_str = score_ss.str();

	window_debug_ss.str("");
	fullscreen_debug_ss.str("");
	player_debug_ss.str("");
	player_debug_2_ss.str("");
	time_debug_ss.str("");
	log_debug_ss.str("");

	window_debug_ss << "width = " << window_width << ", height = " << window_height;
	fullscreen_debug_ss << "fullscreen = " << fullscreen << ", score = " << level->score;
	player_debug_ss << "P1: x = " << (int)players[0]->position.x << ", y = " << (int)players[0]->position.y;
	player_debug_2_ss << "orientation = " << (int)players[0]->orientation;
	time_debug_ss << "time passed (s): " << TIME_PASSED / 1000;
	log_debug_ss << "Log: " << level->log_to_screen;

	window_debug_str = window_debug_ss.str();
	fullscreen_debug_str = fullscreen_debug_ss.str();
	player_debug_str = player_debug_ss.str();
	player_debug_2_str = player_debug_2_ss.str();
	time_debug_str = time_debug_ss.str();
	log_debug_str = log_debug_ss.str();
}

void Text::update() {
}

void Text::render() {

	SDL_Surface* score_surface = TTF_RenderText_Solid(font_2, score_str.c_str(), color);
	SDL_Texture* score_texture = SDL_CreateTextureFromSurface(renderer, score_surface);

	SDL_Surface* window_debug_surface = TTF_RenderText_Solid(font_1, window_debug_str.c_str(), color);
	SDL_Surface* fullscreen_debug_surface = TTF_RenderText_Solid(font_1, fullscreen_debug_str.c_str(), color);
	SDL_Surface* player_debug_surface = TTF_RenderText_Solid(font_1, player_debug_str.c_str(), color);
	SDL_Surface* player_debug_2_surface = TTF_RenderText_Solid(font_1, player_debug_2_str.c_str(), color);
	SDL_Surface* time_debug_surface = TTF_RenderText_Solid(font_1,  time_debug_str.c_str(), color);
	SDL_Surface* log_debug_surface = TTF_RenderText_Solid(font_1, log_debug_str.c_str(), level->color);

	SDL_Texture* window_debug_texture = SDL_CreateTextureFromSurface(renderer, window_debug_surface);
	SDL_Texture* fullscreen_debug_texture = SDL_CreateTextureFromSurface(renderer, fullscreen_debug_surface);
	SDL_Texture* player_debug_texture = SDL_CreateTextureFromSurface(renderer, player_debug_surface);
	SDL_Texture* player_debug_2_texture = SDL_CreateTextureFromSurface(renderer, player_debug_2_surface);
	SDL_Texture* time_debug_texture = SDL_CreateTextureFromSurface(renderer, time_debug_surface);
	SDL_Texture* log_debug_texture = SDL_CreateTextureFromSurface(renderer, log_debug_surface);

	SDL_Rect dst;
	SDL_Rect src;
	src.x = 0; 					// sets src x position
	src.y = 0; 					// sets src y position
	src.w = dimensions.x; 		// sets src width
	src.h = dimensions.y; 		// sets src height
	dst.w = dimensions.x; 		// sets dst width
	dst.h = dimensions.y; 		// sets dst height

	render_text_copy(renderer, score_texture, src, dst, score_str.size() * 2, (init_window_width / 2) - (TILE_SIZE * 11), (init_window_height / 2) - (TILE_SIZE * 16) - 26);

	render_text_copy(renderer, window_debug_texture, src, dst, window_debug_str.size(), TILE_SIZE * 2, 100);
	render_text_copy(renderer, fullscreen_debug_texture, src, dst, fullscreen_debug_str.size(), TILE_SIZE * 2, 125);
	render_text_copy(renderer, player_debug_texture, src, dst, player_debug_str.size(), TILE_SIZE * 2, 150);
	render_text_copy(renderer, player_debug_2_texture, src, dst, player_debug_2_str.size(), TILE_SIZE * 2, 175);
	render_text_copy(renderer, time_debug_texture, src, dst, time_debug_str.size(), TILE_SIZE * 2, 200);
	render_text_copy(renderer, log_debug_texture, src, dst, log_debug_str.size(), TILE_SIZE * 2, 250);

}

void Text::render_text_copy(SDL_Renderer* &renderer, SDL_Texture* &texture, SDL_Rect &src, SDL_Rect &dst, CoUnInt size, CoUnInt x, CoUnInt y) {
	src.w = 7 * size;
	dst.w = 7 * size;
	dst.x = x;
	dst.y = y;
	SDL_RenderCopyEx(renderer, texture, &src, &dst, NULL, NULL, SDL_FLIP_NONE);

	
}