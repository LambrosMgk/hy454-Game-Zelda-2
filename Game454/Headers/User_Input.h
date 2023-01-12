#pragma once
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>
#include <set>

#include "al_init.h"
#include "GameObjectsClasses.h"

ALLEGRO_EVENT_QUEUE* EventQueue;
Game_State GameState;
bool User_input_done = false, Toggle_Grid = false;
bool keyboardUp = false, scrollDown = true, scrollLeft = false, scrollRight = false;	//omit these later, maybe not left and right? useful for animation?
int cameraX = 0, cameraY = 0;

void User_Input_init();

bool isDone();

/*Handles all user input*/
void UserInput(void);
