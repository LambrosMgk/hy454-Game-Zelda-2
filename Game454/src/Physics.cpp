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

			//al_stop_timer(PhysicsTimer);
			//scrollUp = false;
			//scrollDown = true;
		}
	}
}