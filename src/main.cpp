// MAIN.CPP

// Including files containing classes, constant values and library include/using directives
#include "constants.h"
#include "graphics.h"
#include "entities.h"
#include "text.h"
#include "control.h"

// Declaring functions
void process_input(bool &, bool&, int &, int &);
void process_input(bool &);
void process_input(bool&, bool &, unsigned int &);
void update(CoUnInt, CoUnInt, CoUnInt, CoUnInt, CoUnInt, CoUnInt, bool &, bool &, bool &, bool &, vector<Player*> &, vector<Wall*> &, vector<Dot*> &, vector<Big_dot*> &, vector<Ghost*> &, vector<Mix_Chunk*> &, vector<Menu_item> &, Text &);
void update(Sprite &, double &, bool &, CoUnInt);
void update(vector<Menu_item> &, unsigned int &, bool &);
void render(const bool, vector<Player*> &, vector<Wall*> &, vector <Dot*> &, vector<Big_dot*> &, vector<Ghost*> &, Text &, Sprite &, Sprite &);
void render(Sprite &, Text &);
void render(vector<Menu_item> &);
void generate(CoUnInt, CoUnInt, vector<Player*> &, vector<Wall*> &, vector<Dot*> &, vector<Big_dot*> &, vector<Ghost*> &, vector<Menu_item> &);
void handle_movement(vector<Player*> &, vector<Wall*> &, const bool, const bool, const bool, const bool, CoDbl, CoDbl, CoUnInt);
void show_title_screen(CoUnInt, CoUnInt, const bool);
void show_pause_screen(bool&, vector<Menu_item> &);
double current_time();
double current_time_precise();
int initialise();
void close_SDL();

// Declaring global objects
SDL_Window* window = NULL;
SDL_Renderer* renderer = NULL;
SDL_GameController* ctrl_player_1 = NULL;
Mix_Music* music = NULL;
SDL_DisplayMode display;
SDL_Event event;
Control control;

// declaring key map
std::array<bool, 7> buttons;

