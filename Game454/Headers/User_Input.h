#pragma once

#include "GameObjectsClasses.h"

extern ALLEGRO_EVENT_QUEUE* EventQueue;

void User_Input_init();

bool isDone();

/*Handles all user input*/
void UserInput(void);
