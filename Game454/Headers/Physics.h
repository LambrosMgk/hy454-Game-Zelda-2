#pragma once

ALLEGRO_TIMER* PhysicsTimer;
ALLEGRO_EVENT_QUEUE* PhysicsQueue;

void Physics_Init()
{
	PhysicsTimer = al_create_timer(1.0 / 2.0);
	PhysicsQueue = al_create_event_queue();
	al_register_event_source(PhysicsQueue, al_get_timer_event_source(PhysicsTimer));
}

void Calculate_Physics()
{
	ALLEGRO_EVENT event;

	if (player != NULL)
	{
		if (scrollUp == true)	//jumped
		{
			scrollDown = false;
			al_start_timer(PhysicsTimer);	//start jump timer
		}
	}

	if (!al_is_event_queue_empty(PhysicsQueue))
	{
		if (!al_get_next_event(PhysicsQueue, &event))
		{
			std::cout << "Error : EventQueue empty when !al_is_event_queue_empty() returned false\n";
			exit(-1);
		}
		
		al_stop_timer(PhysicsTimer);
		scrollUp = false;
		scrollDown = true;
	}
}