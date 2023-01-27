#include "..\Headers\Collisions.h"

ALLEGRO_EVENT_QUEUE* CollisionQueue;
ALLEGRO_TIMER* PlayerHurtTimer, *PlayerHitTimer;

void Collisions_init()
{
	CollisionQueue = al_create_event_queue();
	PlayerHurtTimer = al_create_timer(1.0);	//1 sec of invicibility, useful for timing incoming damage
	PlayerHitTimer = al_create_timer(0.5);
	al_register_event_source(CollisionQueue, al_get_timer_event_source(FPStimer));
	al_register_event_source(CollisionQueue, al_get_timer_event_source(PlayerHurtTimer));
	al_register_event_source(CollisionQueue, al_get_timer_event_source(PlayerHitTimer));
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

		if (event.any.source == al_get_timer_event_source(PlayerHitTimer))
		{
			player->Set_WaitAfterHit(false);
			al_stop_timer(PlayerHitTimer);
		}


		if (player != NULL)	//a player existing means a level with enemies is loaded, so start checking for collisions
		{
			Rect rP = player->FrameToDraw();
			for (unsigned int i = 0; i < Enemies.size(); i++)
			{
				
				Rect r = Enemies[i]->FrameToDraw();

				if (player->Get_State() == State_Walking && player->Get_HurtInvicibility() == false)
				{
					if (!((Enemies[i]->positionX + r.w < player->positionX + player->screenX*DISPLAY_W) ||
						(player->positionX + rP.w + player->screenX * DISPLAY_W < Enemies[i]->positionX) ||
						(Enemies[i]->positionY + r.h < player->positionY + player->screenY * DISPLAY_H) ||
						(player->positionY + rP.h + player->screenY * DISPLAY_H < Enemies[i]->positionY))) /*No overlap condition*/
					{
						//take damage and start an invicibility timer to prevent "damage stacking" from multiple collisions
						cout << "Player took " << Enemies[i]->Get_Damage() << " from " << typeid(*Enemies[i]).name() << '\n';
						player->Take_Damage(Enemies[i]->Get_Damage());
						

						if (player->Get_Health() <= 0) {
							cout << "Player died \n" ;
						}
						
						player->Set_HurtInvicibility(true);
						al_start_timer(PlayerHurtTimer);

						//add damage animation and change player color temporarily
						//scroll the player to simulate push because of damage taken
					}

				}
				else if (player->Get_State() == State_Attacking && player->LinkSpriteNum == 1 && player->Get_WaitAfterHit() == false)	/*2nd frame of the attack animation*/
				{
					if (!((Enemies[i]->positionX + r.w < player->positionX + player->screenX*DISPLAY_W) ||
						(player->positionX + rP.w + player->screenX * DISPLAY_W < Enemies[i]->positionX) ||
						(Enemies[i]->positionY + r.h < player->positionY + player->screenY * DISPLAY_H) ||
						(player->positionY + rP.h + player->screenY * DISPLAY_H < Enemies[i]->positionY))) /*No overlap condition*/
					{
						cout << typeid(*Enemies[i]).name() << "took "<< player->Get_Damage() << " damage from player \n";
						Enemies[i]->Take_Damage(player->Get_Damage());

						if(Enemies[i]->Get_Health() <= 0)
						{
							cout << typeid(*Enemies[i]).name() << " died \n";
							gameObj.level->Add_Random_Drop(Enemies[i]->positionX, Enemies[i]->positionY);
							delete Enemies[i];
							Enemies.erase(Enemies.begin() + i);
						}
							

						player->Set_WaitAfterHit(true);
						al_start_timer(PlayerHitTimer);
					}
				}
				else if ((player->Get_State() == State_Crounching || player->Get_State() == State_CrounchAttacking) && player->Get_HurtInvicibility() == false) //while crounching link will not take damage from the front but can be damaged from behind
				{
					if (((player->Get_Direction() == dir_left && Enemies[i]->Get_Direction() == dir_left) ||
						(player->Get_Direction() == dir_right && Enemies[i]->Get_Direction() == dir_right)) &&  //looking the same way
						(!((Enemies[i]->positionX + r.w < player->positionX + player->screenX * DISPLAY_W) ||
							(player->positionX + rP.w + player->screenX * DISPLAY_W < Enemies[i]->positionX) ||
							(Enemies[i]->positionY + r.h < player->positionY + player->screenY * DISPLAY_H) ||
							(player->positionY + rP.h + player->screenY * DISPLAY_H < Enemies[i]->positionY)))) // overlap condition
					{
						//take damage and start an invicibility timer to prevent "damage stacking" from multiple collisions
						cout << "Player got railed for " << Enemies[i]->Get_Damage() << " points from " << typeid(*Enemies[i]).name() << " from behind \n";
						player->Take_Damage(Enemies[i]->Get_Damage());

						if (player->Get_Health() <= 0) {
							cout << "Player died \n";
						}


						player->Set_HurtInvicibility(true);
						al_start_timer(PlayerHurtTimer);
					}

					if ((player->Get_State() == State_CrounchAttacking && player->Get_WaitAfterHit() == false) && 
						(!((Enemies[i]->positionX + r.w < player->positionX + player->screenX * DISPLAY_W) ||
						(player->positionX + rP.w + player->screenX * DISPLAY_W < Enemies[i]->positionX) ||
						(Enemies[i]->positionY + r.h < player->positionY + player->screenY * DISPLAY_H) ||
						(player->positionY + rP.h + player->screenY * DISPLAY_H < Enemies[i]->positionY)))) 
					{
					
						cout << typeid(*Enemies[i]).name() << "took " << player->Get_Damage() << " damage from player while crounching\n";
						Enemies[i]->Take_Damage(player->Get_Damage());

						if (Enemies[i]->Get_Health() <= 0)
							cout << typeid(*Enemies[i]).name() << " died \n";

						player->Set_WaitAfterHit(true);
						al_start_timer(PlayerHitTimer);
					
					}
				}
			}

			for (unsigned int i = 0; i < Projectiles.size(); i++ ) {

				Rect r = Projectiles[i]->FrameToDraw();

				if (player->Get_State() == State_Walking && player->Get_HurtInvicibility() == false)
				{
					if (!((Projectiles[i]->Get_Position_X() + r.w < player->positionX + player->screenX * DISPLAY_W) ||
						(player->positionX + rP.w + player->screenX * DISPLAY_W < Projectiles[i]->Get_Position_X()) ||
						(Projectiles[i]->Get_Position_Y() + r.h < player->positionY + player->screenY * DISPLAY_H) ||
						(player->positionY + rP.h + player->screenY * DISPLAY_H < Projectiles[i]->Get_Position_Y()))) /*No overlap condition*/
					{
						//take damage and start an invicibility timer to prevent "damage stacking" from multiple collisions
						cout << "Player took " << Enemies[i]->Get_Damage() << " from " << typeid(*Projectiles[i]).name() << '\n';
						//player->Take_Damage(Projectiles[i]->Get_Damage());


						if (player->Get_Health() <= 0) {
							cout << "Player died \n";
						}

						player->Set_HurtInvicibility(true);
						al_start_timer(PlayerHurtTimer);

						//add damage animation and change player color temporarily
						//scroll the player to simulate push because of damage taken
					}
				}
			}

			/*if the player attacked, check collectables*/
			/*when a collision is detected for a collectable the object must be destroyed, therefore no need for a timer to prevent multiple collisions (like damage stacking above)*/
			if (player->Get_State() == State_Attacking && player->LinkSpriteNum == 1)
			{
				for (unsigned int i = 0; i < Collectables.size(); i++)
				{
					Rect r = Collectables[i]->FrameToDraw();
					if (!((Collectables[i]->positionX + r.w < player->positionX + player->screenX * DISPLAY_W) ||
						(player->positionX + rP.w + player->screenX * DISPLAY_W < Collectables[i]->positionX) ||
						(Collectables[i]->positionY + r.h < player->positionY + player->screenY * DISPLAY_H) ||
						(player->positionY + rP.h + player->screenY * DISPLAY_H < Collectables[i]->positionY))
						) /*No overlap condition*/
					{
						cout << "Sword collision with " << typeid(*Collectables[i]).name() << '\n';
						if (typeid(*Collectables[i]).name() == typeid(SimpleKey).name())
						{
							cout << "Key removed\n";
							player->Add_Key();
							delete Collectables[i];	//calls destructor of object
							Collectables.erase(Collectables.begin() + i);
						}
						else if (typeid(*Collectables[i]).name() == typeid(RedPotion).name())
						{
							cout << "Red potion removed\n";
							//add effect of potion
							delete Collectables[i];
							Collectables.erase(Collectables.begin() + i);
						}
						else if (typeid(*Collectables[i]).name() == typeid(BluePotion).name())
						{
							cout << "Blue potion removed\n";
							//add effect of potion
							delete Collectables[i];
							Collectables.erase(Collectables.begin() + i);
						}
						else if (typeid(*Collectables[i]).name() == typeid(PointBag).name())
						{
							cout << "PointBag potion removed\n";
							PointBag* bag = dynamic_cast<PointBag*>(Collectables[i]);
							if (bag == NULL)
							{
								cout << "Error dynamic casting Pointbag, exiting...\n";
								exit(-1);
							}
							player->Add_Score(bag->Get_Points());
							cout << "Player score now : " << player->Get_Score() << '\n';
							delete Collectables[i];
							Collectables.erase(Collectables.begin() + i);
						}
					}
				}
			}

			//check for door collisions
			Grid *grid = gameObj.level->grids[1];
			/*if link has key && is close to the door*/
			if (player->Get_Keys() != 0 && 
				(grid->GetIndexFromLayer(grid->getPlayerBottomRow(player) - 1, grid->getPlayerLeftCol(player) - 1) == DOOR_MIDDLE_ID) ||
				(grid->GetIndexFromLayer(grid->getPlayerBottomRow(player) - 1, grid->getPlayerRightCol(player) + 1) == DOOR_MIDDLE_ID))
			{
				cout << "Door unlocked!\n";
				player->Remove_Key();
				cout << "Player now has " << player->Get_Keys() << " keys.\n";
				
				//make tiles empty and change CSV so collision won't happen again
				if (player->Get_Direction() == dir_left)
				{
					grid->SetEmptyGridTile(grid->getPlayerBottomRow(player), grid->getPlayerLeftCol(player) - 1);	//upper door
					grid->SetEmptyGridTile(grid->getPlayerBottomRow(player) - 1, grid->getPlayerLeftCol(player) - 1);	//middle door
					grid->SetEmptyGridTile(grid->getPlayerBottomRow(player) - 2, grid->getPlayerLeftCol(player) - 1);	//lower door

					gameObj.level->TileMapCSV[1][grid->getPlayerBottomRow(player)][grid->getPlayerLeftCol(player) - 1] = -1;
					gameObj.level->TileMapCSV[1][grid->getPlayerBottomRow(player) - 1][grid->getPlayerLeftCol(player) - 1] = -1;
					gameObj.level->TileMapCSV[1][grid->getPlayerBottomRow(player) - 2][grid->getPlayerLeftCol(player) - 1] = -1;
				}
				else
				{
					grid->SetEmptyGridTile(grid->getPlayerBottomRow(player), grid->getPlayerRightCol(player) + 1);
					grid->SetEmptyGridTile(grid->getPlayerBottomRow(player) - 1, grid->getPlayerRightCol(player) + 1);
					grid->SetEmptyGridTile(grid->getPlayerBottomRow(player) - 2, grid->getPlayerRightCol(player) + 1);

					gameObj.level->TileMapCSV[1][grid->getPlayerBottomRow(player)][grid->getPlayerRightCol(player) + 1] = -1;
					gameObj.level->TileMapCSV[1][grid->getPlayerBottomRow(player) - 1][grid->getPlayerRightCol(player) + 1] = -1;
					gameObj.level->TileMapCSV[1][grid->getPlayerBottomRow(player) - 2][grid->getPlayerRightCol(player) + 1] = -1;
				}
				
				//Hide door tiles
				al_lock_bitmap(gameObj.level->bitmaps[1], ALLEGRO_PIXEL_FORMAT_ANY, ALLEGRO_LOCK_READWRITE);
				al_set_target_bitmap(gameObj.level->bitmaps[1]);

				ALLEGRO_COLOR color;
				unsigned int row = MUL_TILE_HEIGHT(grid->getPlayerBottomRow(player) - 2), col;

				if (player->Get_Direction() == dir_left)
					col = MUL_TILE_WIDTH(grid->getPlayerLeftCol(player) - 1);
				else
					col = MUL_TILE_WIDTH(grid->getPlayerRightCol(player) + 1);

				for (unsigned int y = row; y < row + 3 * TILE_HEIGHT; y++)
				{
					for (unsigned int x = col; x < col + TILE_WIDTH; x++)
					{
						color = al_get_pixel(gameObj.level->bitmaps[1], x, y);
						unsigned char r, g, b;

						al_unmap_rgb(color, &r, &g, &b);
						//cout << "r = " << (int)r << ", g = " << (int)g << ", b = " << (int)b << ", a = " << (int)a << "\n";
						al_put_pixel(x, y, al_map_rgba(r, g, b, 0));

					}
				}
				al_set_target_bitmap(al_get_backbuffer(gameObj.display));
				al_unlock_bitmap(gameObj.level->bitmaps[1]);

				if(player->Get_Direction() == dir_left)
					Doors.push_back(new Door(MUL_TILE_WIDTH(grid->getPlayerLeftCol(player) - 1), MUL_TILE_HEIGHT(grid->getPlayerBottomRow(player) - 2)));
				else	
					Doors.push_back(new Door(MUL_TILE_WIDTH(grid->getPlayerRightCol(player) + 1), MUL_TILE_HEIGHT(grid->getPlayerBottomRow(player) - 2)));
				Doors[Doors.size() - 1]->Add_To_Draw_Queue();
				Doors[Doors.size() - 1]->SetActive(true);

				//set timer in animation
				al_start_timer(DoorTimer);
			}
		}
	}
}