int main(int argc, char* argv[]) {

	if (initialise()) {
		close_SDL(); return 1;
	}

	// building key map
	buttons[0]		= false;   // left
	buttons[1]		= false;   // down
	buttons[2]		= false;   // right
	buttons[3]		= false;   // up
	buttons[4]		= false;   // boost
	buttons[5]		= false;   // esc
	buttons[6]		= false;   // fullscreen

	// declaring a file output stream to save time interval logs
	ofstream real_time_log("real_time_log.txt");

	SDL_assert(!SDL_GetCurrentDisplayMode(0, &display));
	SDL_assert(!SDL_CreateWindowAndRenderer(display.w / 2, display.h / 2, SDL_WINDOW_RESIZABLE, &window, &renderer));
	control.log("Created window and renderer");
	int width, height;
	SDL_GetWindowSize(window, &width, &height);

	// CoUnInt is a user defined type equal to "constant unsigned int"
	SDL_SetWindowTitle(window, "Isaac Starkie - CGP2011M Assignment 1 - 14536235");
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
	bool running = true, fullscreen = false, paused = true, pause_menu = false;

	event.type = NULL;
	control.log("Event readied");
	// Setting console output format to fixed 
	real_time_log << fixed;

	show_title_screen(width, height, fullscreen);

	SDL_SetWindowSize(window, display.w, display.h);
	SDL_SetWindowPosition(window, 0, 0);
	SDL_GetWindowSize(window, &width, &height);
	CoUnInt init_width = width;
	CoUnInt init_height = height;
	CoDbl GAME_START_DELAY = SDL_GetTicks() + 4250;
	CoDbl GAME_START_TIME = SDL_GetTicks();

	vector<Player*> players;
	vector<Wall*> walls;
	vector<Dot*> dots;
	vector<Big_dot*> big_dots;
	vector<Mix_Chunk*> sfx;
	vector<Ghost*> ghosts;
	vector<Menu_item> menu_items;
	Sprite Black_bar_1(renderer, "assets/black_bar.png", (init_width / 2) - (TILE_SIZE * 15), (init_height / 2) - (TILE_SIZE * 2.5), TILE_SIZE, TILE_SIZE * 3);
	Sprite Black_bar_2(renderer, "assets/black_bar.png", (init_width / 2) + (TILE_SIZE * 14), (init_height / 2) - (TILE_SIZE * 2.5), TILE_SIZE, TILE_SIZE * 3);
	Text Text_1(renderer, control, init_width, init_height, "assets/arial.ttf", "assets/emulogic.ttf");

	generate(width, height, players, walls, dots, big_dots, ghosts, menu_items);
	
	music = Mix_LoadMUS("assets/theme.wav");
	if (Mix_PlayMusic(music, 0) == -1) {
		control.log("Music could not be loaded");
		control.color = { 255, 0, 0 };
	}

	sfx.emplace_back(new Mix_Chunk);
	sfx[0] = Mix_LoadWAV("assets/eat_1.wav");
	sfx.emplace_back(new Mix_Chunk);
	sfx[1] = Mix_LoadWAV("assets/eat_2.wav");
	sfx.emplace_back(new Mix_Chunk);
	sfx[2] = Mix_LoadWAV("assets/dead.wav");
	sfx.emplace_back(new Mix_Chunk);
	sfx[3] = Mix_LoadWAV("assets/winner.wav");
	sfx.emplace_back(new Mix_Chunk);
	sfx[4] = Mix_LoadWAV("assets/boost_start.wav");
	sfx.emplace_back(new Mix_Chunk);
	sfx[5] = Mix_LoadWAV("assets/boost_end.wav");
	sfx.emplace_back(new Mix_Chunk);
	sfx[6] = Mix_LoadWAV("assets/cooldown_end.wav");
	for (int i = 0; i < sfx.size(); i++) {
		if (sfx[i] == NULL) {
			control.log("Sound effects could not be loaded");
			control.color = { 255, 0, 0 };
			break;
		}
	}
	Mix_VolumeChunk(sfx[0], 96);
	Mix_VolumeChunk(sfx[1], 96);

	for (int i = 0; i < players.size(); i++) {
		players[i]->cooldown = current_time();
	}

	CoUnInt STEP_DELAY = 1000 / 60;			// truncates to 16 milliseconds
	CoUnInt MAX_RENDER_DELAY = 1000 / 10;
	CoUnInt DELAY_DIFF = 32;
	CoUnInt SPEED_CHECK_DELAY = 3000;
	CoUnInt MAX_LOOPS = 10;
	unsigned int render_delay = STEP_DELAY;
	double next_step_time = current_time();
	double next_render_time = current_time();
	double next_speed_check = current_time() + SPEED_CHECK_DELAY;
	double loop_time, section_time;

	control.log("Game loop initiated");
	control.color = { 255, 255, 255 };
	while (running)
	{
		////////////////////////////////////////////////////////////////
		loop_time = current_time();

		if (pause_menu) {
			next_step_time = current_time();
			next_render_time = current_time();
			next_speed_check = current_time() + SPEED_CHECK_DELAY;
			pause_menu = false;
		}
		for (int i = 0; i < players.size(); i++) {
			if (players[i]->dead) {
				next_step_time = current_time();
				next_render_time = current_time();
				next_speed_check = current_time() + SPEED_CHECK_DELAY;
				players[i]->dead = false;
			}
		}

		section_time = current_time_precise();

		process_input(running, paused, width, height);

		section_time = current_time_precise() - section_time;

		if (current_time() > next_step_time) {
			real_time_log << "Time taken for process_input(): " << section_time << endl;

			section_time = current_time_precise();

			update(	init_width, init_height, init_width, init_height, GAME_START_DELAY, GAME_START_TIME, fullscreen,
					running, paused, pause_menu, players, walls, dots, big_dots, ghosts, sfx, menu_items, Text_1);

			section_time = current_time_precise() - section_time;
			real_time_log << "Time taken for update(): " << section_time << endl;
			next_step_time += STEP_DELAY;
		}

		if (current_time() > next_render_time) {

			section_time = current_time_precise();

			render(paused, players, walls, dots, big_dots, ghosts, Text_1, Black_bar_1, Black_bar_2);

			section_time = current_time_precise() - section_time;
			real_time_log << "Time taken for render(): " << section_time << endl;

			next_render_time += render_delay;
		}

		loop_time = current_time() - loop_time;

		if (current_time() > next_speed_check) {
			if (abs(loop_time - render_delay) > DELAY_DIFF && loop_time							// this is a routine check to see how fast the game is running.
				> STEP_DELAY && loop_time < MAX_RENDER_DELAY) {									// if the game is unable to reach the ideal framerate,
				render_delay = loop_time;														// and if the time taken by the game loop is significantly different from the current framerate,
				control.log_to_screen = "New frames per second: " + (100 / render_delay);		// a new regulated framerate is set based on how long the game loop took
				control.color = { 0, 0, 255 };
			}
			next_speed_check = current_time() + SPEED_CHECK_DELAY;
		}

		if (current_time() < next_step_time && current_time() < next_render_time) {
			SDL_Delay(1);															// this prevents busy-waiting
		}
		////////////////////////////////////////////////////////////////
	}

	control.log("Game loop terminated");
	control.color = { 0, 0, 255 };

	// Freeing dynamically created elements of each array
	for (int i = 0; i < players.size(); i++) {
		delete players[i];
	};
	for (int i = 0; i < walls.size(); i++) {
		delete walls[i];
	};
	for (int i = 0; i < dots.size(); i++) {
		delete dots[i];
	};
	for (int i = 0; i < big_dots.size(); i++) {
		delete big_dots[i];
	};
	for (int i = 0; i < ghosts.size(); i++) {
		delete ghosts[i];
	};
	for (int i = 0; i < sfx.size(); i++) {
		Mix_FreeChunk(sfx[i]);
	};
	control.log("Game memory freed");
	control.color = { 0, 0, 255 };
	
	close_SDL();
	return 0;

}

