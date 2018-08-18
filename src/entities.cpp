// ENTITIES.CPP

#include "constants.h"
#include "entities.h"

void update_position(plot &, plot, SDL_Rect &);
void wrap(Control &, CoUnInt, double &);
void set_velocity(plot &, unsigned int, unsigned int &);

Player::Player(SDL_Renderer* game_renderer, Control &control, CoUnInt width, CoUnInt height, const float x, const float y) {
	renderer = game_renderer;
	level = &control;
	path = "assets/pacman.png";
	texture = IMG_LoadTexture(renderer, path);
	position = { x, y };
	dimensions = { TILE_SIZE, TILE_SIZE };
	speed = 0.105 * TILE_SIZE;
	velocity = { -speed, 0 };
	orientation = 180;

	collider.x = position.x;
	collider.y = position.y;
	collider.w = dimensions.x;
	collider.h = dimensions.y;
}

void Player::update(Control &control, CoUnInt init_width, CoUnInt init_height, vector<Wall*> &walls, vector<Dot*> &dots, vector<Big_dot*> &big_dots, vector<Ghost*> &ghosts, vector<Mix_Chunk*> &sfx) {
	update_position(position, velocity, collider);
	wrap(*level, init_width, position.x);

	if (position.x > init_width / 2) {
		Mix_SetPosition(1, 90 * ((position.x - (init_width / 2)) / (init_height / 2)), 0);
	}
	else {
		Mix_SetPosition(1, 270 + (90 * (position.x / (init_width / 2))), 0);
	}

	for (int i = 0; i < walls.size(); i++) {
		if (is_colliding(collider, walls[i]->collider)) {
			position.x -= velocity.x;
			position.y -= velocity.y;
			collider.x = position.x;
			collider.y = position.y;
			moving = false;

			break;
		}
	}

	for (int i = 0; i < ghosts.size(); i++) {
		if (is_colliding(collider, ghosts[i]->collider)) {
			const double offset_x = (init_width / 2) - (TILE_SIZE * 14);
			const double offset_y = (init_height / 2) - (TILE_SIZE * 15.5);

			control.log("Player killed!");
			control.color = { 255, 255, 0 };
			Mix_PlayChannel(2, sfx[2], 0);
			SDL_Delay(1000);
			dead = true;
			moving = true;
			position = { (13.5 * TILE_SIZE) + offset_x, (23 * TILE_SIZE) + offset_y };
			ghosts[0]->position = { (12 * TILE_SIZE) + offset_x, (14 * TILE_SIZE) + offset_y };
			ghosts[1]->position = { (13.5 * TILE_SIZE) + offset_x, (14 * TILE_SIZE) + offset_y };
			ghosts[2]->position = { (15 * TILE_SIZE) + offset_x, (14 * TILE_SIZE) + offset_y };

			break;
		}
	}

	for (int i = 0; i < dots.size(); i++) {
		if (is_colliding(collider, dots[i]->collider)) {
			if (eat_toggle) {
				Mix_PlayChannel(1, sfx[0], 0);
				eat_toggle = 0;
			}
			else {
				Mix_PlayChannel(1, sfx[1], 0);
				eat_toggle = 1;
			}

			level->score += 10;
			delete dots[i];
			vector<Dot*>::iterator it = dots.begin() + i;
			dots.erase(it);

			break;
		}
	}
	for (int i = 0; i < big_dots.size(); i++) {
		if (is_colliding(collider, big_dots[i]->collider)) {
			if (eat_toggle) {
				Mix_PlayChannel(1, sfx[0], 0);
				eat_toggle = 0;
			}
			else {
				Mix_PlayChannel(1, sfx[1], 0);
				eat_toggle = 1;
			}
			level->score += 50;
			delete big_dots[i];
			vector<Big_dot*>::iterator it = big_dots.begin() + i;
			big_dots.erase(it);
			control.log("Big dot eaten");
			control.color = { 255, 255, 0 };

			break;
		}
	}

	if (delayed_trajectory != -1) {
		if (orientation != delayed_trajectory) {
			control.log_to_screen = "Direction changed";
			control.color = { 255, 255, 0 };
		}
		switch (delayed_trajectory) {
		case 180:
			collider.w += WALL_CHECK_SENSITIVITY;
			collider.x -= WALL_CHECK_SENSITIVITY;
			for (int i = 0; i < walls.size(); i++) {
				if (is_colliding(collider, walls[i]->collider)) {
					fake_collision = true;
					break;
				}
			}
			if (!fake_collision) {
				velocity = { -speed, 0 };
				orientation = 180;
				moving = true;
				delayed_trajectory = -1;
			}
			else {
				fake_collision = false;
			}
			collider.w -= WALL_CHECK_SENSITIVITY;
			collider.x += WALL_CHECK_SENSITIVITY;
			break;
		case 0:
			collider.w += WALL_CHECK_SENSITIVITY;
			for (int i = 0; i < walls.size(); i++) {
				if (is_colliding(collider, walls[i]->collider)) {
					fake_collision = true;
					break;
				}
			}
			if (!fake_collision) {
				velocity = { speed, 0 };
				orientation = 0;
				moving = true;
				delayed_trajectory = -1;
			}
			else {
				fake_collision = false;
			}
			collider.w -= WALL_CHECK_SENSITIVITY;
			break;
		case 270:
			collider.h += WALL_CHECK_SENSITIVITY;
			collider.y -= WALL_CHECK_SENSITIVITY;
			for (int i = 0; i < walls.size(); i++) {
				if (is_colliding(collider, walls[i]->collider)) {
					fake_collision = true;
					break;
				}
			}
			if (!fake_collision) {
				velocity = { 0, -speed };
				orientation = 270;
				moving = true;
				delayed_trajectory = -1;
			}
			else {
				fake_collision = false;
			}
			collider.h -= WALL_CHECK_SENSITIVITY;
			collider.y += WALL_CHECK_SENSITIVITY;
			break;
		case 90:
			collider.h += WALL_CHECK_SENSITIVITY;
			for (int i = 0; i < walls.size(); i++) {
				if (is_colliding(collider, walls[i]->collider)) {
					fake_collision = true;
					break;
				}
			}
			if (!fake_collision) {
				velocity = { 0, speed };
				orientation = 90;
				moving = true;
				delayed_trajectory = -1;
			}
			else {
				fake_collision = false;
			}
			collider.h -= WALL_CHECK_SENSITIVITY;
			break;
		}
	}
}

