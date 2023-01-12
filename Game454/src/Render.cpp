#include "..\\Headers\\Render.h"


//pre-caching tileset indexes for better perfomance, this function should be called at the start of the programm
void Calculate_Tileset_Indexes()
{
	divIndex = new unsigned char[TILESET_WIDTH * TILESET_HEIGHT];
	modIndex = new unsigned char[TILESET_WIDTH * TILESET_HEIGHT];
	if (divIndex == NULL || modIndex == NULL)
	{
		cout << "Failed to allocate memory for tileset indexes\n";
		exit(-1);
	}
	for (unsigned short i = 0; i < TILESET_WIDTH * TILESET_HEIGHT; ++i)
		divIndex[i] = i / TILESET_WIDTH, modIndex[i] = i % TILESET_WIDTH;
}

/*loads a bitmap from the given filepath, sets the global variables TILESET_WIDTH and TILESET_HEIGHT then returns the loaded tileset as a bitmap
in case of bad file path exits program with -1*/
ALLEGRO_BITMAP* load_tileset(const char* filepath)
{
	ALLEGRO_BITMAP* TileSet = al_load_bitmap(filepath);
	if (TileSet == NULL)
	{
		fprintf(stderr, "\nFailed to initialize Tileset (al_load_bitmap() failed).\n");
		exit(-1);
	}
	/*initialize TILESET_WIDTH and TILESET_HEIGHT base on the given tileset*/
	TILESET_WIDTH = al_get_bitmap_width(TileSet) / TILE_WIDTH;
	TILESET_HEIGHT = al_get_bitmap_height(TileSet) / TILE_HEIGHT;
	//cout << "filepath = " << filepath << "\n";
	//cout << "TILESET_WIDTH = " << TILESET_WIDTH << " TILESET_HEIGHT = " << TILESET_HEIGHT << "\n";

	return TileSet;
}

/*Given the path of a .csv file, returns a vector<vector<int>> of indices that correspondes to a TileSet*/
vector<vector<int>> ReadTextMap(string TileMapFileName)
{
	ifstream TileMap(TileMapFileName);
	vector<vector<int>> content;
	vector<int> row;
	string line, word;

	if (TileMap.fail()) /*check for file not found*/
	{
		cout << "Failed to open " << TileMapFileName << " in function ReadTextMap().\n";
		exit(-1);
	}

	while (getline(TileMap, line))
	{
		row.clear();
		stringstream str(line);
		while (getline(str, word, ','))
		{
			row.push_back(stoi(word));
		}
		content.push_back(row);
	}

	/*for (int i = 0; i < content.size(); i++)
	{
		for (int j = 0; j < content[i].size(); j++)
		{
			cout << content[i][j] << " ";
		}
		cout << "\n";
	}*/

	return content;
}

/*Creates and returns a bitmap from an CSV and a Tileset, WILL NOT WORK IF A TILESET IS NOT LOADED (tileset width and height vars will have a value of 0)*/
ALLEGRO_BITMAP* Create_Bitmap_From_CSV(vector<vector<int>> CSV, ALLEGRO_BITMAP* Tileset, ALLEGRO_DISPLAY* display)
{
	assert(CSV.size != 0);
	ALLEGRO_BITMAP* bitmap = al_create_bitmap(CSV[0].size() * TILE_WIDTH, CSV.size() * TILE_HEIGHT);
	int TileSetIndex;

	//cout << "Create_Bitmap_From_CSV() height : " << al_get_bitmap_height(bitmap) << " width : " << al_get_bitmap_width(bitmap) << "\n";
	al_set_target_bitmap(bitmap);	/*Select the bitmap to which all subsequent drawing operations in the calling thread will draw.*/
	for (size_t y = 0; y < CSV.size(); y++)
	{
		for (size_t x = 0; x < CSV[0].size(); x++)
		{
			TileSetIndex = CSV[y][x];
			if (TileSetIndex != -1)
				al_draw_bitmap_region(Tileset, MUL_TILE_WIDTH(modIndex[TileSetIndex]), MUL_TILE_HEIGHT(divIndex[TileSetIndex]), TILE_WIDTH, TILE_HEIGHT, MUL_TILE_WIDTH(x), MUL_TILE_HEIGHT(y), 0);
		}
	}

	al_set_target_bitmap(al_get_backbuffer(display));/*Select the backbuffer to return to drawing to the screen normally.*/
	return bitmap;
}