// This is the process_input() overload used in-game
void process_input(bool &running,
					bool &paused,
					int &width,
					int &height)
{
		SDL_PollEvent(&event);
		if (!paused) {
			switch (event.type) {

			case SDL_QUIT:
				running = 0;
				break;

			case SDL_WINDOWEVENT:
				// handling window resize events
				if (event.window.event == SDL_WINDOWEVENT_RESIZED) {
					SDL_GetWindowSize(window, &width, &height);
				}
				break;
			case SDL_CONTROLLERBUTTONDOWN:
				// handling controller buttons
				if (SDL_NumJoysticks() > 0) {
					switch (event.cbutton.button) {
					case 13:
						buttons[0] = true;
						break;
					case 14:
						buttons[2] = true;
						break;
					case 11:
						buttons[3] = true;
						break;
					case 12:
						buttons[1] = true;
						break;
					case 6:
						buttons[5] = true;
						break;
					case 0:
					case 1:
						buttons[4] = true;
						break;
					}
				}
				break;

			case SDL_CONTROLLERAXISMOTION:
				// handling thumbstick movement
				if (event.jaxis.value < -DEAD_ZONE) {
					if (event.jaxis.axis == 0)
					{
						buttons[0] = true;
					}

					if (event.jaxis.axis == 1)
					{
						buttons[3] = true;
					}
				}
				else if (event.jaxis.value > DEAD_ZONE) {
					if (event.jaxis.axis == 0)
					{
						buttons[2] = true;
					}

					if (event.jaxis.axis == 1)
					{
						buttons[1] = true;
					}
				}
				break;
			}
		if (event.type == SDL_KEYDOWN && event.key.repeat == 0) {
			// detecting control keys, fullscreen toggle key, exit game key, boost key
			switch (event.key.keysym.sym) {
			case SDLK_LEFT:
			case SDLK_a:
				buttons[0] = true;
				break;

			case SDLK_RIGHT:
			case SDLK_d:
				buttons[2] = true;
				break;

			case SDLK_UP:
			case SDLK_w:
				buttons[3] = true;
				break;

			case SDLK_DOWN:
			case SDLK_s:
				buttons[1] = true;
				break;

			case SDLK_SPACE:
				buttons[4] = true;
				break;

			case SDLK_ESCAPE:
				buttons[5] = true;
				break;

			case SDLK_f:
				buttons[6] = true;
				break;
			}
		}	
	}
}

