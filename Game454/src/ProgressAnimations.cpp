#include "..\\Headers\\ProgressAnimations.h"

void Animator_Init()
{
	AnimationTimer = al_create_timer(1.0 / 5.0);
	AttackTimer = al_create_timer(1.0 / 10.0);
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
			if (player->Get_State() == State_Walking)
			{
				if (scrollLeft == true || scrollRight == true)	//walking
					player->LinkSpriteNum = ++player->LinkSpriteNum % 4;
				else if (scrollLeft == false && scrollRight == false)	//idle
					player->LinkSpriteNum = 0;
			}
			else if (event.any.source == al_get_timer_event_source(AttackTimer) && player->Get_State() == State_Attacking)
			{
				player->LinkSpriteNum++;

				if (player->LinkSpriteNum >= 2)
				{
					StartAttack = false;
					al_stop_timer(AttackTimer);
					player->Set_State(State_Walking);
					player->LinkSpriteNum = 0;
				}
			}
			else if (event.any.source == al_get_timer_event_source(AttackTimer) && player->Get_State() == State_CrounchAttacking)
			{
				player->LinkSpriteNum++;

				if (player->LinkSpriteNum >= 1)
				{
					StartAttack = false;
					al_stop_timer(AttackTimer);
					player->Set_State(State_Crounching);
					player->LinkSpriteNum = 0;
				}
			}
			else if ((player->Get_State() == State_Attacking || player->Get_State() == State_CrounchAttacking) && StartAttack == false)
			{
				player->LinkSpriteNum = 0;
				StartAttack = true;
				al_start_timer(AttackTimer);
			}

		}
	}