#pragma once

#include "GameObjectsClasses.h"

ALLEGRO_EVENT_QUEUE* EventQueue;
Game_State GameState;
Player *player = NULL;
bool User_input_done = false, Toggle_Grid = false;


void Init_Player(int PlayerX, int PlayerY);

void User_Input_init();

bool isDone();

/*Handles all user input*/
void UserInput(void);
