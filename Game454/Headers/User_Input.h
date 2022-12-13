#pragma once

#include "Event_Queue.h"

#define scrollDistanceX 5.0
#define scrollDistanceY 5.0

bool User_input_done = false;

void User_Input_init()
{
	if (!al_install_keyboard())
	{
		fprintf(stderr, "failed to install keyboard().\n");
		exit(-1);
	}
}

bool isDone()
{
	return User_input_done;
}

/*Checks for keyboard input and pushes "move" events in the event queue*/
void CheckScroll(ALLEGRO_KEYBOARD_STATE KbState, float scrollX, float scrollY)
{
	//assert(scrollDistanceX > 0 && scrollDistanceY > 0);		/*scrollDistance <= 0 doesn't make sense*/
	Event *e = new Event;

	e->ScrollDistanceX = 0;
	e->ScrollDistanceY = 0;
	al_get_keyboard_state(&KbState);
	if (al_key_down(&KbState, ALLEGRO_KEY_DOWN)) /*"if" is not connected with "else" so if played presses "up" and "down" nothing will happen*/
	{
		e->ScrollDistanceY += scrollY;
	}
	if (al_key_down(&KbState, ALLEGRO_KEY_UP))	
	{
		e->ScrollDistanceY -= scrollY;
	}
	if (al_key_down(&KbState, ALLEGRO_KEY_RIGHT))
	{
		e->ScrollDistanceX += scrollX;
	}
	if (al_key_down(&KbState, ALLEGRO_KEY_LEFT))
	{
		e->ScrollDistanceX -= scrollX;
	}

	
	if (e->ScrollDistanceX == 0 && e->ScrollDistanceY == 0)	//if no input no need to fill the queue with zeros
	{
		delete e;	//delete calls the destructor while free() does not
		return;
	}
	EventQueue.push(*e);
}

/*Handles all user input*/
void UserInput(void)
{
	ALLEGRO_KEYBOARD_STATE KbState;

	al_get_keyboard_state(&KbState);
	if (al_key_down(&KbState, ALLEGRO_KEY_ESCAPE))
	{
		User_input_done = true;
	}
	if (al_key_down(&KbState, ALLEGRO_KEY_ENTER))	//Later i might need a Finite State Machine to know whats going on in the game
	{
		Event* e = new Event;
		e->eventType = EventType_Action;
		EventQueue.push(*e);
	}
	CheckScroll(KbState, scrollDistanceX, scrollDistanceY);
}
