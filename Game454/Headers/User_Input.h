#pragma once

ALLEGRO_EVENT_QUEUE *EventQueue;
bool User_input_done = false;


void User_Input_init()
{
	if (!al_install_keyboard())
	{
		fprintf(stderr, "failed to install keyboard().\n");
		exit(-1);
	}

	EventQueue = al_create_event_queue();
	al_register_event_source(EventQueue, al_get_keyboard_event_source());
}

bool isDone()
{
	return User_input_done;
}

/*Checks for keyboard input and pushes "move" events in the event queue*/
/*void CheckScroll(ALLEGRO_KEYBOARD_STATE KbState, float scrollX, float scrollY)
{
	//assert(scrollDistanceX > 0 && scrollDistanceY > 0);
	Event *e = new Event;

	if (e == NULL)
	{
		cout << "Failed to allocate memory for Event object.\n";
		exit(-1);
	}

	e->ScrollDistanceX = 0;
	e->ScrollDistanceY = 0;
	e->eventType = EventType_Scroll;
	al_get_keyboard_state(&KbState);
	if (al_key_down(&KbState, ALLEGRO_KEY_DOWN))
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
	EventQueue.push(e);
}*/

/*Handles all user input*/
void UserInput(void)
{
	/*ALLEGRO_KEYBOARD_STATE KbState;

	al_get_keyboard_state(&KbState);
	if (al_key_down(&KbState, ALLEGRO_KEY_ESCAPE))
	{
		User_input_done = true;
	}
	if (al_key_down(&KbState, ALLEGRO_KEY_ENTER))	//Later i might need a Finite State Machine to know whats going on in the game
	{
		Event* e = new Event;
		e->eventType = EventType_Action;
		EventQueue.push(e);
	}
	if (al_key_down(&KbState, ALLEGRO_KEY_ALT) && al_key_down(&KbState, ALLEGRO_KEY_G))	// Ctrl + g combination to toggle the grid
	{
		Event* e = new Event;
		e->eventType = EventType_ToggleGrid;
		EventQueue.push(e);
	}
	CheckScroll(KbState, scrollDistanceX, scrollDistanceY);
	*/
}
