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


extern bool draw;
extern ALLEGRO_EVENT_QUEUE* RenderTimerQueue;

/*By screen i mean the target bitmap*/
void Paint_Player_to_Screen(Rect r);

void Paint_Enemies_to_Screen();

void Paint_Collectables_to_Screen();

void Paint_Princess_to_Screen();

void Paint_Projectiles_to_Screen();

void Paint_UI_to_Screen();

void Paint_Paused_UI_to_Screen();

void Draw_Level(Level* level, unsigned int layer);

void Load_Start_Screen();

void Load_Loading_Screen();

void Load_Credits();

void Render_init();

// use this to render grid (toggle on / off), used only for development time testing -
// a tile grid block is consecutive GRID_BLOCK_ROWS x GRID_BLOCK_COLUMNS block of grid indices
void DisplayGrid(unsigned int grid_num);

void Renderer();

/*might be useful to have this in the future, also add more stuff cuz i'll forget for sure*/
void Render_Clear();