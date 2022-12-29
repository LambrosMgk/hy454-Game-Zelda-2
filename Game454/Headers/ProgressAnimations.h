#pragma once

#include "User_Input.h"

ALLEGRO_TIMER* AnimationTimer;
ALLEGRO_EVENT_QUEUE* AnimatorQueue;

void Animator_Init()
{
	AnimationTimer = al_create_timer(1.0 / 5.0);
	AnimatorQueue = al_create_event_queue();
	al_register_event_source(AnimatorQueue, al_get_timer_event_source(AnimationTimer));
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
		if (player != NULL)
		{
			if (scrollLeft == true || scrollRight == true)	//walking
				player->LinkSpriteNum = ++player->LinkSpriteNum % 4;
			if (scrollLeft == false && scrollRight == false)
				player->LinkSpriteNum = 0;
		}
	}
}