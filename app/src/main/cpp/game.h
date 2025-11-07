#pragma once
#include <iostream>
#include <vector>
#include <string>
#include <cmath>
#include <cstdlib>
#include <algorithm>
#include <map>
#include "debug.h"

#define NOMINMAX
#include <chrono>
#include <thread>
#include <sstream>
#include <limits>
#include <thread>
#include <unistd.h>

using namespace std;

#define DISPLAY_TYPE 1
#define POV 120

#if DISPLAY_TYPE == 1

    //#define CANVAS_WIDTH  96 // to big
    //#define CANVAS_HEIGHT 64 // to big

    #define CANVAS_WIDTH  (16 * 3)
    #define CANVAS_HEIGHT (9 * 3)

    //#define CANVAS_WIDTH  24
    //#define CANVAS_HEIGHT 16

#elif DISPLAY_TYPE == 2

    //#define CANVAS_WIDTH  20
    //#define CANVAS_HEIGHT 13

    #define CANVAS_WIDTH  18
    #define CANVAS_HEIGHT 12

    //#define CANVAS_WIDTH  12
    //#define CANVAS_HEIGHT 8

#endif

unsigned int getRandomInt(unsigned int min, unsigned int max);
void initGame();
void setupGame();
void movePlayerUserInput(int x, int y);
void movePlayerFastAuto(int x, int y);

/*
    check set or reset if key is pressed
    0 = up
    1 = down
    2 = left
    3 = right
    4 = next level
*/
extern bool keyStates[5];

/*
 * Flip to true, too go too next level
 */
extern bool nextLevel;

/*
 * If true then map is being read or written to
 */
extern bool mainMapLocked;

extern std::vector<int> mainMap;
extern std::vector<int> colors;
extern std::vector<int> secondMap;