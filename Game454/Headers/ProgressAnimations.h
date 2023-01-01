#pragma once

#include "User_Input.h"

ALLEGRO_TIMER* AnimationTimer, *AttackTimer;
ALLEGRO_EVENT_QUEUE* AnimatorQueue;

bool StartAttack = false;

void Animator_Init()
{
	AnimationTimer = al_create_timer(1.0 / 5.0);
	AttackTimer = al_create_timer(1.0 / 2.0);
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
			if (player->state == State_Walking)
			{
				if (scrollLeft == true || scrollRight == true)	//walking
					player->LinkSpriteNum = ++player->LinkSpriteNum % 4;
				else if (scrollLeft == false && scrollRight == false)	//idle
					player->LinkSpriteNum = 0;
			}
			else if (event.any.source == al_get_timer_event_source(AttackTimer))
			{
				player->LinkSpriteNum = ++player->LinkSpriteNum;
				std::cout << "TATAKAE\n";

				if (player->LinkSpriteNum >= 2)
				{
					StartAttack = false;
					al_stop_timer(AttackTimer);
					std::cout << "changing state to walking\n";
					player->state == State_Walking;
					player->LinkSpriteNum = 0;
				}
			}
			else if (player->state == State_Attacking && StartAttack == false)
			{
				std::cout << "starting attack\n";
				player->LinkSpriteNum = 0;
				StartAttack = true;
				al_start_timer(AttackTimer);
			}
		}
	}
}