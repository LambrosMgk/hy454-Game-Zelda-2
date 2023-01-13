#include "..\\Headers\\Render.h"

bool draw = false;
ALLEGRO_TIMER* FPStimer;
ALLEGRO_EVENT_QUEUE* timerQueue;

/*By screen i mean the target bitmap*/
void Paint_Player_to_Screen(Rect r)
{
	al_draw_bitmap_region(player->PlayerSpriteSheet, r.x, r.y, r.w, r.h, player->positionX, player->positionY, 0);
}

void Load_Start_Screen()
{
	gameObj.Start_Screen_bitmap = al_load_bitmap(START_SCREEN_PATH);
	if (gameObj.Start_Screen_bitmap == NULL)
	{
		fprintf(stderr, "\nFailed to initialize Start_Screen_bitmap (al_load_bitmap() failed).\n");
		exit(-1);
	}
	al_draw_bitmap(gameObj.Start_Screen_bitmap, 0, 0, 0);
	al_flip_display();
	//cout << "Drawing " << filepath << '\n';
}

void Render_init()
{
	FPStimer = al_create_timer(1.0 / FPS);
	timerQueue = al_create_event_queue();
	al_register_event_source(timerQueue, al_get_timer_event_source(FPStimer));
	al_start_timer(FPStimer);	//a video said not to initialize any variables after this cuz it might mess up the timer, we'll see

	al_set_new_display_flags(ALLEGRO_WINDOWED);
	gameObj.display = al_create_display(DISPLAY_W, DISPLAY_H);
	if (!gameObj.display)
	{
		fprintf(stderr, "failed to create display!\n");
		exit(-1);
	}
	al_set_window_title(gameObj.display, "Zelda II: The Adventure of Link");
}

// use this to render grid (toggle on / off), used only for development time testing -
// a tile grid block is consecutive GRID_BLOCK_ROWS x GRID_BLOCK_COLUMNS block of grid indices
void DisplayGrid(unsigned int grid_num)
{
	std::vector<Grid*> grids = gameObj.level->grids;
	int cameraX = gameObj.level->cameraX, cameraY = gameObj.level->cameraY;
	if (grids[grid_num] == NULL)
	{
		return;
	}

	unsigned int startCol = DIV_TILE_WIDTH(cameraX * DISPLAY_W);
	unsigned int startRow = DIV_TILE_HEIGHT(cameraY * DISPLAY_H);
	unsigned int endCol = DIV_TILE_WIDTH(cameraX * DISPLAY_W + 1 * DISPLAY_W);
	unsigned int endRow = DIV_TILE_HEIGHT(cameraY * DISPLAY_H + 1 * DISPLAY_H);

	//if some part of the bitmap is not showing on the screen because of scrolling skip that part
	if (cameraX < 0)
		startCol += DIV_TILE_WIDTH(abs(cameraX * DISPLAY_W)) + 1;
	if (cameraY < 0)
		startRow += DIV_TILE_HEIGHT(abs(cameraY * DISPLAY_H)) + 1;

	//in case of a bitmap smaller than the screen (e.g. when testing the engine)
	if (endCol > grids[grid_num]->Grid_Max_Width)
		endCol = grids[grid_num]->Grid_Max_Width / grids[grid_num]->Grid_Block_Columns;	//divide with block cols and rows to get the max width and height for tiles
	if (endRow > grids[grid_num]->Grid_Max_Height)
		endRow = grids[grid_num]->Grid_Max_Height / grids[grid_num]->Grid_Block_Rows;

	//cout << "StartCol : " << startCol << " , StarRow : " << startRow << ", endCol : " << endCol << " , endrow : " << endRow << '\n';
	for (Dim rowTile = startRow; rowTile < endRow; rowTile++)
	{
		for (Dim colTile = startCol; colTile < endCol; colTile++)
		{
			for (unsigned int rowElem = 0; rowElem < grids[grid_num]->Grid_Block_Rows; ++rowElem)
			{
				for (unsigned int colElem = 0; colElem < grids[grid_num]->Grid_Block_Columns; ++colElem)
				{
					//printf("grid[rowTile + rowElem][colTile + colElem] : [%d + %d][%d + %d]\n", rowTile, rowElem, colTile, colElem);
					if (grids[grid_num]->grid[rowTile + rowElem][colTile + colElem] & GRID_SOLID_TILE)
					{
						auto x = MUL_TILE_WIDTH(colTile) + (colElem)*grids[grid_num]->Grid_Element_Width;	//if i want better perfomance note : try pre-caching the power of 2 of the grid elem width so that i can do something like colelem >> MUL_GRID_WIDTH
						auto y = MUL_TILE_HEIGHT(rowTile) + (rowElem)*grids[grid_num]->Grid_Element_Height;
						auto w = grids[grid_num]->Grid_Element_Width - 1;
						auto h = grids[grid_num]->Grid_Element_Height - 1;

						al_draw_filled_rectangle(x + cameraX, y + cameraY, x + w + cameraX, y + h + cameraY, al_map_rgba(255, 0, 0, 64));
					}
				}
			}

		}
	}

	al_draw_rectangle(player->positionX, player->positionY, player->positionX + player->FrameToDraw().w, player->positionY + player->FrameToDraw().h, al_map_rgba(0, 255, 0, 64), 1.0);
}

void Renderer()
{
	ALLEGRO_EVENT event;
	//al_wait_for_event(timerQueue, &event);

	if (!al_is_event_queue_empty(timerQueue))	//time to draw, every 0.0167 of a second (60 fps)
	{
		if (!al_get_next_event(timerQueue, &event))
		{
			std::cout << "Error : EventQueue empty when !al_is_event_queue_empty() returned false\n";
			exit(-1);
		}
		//no need to check for the type of event, the only events in the queue are timer events
		if (gameObj.Get_State() == PlayingLevel1)
		{
			al_clear_to_color(al_map_rgb(0, 0, 0));	//Clear the complete target bitmap, but confined by the clipping rectangle.
			if (gameObj.level == NULL)
			{
				gameObj.Load_Level(1);
			}


			al_draw_bitmap(gameObj.level->bitmap, gameObj.level->cameraX, gameObj.level->cameraY, 0);
			Paint_Player_to_Screen(player->FrameToDraw());
			if (gameObj.level->Toggle_Grid)
			{
				DisplayGrid(0);
				DisplayGrid(1);
			}

			al_flip_display(); //Copies or updates the front and back buffers so that what has been drawn previously on the currently selected display becomes visible on screen.
		}
	}
}

/*might be useful to have this in the future, also add more stuff cuz i'll forget for sure*/
void Render_Clear()
{
	
}