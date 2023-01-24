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

				}
				else if (typeid(*Enemies[i]).name() == typeid(PalaceBot).name())
				{

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

				}
			}
		}
	}
}