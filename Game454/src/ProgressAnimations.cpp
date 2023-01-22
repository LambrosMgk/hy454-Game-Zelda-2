#include "..\\Headers\\ProgressAnimations.h"

ALLEGRO_TIMER* AnimationTimer, * AttackTimer;
ALLEGRO_EVENT_QUEUE* AnimatorQueue;
bool StartAttack = false;

void Animator_Init()
{
	AnimationTimer = al_create_timer(1.0 / 5.0);
	AttackTimer = al_create_timer(1.0 / 10.0);
	AnimatorQueue = al_create_event_queue();
	al_register_event_source(AnimatorQueue, al_get_timer_event_source(AnimationTimer));
	al_register_event_source(AnimatorQueue, al_get_timer_event_source(AttackTimer));
	al_start_timer(AnimationTimer);
}


void Animator()
{
	ALLEGRO_EVENT event;

	if (!al_is_event_queue_empty(AnimatorQueue))
	{
		if (!al_get_next_event(AnimatorQueue, &event))
		{
			std::cout << "Error : EventQueue empty when !al_is_event_queue_empty() returned false\n";
			exit(-1);
		}
		if (player != NULL)	//animator will be called from the start of the game when we haven't initialized a player yet
		{
			if (player->Get_State() == State_Walking)
			{
				if (scrollLeft == true || scrollRight == true)	//walking
					player->LinkSpriteNum = ++player->LinkSpriteNum % 4;
				else if (scrollLeft == false && scrollRight == false)	//idle
					player->LinkSpriteNum = 0;
			}
			else if (player->Get_State() == State_Elevator)	//move both the player and the elevator
			{
				unsigned int act_elevator = gameObj.level->active_elevator;
				if (act_elevator == -1)
				{
					cout << "Error : player state set to elevator but no activate elevator on \"Level\" object.\n";
					exit(-1);
				}

				//as the elevator is going down, if the next tile is a solid tile from layer 1 we must stop scrolling the elevator 
				if (gameObj.level->grids[0]->GetGridTile(DIV_TILE_HEIGHT(elevators[act_elevator].getCurrRow()) + 2, DIV_TILE_WIDTH(elevators[act_elevator].getCol())) & GRID_SOLID_TILE)
				{
					cout << "Elevator state ended\n";
					player->Set_State(State_Walking);
					elevators[act_elevator].addToCurrRow(5);	//a small offset because otherwise it will look like link is inside the elevator
					elevators[act_elevator].Update_draw_obj();
					gameObj.level->active_elevator = -1;	//no active elevator
				}
				else
				{
					elevators[act_elevator].addToCurrRow(2*player->Get_Speed_Y());
					elevators[act_elevator].Update_draw_obj();
				}

				//change the layer 2 grid to allow the player to fall
				if (gameObj.level->grids[1]->GetGridTile(DIV_TILE_HEIGHT(elevators[act_elevator].getCurrRow()), DIV_TILE_WIDTH(elevators[act_elevator].getCol())) & GRID_SOLID_TILE)
				{
					//left tile of elevator
					gameObj.level->grids[1]->SetEmptyGridTile(DIV_TILE_HEIGHT(elevators[act_elevator].getCurrRow()), DIV_TILE_WIDTH(elevators[act_elevator].getCol()));
					gameObj.level->grids[1]->SetSolidGridTile(DIV_TILE_HEIGHT(elevators[act_elevator].getCurrRow()) + 1, DIV_TILE_WIDTH(elevators[act_elevator].getCol()));
					//right tile of elevator
					gameObj.level->grids[1]->SetEmptyGridTile(DIV_TILE_HEIGHT(elevators[act_elevator].getCurrRow()), DIV_TILE_WIDTH(elevators[act_elevator].getCol()) + 1);
					gameObj.level->grids[1]->SetSolidGridTile(DIV_TILE_HEIGHT(elevators[act_elevator].getCurrRow()) + 1, DIV_TILE_WIDTH(elevators[act_elevator].getCol()) + 1);
				}
			}
			else if (event.any.source == al_get_timer_event_source(AttackTimer) && player->Get_State() == State_Attacking)
			{
				player->LinkSpriteNum++;

				if (player->LinkSpriteNum >= 2)
				{
					StartAttack = false;
					al_stop_timer(AttackTimer);
					player->Set_State(State_Walking);
					player->LinkSpriteNum = 0;
				}
			}
			else if (event.any.source == al_get_timer_event_source(AttackTimer) && player->Get_State() == State_CrounchAttacking)
			{
				player->LinkSpriteNum++;

				if (player->LinkSpriteNum >= 1)
				{
					StartAttack = false;
					al_stop_timer(AttackTimer);
					player->Set_State(State_Crounching);
					player->LinkSpriteNum = 0;
				}
			}
			else if ((player->Get_State() == State_Attacking || player->Get_State() == State_CrounchAttacking) && StartAttack == false)
			{
				player->LinkSpriteNum = 0;
				StartAttack = true;
				al_start_timer(AttackTimer);
			}

		}
	}
}