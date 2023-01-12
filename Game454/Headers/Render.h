#pragma once

#include <stdio.h>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>
#include <set>

#include "GameObjectsClasses.h"

using namespace std;


bool draw = false;
ALLEGRO_TIMER* FPStimer;
ALLEGRO_EVENT_QUEUE* timerQueue;

/*By screen i mean the target bitmap*/
void Paint_Player_to_Screen(Rect r);

void Load_Start_Screen();

void Render_init();

// use this to render grid (toggle on / off), used only for development time testing -
// a tile grid block is consecutive GRID_BLOCK_ROWS x GRID_BLOCK_COLUMNS block of grid indices
void DisplayGrid(unsigned int grid_num);

void Renderer();

/*might be useful to have this in the future, also add more stuff cuz i'll forget for sure*/
void Render_Clear();