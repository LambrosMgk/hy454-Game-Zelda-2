#include "..\Headers\Collisions.h"

ALLEGRO_EVENT_QUEUE* CollisionFPSQueue;

void Collisions_init()
{
	CollisionFPSQueue = al_create_event_queue();
	al_register_event_source(CollisionFPSQueue, al_get_timer_event_source(FPStimer));
}

void CheckCollisions()
{
	ALLEGRO_EVENT event;
	if (!al_is_event_queue_empty(CollisionFPSQueue))	//check collisions every frame, every 0.0167 of a second (60 fps)
	{
		if (!al_get_next_event(CollisionFPSQueue, &event))
		{
			std::cout << "Error : EventQueue empty when !al_is_event_queue_empty() returned false\n";
			exit(-1);
		}

		if (player != NULL)	//a player existing means a level with enemies is loaded, so start checking for collisions
		{
			for (unsigned int i = 0; i < Enemies.size(); i++)
			{
				Rect r = Enemies[i]->FrameToDraw(), rP = player->FrameToDraw();

				if (player->Get_State() == State_Walking)
				{
					if (!((Enemies[i]->positionX + r.w < player->positionX) ||
						(player->positionX + rP.w < Enemies[i]->positionX) ||
						(Enemies[i]->positionY + r.h < player->positionY) ||
						(player->positionY + rP.h < Enemies[i]->positionY))) /*No overlap condition*/
					{
						cout << "Player Collision with " << typeid(*Enemies[i]).name() << '\n';
						//take damage and add an invicibility timer
					}
				}
				else if (player->Get_State() == State_Attacking && player->LinkSpriteNum == 1)	/*2nd frame of the attack animation*/
				{
					if (!((Enemies[i]->positionX + r.w < player->positionX) ||
						(player->positionX + rP.w < Enemies[i]->positionX) ||
						(Enemies[i]->positionY + r.h < player->positionY) ||
						(player->positionY + rP.h < Enemies[i]->positionY))) /*No overlap condition*/
					{
						cout << "Sword Collision with " << typeid(*Enemies[i]).name() << '\n';
					}
				}
				else if (player->Get_State() == State_CrounchAttacking)
				{

				}
			}
		}
	}
}