/*Paints over the given bitmap, useful for maps with multiple layers*/
void Paint_To_Bitmap(ALLEGRO_BITMAP* bitmap, vector<vector<int>> CSV, ALLEGRO_BITMAP* Tileset, ALLEGRO_DISPLAY* display)
{
	assert(CSV.size != 0);
	int TileSetIndex;

	al_set_target_bitmap(bitmap);	/*Select the bitmap to which all subsequent drawing operations in the calling thread will draw.*/
	for (size_t y = 0; y < CSV.size(); y++)
	{
		for (size_t x = 0; x < CSV[0].size(); x++)
		{
			TileSetIndex = CSV[y][x];
			if (TileSetIndex != -1)
				al_draw_bitmap_region(Tileset, MUL_TILE_WIDTH(modIndex[TileSetIndex]), MUL_TILE_HEIGHT(divIndex[TileSetIndex]), TILE_WIDTH, TILE_HEIGHT, MUL_TILE_WIDTH(x), MUL_TILE_HEIGHT(y), 0);
		}
	}

	al_set_target_bitmap(al_get_backbuffer(display));/*Select the backbuffer to return to drawing to the screen normally.*/
}

/*uses the indeces from the CSV file which are stored in the CSV vector and uses them to blit from the given Tileset*/
void Draw_BitMap_From_CSV(vector<vector<int>> CSV, ALLEGRO_BITMAP* Tileset)
{
	assert(CSV.size != 0);
	ALLEGRO_BITMAP* bitmap = al_create_bitmap(CSV[0].size(), CSV.size());
	int TileSetIndex;

	//cout << "height : " << al_get_bitmap_height(bitmap) << " width : " << al_get_bitmap_width(bitmap) << "\n";
	//cout << "CSV[0].size() = " << CSV[0].size() << " CSV.size() = " << CSV.size() << "\n";
	for (auto y = 0; y < al_get_bitmap_height(bitmap); y++)
	{
		for (auto x = 0; x < al_get_bitmap_width(bitmap); x++)
		{
			TileSetIndex = CSV[y][x];
			if (TileSetIndex != -1)
				al_draw_bitmap_region(Tileset, MUL_TILE_WIDTH(modIndex[TileSetIndex]), MUL_TILE_HEIGHT(divIndex[TileSetIndex]), TILE_WIDTH, TILE_HEIGHT, MUL_TILE_WIDTH(x), MUL_TILE_HEIGHT(y), 0);
		}
	}
}

/*uses the indeces from the CSV file which are stored in the CSV vector and uses them to blit from the given Tileset with the given scaling*/
void Draw_Scaled_BitMap_From_CSV(vector<vector<int>> CSV, ALLEGRO_BITMAP* Tileset, float scaleWidth, float scaleHeight)
{
	assert(CSV.size != 0);
	ALLEGRO_BITMAP* bitmap = al_create_bitmap(CSV[0].size(), CSV.size());
	int TileSetIndex;

	for (auto y = 0; y < al_get_bitmap_height(bitmap); y++)
	{
		for (auto x = 0; x < al_get_bitmap_width(bitmap); x++)
		{
			TileSetIndex = CSV[y][x];
			al_draw_scaled_bitmap(Tileset, MUL_TILE_WIDTH(modIndex[TileSetIndex]), MUL_TILE_HEIGHT(divIndex[TileSetIndex]), TILE_WIDTH, TILE_HEIGHT, scaleWidth * MUL_TILE_WIDTH(x), scaleHeight * MUL_TILE_HEIGHT(y), scaleWidth * TILE_WIDTH, scaleHeight * TILE_HEIGHT, 0);
		}
	}
}

/*By screen i mean the target bitmap*/
void Paint_Player_to_Screen(Rect r)
{
	al_draw_bitmap_region(PlayerSpriteSheet, r.x, r.y, r.w, r.h, player->positionX, player->positionY, 0);
}

void Load_Start_Screen()
{
	Start_Screen_bitmap = al_load_bitmap(START_SCREEN_PATH);
	if (Start_Screen_bitmap == NULL)
	{
		fprintf(stderr, "\nFailed to initialize Start_Screen_bitmap (al_load_bitmap() failed).\n");
		exit(-1);
	}
	al_draw_bitmap(Start_Screen_bitmap, 0, 0, 0);
	al_flip_display();
	//cout << "Drawing " << filepath << '\n';
}

void Load_Player_Spiresheet()
{
	PlayerSpriteSheet = al_load_bitmap(LINK_SPRITES_PATH);
	if (PlayerSpriteSheet == NULL)
	{
		fprintf(stderr, "\nFailed to initialize PlayerSpriteSheet (al_load_bitmap() failed).\n");
		exit(-1);
	}
}