// This is the process_input() overload used by the title screen
void process_input(bool &running) {
	SDL_PollEvent(&event);

	// handling start game keys
	switch (event.type) {
	case SDL_KEYDOWN:
		switch (event.key.keysym.sym) {
		case SDLK_RETURN:
			running = 0;
			break;
		}
		break;

	case SDL_CONTROLLERBUTTONDOWN:
		if (event.cbutton.button == 6) {
			running = 0;
		}
		break;
	}
}

// This is the process_input() overload used by the pause screen
void process_input(bool &running, bool &menu_running, unsigned int &menu_index) {
	if (SDL_PollEvent(&event)) {
		switch (event.type) {
		case SDL_KEYDOWN:
			// getting keyboard menu control keys
			switch (event.key.keysym.sym) {
			case SDLK_UP:
			case SDLK_w:
				buttons[3] = true;
				break;

			case SDLK_DOWN:
			case SDLK_s:
				buttons[1] = true;
				break;

			case SDLK_RETURN:
				switch (menu_index) {
				case 0:
					menu_running = false;
					break;
				case 1:
					break;
				case 2:
					menu_running = false;
					running = false;
					break;
				}
				break;
			}
			break;
		case SDL_CONTROLLERBUTTONDOWN:
			// handling controller menu control keys
			if (SDL_NumJoysticks() > 0) {
				switch (event.cbutton.button) {
				case 11:
					buttons[3] = true;
					break;
				case 12:
					buttons[1] = true;
					break;
				case 0:
					switch (menu_index) {
					case 0:
						menu_running = false;
						break;
					case 1:
						break;
					case 2:
						menu_running = false;
						running = false;
						break;
					}
					break;
				}
			}
			break;
		}
	}
}

// This is the update() overload used in-game
void update(CoUnInt width,
			CoUnInt height,
			CoUnInt init_width,
			CoUnInt init_height,
			CoUnInt GAME_START_DELAY,
			CoUnInt GAME_START_TIME,
			bool &fullscreen,
			bool &running,
			bool &paused,
			bool &pause_menu,
			vector<Player*> &players,
			vector<Wall*> &walls,
			vector<Dot*> &dots,
			vector<Big_dot*> &big_dots,
			vector<Ghost*> &ghosts,
			vector<Mix_Chunk*> &sfx,
			vector<Menu_item> &menu_items,
			Text &Text_1)
{
	// ends a delay at the start of the game so that the player can assume control
	if (paused) {
		if (SDL_GetTicks() > GAME_START_DELAY) {
			paused = false;
		}
	} else {
		// handling keyboard input
		if (buttons[0]) {
			handle_movement(players, walls, true, false, true, false, -players[0]->speed, 0, 180);
			buttons[0] = false;
		}
		if (buttons[2]) {
			handle_movement(players, walls, true, false, false, false, players[0]->speed, 0, 0);
			buttons[2] = false;
		}
		if (buttons[3]) {
			handle_movement(players, walls, false, true, false, true, 0, -players[0]->speed, 270);
			buttons[3] = false;
		}
		if (buttons[1]) {
			handle_movement(players, walls, false, true, false, false, 0, players[0]->speed, 90);
			buttons[1] = false;
		}
		if (buttons[4]) {
			if (!players[0]->boosting) {
				if (current_time() > players[0]->cooldown) {
					control.log("Boosting");
					control.color = { 255, 255, 0 };
					players[0]->speed += players[0]->boost_amount;
					players[0]->boost_end = current_time() + players[0]->boost_time;
					switch (players[0]->orientation) {
					case 0:
						players[0]->velocity = { players[0]->speed, 0 };
						break;
					case 90:
						players[0]->velocity = { 0, players[0]->speed };
						break;
					case 180:
						players[0]->velocity = { -players[0]->speed, 0 };
						break;
					case 270:
						players[0]->velocity = { 0, -players[0]->speed };
						break;
					}
					Mix_PlayChannel(2, sfx[4], 0);
					players[0]->boosting = true;
				}
			}
			buttons[4] = false;
		}
		if (buttons[5]) {
			buttons[5] = false;
			show_pause_screen(running, menu_items);
			pause_menu = true;
		}
		if (buttons[6]) {
			if (!fullscreen) {
				SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN);
				control.log("Fullscreen on");
				control.color = { 0, 0, 255 };
				SDL_ShowCursor(SDL_DISABLE);
				fullscreen = true;
			}
			else {
				SDL_SetWindowFullscreen(window, 0);
				SDL_SetWindowSize(window, display.w, display.h);
				control.log("Fullscreen off");
				control.color = { 0, 0, 255 };
				SDL_ShowCursor(SDL_ENABLE);
				fullscreen = false;
			}
			buttons[6] = false;
		}

		for (int i = 0; i < players.size(); i++) {
			players[i]->update(control, init_width, init_height, walls, dots, big_dots, ghosts, sfx);
			players[i]->animate();

			if (players[i]->cooldown_active && current_time() > players[i]->cooldown) {
				control.log("Boost cooldown expired");
				control.color = { 255, 255, 0 };
				Mix_PlayChannel(2, sfx[6], 0);
				players[i]->cooldown_active = false;
			}

			if (players[i]->boosting && current_time() > players[i]->boost_end) {
				control.log("Boost depleted");
				control.color = { 255, 255, 0 };
				players[i]->speed -= players[i]->boost_amount;
				switch (players[0]->orientation) {
				case 0:
					players[0]->velocity = { players[0]->speed, 0 };
					break;
				case 90:
					players[0]->velocity = { 0, players[0]->speed };
					break;
				case 180:
					players[0]->velocity = { -players[0]->speed, 0 };
					break;
				case 270:
					players[0]->velocity = { 0, -players[0]->speed };
					break;
				}
				players[i]->cooldown = current_time() + players[i]->cooldown_time;
				Mix_PlayChannel(2, sfx[5], 0);
				players[i]->boosting = false;
				players[i]->cooldown_active = true;
			}
		}
		for (int i = 0; i < ghosts.size(); i++) {
			ghosts[i]->update(walls);
			ghosts[i]->animate();
		}
	}

	Text_1.update(fullscreen, players, width, height, SDL_GetTicks() - GAME_START_TIME);

	if (!dots.size() && !big_dots.size()) {
		control.log("Level complete");
		control.color = { 0, 255, 0 };
		Mix_PlayChannel(2, sfx[3], 0);
		SDL_Delay(1000);
		running = false;
	}
}

