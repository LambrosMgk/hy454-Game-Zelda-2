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
						cout << "Player Collision with " << typeid(*Enemies[i]).name() << '\n';
						//take damage and start an invicibility timer to prevent "damage stacking" from multiple collisions
						player->Take_Damage(1);
						player->Set_HurtInvicibility(true);
						al_start_timer(PlayerHurtTimer);

						//add damage animation and change player color temporarily
						//scroll the player to simulate push because of damage taken
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

				//play door opening animation? (set timer in animation)
			}
		}
	}
}