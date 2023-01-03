#pragma once

ALLEGRO_TIMER* PhysicsTimer;
ALLEGRO_EVENT_QUEUE* PhysicsQueue;

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
	float scrollx = 0, scrolly = 0;

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
			if (scrollUp == true)
				scrolly = -player->Get_Speed_Y();
			if (scrollDown == true)
				scrolly += 0.2;	//gravity
			if (scrollLeft == true)
				scrollx -= player->Get_Speed_X();
			if (scrollRight == true)
				scrollx += player->Get_Speed_X();

			if (scrollx != 0 || scrolly != 0)
			{
				//first check grid if i can scroll where i want to
				grid->FilterGridMotion(player, &scrollx, &scrolly);
				player->Scroll_Player(scrollx, scrolly);
				Scroll_Bitmap();
			}

			//al_stop_timer(PhysicsTimer);
			//scrollUp = false;
			//scrollDown = true;
		}
	}
}