// This is the update() overload used by the title screen
void update(Sprite &Press_start,
			double &press_start_blink_time,
			bool &blink,
			CoUnInt PRESS_START_BLINK_DELAY)
{
	Press_start.update(press_start_blink_time, blink, PRESS_START_BLINK_DELAY);
}

// This is the update() overload used by the pause screen
void update(vector<Menu_item> &menu_items,
			unsigned int &menu_index, bool &menu_running)
{
	// handling keyboard/controller input
	if (buttons[3]) {
		if (menu_index > 0) {
			menu_index--;
		}
		else {
			menu_index = 2;
		}
		buttons[3] = false;
	}
	if (buttons[1]) {
		if (menu_index < 2) {
			menu_index++;
		}
		else {
			menu_index = 0;
		}
		buttons[1] = false;
	}

	for (int i = 0; i < menu_items.size(); i++) {
		menu_items[i].update(menu_index);
	}
}

// This is the render() overload used in-game
void render(const bool paused,
			vector<Player*> &players,
			vector<Wall*> &walls,
			vector<Dot*> &dots,
			vector<Big_dot*> &big_dots,
			vector<Ghost*> &ghosts,
			Text &Text_1,
			Sprite &Black_bar_1,
			Sprite &Black_bar_2)
{
	SDL_RenderClear(renderer);

	for (int i = 0; i < players.size(); i++) {
		players[i]->render(true);
	}
	for (int i = 0; i < walls.size(); i++) {
		walls[i]->render(true);
	}
	for (int i = 0; i < dots.size(); i++) {
		dots[i]->render(true);
	}
	for (int i = 0; i < big_dots.size(); i++) {
		big_dots[i]->render(true);
	}
	for (int i = 0; i < ghosts.size(); i++) {
		ghosts[i]->render();
	}
	Text_1.render();
	Black_bar_1.render(false);
	Black_bar_2.render(false);

	SDL_RenderPresent(renderer);
}

// This is the render() overload used by the title screen
void render(	Sprite &Title,
				Sprite &Press_start)
{
	SDL_RenderClear(renderer);

	Title.render(false);
	Press_start.render(false);

	SDL_RenderPresent(renderer);
}

