#include "..\\Headers\\User_Input.h"

ALLEGRO_EVENT_QUEUE* UserInput_EventQueue;

void User_Input_init()
{
	UserInput_EventQueue = al_create_event_queue();
	al_register_event_source(UserInput_EventQueue, al_get_keyboard_event_source());
	gameObj.Set_State(StartingScreen);
}

bool isDone()
{
	if (gameObj.Get_State() == GameFinished)
		return true;
	return false;
}

/*Handles all user input*/
void UserInput(void)
{
	while (!al_is_event_queue_empty(UserInput_EventQueue)) //Check all the user input
	{
		ALLEGRO_EVENT event;
		if (!al_get_next_event(UserInput_EventQueue, &event))
		{
			std::cout << "Error : EventQueue empty when !al_is_event_queue_empty() returned false\n";
			exit(-1);
		}

		if (gameObj.Get_State() == StartingScreen)
		{
			if (event.type == ALLEGRO_EVENT_KEY_DOWN)
			{
				switch (event.keyboard.keycode)
				{
				case ALLEGRO_KEY_ENTER:			//Enter == continue, load first level
					gameObj.Set_State(PlayingLevel1);
					gameObj.Stop_Music();
					//gameObj.Play_Music(LEVEL_1_MUSIC);
					break;
				}
			}
		}
		else if (event.type == ALLEGRO_EVENT_KEY_DOWN)
		{
			switch (event.keyboard.keycode)
			{
			case ALLEGRO_KEY_ESCAPE:
				gameObj.End_Game();		//ends the game for now
				break;
			case ALLEGRO_KEY_DOWN:
			{
				Grid* grid = gameObj.level->grids[1];
				
				/*both tiles of the player must be inside of the elevator*/
				if ((grid->GetIndexFromLayer(grid->getPlayerBottomRow(player) + 1, grid->getPlayerLeftCol(player)) == ELEVATORID1 ||
					grid->GetIndexFromLayer(grid->getPlayerBottomRow(player) + 1, grid->getPlayerLeftCol(player)) == ELEVATORID2)
					&&
					(grid->GetIndexFromLayer(grid->getPlayerBottomRow(player) + 1, grid->getPlayerRightCol(player)) == ELEVATORID1 ||
						grid->GetIndexFromLayer(grid->getPlayerBottomRow(player) + 1, grid->getPlayerRightCol(player)) == ELEVATORID2))
				{
					player->Set_State(State_Elevator);
					if (elevators.size() == 0)
					{
						cout << "Error : elevators vector is empty!\n";
						exit(-1);
					}
					
					for (unsigned int i = 0; i < elevators.size(); i++)
					{
						//grid->getPlayerBottomRow(player) - 3 to match the upper part of the elevator
						if ((DIV_TILE_HEIGHT(elevators[i].getRow()) == grid->getPlayerBottomRow(player) - 3) &&
							(DIV_TILE_WIDTH(elevators[i].getCol()) == grid->getPlayerLeftCol(player) ||
							DIV_TILE_WIDTH(elevators[i].getCol()) == grid->getPlayerRightCol(player))
							)
						{
							gameObj.level->active_elevator = i;
							elevators[i].hide_og_elevator();
							elevators[i].Add_to_draw_queue();
							break;
						}
					}
					
				}
				else
				{
					player->Set_State(State_Crounching);
				}
			}
				break;
			case ALLEGRO_KEY_LEFT:
				player->Set_Direction(dir_left);
				scrollLeft = true;
				break;
			case ALLEGRO_KEY_RIGHT:
				player->Set_Direction(dir_right);
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
					gameObj.level->Toggle_Grid = gameObj.level->Toggle_Grid ? false : true;
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
				if(player->Get_State() != State_Elevator)
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
