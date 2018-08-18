// GRAPHICS.CPP

#include "graphics.h"

double get_current_time();

Sprite::Sprite() {
	// default sprite constructor
}

Sprite::Sprite(SDL_Renderer* game_renderer) {
	renderer = game_renderer;
	path = "assets/debug.png";
	texture = IMG_LoadTexture(renderer, path);
	position = { 0, 0 };
	dimensions = { TILE_SIZE, TILE_SIZE };
	velocity = { 0, 0 };

	collider.x = position.x;
	collider.y = position.y;
	collider.w = dimensions.x;
	collider.h = dimensions.y;
}

Sprite::Sprite(SDL_Renderer* game_renderer, char* file_path, CoDbl x, CoDbl y, CoDbl w, CoDbl h) {
	renderer = game_renderer;
	path = file_path;
	texture = IMG_LoadTexture(renderer, path);
	position = { x, y };
	dimensions = { w, h };
	src_dimensions = dimensions;
	velocity = { 0, 0 };

	collider.x = position.x;
	collider.y = position.y;
	collider.w = dimensions.x;
	collider.h = dimensions.y;
}

Sprite::Sprite(SDL_Renderer* game_renderer, char* file_path, CoDbl x, CoDbl y, CoDbl w, CoDbl h, CoDbl src_w, CoDbl src_h) {
	renderer = game_renderer;
	path = file_path;
	texture = IMG_LoadTexture(renderer, path);
	position = { x, y };
	dimensions = { w, h };
	src_dimensions = { src_w, src_h };
	velocity = { 0, 0 };

	collider.x = position.x;
	collider.y = position.y;
	collider.w = dimensions.x;
	collider.h = dimensions.y;
}

void Sprite::update() {
	// no update content
}

void Sprite::update(double &press_start_blink_time, bool &blink, CoUnInt PRESS_START_BLINK_DELAY) {
	if (get_current_time() > press_start_blink_time) {
		if (blink) {
			SDL_SetTextureAlphaMod(texture, 255);
			blink = false;
		} else {
			SDL_SetTextureAlphaMod(texture, 0);
			blink = true;
		}
		press_start_blink_time += PRESS_START_BLINK_DELAY;
	}
}

void Sprite::animate() {
	// no animation
}

void Sprite::render(const bool is_tile) {
	SDL_Rect dst;
	SDL_Rect src;
	dst.x = position.x; 								// sets dst x position
	dst.y = position.y; 								// sets dst y position
	dst.w = dimensions.x; 								// sets dst width
	dst.h = dimensions.y; 								// sets dst height
	
	src.x = cycle * SRC_TILE_SIZE;						// set sprite image cycle
	src.y = 0;
	src.w = SRC_TILE_SIZE;
	src.h = SRC_TILE_SIZE;

	if (is_tile) {
		SDL_RenderCopyEx(renderer, texture, &src, &dst, orientation, NULL, SDL_FLIP_NONE);
	} else {
		SDL_RenderCopyEx(renderer, texture, NULL, &dst, orientation, NULL, SDL_FLIP_NONE);
	}
}

bool Sprite::is_colliding(SDL_Rect &a, SDL_Rect &b) {
	bool collision = true;

	if (a.y + a.h <= b.y) {
		collision = false;
	}
	else if (a.y >= b.y + b.h)
	{
		collision = false;
	}
	else if (a.x + a.w <= b.x)
	{
		collision = false;
	}
	else if (a.x >= b.x + b.w)
	{
		collision = false;
	}

	return collision;
}

Wall::Wall(SDL_Renderer* game_renderer, CoDbl x, CoDbl y) {
	renderer = game_renderer;
	path = "assets/wall.png";
	texture = IMG_LoadTexture(renderer, path);
	position = { x, y };
	dimensions = { TILE_SIZE, TILE_SIZE };
	velocity = { 0, 0 };

	collider.x = position.x + 2;
	collider.y = position.y + 2;
	collider.w = dimensions.x - 4;
	collider.h = dimensions.y - 4;
}

Menu_item::Menu_item(SDL_Renderer* game_renderer, CoUnInt i, CoDbl x, CoDbl y, CoDbl w, CoDbl h, CoDbl src_x, CoDbl src_y) {
	renderer = game_renderer;
	path = "assets/pause_menu_text.png";
	texture = IMG_LoadTexture(renderer, path);
	index = i;
	if (index == 0) SDL_SetTextureAlphaMod(texture, 255); else SDL_SetTextureAlphaMod(texture, 100);
	position = { x, y };
	dimensions = { w, h };
	src_position = { src_x, src_y };
}

void Menu_item::render() {
	SDL_Rect dst;
	SDL_Rect src;
	dst.x = position.x; 								// sets dst x position
	dst.y = position.y; 								// sets dst y position
	dst.w = dimensions.x; 								// sets dst width
	dst.h = dimensions.y; 								// sets dst height

	src.x = src_position.x;								// sets src x position
	src.y = src_position.y;								// sets src y position
	src.w = 337;										// sets src width
	src.h = 42;											// sets src height
		
	SDL_RenderCopyEx(renderer, texture, &src, &dst, orientation, NULL, SDL_FLIP_NONE);
}

void Menu_item::update(CoUnInt menu_index) {
	if (active_menu_index != menu_index) {
		if (index == menu_index) {
			SDL_SetTextureAlphaMod(texture, 255);
		} else {
			SDL_SetTextureAlphaMod(texture, 100);
		}
		active_menu_index = menu_index;
	}
}

double get_current_time() {
	return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
}