// This is the render() overload used by the pause screen
void render(vector<Menu_item> &menu_items)
{
	SDL_RenderClear(renderer);

	for (int i = 0; i < menu_items.size(); i++) {
		menu_items[i].render();
	}

	SDL_RenderPresent(renderer);
}

// This generates the map objects - the walls and dots are generated using a map data array
void generate(	CoUnInt width,
				CoUnInt height,
				vector<Player*> &players,
				vector<Wall*> &walls,
				vector<Dot*> &dots,
				vector<Big_dot*> &big_dots,
				vector<Ghost*> &ghosts,
				vector<Menu_item> &menu_items)
{
	// setting the offset of the game map in relation to the window size
	CoUnInt offset_x = (width / 2) - (TILE_SIZE * 14);
	CoUnInt offset_y = (height / 2) - (TILE_SIZE * 15.5);

	// generating uncommon game elements
	players.emplace_back(new Player(renderer, control, width, height, (13.5 * TILE_SIZE) + offset_x, (23 * TILE_SIZE) + offset_y));
	ghosts.emplace_back(new Ghost(renderer, (12 * TILE_SIZE) + offset_x, (14 * TILE_SIZE) + offset_y, 0));
	ghosts.emplace_back(new Ghost(renderer, (13.5 * TILE_SIZE) + offset_x, (14 * TILE_SIZE) + offset_y, 1));
	ghosts.emplace_back(new Ghost(renderer, (15 * TILE_SIZE) + offset_x, (14 * TILE_SIZE) + offset_y, 2));

	// generating common game elements
	for (int i = 0; i < control.map_data.size(); i++) {
		for (int j = 0; j < control.map_data[i].size(); j++) {
			switch (control.map_data[i][j]) {
			case 1:
				walls.emplace_back(new Wall(renderer, (j * TILE_SIZE) + offset_x, (i * TILE_SIZE) + offset_y));
				break;
			case 2:
				dots.emplace_back(new Dot(renderer, (j * TILE_SIZE) + offset_x, (i * TILE_SIZE) + offset_y));
				break;
			case 3:
				big_dots.emplace_back(new Big_dot(renderer, (j * TILE_SIZE) + offset_x, (i * TILE_SIZE) + offset_y));
				break;
			}
		}
	}

	// generating pause menu items
	menu_items.emplace_back(Menu_item(renderer, 0, (width / 2) - (TILE_SIZE * 5), (height / 2) - TILE_SIZE, TILE_SIZE * 10, TILE_SIZE * 2, 0, 0));
	menu_items.emplace_back(Menu_item(renderer, 1, (width / 2) - (TILE_SIZE * 5), (height / 2) + (TILE_SIZE * 3), TILE_SIZE * 10, TILE_SIZE * 2, 0, 42));
	menu_items.emplace_back(Menu_item(renderer, 2, (width / 2) - (TILE_SIZE * 5), (height / 2) + (TILE_SIZE * 7), TILE_SIZE * 10, TILE_SIZE * 2, 0, 84));

	control.log("Level generated");
	control.color = { 255, 255, 255 };
}

// This handles collisions, sets the players' direction of movement and delays movements if a wall is obstructing the player
void handle_movement(	vector<Player*> &players,
						vector<Wall*> &walls,
						const bool w_plus,
						const bool h_plus,
						const bool x_minus,
						const bool y_minus,
						CoDbl velX,
						CoDbl velY,
						CoUnInt _orientation)
{
	if (w_plus) players[0]->collider.w += WALL_CHECK_SENSITIVITY;
	if (h_plus) players[0]->collider.h += WALL_CHECK_SENSITIVITY;
	if (x_minus) players[0]->collider.x -= WALL_CHECK_SENSITIVITY;
	if (y_minus) players[0]->collider.y -= WALL_CHECK_SENSITIVITY;
	for (int i = 0; i < walls.size(); i++) {
		if (players[0]->is_colliding(players[0]->collider, walls[i]->collider)) {
			players[0]->fake_collision = true;
			players[0]->delayed_trajectory = _orientation;
			break;
		}
	}
	if (!players[0]->fake_collision) {
		if (players[0]->orientation != _orientation) control.log_to_screen = "Direction changed";
		control.color = { 255, 255, 0 };

		players[0]->velocity = { velX, velY };
		players[0]->orientation = _orientation;
		players[0]->moving = true;
		players[0]->delayed_trajectory = -1;
	}
	else {
		players[0]->fake_collision = false;
	}
	if (w_plus) players[0]->collider.w -= WALL_CHECK_SENSITIVITY;
	if (h_plus) players[0]->collider.h -= WALL_CHECK_SENSITIVITY;
	if (x_minus) players[0]->collider.x += WALL_CHECK_SENSITIVITY;
	if (y_minus) players[0]->collider.y += WALL_CHECK_SENSITIVITY;
}

