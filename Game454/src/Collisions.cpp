#include "..\Headers\Collisions.h"

ALLEGRO_EVENT_QUEUE* CollisionQueue;
ALLEGRO_TIMER* PlayerHurtTimer;

void Collisions_init()
{
	CollisionQueue = al_create_event_queue();
	PlayerHurtTimer = al_create_timer(1.0);	//1 sec of invicibility, useful for timing incoming damage
	al_register_event_source(CollisionQueue, al_get_timer_event_source(FPStimer));
	al_register_event_source(CollisionQueue, al_get_timer_event_source(PlayerHurtTimer));
}

void CheckCollisions()
{
	ALLEGRO_EVENT event;
	if (!al_is_event_queue_empty(CollisionQueue))	//check collisions every frame, every 0.0167 of a second (60 fps)
	{
		if (!al_get_next_event(CollisionQueue, &event))
		{
			std::cout << "Error : EventQueue empty when !al_is_event_queue_empty() returned false\n";
			exit(-1);
		}

		if (event.any.source == al_get_timer_event_source(PlayerHurtTimer))
		{
			player->Set_HurtInvicibility(false);
			al_stop_timer(PlayerHurtTimer);
		}

		if (player != NULL)	//a player existing means a level with enemies is loaded, so start checking for collisions
		{
			for (unsigned int i = 0; i < Enemies.size(); i++)
			{
				Rect r = Enemies[i]->FrameToDraw(), rP = player->FrameToDraw();

				if (player->Get_State() == State_Walking && player->Get_HurtInvicibility() == false)
				{
					if (!((Enemies[i]->positionX + r.w < player->positionX + player->screenX*DISPLAY_W) ||
						(player->positionX + rP.w + player->screenX * DISPLAY_W < Enemies[i]->positionX) ||
						(Enemies[i]->positionY + r.h < player->positionY + player->screenY * DISPLAY_H) ||
						(player->positionY + rP.h + player->screenY * DISPLAY_H < Enemies[i]->positionY))) /*No overlap condition*/
					{
						cout << "Player Collision with " << typeid(*Enemies[i]).name() << '\n';
						//take damage and start an invicibility timer
						player->Take_Damage(1);
						player->Set_HurtInvicibility(true);
						al_start_timer(PlayerHurtTimer);

						//maybe scroll the player to simulate push because of damage taken
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