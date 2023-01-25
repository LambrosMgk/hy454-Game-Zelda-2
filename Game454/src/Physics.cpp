#include "..\\Headers\\Physics.h"

ALLEGRO_TIMER* PhysicsTimer;
ALLEGRO_EVENT_QUEUE* PhysicsQueue;
unsigned short jumpCountPixels = 0;
bool scrollUp = false;

void Physics_Init()
{
	PhysicsTimer = al_create_timer(1.0 / 60.0);
	PhysicsQueue = al_create_event_queue();
	al_register_event_source(PhysicsQueue, al_get_timer_event_source(PhysicsTimer));
	al_start_timer(PhysicsTimer);
}


void Calculate_Physics()
{
	ALLEGRO_EVENT event;
	int scrollx = 0, scrolly = 0;

	if (player != NULL)
	{
		if (!al_is_event_queue_empty(PhysicsQueue))
		{
			if (!al_get_next_event(PhysicsQueue, &event))
			{
				std::cout << "Error : EventQueue empty when !al_is_event_queue_empty() returned false\n";
				exit(-1);
			}


			//check user movement

			if (keyboardUp == true && player->jumping == false) {

				scrollUp = true;
				player->jumping = true;
				keyboardUp = false;
			}
			if (scrollUp == true && player->jumping == true)
			{
				scrollDown = false;
				scrolly -= player->Get_Speed_Y();
				jumpCountPixels += player->Get_Speed_Y();

			}
			if (scrollDown == true)
				scrolly += player->Get_Speed_Y();
			if (scrollLeft == true)
				scrollx -= player->Get_Speed_X();
			if (scrollRight == true)
				scrollx += player->Get_Speed_X();

			if (jumpCountPixels >= 32)	//add check when the player hits the ground only then he will be able to jump again
			{
				scrollUp = false;
				scrollDown = true;
				jumpCountPixels = 0;
			}

			if (scrollx != 0 || scrolly != 0)
			{
				//first check grid if i can scroll where i want to
				gameObj.level->grids[0]->FilterGridMotion(player, &scrollx, &scrolly);
				gameObj.level->grids[1]->FilterGridMotion(player, &scrollx, &scrolly);
				player->Scroll_Player(scrollx, scrolly);
				gameObj.level->Scroll_Bitmap();
			}

			for (unsigned short i = 0; i < Enemies.size(); i++)
			{
				int e_scrollx = 0, e_scrolly = 0;

				if (typeid(*Enemies[i]).name() == typeid(Stalfos).name())
				{

				}
				else if (typeid(*Enemies[i]).name() == typeid(PalaceBot).name())
				{
					PalaceBot* palaceBot = dynamic_cast<PalaceBot*>(Enemies[i]);	//do this to get access to PalaceBot specific functions
					assert(palaceBot != NULL);

					if (palaceBot->Get_State() == E_State_Jumping)
					{
						//palaceBots will travel 3 tiles distance while jumping
						
						e_scrollx = palaceBot->Get_Speed_X();
						e_scrolly = palaceBot->Get_dy();
						if (palaceBot->Get_Direction() == dir_left)
							e_scrollx = -e_scrollx;
						gameObj.level->grids[0]->FilterEnemyGridMotion(palaceBot, &e_scrollx, &e_scrolly);
						//cout << "grid say : e_scrollx = " << e_scrollx << ", e_scrolly = " << e_scrolly << '\n';
						palaceBot->Scroll_Enemy(e_scrollx, e_scrolly);
						palaceBot->Increment_dy();	//increment because going up means subtracting so if i want to slow down and go back down i need to start adding
						
						//if i hit a solid tile i need to stop (check left, right and down, no need to check up because grid filter will block us from moving and we will eventually fall down)
						if ((gameObj.level->grids[0]->GetGridTile(DIV_TILE_HEIGHT(palaceBot->positionY) + 1, DIV_TILE_WIDTH(palaceBot->positionX)) & GRID_SOLID_TILE) ||
							(gameObj.level->grids[0]->GetGridTile(DIV_TILE_HEIGHT(palaceBot->positionY), DIV_TILE_WIDTH(palaceBot->positionX) + 1) & GRID_SOLID_TILE) ||
							(gameObj.level->grids[0]->GetGridTile(DIV_TILE_HEIGHT(palaceBot->positionY), DIV_TILE_WIDTH(palaceBot->positionX) - 1) & GRID_SOLID_TILE))
						{
							palaceBot->Set_State(E_State_Walking);
						}
					}
				}
				else if (typeid(*Enemies[i]).name() == typeid(Wosu).name())
				{

				}
				else if (typeid(*Enemies[i]).name() == typeid(Guma).name())
				{

				}
			}
		}
	}
}