// Title screen real-time loop
void show_title_screen(CoUnInt init_width, CoUnInt init_height, const bool fullscreen) {

	Sprite Title(renderer, "assets/title.png", 0, 0, init_width, init_height, 1600, 900);
	Sprite Press_start(renderer, "assets/press_start.png", (init_width / 2) - (TILE_SIZE * 5), (init_height / 2) + (TILE_SIZE * 6), TILE_SIZE * 10, TILE_SIZE * 1);

	bool running = true;
	CoUnInt STEP_DELAY = 1000 / 60;
	double next_step_time = current_time();
	CoUnInt PRESS_START_BLINK_DELAY = 700;
	bool blink = false;
	double press_start_blink_time = current_time() + 500;

	while (running)
	{
		////////////////////////////////////////////////////////////////
		if (current_time() > next_step_time) {
			process_input(running);
			update(Press_start, press_start_blink_time, blink, PRESS_START_BLINK_DELAY);
			render(Title, Press_start);
			next_step_time += STEP_DELAY;
		} else {
			SDL_Delay(1);
		}
		////////////////////////////////////////////////////////////////
	}

}

void show_pause_screen(bool &running, vector<Menu_item> &menu_items) {

	bool menu_running = true;
	unsigned int menu_index = 0;
	CoUnInt STEP_DELAY = 1000 / 60;
	double next_step_time = current_time();

	while (menu_running)
	{
		////////////////////////////////////////////////////////////////
		if (current_time() > next_step_time) {
			process_input(running, menu_running, menu_index);
			update(menu_items, menu_index, menu_running);
			render(menu_items);
			next_step_time += STEP_DELAY;
		}
		else {
			SDL_Delay(1);
		}
		////////////////////////////////////////////////////////////////
	}

}

// Gets the current time in milliseconds
double current_time() {
	return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
}

// Same as above but the return value has nanosecond precision
double current_time_precise() {
	return std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::system_clock::now().time_since_epoch()).count() / 1000000.0;
}

int initialise() {
	if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
		SDL_LogCritical(SDL_LOG_CATEGORY_APPLICATION, "SDL_Init Error: %s\n", SDL_GetError());
		return 1;
	}
	control.log("SDL initialised OK!");

	int imgFlags = IMG_INIT_PNG;
	if (!(IMG_Init(imgFlags) & imgFlags)) {
		SDL_LogCritical(SDL_LOG_CATEGORY_APPLICATION,
			"SDL_image init error: %s\n", IMG_GetError());
		return 1;
	}
	control.log("SDL_image initialised OK!");

	if (TTF_Init() == -1) {
		return 1;
	}
	control.log("SDL_ttf initialised OK!");

	if (Mix_OpenAudio(22050, MIX_DEFAULT_FORMAT, 2, 1024) == -1) {
		return 1;
	}
	control.log("SDL_mixer initialised OK!");

	if (SDL_NumJoysticks() < 1)
	{
		control.log("No controllers detected");
		return 0;
	}
	ctrl_player_1 = SDL_GameControllerOpen(0);
	if (ctrl_player_1 == NULL) {
		control.log("Error opening game controller");
	}
	else {
		control.log("Controller(s) detected");
	}
	
	return 0;
}

void close_SDL() {
	control.log("Closing SDL");
	control.color = { 255, 255, 255 };
	Mix_HaltMusic();
	Mix_FreeMusic(music);
	SDL_GameControllerClose(ctrl_player_1);
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);

	music = NULL;
	ctrl_player_1 = NULL;
	renderer = NULL;
	window = NULL;
	
	Mix_CloseAudio();
	TTF_Quit();
	IMG_Quit();
	SDL_Quit();
}