#include "..\Headers\AI.h"

ALLEGRO_EVENT_QUEUE* AI_Queue;

void AI_Init()
{
	AI_Queue = al_create_event_queue();
	al_register_event_source(AI_Queue, al_get_timer_event_source(FPStimer));
}

void Calculate_AI()
{
	ALLEGRO_EVENT event;
	if (!al_is_event_queue_empty(AI_Queue))	//check collisions every frame, every 0.0167 of a second (60 fps)
	{
		if (!al_get_next_event(AI_Queue, &event))
		{
			std::cout << "Error : EventQueue empty when !al_is_event_queue_empty() returned false\n";
			exit(-1);
		}

		if (player != NULL)
		{
			for (unsigned short i = 0; i < Enemies.size(); i++)
			{
				if (typeid(*Enemies[i]).name() == typeid(Stalfos).name())
				{
					Stalfos* stalfos = dynamic_cast<Stalfos*>(Enemies[i]);	//do this to get access to Stalfos specific functions
					assert(stalfos != NULL);

					
					if (stalfos->Get_State() == E_State_Waiting &&
						DIV_TILE_WIDTH(player->positionX + player->screenX * DISPLAY_W) == DIV_TILE_WIDTH(stalfos->positionX) &&
						(player->positionY + player->screenY * DISPLAY_H > stalfos->positionY)	// higher player Y position means lower in the level
						)
					{
						stalfos->Set_State(E_State_Falling);
					}
					else if (stalfos->Get_State() == E_State_Walking)
					{
						if (DIV_TILE_WIDTH(abs(player->positionX + player->screenX * DISPLAY_W - stalfos->positionX)) == 1)
						{
							stalfos->Set_State(E_State_Attacking);
						}
						else if (DIV_TILE_WIDTH(abs(player->positionX + player->screenX * DISPLAY_W - (stalfos->positionX))) < 10)
						{
							if (player->positionX + player->screenX * DISPLAY_W < (stalfos->positionX))
							{
								stalfos->Set_Direction(dir_left);
								stalfos->Scroll_Enemy(-stalfos->Get_Speed_X(), 0);
							}
							else
							{
								stalfos->Set_Direction(dir_right);
								stalfos->Scroll_Enemy(stalfos->Get_Speed_X(), 0);
							}	
						}
						else
						{
							stalfos->Set_State(E_State_Idle);
						}
					}
					else if (stalfos->Get_State() == E_State_Idle)
					{
						if (DIV_TILE_WIDTH(abs(player->positionX + player->screenX * DISPLAY_W - (stalfos->positionX))) < 10)
							stalfos->Set_State(E_State_Walking);
					}
				}
				else if (typeid(*Enemies[i]).name() == typeid(PalaceBot).name())
				{
					PalaceBot* palaceBot = dynamic_cast<PalaceBot*>(Enemies[i]);	//do this to get access to PalaceBot specific functions
					assert(palaceBot != NULL);

					//if the player enters a 5 tile range, start homing to the player
					if (palaceBot->Get_State() == E_State_Walking &&
						DIV_TILE_WIDTH(abs(player->positionX + player->screenX * DISPLAY_W - (palaceBot->positionX))) < 5 &&
						DIV_TILE_HEIGHT(abs(player->positionY + player->screenY * DISPLAY_H - (palaceBot->positionY))) < 5
						)
					{
						//palaceBots will travel 3 tiles distance while jumping
						palaceBot->Set_State(E_State_Jumping);
						//check the direction which the bot will jump towards to
						if (player->positionX + player->screenX * DISPLAY_W < (palaceBot->positionX))
						{
							palaceBot->Set_Direction(dir_left);
						}
						else
						{
							palaceBot->Set_Direction(dir_right);
						}
					}
				}
				else if (typeid(*Enemies[i]).name() == typeid(Wosu).name())
				{
					Wosu* wosu = dynamic_cast<Wosu*>(Enemies[i]);	//do this to get access to Wosu specific functions
					assert(wosu != NULL);

					//wosus will travel 6 tiles distance
					wosu->Scroll_Enemy(wosu->Get_Speed_X(), 0);	//let this function do the job
				}
				else if (typeid(*Enemies[i]).name() == typeid(Guma).name())
				{
					Guma* guma = dynamic_cast<Guma*>(Enemies[i]);
					assert(guma != NULL);
					unsigned short tileDistanceX = DIV_TILE_WIDTH(abs(player->positionX + player->screenX * DISPLAY_W - (guma->positionX)));

					if(guma->canAttack == false)
						guma->add_to_Attack_Interval(-1);

					if (guma->get_Interval() <= 0) {
						guma->canAttack = true;
					
					}

					//if walking or idle and on the same height level as the player
					if ((guma->Get_State() == E_State_Walking || guma->Get_State() == E_State_Idle) &&
						DIV_TILE_HEIGHT(abs(player->positionY + player->screenY * DISPLAY_H - (guma->positionY))) < 5
						)
					{
						//look at the player
						if (player->positionX + player->screenX * DISPLAY_W < (guma->positionX))
						{
							guma->Set_Direction(dir_left);
						}
						else
						{
							guma->Set_Direction(dir_right);
						}

						//if the player enters a 9 tile range but still has at least 6 tiles distance, start attacking the player
						if (tileDistanceX > 6 && tileDistanceX <= 9 && guma->canAttack)
						{
							guma->Set_State(E_State_Attacking);
							guma->canAttack = false;
							guma->reset_Interval();
						}
						else if (tileDistanceX <= 6)
						{
							//Start walking away
							guma->Set_State(E_State_Walking);
							if (guma->Get_Direction() == dir_left)
							{
								guma->Set_Direction(dir_right);
							}
							else
							{
								guma->Set_Direction(dir_left);
							}	
						}
						else
						{
							guma->Set_State(E_State_Idle);
						}
					}
				}
			}
		}
	}
}