void Player::animate() {
	if (moving) {
		if (counter % 6 == 1) {								// for every 7 frames
			if (cycle_direction) {							// if pacman is opening its mouth
				if (cycle < 2) cycle++; else {				// and if it isn't fully open, go to next image
					cycle--;								// otherwise go to previous image
					cycle_direction = 0;					// also start closing it
				}
			}
			else {
				if (cycle) cycle--; else {					// if pacman is closing its mouth, go to previous game
					cycle++;								// otherwise go to next image
					cycle_direction = 1;					// also start opening it
				}
			}
		}
		counter++;
	}
	if (counter > 65500) {									// resetting the counter if it gets too big
		counter = 0;
	}
}

Dot::Dot(SDL_Renderer* game_renderer, const float x, const float y) {
	renderer = game_renderer;
	path = "assets/dot.png";
	texture = IMG_LoadTexture(renderer, path);
	position = { x, y };
	dimensions = { TILE_SIZE, TILE_SIZE };
	velocity = { 0, 0 };

	collider.x = position.x + 5;
	collider.y = position.y + 5;
	collider.w = dimensions.x - 10;
	collider.h = dimensions.y - 10;
}

Big_dot::Big_dot(SDL_Renderer* game_renderer, const float x, const float y) {
	renderer = game_renderer;
	path = "assets/big_dot.png";
	texture = IMG_LoadTexture(renderer, path);
	position = { x, y };
	dimensions = { TILE_SIZE, TILE_SIZE };
	velocity = { 0, 0 };

	collider.x = position.x + 3;
	collider.y = position.y + 3;
	collider.w = dimensions.x - 6;
	collider.h = dimensions.y - 6;
}

