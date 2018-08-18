// GRAPHICS.H

#pragma once
#include "constants.h"

class Sprite {
protected:
	int id = NULL;
	char* path;
	SDL_Renderer* renderer;
	SDL_Texture* texture;
	plot dimensions;
	plot src_dimensions;
	unsigned int cycle = 0;
public:
	SDL_Rect collider;
	plot init_position;
	plot position;
	plot velocity;
	bool moving = false;
	bool fake_collision = false;
	int orientation = 0;
	Sprite();
	Sprite(SDL_Renderer*);
	Sprite(SDL_Renderer*, char*, CoDbl, CoDbl, CoDbl, CoDbl);
	Sprite(SDL_Renderer*, char*, CoDbl, CoDbl, CoDbl, CoDbl, CoDbl, CoDbl);
	virtual void update();
	virtual void update(double &, bool&, CoUnInt);
	virtual void animate();
	virtual void render(const bool);
	bool is_colliding(SDL_Rect &, SDL_Rect &);
};

class Wall : public Sprite {
public:
	Wall(SDL_Renderer*, CoDbl, CoDbl);
};

class Menu_item : public Sprite {
	plot src_position;
	unsigned int index;
	unsigned int active_menu_index = 0;
public:
	Menu_item(SDL_Renderer*, CoUnInt, CoDbl, CoDbl, CoDbl, CoDbl, CoDbl, CoDbl);
	void update(CoUnInt);
	void render();
};