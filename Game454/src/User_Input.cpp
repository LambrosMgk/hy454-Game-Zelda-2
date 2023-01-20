#include "..\\Headers\\User_Input.h"

ALLEGRO_EVENT_QUEUE* EventQueue;

void User_Input_init()
{
	EventQueue = al_create_event_queue();
	al_register_event_source(EventQueue, al_get_keyboard_event_source());
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
	while (!al_is_event_queue_empty(EventQueue)) //Check all the user input
	{
		ALLEGRO_EVENT event;
		if (!al_get_next_event(EventQueue, &event))
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
				//cout << "grid->GetIndexFromLayer(grid->getPlayerBottomRow(player) = " << grid->GetIndexFromLayer(grid->getPlayerBottomRow(player), grid->getPlayerLeftCol(player)) << "\n";
				//cout << "grid->getPlayerBottomRow(player)  + 1=  " << grid->getPlayerBottomRow(player) + 1 << "\n";
				//cout << "grid->getPlayerleftCol(player) = " << grid->getPlayerLeftCol(player) << "\n";
				//cout << "grid->getPlayerRightCol(player) = " << grid->getPlayerRightCol(player) << "\n";
				
				/*both tiles of the player must be inside of the elevator*/
				if ((grid->GetIndexFromLayer(grid->getPlayerBottomRow(player) + 1, grid->getPlayerLeftCol(player)) == ELEVATORID1 ||
					grid->GetIndexFromLayer(grid->getPlayerBottomRow(player) + 1, grid->getPlayerLeftCol(player)) == ELEVATORID2)
					&&
					grid->GetIndexFromLayer(grid->getPlayerBottomRow(player) + 1, grid->getPlayerRightCol(player)) == ELEVATORID1 ||
					grid->GetIndexFromLayer(grid->getPlayerBottomRow(player) + 1, grid->getPlayerRightCol(player)) == ELEVATORID2)
				{
					player->Set_State(State_Elevator);
					if (elevators.size() == 0)
					{
						cout << "Error : elevators vector is empty!\n";
						exit(-1);
					}

					for (unsigned int i = 0; i < elevators.size(); i++)
					{
						cout << "DIV_TILE_HEIGHT(elevators[i].getRow()) = " << DIV_TILE_HEIGHT(elevators[i].getRow()) << "\n";
						//grid->getPlayerBottomRow(player) - 3 to get the upper part of the elevator
						if ((DIV_TILE_HEIGHT(elevators[i].getRow()) == grid->getPlayerBottomRow(player) - 3) &&
							(DIV_TILE_WIDTH(elevators[i].getCol()) == grid->getPlayerLeftCol(player) ||
							DIV_TILE_WIDTH(elevators[i].getCol()) == grid->getPlayerRightCol(player))
							)
						{
							cout << "Active elevator = " << i << "\n";
							gameObj.level->active_elevator = i;
							break;
						}
					}
					elevators[0].hide_og_elevator();
					elevators[0].Paint_Sprite_Elevator();
				}
				else
				{
					player->Set_State(State_Crounching);
				}
			}
				break;
			case ALLEGRO_KEY_LEFT:
				player->set_Direction(dir_left);
				scrollLeft = true;
				break;
			case ALLEGRO_KEY_RIGHT:
				player->set_Direction(dir_right);
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