Ghost::Ghost(SDL_Renderer* game_renderer, const float x, const float y, CoUnInt colour) {
	renderer = game_renderer;
	switch (colour) {
	case 0:
		path = "assets/ghost_red.png";
		break;
	case 1:
		path = "assets/ghost_blue.png";
		break;
	case 2:
		path = "assets/ghost_green.png";
		break;
	}
	texture = IMG_LoadTexture(renderer, path);
	position = { x, y };
	dimensions = { TILE_SIZE, TILE_SIZE };
	velocity = { 0, -GHOST_SPEED };
	moving = true;

	collider.x = position.x;
	collider.y = position.y;
	collider.w = dimensions.x;
	collider.h = dimensions.y;
}

void Ghost::update(vector<Wall*> &walls) {
	position.x += velocity.x;
	position.y += velocity.y;
	collider.x = position.x;
	collider.y = position.y;

	for (int i = 0; i < walls.size(); i++) {
		if (is_colliding(collider, walls[i]->collider)) {
			position.x -= velocity.x;
			position.y -= velocity.y;
			collider.x = position.x;
			collider.y = position.y;

			set_velocity(velocity, floor(rand() % 4), cycle_y);
			break;
		}
	}
}

void Ghost::animate() {
	if (counter % 6 == 1) {								// for every 7 frames
		if (!cycle_x) { cycle_x = 1; }
		else { cycle_x = 0; }
	}
	counter++;
	if (counter > 65500) {								// resetting the counter if it gets too big
		counter = 0;
	}
}

void Ghost::render() {
	SDL_Rect dst;
	SDL_Rect src;
	dst.x = position.x; 								// sets dst x position
	dst.y = position.y; 								// sets dst y position
	dst.w = dimensions.x; 								// sets dst width
	dst.h = dimensions.y; 								// sets dst height

	src.x = cycle_x * SRC_TILE_SIZE;					// set sprite image cycle
	src.y = cycle_y * SRC_TILE_SIZE;
	src.w = SRC_TILE_SIZE;
	src.h = SRC_TILE_SIZE;

	SDL_RenderCopyEx(renderer, texture, &src, &dst, orientation, NULL, SDL_FLIP_NONE);
}

void update_position(plot &position, plot velocity, SDL_Rect &collider) {
	position.x += velocity.x;
	position.y += velocity.y;
	collider.x = position.x;
	collider.y = position.y;
}

void wrap(Control &level, CoUnInt init_width, double &x) {
	CoUnInt offset_x = (init_width / 2) - (TILE_SIZE * 14);
	CoUnInt map_size = (28 * TILE_SIZE);

	if (x < offset_x - TILE_SIZE) {
		x += map_size + TILE_SIZE;
		level.log("Player warped");
		level.color = { 255, 255, 0 };
	}
	else if (x > offset_x + map_size) {
		x -= map_size + TILE_SIZE;
		level.log("Player warped");
		level.color = { 255, 255, 0 };
	}
}

void set_velocity(plot &velocity, unsigned int direction, unsigned int &cycle_y) {
	switch (direction) {
	case 0:
		velocity = { GHOST_SPEED, 0 };
		cycle_y = direction;
		break;
	case 1:
		velocity = { 0, GHOST_SPEED };
		cycle_y = direction;
		break;
	case 2:
		velocity = { -GHOST_SPEED, 0 };
		cycle_y = direction;
		break;
	case 3:
		velocity = { 0, -GHOST_SPEED };
		cycle_y = direction;
		break;
	}
}