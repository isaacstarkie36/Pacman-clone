// CONSTANTS.H

#pragma once
#include "SDL.h"
#include "SDL_image.h"
#include "SDL_mixer.h"
#include "SDL_ttf.h"
#include <iostream>
#include <cstdlib>
#include <fstream>
#include <sstream>
#include <array>
#include <vector>
#include <iterator>
#include <chrono>
using std::stringstream;
using std::ifstream;
using std::istream;
using std::array;
using std::vector;
using std::iterator;
using std::string;
using std::cout;
using std::endl;
using std::ofstream;
using std::fixed;

typedef const unsigned int CoUnInt;
typedef const double CoDbl;

CoUnInt WALL_CHECK_SENSITIVITY = 5;
CoUnInt TILE_SIZE = 26;
CoUnInt SRC_TILE_SIZE = 13;
const float GHOST_SPEED = 2.75;
const int DEAD_ZONE = 15000;

struct plot {
	double x;
	double y;
};