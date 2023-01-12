#include "..\\Headers\\User_Input.h"


void User_Input_init()
{
	EventQueue = al_create_event_queue();
	al_register_event_source(EventQueue, al_get_keyboard_event_source());
	GameState = StartingScreen;
}

bool isDone()
{
	return User_input_done;
}

/*Handles all user input*/
void UserInput(void)
{
	while (!al_is_event_queue_empty(EventQueue)) //Check all the user input
	{
		ALLEGRO_EVENT event;
		if (!al_get_next_event(EventQueue, &event))
		{
			std::cout << "Error : EventQueue empty when !al_is_event_queue_empty() returned false\n";
			exit(-1);
		}

		if (GameState == StartingScreen)
		{
			if (event.type == ALLEGRO_EVENT_KEY_DOWN)
			{
				switch (event.keyboard.keycode)
				{
				case ALLEGRO_KEY_ENTER:			//Enter == continue, load first level
					GameState = PlayingLevel1;
					break;
				}
			}
		}
		else if (event.type == ALLEGRO_EVENT_KEY_DOWN)
		{
			switch (event.keyboard.keycode)
			{
			case ALLEGRO_KEY_ESCAPE:
				User_input_done = true;		//ends the game for now
				break;
			case ALLEGRO_KEY_DOWN:

				if (grids[1]->GetIndexFromLayer(grids[1]->getPlayerBottomRow(player), grids[1]->getPlayerStartCol(player)) == ELEVATORID1 ||
					grids[1]->GetIndexFromLayer(grids[1]->getPlayerBottomRow(player), grids[1]->getPlayerStartCol(player)) == ELEVATORID2)
				{
					player->Set_State(State_Elevator);

					elevators[0]->hide_og_elevator();
				}
				else
				{
					player->Set_State(State_Crounching);
				}
				break;
			case ALLEGRO_KEY_LEFT:
				player->direction = dir_left;
				scrollLeft = true;
				break;
			case ALLEGRO_KEY_RIGHT:
				player->direction = dir_right;
				scrollRight = true;
				break;
			case ALLEGRO_KEY_A:			// Jump
				keyboardUp = true;
				break;
			case ALLEGRO_KEY_B:			// Slash
				player->LinkSpriteNum = 0;
				player->Set_State(State_Attacking);
				break;
			case ALLEGRO_KEY_G:			// L_Ctrl + G combination
				ALLEGRO_KEYBOARD_STATE KbState;

				al_get_keyboard_state(&KbState);
				if (al_key_down(&KbState, ALLEGRO_KEY_LCTRL))
				{
					Toggle_Grid = Toggle_Grid ? false : true;
					std::cout << "Grid toggled\n";
				}
				break;
			}
		}
		else if (event.type == ALLEGRO_EVENT_KEY_UP)
		{
			switch (event.keyboard.keycode)
			{
			case ALLEGRO_KEY_DOWN:
				player->Set_State(State_Walking);
				break;
			case ALLEGRO_KEY_LEFT:
				scrollLeft = false;
				break;
			case ALLEGRO_KEY_RIGHT:
				scrollRight = false;
				break;
			}
		}
		else
		{
			//isws piasei san constant char event alla meta isws einai polu grhgoro to scroll
			//cout << "Event type : " << event.type << "\n";
		}
	}
}
