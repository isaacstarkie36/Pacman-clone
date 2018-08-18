// ENTITIES.H

#pragma once
#include "constants.h"
#include "graphics.h"
#include "entities.h"
#include "control.h"

class Dot : public Sprite {
public:
	Dot(SDL_Renderer*, const float, const float);
};

class Big_dot : public Sprite {
public:
	Big_dot(SDL_Renderer*, const float, const float);
};

class Ghost : public Sprite {
	unsigned int counter = 0;
	unsigned int cycle_x = 0;
	unsigned int cycle_y = 3;
public:
	bool moving = true;
	Ghost(SDL_Renderer*, const float, const float, CoUnInt);
	void update(vector<Wall*> &);
	void animate();
	void render();
};

class Player : public Sprite {
	Control* level;
	unsigned int counter = 0;
	bool cycle_direction = 1;
	unsigned int lives = 3;
	bool eat_toggle = 1;
public:
	CoUnInt boost_amount = 2;
	CoUnInt boost_time = 1000;
	CoUnInt cooldown_time = 6000;
	double speed;
	bool moving = true;
	bool boosting = false;
	bool dead = false;
	bool cooldown_active = false;
	int delayed_trajectory = -1;
	double boost_end;
	double cooldown;
	Player(SDL_Renderer*, Control &, CoUnInt, CoUnInt, const float, const float);
	void update(Control &, CoUnInt, CoUnInt, vector<Wall*> &, vector<Dot*> &, vector<Big_dot*> &, vector<Ghost*> &, vector<Mix_Chunk*> &);
	void animate();
};