void Load_Level(unsigned short levelNum)
{
	TileMapCSV.push_back(ReadTextMap("UnitTests\\Media\\Level_1\\Level_1_Tile Layer 1.csv"));
	TileMapCSV.push_back(ReadTextMap("UnitTests\\Media\\Level_1\\Level_1_Tile Layer 2.csv"));
	if (TileMapCSV.size() == 0)
	{
		fprintf(stderr, "ReadTextMap returned empty vector.\n");
		exit(-1);
	}

	//create the bitmap of the level
	bitmap = Create_Bitmap_From_CSV(TileMapCSV[0], TileSet, display);
	Paint_To_Bitmap(bitmap, TileMapCSV[1], TileSet, display);
	cout << "Created the whole bitmap\n";

	add_Grid(0, TILE_WIDTH, TILE_HEIGHT, TileMapCSV[0][0].size(), TileMapCSV[0].size());		//initialize the grid for layer 1
	grids[0]->ComputeTileGridBlocks2(TileMapCSV[0], TileSet, 128);
	cout << "Created the whole grid for layer 1\n";

	add_Grid(1, TILE_WIDTH, TILE_HEIGHT, TileMapCSV[1][0].size(), TileMapCSV[1].size());  		//initialize the grid for layer 2
	grids[1]->ComputeTileGridBlocks2(TileMapCSV[1], TileSet, 64);
	cout << "Created the whole grid for layer 2\n";

	createElevators();

	if (PlayerSpriteSheet == NULL)
	{
		Load_Player_Spiresheet();
		Init_Player(StartPlayerPositionX, StartPlayerPositionY);
	}
}

void Render_init()
{
	FPStimer = al_create_timer(1.0 / FPS);
	timerQueue = al_create_event_queue();
	al_register_event_source(timerQueue, al_get_timer_event_source(FPStimer));
	al_start_timer(FPStimer);	//a video said not to initialize any variables after this cuz it might mess up the timer, we'll see

	al_set_new_display_flags(ALLEGRO_WINDOWED);
	display = al_create_display(DISPLAY_W, DISPLAY_H);
	if (!display)
	{
		fprintf(stderr, "failed to create display!\n");
		exit(-1);
	}
	al_set_window_title(display, "Zelda II: The Adventure of Link");


	TileSet = load_tileset(TILESET_PATH);
	Calculate_Tileset_Indexes();//pre-caching
	//initalize the empty colors of the tileset
	for (int i = 0; i < LAYERS; i++) {
		string tmp = ASSUMED_EMPTY_LAYER_PATH;
		cout << tmp;
		string txt = ".txt";
		cout << tmp;
		tmp = tmp + to_string(i + 1);
		tmp = tmp + txt;
		cout << tmp;
		Init_emptyTileColorsHolder(tmp.c_str());
	}
}

void Scroll_Bitmap()
{
	if (player->positionX > DISPLAY_W)	//scroll left
	{
		cameraX -= DISPLAY_W;
		player->screenX++;
		player->positionX = 5;	//scroll the player to the left side of the screen
	}
	else if (player->positionX < 0) //scroll right
	{
		cameraX += DISPLAY_W;
		player->screenX--;
		player->positionX = DISPLAY_W - 10;
	}
	else if (player->positionY / DISPLAY_H > cameraY / DISPLAY_H)	//check if this statemnet if correct
	{

	}
}

// use this to render grid (toggle on / off), used only for development time testing -
// a tile grid block is consecutive GRID_BLOCK_ROWS x GRID_BLOCK_COLUMNS block of grid indices
void DisplayGrid(unsigned int grid_num)
{

	if (grids[grid_num] == NULL)
	{
		return;
	}

	auto startCol = DIV_TILE_WIDTH(cameraX * DISPLAY_W);
	auto startRow = DIV_TILE_HEIGHT(cameraY * DISPLAY_H);
	auto endCol = DIV_TILE_WIDTH(cameraX * DISPLAY_W + 1 * DISPLAY_W);
	auto endRow = DIV_TILE_HEIGHT(cameraY * DISPLAY_H + 1 * DISPLAY_H);

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
			for (auto rowElem = 0; rowElem < grids[grid_num]->Grid_Block_Rows; ++rowElem)
			{
				for (auto colElem = 0; colElem < grids[grid_num]->Grid_Block_Columns; ++colElem)
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
		if (GameState == PlayingLevel1)
		{
			al_clear_to_color(al_map_rgb(0, 0, 0));	//Clear the complete target bitmap, but confined by the clipping rectangle.
			if (levelLoaded == -1)
			{
				levelLoaded = 1;
				Load_Level(1);
			}


			al_draw_bitmap(bitmap, cameraX, cameraY, 0);
			Paint_Player_to_Screen(player->FrameToDraw());
			if (Toggle_Grid)
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
	for (int i = 0; i < grids.size(); i++) {
		delete grids[i];
	}
	delete divIndex;
	delete modIndex;
}