#include "..\\Headers\\GameObjectsClasses.h"

//Start of global variables

std::vector<TileColorsHolder> emptyTileColors;
GameLogic gameObj;
Player* player = NULL;
std::vector<Elevator> elevators;
std::vector<Enemy*> Enemies;
std::vector<Collectable*> Collectables;
std::vector<Door*> Doors;

bool keyboardUp = false, scrollDown = true, scrollLeft = false, scrollRight = false;

//End of global variables

//Start of class DrawOrder

DrawOrder::DrawOrder(ALLEGRO_BITMAP* bmp, unsigned int sx, unsigned int sy, unsigned int w, unsigned int h, int xPos, int yPos)
{
	this->bitmap = bmp;
	this->sx = sx;
	this->sy = sy;
	this->w = w;
	this->h = h;
	this->xPos = xPos;
	this->yPos = yPos;
}

//End of class DrawOrder

//Start of Class Level


void Level::load_tileset(const char* filepath)
{
	this->TileSet = al_load_bitmap(filepath);
	if (this->TileSet == NULL)
	{
		fprintf(stderr, "\nFailed to initialize Tileset (al_load_bitmap() failed).\n");
		exit(-1);
	}
	/*initialize TILESET_WIDTH and TILESET_HEIGHT base on the given tileset*/
	this->TILESET_WIDTH = al_get_bitmap_width(TileSet) / TILE_WIDTH;
	this->TILESET_HEIGHT = al_get_bitmap_height(TileSet) / TILE_HEIGHT;
	//cout << "filepath = " << filepath << "\n";
	//cout << "TILESET_WIDTH = " << TILESET_WIDTH << " TILESET_HEIGHT = " << TILESET_HEIGHT << "\n";
}

void Level::Calculate_Tileset_Indexes()
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


vector<vector<int>> Level::ReadTextMap(string TileMapFileName)
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


ALLEGRO_BITMAP* Level::Create_Bitmap_From_CSV(vector<vector<int>> CSV, ALLEGRO_BITMAP* Tileset, ALLEGRO_DISPLAY* display)
{
	assert(CSV.size != 0);
	ALLEGRO_BITMAP* bitmap = al_create_bitmap(CSV[0].size() * TILE_WIDTH, CSV.size() * TILE_HEIGHT);
	int TileSetIndex;

	// Enable blending and set the blending mode to use the alpha channel
	al_set_blender(ALLEGRO_ADD, ALLEGRO_ALPHA, ALLEGRO_INVERSE_ALPHA);
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

/*Paints over the given bitmap*/
void Level::Paint_To_Bitmap(ALLEGRO_BITMAP* bitmap, vector<vector<int>> CSV, ALLEGRO_BITMAP* Tileset, ALLEGRO_DISPLAY* display)
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


void Level::Scroll_Bitmap()
{
	if (player->positionX > DISPLAY_W)	//scroll left
	{
		cameraX -= DISPLAY_W;
		ScreenX++;
		player->screenX++;
		player->positionX = 5;	//scroll the player to the left side of the screen
	}
	else if (player->positionX < 0) //scroll right
	{
		cameraX += DISPLAY_W;
		ScreenX--;
		player->screenX--;
		player->positionX = DISPLAY_W - 10;
	}
	else if (player->positionY > DISPLAY_H)	//check if this statemnet if correct
	{
		cameraY -= DISPLAY_H;
		ScreenY++;
		player->screenY++;
		player->positionY = 5;
	}
	else if (player->positionY < 0)	//check if this statemnet if correct
	{
		cameraY += DISPLAY_H;
		ScreenY--;
		player->screenY--;
		player->positionY = DISPLAY_H - 10;
	}
}

void Level::Load_Enemy_SpriteSheets()
{
	EnemySpriteSheetLeft = al_load_bitmap(ENEMY1_SPRITES_FLIPPED_PATH);
	EnemySpriteSheetRight = al_load_bitmap(ENEMY1_SPRITES_PATH);

	if (EnemySpriteSheetLeft == NULL || EnemySpriteSheetRight == NULL)
	{
		fprintf(stderr, "\nLevel : Failed to initialize Enemy SpriteSheets (al_load_bitmap() failed).\n");
		exit(-1);
	}
}

/*Must be called after loading the csv files, enemy type and position is based on transparent tiles in the csv
and after creating the grids because some tiles of the grid will be changed*/
void Level::Load_Enemies()
{
	for (unsigned int i = 0; i < TileMapCSV[1].size(); i++)
	{
		for (unsigned int j = 0; j < TileMapCSV[1][i].size(); j++)
		{
			// i goes through rows == height == Y, j goes through columns == width == X
			if (TileMapCSV[1][i][j] == STALFOS_ID)
			{
				//"spawner" tiles are always place at the bottom so for a stalfos to be drawn correctly we must go 1 tile up
				add_Stalfos(j * TILE_WIDTH, i * TILE_HEIGHT -TILE_HEIGHT);
				grids[1]->SetEmptyGridTile(i, j);	//some transparent tiles that i use for spawn marking are marked as solid which is wrong
			}
			else if (TileMapCSV[1][i][j] == PALACE_BOT_ID)
			{
				add_PalaceBot(j * TILE_WIDTH, i * TILE_HEIGHT);
				grids[1]->SetEmptyGridTile(i, j);
			}
			else if (TileMapCSV[1][i][j] == WOSU_ID)
			{
				add_Wosu(j * TILE_WIDTH, i * TILE_HEIGHT - TILE_HEIGHT);
				grids[1]->SetEmptyGridTile(i, j);
			}
			else if (TileMapCSV[1][i][j] == GUMA_ID)
			{
				add_Guma(j * TILE_WIDTH, i * TILE_HEIGHT - TILE_HEIGHT);
				grids[1]->SetEmptyGridTile(i, j);
			}
		}
	}
}

void Level::Load_Object_SpriteSheets()
{
	ObjectSpriteSheet = al_load_bitmap(ITEMS_OBJECTS_PATH);

	if (ObjectSpriteSheet == NULL)
	{
		fprintf(stderr, "\nLevel : Failed to initialize Collectable SpriteSheets (al_load_bitmap() failed).\n");
		exit(-1);
	}

	/*Remove purple background*/
	al_lock_bitmap(ObjectSpriteSheet, ALLEGRO_PIXEL_FORMAT_ANY, ALLEGRO_LOCK_READWRITE);
	al_set_target_bitmap(ObjectSpriteSheet);

	ALLEGRO_COLOR purple = al_get_pixel(ObjectSpriteSheet, 2, 15);
	ALLEGRO_COLOR c;
	unsigned char pr, pg, pb;
	unsigned char r, g, b;

	al_unmap_rgb(purple, &pr, &pg, &pb);
	for (auto i = 0; i < al_get_bitmap_width(ObjectSpriteSheet); i++)
	{
		for (auto j = 0; j <  al_get_bitmap_height(ObjectSpriteSheet); j++)
		{
			c = al_get_pixel(ObjectSpriteSheet, i, j);
			
			al_unmap_rgb(c, &r, &g, &b);

			if (pr == r && pg == g && pb == b)	//may be a faster comparison if i do !(pr != r || pg != g || pb != b)
			{
				al_put_pixel(i, j, al_map_rgba(r, g, b, 0));
			}
		}
	}
	al_set_target_bitmap(al_get_backbuffer(gameObj.display));
	al_unlock_bitmap(ObjectSpriteSheet);
}

void Level::Load_Objects()
{
	for (unsigned int i = 0; i < TileMapCSV[1].size(); i++)
	{
		for (unsigned int j = 0; j < TileMapCSV[1][i].size(); j++)
		{
			// i goes through rows == height == Y, j goes through columns == width == X
			if (TileMapCSV[1][i][j] == BLUEPOTION1_ID)
			{
				add_BluePotion(j * TILE_WIDTH, i * TILE_HEIGHT, 30, 1);
				grids[1]->SetEmptyGridTile(i, j);	//some transparent tiles that i use for spawn marking are marked as solid which is wrong
			}
			else if (TileMapCSV[1][i][j] == BLUEPOTION2_ID)
			{
				add_BluePotion(j * TILE_WIDTH, i * TILE_HEIGHT, 24, 2);
				grids[1]->SetEmptyGridTile(i, j);
			}
			else if (TileMapCSV[1][i][j] == BLUEPOTION3_ID)
			{
				add_BluePotion(j * TILE_WIDTH, i * TILE_HEIGHT, 16, 3);
				grids[1]->SetEmptyGridTile(i, j);
			}
			else if (TileMapCSV[1][i][j] == REDPOTION1_ID)
			{
				add_RedPotion(j * TILE_WIDTH, i * TILE_HEIGHT, 128, 1);
				grids[1]->SetEmptyGridTile(i, j);
			}
			else if (TileMapCSV[1][i][j] == REDPOTION2_ID)
			{
				add_RedPotion(j * TILE_WIDTH, i * TILE_HEIGHT, 96, 2);
				grids[1]->SetEmptyGridTile(i, j);
			}
			else if (TileMapCSV[1][i][j] == REDPOTION3_ID)
			{
				add_RedPotion(j * TILE_WIDTH, i * TILE_HEIGHT, 60, 3);
				grids[1]->SetEmptyGridTile(i, j);
			}
			else if (TileMapCSV[1][i][j] == POINTBAG_ID)
			{
				add_PointBag(j * TILE_WIDTH, i * TILE_HEIGHT);
				grids[1]->SetEmptyGridTile(i, j);
			}
			else if (TileMapCSV[1][i][j] == UPDOLL_ID)
			{
				add_UpDoll(j * TILE_WIDTH, i * TILE_HEIGHT);
				grids[1]->SetEmptyGridTile(i, j);
			}
			else if (TileMapCSV[1][i][j] == SIMPLE_KEY_ID)
			{
				add_SimpleKey(j * TILE_WIDTH, i * TILE_HEIGHT);
				grids[1]->SetEmptyGridTile(i, j);
			}
		}
	}
}
//End of Class Level

//Start of Class GameLogic

void GameLogic::Play_Music(const char* path)
{
	if (song != NULL)
	{
		al_destroy_sample(song);
		song = NULL;
	}
	if (songInstance != NULL)
	{
		al_destroy_sample_instance(songInstance);
		songInstance = NULL;
	}

	song = al_load_sample(path);
	if (song == NULL)
	{
		cout << "Error in game object : sample song empty\n";
		exit(-1);
	}
	songInstance = al_create_sample_instance(song);
	al_set_sample_instance_playmode(songInstance, ALLEGRO_PLAYMODE_LOOP);
	al_attach_sample_instance_to_mixer(songInstance, al_get_default_mixer());
	if (!al_play_sample_instance(songInstance))
	{
		cout << "Error in game object when playing start screen music...\n";
		exit(-1);
	}
}

void GameLogic::Stop_Music()
{
	al_stop_sample_instance(songInstance);
}

void GameLogic::Clear()
{
	al_destroy_sample(song);
	al_destroy_sample_instance(songInstance);
}

Game_State GameLogic::Get_State()
{
	return this->GameState;
}

void GameLogic::Set_State(Game_State state)
{
	this->GameState = state;
}

void GameLogic::End_Game()
{
	this->GameState = GameFinished;
}

void GameLogic::Load_Level(unsigned short levelNum)
{
	Init_Player(StartPlayerPositionX, StartPlayerPositionY);
	
	Level* level = new Level();
	gameObj.level = level;
	level->load_tileset(TILESET_PATH);
	level->Calculate_Tileset_Indexes();//pre-caching
	level->TileMapCSV.push_back(level->ReadTextMap("UnitTests\\Media\\Level_1\\Level_1_Tile Layer 1.csv"));
	level->TileMapCSV.push_back(level->ReadTextMap("UnitTests\\Media\\Level_1\\Level_1_Tile Layer 2.csv"));
	if (level->TileMapCSV.size() == 0)
	{
		fprintf(stderr, "ReadTextMap returned empty vector.\n");
		exit(-1);
	}


	//initalize the empty colors of the tileset
	for (int i = 0; i < LEVEL_LAYERS; i++) {
		string tmp = ASSUMED_EMPTY_LAYER_PATH;
		string txt = ".txt";
		tmp = tmp + to_string(i + 1);
		tmp = tmp + txt;
		Init_emptyTileColorsHolder(tmp.c_str());
	}

	//create the bitmap of the level
	level->bitmaps.push_back(level->Create_Bitmap_From_CSV(level->TileMapCSV[0], level->TileSet, gameObj.display));
	level->bitmaps.push_back(level->Create_Bitmap_From_CSV(level->TileMapCSV[1], level->TileSet, gameObj.display));
	cout << "Created Level bitmap\n";

	add_Grid(0, TILE_WIDTH, TILE_HEIGHT, level->TileMapCSV[0][0].size(), level->TileMapCSV[0].size());	//initialize the grid for layer 1
	level->grids[0]->ComputeTileGridBlocks2(level->TileMapCSV[0], level->TileSet, 128);
	cout << "Created grid for layer 1\n";

	add_Grid(1, TILE_WIDTH, TILE_HEIGHT, level->TileMapCSV[1][0].size(), level->TileMapCSV[1].size());  //initialize the grid for layer 2
	level->grids[1]->ComputeTileGridBlocks2(level->TileMapCSV[1], level->TileSet, 64);
	cout << "Created grid for layer 2\n";

	level->Load_Enemy_SpriteSheets();
	cout << "Loaded Enemies sprite sheets\n";
	level->Load_Enemies();
	cout << "Loaded Enemies\n";
	level->Load_Object_SpriteSheets();
	cout << "Loaded Object spritesheet\n";
	level->Load_Objects();
	cout << "Loaded Objects\n";

	createElevators();

	//Level loaded play some music
	//gameObj.Play_Music(LEVEL_1_MUSIC);
}

void GameLogic::insert_DrawingOrder(DrawOrder *dro, unsigned int layer)
{
	DrawingOrder[layer].push_back(dro);
}

//End of Class GameLogic 

//start of Class Elevator functions

Elevator::Elevator(unsigned int Row, unsigned int Col) 
{
	this->row = Row;
	this->col = Col;
	this->curr_row = Row + 4 * TILE_HEIGHT;	//to point to the bottom part of the elevator
	this->DrawObj = NULL;

	this->elevatorbitmap = al_create_bitmap(2 * TILE_WIDTH, TILE_HEIGHT); //create a bitmap for the bottom part of the elevator with dimensions (16,2x16)
	al_set_target_bitmap(elevatorbitmap);

	al_draw_bitmap_region(gameObj.level->TileSet, MUL_TILE_WIDTH(gameObj.level->modIndex[ELEVATORID1]), MUL_TILE_HEIGHT(gameObj.level->divIndex[ELEVATORID1]), TILE_WIDTH, TILE_HEIGHT, MUL_TILE_WIDTH(0), MUL_TILE_HEIGHT(0), 0);
	al_draw_bitmap_region(gameObj.level->TileSet, MUL_TILE_WIDTH(gameObj.level->modIndex[ELEVATORID2]), MUL_TILE_HEIGHT(gameObj.level->divIndex[ELEVATORID2]), TILE_WIDTH, TILE_HEIGHT, MUL_TILE_WIDTH(1), MUL_TILE_HEIGHT(0), 0);

	al_set_target_bitmap(al_get_backbuffer(gameObj.display));
}

void Elevator::Add_to_draw_queue()
{
	if (this->DrawObj != NULL)	//in case i try to add to the queue again
		return;

	DrawOrder* drawObj = new DrawOrder(this->elevatorbitmap, 0, 0, 2*TILE_WIDTH, TILE_HEIGHT, this->col, this->curr_row);
	this->DrawObj = drawObj;
	gameObj.insert_DrawingOrder(drawObj, 1);
}

/*updates the height (row or y) of the draw object*/
void Elevator::Update_draw_obj()
{
	assert(this->DrawObj != NULL);

	this->DrawObj->yPos = this->curr_row;
}

void Elevator::hide_og_elevator() 
{
	assert(gameObj.level->bitmaps.size() > 1);

	al_lock_bitmap(gameObj.level->bitmaps[1], ALLEGRO_PIXEL_FORMAT_ANY, ALLEGRO_LOCK_READWRITE);
	al_set_target_bitmap(gameObj.level->bitmaps[1]);
	//(row+4) because row points to the upper part of the elevator and we want to hide the lower part which is 4 tiles apart
	for (unsigned int y = row + 4*TILE_HEIGHT; y < row +4*TILE_HEIGHT + TILE_HEIGHT; y++) {
		for (unsigned int x = col; x < col + 2 * TILE_WIDTH; x++) {
			ALLEGRO_COLOR color = al_get_pixel(gameObj.level->bitmaps[1], x, y);
			unsigned char r, g, b;

			al_unmap_rgb(color, &r, &g, &b);
			//cout << "r = " << (int)r << ", g = " << (int)g << ", b = " << (int)b << ", a = " << (int)a << "\n";
			al_put_pixel(x, y, al_map_rgba(r, g, b, 0));

		}
	}
	al_set_target_bitmap(al_get_backbuffer(gameObj.display));
	al_unlock_bitmap(gameObj.level->bitmaps[1]);
}


unsigned int Elevator::getRow()
{
	return this->row;
}

unsigned int Elevator::getCurrRow()
{
	return this->curr_row;
}

unsigned int Elevator::getCol()
{
	return this->col;
}

void Elevator::setRow(unsigned int Row)
{
	this->row = Row;
}

void Elevator::setCurrRow(unsigned int X)
{
	this->curr_row = X;
}

void Elevator::setCol(unsigned int Col)
{
	this->col = Col;
}

void Elevator::addToCurrRow(unsigned int X)
{
	this->curr_row += X;
}

//End of Class Elevator

void createElevators()
{
	bool isTopElev = false;	//flag to help skip the lower part of the elevator
	
	for (unsigned int row = 0; row < gameObj.level->TileMapCSV[1].size(); row++) {
		for (unsigned int col = 0; col < gameObj.level->TileMapCSV[1][row].size(); col++) {

			
			if (gameObj.level->TileMapCSV[1][row][col] == ELEVATORID1)	//found upper part
			{
				//search if i already added this elevator e.g. if i add the top part the for-loop will eventually hit the bottom part and it might mistake it as a new elevator
				for (unsigned int k = 0; k < elevators.size(); k++)
				{
					if (DIV_TILE_HEIGHT(elevators[k].getRow()) == row - 4 && DIV_TILE_WIDTH(elevators[k].getCol()) == col)	//if the upper part is already added don't create new elevator
					{
						isTopElev = true;
						break;
					}
				}

				if (isTopElev != true)
				{
					cout << "Created elevator at row = " << row << "and col = " << col << "\n";
					elevators.push_back(Elevator(MUL_TILE_HEIGHT(row), MUL_TILE_WIDTH(col)));
				}

				isTopElev = false;
			}
		}
	}
}

//Start of class Door

Door::Door(unsigned int _posX, unsigned int _posY) : posX(_posX), posY(_posY)
{
	this->Init_Frames();
}

void Door::Init_Frames()
{
	Rect* r = NULL;

	for(unsigned short i = 0; i < 17; i++)
	{
		r = new Rect();
		r->x = 184 - 1 + i + i * OBJECT_SPRITE_WIDTH;
		if (i > 7)
			r->x -= OBJECT_SPRITE_WIDTH;	//offset because sprite sheet is bad
		r->y = 48 - 6;
		r->w = OBJECT_SPRITE_WIDTH;
		r->h = 3 * OBJECT_SPRITE_HEIGHT;

		this->UnlockFrames.push_back(r);
	}
}

Rect* Door::Get_Frame(unsigned short i)
{
	return this->UnlockFrames[i];
}

void Door::Add_To_Draw_Queue()
{
	if (this->DrawObj != NULL)
		return;

	Rect *r = this->Get_Frame(0);
	this->DrawObj = new DrawOrder(gameObj.level->ObjectSpriteSheet, r->x, r->y, r->w, r->h, this->posX, this->posY);
	gameObj.insert_DrawingOrder(this->DrawObj, 1);
}

/*removes the DrawOrder object from the draw queue, also destroys it*/
void Door::Remove_From_Draw_Queue()
{
	for (unsigned short i = 0; i < gameObj.DrawingOrder[1].size(); i++)
	{
		if (gameObj.DrawingOrder[1][i] == this->DrawObj)
		{
			gameObj.DrawingOrder[1].erase(gameObj.DrawingOrder[1].begin() + i);
			delete this->DrawObj;
			this->DrawObj = NULL;
			//cout << "Removed door draw obj from queue\n";
		}
	}
}

void Door::Update_Draw_Obj()
{
	assert(this->DrawObj != NULL);

	Rect* r = this->Get_Frame(this->DoorSpriteNum);
	this->DrawObj->sx = r->x;
}

void Door::SetActive(bool act)
{
	this->is_active = act;
}

bool Door::IsActive()
{
	return this->is_active;
}

//End of class Door

//Class Player functions

Player::Player(int _positionX, int _positionY)
{
	positionX = _positionX;
	positionY = _positionY;
	screenX = 0;
	screenY = 0;
}

Player::~Player()
{

}

void Player::Set_Speed_X(int speedX)
{
	this->scrollDistanceX = speedX;
}
void Player::Increment_Speed_X()
{
	this->scrollDistanceX++;
}
int Player::Get_Speed_X()
{
	return this->scrollDistanceX;
}
void Player::Set_Speed_Y(int speedY)
{
	this->scrollDistanceY = speedY;
}
void Player::Increment_Speed_Y()
{
	this->scrollDistanceY++;
}
void Player::Decrement_Speed_Y()
{
	this->scrollDistanceY--;
}
int Player::Get_Speed_Y()
{
	return this->scrollDistanceY;
}

void Player::Set_Direction(Direction direction)
{
	if (direction != dir_left && direction != dir_right)
	{
		cout << "Error : invalid direction \"" << direction << "\"\n";
		exit(-1);
	}

	this->direction = direction;
}

Direction Player::Get_Direction()
{
	return this->direction;
}

void Player::Set_State(Player_State state)
{
	if (this->state == State_Walking && state == State_Crounching)	//Walking -> Crounching
	{
		this->Scroll_Player(5, 5);	//scroll because the sprite is a bit shorter than then rest, scroll before changing the state because in crounch state you can't scroll
		this->state = state;
	}
	else if (this->state == State_Walking && state == State_Attacking) //Walking -> Attacking
	{
		this->state = state;
	}
	else if (this->state == State_Walking && state == State_Elevator) //Walking -> Elevator
	{
		this->state = state;
	}
	else if (this->state == State_Elevator && state == State_Walking) //Elevator -> Walking
	{
		this->state = state;
	}
	else if (this->state == State_Crounching && state == State_Walking) //Crounching -> Walking
	{
		this->state = State_Walking;	//to enable scrolling
		this->Scroll_Player(-5, -5);
	}
	else if (this->state == State_Crounching && state == State_Attacking)	//Crounching -> CrounchAttacking
	{
		this->state = State_CrounchAttacking;
		//this->state = state;
	}
	else if ((this->state == State_CrounchAttacking && state == State_Crounching)) { //CrounchAttacking -> Crouching
		this->state = State_Crounching;
	}
	else if (this->state == State_Attacking && state == State_Walking) //Attacking -> Walking
	{
		this->state = state;
	}
}

Player_State Player::Get_State()
{
	return this->state;
}

void Player::Scroll_Player(float ScrollDistanceX, float ScrollDistanceY)
{
	if (state == State_Walking)
	{
		this->positionX += ScrollDistanceX;
		this->positionY += ScrollDistanceY;
	}
	else if (state == State_Elevator)	//allow the player only to fall (and also jump)
	{
		this->positionY += ScrollDistanceY;
	}
	/*else if ((state == State_CrounchAttacking || state == State_Crounching) && ScrollDistanceY > 0)	//allow the player only to fall
	{
		this->positionY += ScrollDistanceY;
	}*/
}

void Player::Init_frames_bounding_boxes()
{
	Rect* r;
	int i = 0;

	//FramesWalkingLeft
	for (i = 0; i < 4; i++)
	{
		r = new Rect;
		r->h = LINK_SPRITE_HEIGHT * 2;
		r->w = LINK_SPRITE_WIDTH;
		r->y = 0;
		FramesWalkingLeft.push_back(*r);
	}
	FramesWalkingLeft[0].x = LINK_SPRITE_WIDTH / 2;
	FramesWalkingLeft[1].x = 3 * LINK_SPRITE_WIDTH;
	FramesWalkingLeft[2].x = 5 * LINK_SPRITE_WIDTH + LINK_SPRITE_WIDTH / 2;
	FramesWalkingLeft[3].x = 8 * LINK_SPRITE_WIDTH;
	std::reverse(FramesWalkingLeft.begin(), FramesWalkingLeft.end());

	//FramesWalkingRight
	for (i = 0; i < 4; i++)
	{
		r = new Rect;
		r->h = LINK_SPRITE_HEIGHT * 2;
		r->w = LINK_SPRITE_WIDTH;
		r->y = 0;
		FramesWalkingRight.push_back(*r);
	}
	FramesWalkingRight[0].x = 10 * LINK_SPRITE_WIDTH + LINK_SPRITE_WIDTH / 2;
	FramesWalkingRight[1].x = 13 * LINK_SPRITE_WIDTH;
	FramesWalkingRight[2].x = 15 * LINK_SPRITE_WIDTH + LINK_SPRITE_WIDTH / 2;
	FramesWalkingRight[3].x = 18 * LINK_SPRITE_WIDTH;

	//FramesCrounch (left)
	r = new Rect;
	r->h = LINK_SPRITE_HEIGHT + LINK_SPRITE_HEIGHT;
	r->w = LINK_SPRITE_WIDTH;
	r->y = LINK_SPRITE_HEIGHT * 2 + 10;	//10 pixels offset for the next row of sprites
	r->x = LINK_SPRITE_WIDTH * 3;
	FramesCrounch.push_back(*r);

	//FramesCrounch (right)
	r = new Rect;
	r->h = LINK_SPRITE_HEIGHT + LINK_SPRITE_HEIGHT;
	r->w = LINK_SPRITE_WIDTH;
	r->y = LINK_SPRITE_HEIGHT * 2 + 10;	//0 pixels offset for the next row of sprites
	r->x = LINK_SPRITE_WIDTH * 15 + LINK_SPRITE_WIDTH / 2;
	FramesCrounch.push_back(*r);

	//FramesSlashLeft
	r = new Rect;
	r->h = LINK_SPRITE_HEIGHT * 2;
	r->w = LINK_SPRITE_WIDTH * 2;
	r->y = LINK_SPRITE_HEIGHT * 2 + 10;	//10 pixels offset for the next row of sprites
	r->x = LINK_SPRITE_WIDTH * 7 + 10;
	FramesSlashLeft.push_back(*r);

	r = new Rect;
	r->h = LINK_SPRITE_HEIGHT * 2;
	r->w = LINK_SPRITE_WIDTH * 2;
	r->y = LINK_SPRITE_HEIGHT * 2 + 10;	//10 pixels offset for the next row of sprites
	r->x = LINK_SPRITE_WIDTH * 5;
	FramesSlashLeft.push_back(*r);

	//FramesSlashRight
	r = new Rect;
	r->h = LINK_SPRITE_HEIGHT * 2;
	r->w = LINK_SPRITE_WIDTH * 2;
	r->y = LINK_SPRITE_HEIGHT * 2 + 10;	//10 pixels offset for the next row of sprites
	r->x = LINK_SPRITE_WIDTH * 10;
	FramesSlashRight.push_back(*r);

	r = new Rect;
	r->h = LINK_SPRITE_HEIGHT * 2;
	r->w = LINK_SPRITE_WIDTH * 2;
	r->y = LINK_SPRITE_HEIGHT * 2 + 10;	//10 pixels offset for the next row of sprites
	r->x = LINK_SPRITE_WIDTH * 12 + 10;
	FramesSlashRight.push_back(*r);

	//FramesCrounchSlashLeft
	r = new Rect;
	r->h = LINK_SPRITE_HEIGHT * 2;
	r->w = LINK_SPRITE_WIDTH * 2;
	r->y = LINK_SPRITE_HEIGHT * 2 + 10;
	r->x = 0;
	FramesCrounchSlash.push_back(*r);

	//FramesCrounchSlashRight
	r = new Rect;
	r->h = LINK_SPRITE_HEIGHT * 2;
	r->w = LINK_SPRITE_WIDTH * 2;
	r->y = LINK_SPRITE_HEIGHT * 2 + 10;
	r->x = LINK_SPRITE_WIDTH * 18;
	FramesCrounchSlash.push_back(*r);

}

void Player::Load_Player_Spritesheet() 
{

	this->PlayerSpriteSheet = al_load_bitmap(LINK_SPRITES_PATH);
	if (this->PlayerSpriteSheet == NULL)
	{
		fprintf(stderr, "\nFailed to initialize PlayerSpriteSheet (al_load_bitmap() failed).\n");
		exit(-1);
	}
}

Rect Player::FrameToDraw()
{
	if (state == State_Walking && direction == dir_left)	//Walking
	{
		return FramesWalkingLeft[LinkSpriteNum];
	}
	else if (state == State_Walking && direction == dir_right)
	{
		return FramesWalkingRight[LinkSpriteNum];
	}
	if (state == State_Elevator && direction == dir_left)	//Elevator
	{
		return FramesWalkingLeft[0];
	}
	else if (state == State_Elevator && direction == dir_right)
	{
		return FramesWalkingRight[0];
	}
	else if (state == State_Crounching)		//Crounching
	{
		return FramesCrounch[direction];
	}
	else if (state == State_Attacking && direction == dir_left)		//Attacking (standing)
	{
		return FramesSlashLeft[LinkSpriteNum];
	}
	else if (state == State_Attacking && direction == dir_right)
	{
		return FramesSlashRight[LinkSpriteNum];
	}
	else if (state == State_CrounchAttacking)		//Crounch Attacking
	{
		return FramesCrounchSlash[direction];
	}
	else
	{
		fprintf(stderr, "Error with player state : invalid value %d.\n", this->state);
		exit(-1);
	}
}

float Player::Get_Health()
{
	return this->Health;
}

void Player::Set_Health(float health)
{
	this->Health = health;
}

void Player::Take_Damage(float health_damage)
{
	this->Health -= health_damage;
}

void Player::Heal(float health_gain)
{
	this->Health += health_gain;
}

void Player::Set_HurtInvicibility(bool hi)
{
	this->HurtInvicibility = hi;
}

bool Player::Get_HurtInvicibility()
{
	return this->HurtInvicibility;
}

void Player::Add_Key()
{
	this->Keys++;
}

void Player::Remove_Key()
{
	if(this->Keys > 0)
		this->Keys--;
}

void Player::Set_Keys(unsigned short keys)
{
	this->Keys = keys;
}

unsigned short Player::Get_Keys()
{
	return this->Keys;
}

//end of class Player functions

//Start of Class TileColorsHolder

void TileColorsHolder::Insert(ALLEGRO_BITMAP* bmp, Index index)
{
	if (indices.find(index) == indices.end())
	{
		indices.insert(index);
		al_lock_bitmap(bmp, ALLEGRO_PIXEL_FORMAT_ANY, ALLEGRO_LOCK_READONLY);
		auto width = al_get_bitmap_width(bmp), height = al_get_bitmap_height(bmp);
		for (auto i = 0; i < width; i++)
		{
			for (auto j = 0; j < height; j++)
			{
				/*ALLEGRO_COLOR* color = new ALLEGRO_COLOR;
				*color =*/
				colors.insert(al_get_pixel(bmp, i, j));
			}
		}
		al_unlock_bitmap(bmp);
	}
}

bool TileColorsHolder::ColorIn(ALLEGRO_COLOR c) const
{
	set<ALLEGRO_COLOR>::iterator ptr = colors.begin();

	while (ptr != colors.end())
	{
		if (ptr->r == c.r && ptr->g == c.g && ptr->b == c.b)
		{
			return true;
		}
		ptr = next(ptr);
	}

	return false;
}

bool TileColorsHolder::IndexIn(Index index)
{
	return indices.find(index) != indices.end();
}

//End of Class TileColorsHolder

//Start of Class Grid

Grid::Grid(unsigned int layer_num, unsigned int _Grid_Element_Width, unsigned int _Grid_Element_Height, unsigned int MAX_WIDTH, unsigned int MAX_HEIGHT)
{
	Grid_Element_Width = _Grid_Element_Width;
	Grid_Element_Height = _Grid_Element_Height;
	layer = layer_num;


	if (TILE_WIDTH % Grid_Element_Width != 0)
	{
		cout << "TILE_WIDTH % GRID_ELEMENT_WIDTH must be zero!";
		exit(-1);
	}
	if (TILE_HEIGHT % Grid_Element_Height != 0)
	{
		cout << "TILE_HEIGHT % GRID_ELEMENT_HEIGHT must be zero!";
		exit(-1);
	}

	Grid_Block_Columns = TILE_WIDTH / Grid_Element_Width;
	Grid_Block_Rows = TILE_HEIGHT / Grid_Element_Height;
	Grid_Elements_Per_Tile = Grid_Block_Rows * Grid_Block_Columns;
	Grid_Max_Width = MAX_WIDTH * Grid_Block_Columns;	//MAX_WIDTH of the tilemap (bitmap, our level not the tileset)
	Grid_Max_Height = MAX_HEIGHT * Grid_Block_Rows;		//MAX_HEIGHT of the tilemap (number of tiles)

	/*cout << "Grid created with : Grid_Block_Columns = " << Grid_Block_Columns << " Grid_Block_Rows = " << Grid_Block_Rows << '\n';
	cout << "Grid_Elements_Per_Tile = " << Grid_Elements_Per_Tile << "\n";
	cout << "Grid_Max_Width = " << Grid_Max_Width << " Grid_Max_Height" << Grid_Max_Height << '\n';
	*/

	grid = new byte * [Grid_Max_Height];
	for (unsigned int i = 0; i < Grid_Max_Height; i++)
		grid[i] = new byte[Grid_Max_Width];

	MAX_PIXEL_WIDTH = MUL_TILE_WIDTH(MAX_WIDTH);
	MAX_PIXEL_HEIGHT = MUL_TILE_HEIGHT(MAX_HEIGHT);
}

Grid::~Grid()	//Destructor
{
	//Free the memory for the grid array
	for (unsigned int i = 0; i < Grid_Max_Height; i++)
		delete[] grid[i];
	delete[] grid;
}


void Grid::SetGridTile(unsigned short row, unsigned short col, byte index)
{
	grid[row][col] = index;
}

byte Grid::GetGridTile(unsigned short row, unsigned short col)
{
	return grid[row][col];
}

void Grid::SetSolidGridTile(unsigned short row, unsigned short col)
{
	SetGridTile(row, col, GRID_SOLID_TILE);
}

void Grid::SetEmptyGridTile(unsigned short row, unsigned short col)
{
	SetGridTile(row, col, GRID_EMPTY_TILE);
}

void Grid::SetGridTileFlags(unsigned short row, unsigned short col, byte flags)
{
	SetGridTile(row, col, flags);
}

void Grid::SetGridTileTopSolidOnly(unsigned short row, unsigned short col)
{
	SetGridTileFlags(row, col, GRID_TOP_SOLID_MASK);
}

bool Grid::CanPassGridTile(unsigned short row, unsigned short col, byte flags) // i.e. checks if flags set
{
	return (GetGridTile(row, col) & flags) == 0;	//changed from != 0
}

/*Returns the row based on the TILE_HEIGHT (not the Grid_element_height)*/
int Grid::getPlayerBottomRow(Player* player) {
	return (player->positionY + player->screenY * DISPLAY_H + LINK_SPRITE_HEIGHT * 2 - 1) / TILE_HEIGHT;
}

/*Returns the column based on the TILE_WIDTH (not the Grid_element_width)*/
int Grid::getPlayerLeftCol(Player* player) {
	return DIV_TILE_WIDTH(player->positionX + player->screenX * DISPLAY_W);
}

int Grid::getPlayerRightCol(Player* player) {
	return DIV_TILE_WIDTH(player->positionX + player->screenX * DISPLAY_W + LINK_SPRITE_WIDTH - 1);
}

int Grid::GetIndexFromLayer(int gridRow, int gridCol)
{
	//For a grid to exist it means that a level object already exists
	return gameObj.level->TileMapCSV[layer][gridRow][gridCol];
}

void Grid::FilterGridMotion(Player* player, int* dx, int* dy)
{
	assert(abs(*dx) <= Grid_Element_Width && abs(*dy) <= Grid_Element_Height);	//can't have the player pass 2 blocks instead of 1 and bypass some random wall

	// try horizontal move
	if (*dx < 0)
		FilterGridMotionLeft(player, dx);
	else if (*dx > 0)
		FilterGridMotionRight(player, dx);

	// try vertical move
	if (*dy < 0)
		FilterGridMotionUp(player, dy);
	else if (*dy > 0)
		FilterGridMotionDown(player, dy);
}

void Grid::FilterGridMotionLeft(Player* player, int* dx)
{
	auto x1_next = player->positionX + player->screenX * DISPLAY_W + *dx;
	if (x1_next < 0)
		*dx = -player->positionX;
	else
	{
		auto newCol = (x1_next) / Grid_Element_Width;
		auto currCol = (player->positionX + player->screenX * DISPLAY_W) / Grid_Element_Width;
		if (newCol != currCol)
		{
			assert(newCol + 1 == currCol); // we really move left
			auto startRow = (player->positionY + player->screenY * DISPLAY_H) / Grid_Element_Height;
			auto endRow = (player->positionY + player->screenY * DISPLAY_H + LINK_SPRITE_HEIGHT * 2 - 1) / Grid_Element_Height;
			//cout << "newCol : " << newCol << ", currCol : " << currCol << ", startRow : " << startRow << ", endrow : " << endRow << '\n';
			for (auto row = startRow; row <= endRow; ++row)
				if (!CanPassGridTile(row, newCol, GRID_LEFT_SOLID_MASK))
				{
					*dx = Grid_Element_Width * (currCol)-(player->positionX + player->screenX * DISPLAY_W);
					//cout << "*dx = " << *dx << "\n";
					break;
				}
		}
	}
}

void Grid::FilterGridMotionRight(Player* player, int* dx)
{
	unsigned int x2 = player->positionX + player->screenX * DISPLAY_W + LINK_SPRITE_WIDTH * 2 - 1;
	unsigned int x2_next = x2 + *dx;
	if (x2_next >= MAX_PIXEL_WIDTH)
		*dx = (MAX_PIXEL_WIDTH - 1) - x2;
	else
	{
		auto newCol = (x2_next) / Grid_Element_Width;
		auto currCol = (x2) / Grid_Element_Width;
		if (newCol != currCol)
		{
			assert(newCol - 1 == currCol); // we really move right
			auto startRow = (player->positionY + player->screenY * DISPLAY_H) / Grid_Element_Height;
			auto endRow = (player->positionY + player->screenY * DISPLAY_H + LINK_SPRITE_HEIGHT * 2 - 1) / Grid_Element_Height;
			//cout << "newCol : " << newCol << ", currCol : " << currCol << ", startRow : " << startRow << ", endrow : " << endRow << '\n';
			for (auto row = startRow; row <= endRow; ++row)
				if (!CanPassGridTile(row, newCol, GRID_RIGHT_SOLID_MASK))
				{
					*dx = (Grid_Element_Width * (newCol)-1) - (x2);
					//cout << "*dx = " << *dx << "\n";
					break;
				}
		}
	}
}

void Grid::FilterGridMotionUp(Player* player, int* dy)
{
	auto x1_next = player->positionY + player->screenY * DISPLAY_H + *dy;
	if (x1_next < 0)
		*dy = -player->positionY;
	else
	{
		auto newRow = (x1_next) / Grid_Element_Height;
		auto currRow = (player->positionY + player->screenY * DISPLAY_H) / Grid_Element_Height;
		if (newRow != currRow)
		{
			assert(newRow + 1 == currRow); // we really move up
			auto startCol = (player->positionX + player->screenX * DISPLAY_W) / Grid_Element_Width;
			auto endCol = (player->positionX + player->screenX * DISPLAY_W + LINK_SPRITE_WIDTH * 2 - 1) / Grid_Element_Width;
			//cout << "newRow : " << newRow << ", currRow : " << currRow << ", startCol : " << startCol << ", endCol : " << endCol << '\n';
			for (auto col = startCol; col <= endCol; ++col)
				if (!CanPassGridTile(newRow, col, GRID_TOP_SOLID_MASK))
				{
					*dy = Grid_Element_Height * (currRow)-(player->positionY + player->screenY * DISPLAY_H);
					//cout << "*dy = " << *dy << "\n";
					break;
				}
		}
	}
}

void Grid::FilterGridMotionDown(Player* player, int* dy)
{
	unsigned int x2 = player->positionY + player->screenY * DISPLAY_H + LINK_SPRITE_HEIGHT * 2 - 1;
	auto x2_next = x2 + *dy;
	if (x2_next >= MAX_PIXEL_HEIGHT)
		*dy = (MAX_PIXEL_HEIGHT - 1) - x2;
	else
	{
		auto newRow = (x2_next) / Grid_Element_Height;
		auto currRow = (x2) / Grid_Element_Height;
		if (newRow != currRow)
		{
			assert(newRow - 1 == currRow); // we really move down
			auto startCol = (player->positionX + player->screenX * DISPLAY_W) / Grid_Element_Width;
			auto endCol = (player->positionX + player->screenX * DISPLAY_W + LINK_SPRITE_WIDTH * 2 - 1) / Grid_Element_Width;
			//cout << "newRow : " << newRow << ", currRow : " << currRow << ", startCol : " << startCol << ", endCol : " << endCol << '\n';
			for (auto col = startCol; col <= endCol; ++col) {

				if (!CanPassGridTile(newRow, col, GRID_BOTTOM_SOLID_MASK))
				{
					player->jumping = false;
					*dy = (Grid_Element_Width * (newRow)-1) - (x2);
					//cout << "*dy = " << *dy << "\n";
					break;
				}
			}

		}
	}
}

/*void ComputeTileGridBlocks1(ALLEGRO_BITMAP* map)
{
	for (auto row = 0; row < al_get_bitmap_height(map); ++row)
		for (auto col = 0; col < MAX_WIDTH; ++col)
		{
			memset(
				grid,
				IsTileIndexAssumedEmpty(GetTile(map, col, row)) ? GRID_EMPTY_TILE : GRID_SOLID_TILE,
				GRID_ELEMENTS_PER_TILE
			);
			//grid += GRID_ELEMENTS_PER_TILE;
		}
}*/

bool Grid::ComputeIsGridIndexEmpty(ALLEGRO_BITMAP* gridElement, byte solidThreshold)
{
	auto n = 0;

	al_lock_bitmap(gridElement, ALLEGRO_PIXEL_FORMAT_ANY, ALLEGRO_LOCK_READONLY);
	for (byte i = 0; i < Grid_Element_Width; i++)
	{
		for (byte j = 0; j < Grid_Element_Height; j++)
		{
			ALLEGRO_COLOR c = al_get_pixel(gridElement, i, j);
			//byte r = 0, g = 0, b = 0;//, a = 0; i don't think i need alpha
			//byte rt = 0, gt = 0, bt = 0;//, at = 0;
			//al_unmap_rgb(c, &r, &g, &b);
			/*al_unmap_rgb(transColor, &rt, &gt, &bt);*/

			if (/*(r != rt && g != gt && b != bt) && */ !IsTileColorEmpty(layer, c))
				++n;
		}
	}//cout << "\nn = " << n << '\n';
	al_unlock_bitmap(gridElement);

	return n <= solidThreshold;
}

void Grid::ComputeTileGridBlocks2(vector<vector<int>> map, ALLEGRO_BITMAP* tileSet, byte solidThreshold)
{
	if (map.size() == 0)
	{
		cout << "ComputeTileGridBlocks2 got called with bad argument \"map\".";
		return;
	}
	auto tileElem = al_create_bitmap(TILE_WIDTH, TILE_HEIGHT);		//e.g. 16x16
	auto gridElem = al_create_bitmap(Grid_Element_Width, Grid_Element_Height);	//e.g. 8x8

	for (unsigned int row = 0; row < map.size(); ++row)
		for (unsigned int col = 0; col < map[0].size(); ++col)
		{
			auto index = map[row][col];
			al_set_target_bitmap(tileElem);	//set as target the tileElem and copy/paint the tile onto it
			al_draw_bitmap_region(tileSet, MUL_TILE_WIDTH(gameObj.level->modIndex[index]), MUL_TILE_HEIGHT(gameObj.level->divIndex[index]), TILE_WIDTH, TILE_HEIGHT, 0, 0, 0);
			al_set_target_bitmap(al_get_backbuffer(gameObj.display));	//reset the target back to the display

			if (IsTileIndexAssumedEmpty(layer, index) || index == -1)
			{
				//printf("this->grid[%d][%d] = %d\n", row, col, this->grid[row][col]);
				this->grid[row][col] = GRID_EMPTY_TILE;
			}
			else /*subdivide the tile Grid_Elements_Per_Tile times and check the colors of each sub-tile to see which are solid*/
			{
				al_set_target_bitmap(gridElem);
				for (unsigned int i = 0; i < Grid_Elements_Per_Tile; i++)
				{
					auto x = i % Grid_Block_Rows;
					auto y = i / Grid_Block_Rows;

					al_draw_bitmap_region(tileSet, MUL_TILE_WIDTH(gameObj.level->modIndex[index]) + x * Grid_Element_Width, MUL_TILE_HEIGHT(gameObj.level->divIndex[index]) + y * Grid_Element_Height, Grid_Element_Width, Grid_Element_Height, 0, 0, 0);

					auto isEmpty = ComputeIsGridIndexEmpty(gridElem, solidThreshold);
					this->grid[row][col] = isEmpty ? GRID_EMPTY_TILE : GRID_SOLID_TILE;
					//printf("\nthis->grid[%d][%d] = %d", row, col, this->grid[row][col]);
				}
				al_set_target_bitmap(al_get_backbuffer(gameObj.display));
			}
		}

	al_destroy_bitmap(tileElem);
	al_destroy_bitmap(gridElem);
}

//End of Class Grid

//start of class Enemy functions

Enemy::Enemy(int posX, int posY)
{
	positionX = posX;
	positionY = posY;
}

Enemy::~Enemy()
{

}

void Enemy::Reset_Sprite_Counter()
{
	this->EnemySpriteNum = 0;
}

void Enemy::Set_Speed_X(int speedX)
{
	this->scrollDistanceX = speedX;
}

void Enemy::Increment_Speed_X()
{
	this->scrollDistanceX++;
}

int Enemy::Get_Speed_X()
{
	return this->scrollDistanceX;
}

void Enemy::Set_Speed_Y(int speedY)
{
	this->scrollDistanceY = speedY;
}

void Enemy::Increment_Speed_Y()
{
	this->scrollDistanceY++;
}

void Enemy::Decrement_Speed_Y()
{
	this->scrollDistanceY--;
}

int Enemy::Get_Speed_Y()
{
	return this->scrollDistanceY;
}

void Enemy::Set_Direction(Direction direction)
{
	if (direction != dir_left && direction != dir_right)
	{
		cout << "Error in Enemy Set_Direction, direction = " << direction << " doesn't exists\n";
		exit(-1);
	}

	this->direction = direction;
}

Direction Enemy::Get_Direction()
{
	return this->direction;
}

Enemy_State Enemy::Get_State()
{
	return this->state;
}

float Enemy::Get_Health()
{
	return this->Health;
}

void Enemy::Set_Health(float health)
{
	this->Health = health;
}

void Enemy::Take_Damage(float health_damage)
{
	this->Health -= health_damage;
}

void Enemy::Heal(float health_gain)
{
	this->Health += health_gain;
}

void Enemy::Set_Points(int points)
{
	this->Points = points;
}

int Enemy::Get_Points()
{
	return this->Points;
}

//end of class enemy

//Start of Stalfos Class

Stalfos::Stalfos(int x, int y) : Enemy(x, y) 
{

}

void Stalfos::Init_frames_bounding_boxes() {
	Rect* r;
	int i = 0;

	//FramesWalkingRight
	for (i = 0; i < 2; i++)
	{
		r = new Rect;
		r->h = ENEMY_SPRITE_HEIGHT * 2;
		r->w = i % 2 == 1 ? ENEMY_SPRITE_WIDTH + ENEMY_SPRITE_WIDTH/2 : ENEMY_SPRITE_WIDTH * 2;
		
		r->y = 448 + 5;
		r->x = 288 + i* 2 *ENEMY_SPRITE_WIDTH;
		FramesWalkingRight.push_back(*r);
	}

	//FramesAttackingRight
	for (i = 0; i < 3; i++)
	{
		r = new Rect;
		r->h = ENEMY_SPRITE_HEIGHT * 2;
		if (i == 0) {
			r->w = ENEMY_SPRITE_WIDTH + ENEMY_SPRITE_WIDTH / 2;
			r->y = 448 + 5;
			r->x = 336 + 10;
		}
		else if (i == 1) {
			r->w = ENEMY_SPRITE_WIDTH + ENEMY_SPRITE_WIDTH / 2;
			r->y = 448 + 5;
			r->x = 368;
		}
		else {
			r->w = 2 * ENEMY_SPRITE_WIDTH + ENEMY_SPRITE_WIDTH/2;
			r->y = 448 + 5;
			r->x = 384 + 5;
		}

		FramesSlashRight.push_back(*r);
	}

	//FramesFalling
	r = new Rect;
	r->h = ENEMY_SPRITE_HEIGHT * 2 +ENEMY_SPRITE_HEIGHT/2;
	r->w = 2 * ENEMY_SPRITE_WIDTH;
	r->y = 432 + 10;
	r->x = 448 + 5;
	
	FramesFalling.push_back(*r);

	//FramesWalkingLeft 
	for (i = 0; i < 2; i++)
	{
		r = new Rect;
		r->h = ENEMY_SPRITE_HEIGHT * 2;
		r->w = ENEMY_SPRITE_WIDTH;

		r->y = 448 + 5;
		if (i == 0)
			r->x = 272 + 3;
		else
			r->x = 288 + 12;
		FramesWalkingLeft.push_back(*r);
	}

	//FramesSlashLeft 
	for (i = 0; i < 3; i++)
	{
		r = new Rect;
		r->h = ENEMY_SPRITE_HEIGHT * 2;
		r->y = 448 + 5;
		if (i == 0) {
			r->w = ENEMY_SPRITE_WIDTH*2;
			r->x = 192 - 1;
		}
		else {
			r->w = ENEMY_SPRITE_WIDTH + ENEMY_SPRITE_WIDTH / 2;
			if (i == 1)
				r->x = 224 + 3;
			else
				r->x = 256 - 3;
		}
		FramesSlashLeft.push_back(*r);
	}
}

void Stalfos::Increment_Sprite_Counter()
{
	if (this->state == E_State_Walking)
	{
		this->EnemySpriteNum = ++this->EnemySpriteNum % 2;	//2 frames for walking
	}
	else if (this->state == E_State_Attacking)
	{
		this->EnemySpriteNum = ++this->EnemySpriteNum % 3;	//3 frames for attacking
	}
	else if (this->state == E_State_Falling)
	{
		this->EnemySpriteNum = 0;	//1 frame for falling
	}
}

void Stalfos::Set_State(Enemy_State state) 
{
	if (this->state == E_State_Walking && state == E_State_Attacking) //Walking -> Attacking
	{
		this->state = state;
	}
	else if (this->state == E_State_Attacking && state == E_State_Walking) //Attacking -> Walking
	{
		this->state = state;
	}
}

void Stalfos::Scroll_Enemy(float ScrollDistanceX, float ScrollDistanceY)
{
	if (state == E_State_Walking)
	{
		this->positionX += ScrollDistanceX;
		this->positionY += ScrollDistanceY;
	}
}

Rect Stalfos::FrameToDraw() 
{
	if (state == E_State_Walking && direction == dir_left) 
	{
		return FramesWalkingLeft[EnemySpriteNum];
	}
	else if (state == E_State_Walking && direction == dir_right)
	{
		return FramesWalkingRight[EnemySpriteNum];
	}
	else if (state == E_State_Falling)
	{
		return FramesFalling[EnemySpriteNum];
	}
	else if (state == E_State_Attacking && direction == dir_right)
	{
		return FramesSlashRight[EnemySpriteNum];
	}
	else if (state == E_State_Attacking && direction == dir_left)
	{
		return FramesSlashLeft[EnemySpriteNum];
	}
	else
	{
		fprintf(stderr, "Error with Stalfos state : invalid value %d.\n", this->state);
		exit(-1);
	}
}

//End of Stalfos Class

//Start of PalaceBot

PalaceBot::PalaceBot(int x, int y) : Enemy(x, y)
{

}

void PalaceBot::Init_frames_bounding_boxes() 
{
	Rect* r;
	int i = 0;

	//FramesWalking
	r = new Rect;
	r->h = ENEMY_SPRITE_HEIGHT;
	r->w =  ENEMY_SPRITE_WIDTH;

	r->y = 688 + 8;
	r->x = 160 + 3;
	FramesWalking.push_back(*r);
	

	//FramesJumping
	r = new Rect;
	r->h = ENEMY_SPRITE_HEIGHT;
	r->w = ENEMY_SPRITE_WIDTH;

	r->y = 688 + 7;
	r->x = 176 + 5;
	FramesJumping.push_back(*r);
		
}

void PalaceBot::Increment_Sprite_Counter()
{
	if (this->state == E_State_Walking)
	{
		this->EnemySpriteNum = 0;	//1 frame for walking
	}
	else if (this->state == E_State_Jumping)
	{
		this->EnemySpriteNum = 0;	//1 frames for jumping
	}
}

void PalaceBot::Set_State(Enemy_State state) 
{
	if (this->state == E_State_Walking && state == E_State_Jumping) //Walking -> Jumping
	{
		this->state = state;
	}
	else if (this->state == E_State_Jumping && state == E_State_Walking) //Jumping -> Walking
	{
		this->state = state;
	}
}

Rect PalaceBot::FrameToDraw() 
{
	if (state == E_State_Walking)
	{
		return FramesWalking[EnemySpriteNum];
	}
	else if (state == E_State_Jumping)
	{
		return FramesJumping[EnemySpriteNum];
	}
	else
	{
		fprintf(stderr, "Error with PalaceBot state : invalid value %d.\n", this->state);
		exit(-1);
	}
}

void PalaceBot::Scroll_Enemy(float ScrollDistanceX, float ScrollDistanceY)
{
	if (state == E_State_Jumping)
	{
		this->positionX += ScrollDistanceX;
		this->positionY += ScrollDistanceY;
	}
}
//End of PalaceBot


//Start of Wosu Class
Wosu::Wosu(int x, int y) : Enemy(x, y)
{
	StartPosX = positionX;
	StartPosY = positionY;
	//check the tiles in the direction wosu is looking at and if there is any solid tiles or screen changing
	//change the direction of wosu to try and create a nice walking pattern
	//by default all enemies look to the right, so we check for that direction
	unsigned short screenX = this->positionX / DISPLAY_W;
	for (unsigned short i = 1; i < this->WalkingTilesDistance; i++)
	{
		if (gameObj.level->grids[0]->GetGridTile(DIV_TILE_HEIGHT(this->positionY), DIV_TILE_WIDTH(this->positionX) + i) & GRID_SOLID_TILE)
		{
			this->direction = dir_left;
			break;
		}
		if ((this->positionX + i * TILE_WIDTH) / DISPLAY_W > screenX)
		{
			this->direction = dir_left;
			break;
		}
	}
}

void Wosu::Init_frames_bounding_boxes() 
{
	Rect* r;
	int i = 0;

	//FramesWalkingRight
	for (int i = 0; i < 2; i++) {
		r = new Rect;
		r->h = ENEMY_SPRITE_HEIGHT*2;
		r->w = ENEMY_SPRITE_WIDTH;

		r->y = 768 + 2;

		if (i == 0)
			r->x = 16 + 5;
		else if(i == 1)
			r->x = 48;

		FramesWalkingRight.push_back(*r);
	}

	//FramesWalkingLeft
	for (int i = 0; i < 2; i++) {
		r = new Rect;
		r->h = ENEMY_SPRITE_HEIGHT * 2 ;
		r->w = ENEMY_SPRITE_WIDTH;

		r->y = 768 + 2;

		if (i == 0)
			r->x = 544 + 5;
		else if (i == 1)
			r->x = 576;

		FramesWalkingLeft.push_back(*r);
	}
}

void Wosu::Increment_Sprite_Counter()
{
	if (this->state == E_State_Walking)
	{
		this->EnemySpriteNum = ++this->EnemySpriteNum % 2;	//2 frames for walking
	}
}

void Wosu::Set_State(Enemy_State state) 
{

	if (this->state == E_State_Walking && state == E_State_Walking) //Walking -> Walking
	{
		this->state = state;
	}
}

Rect Wosu::FrameToDraw() 
{
	if (state == E_State_Walking && direction == dir_right)
	{
		return FramesWalkingRight[EnemySpriteNum];
	}
	else if (state == E_State_Walking && direction == dir_left)
	{
		return FramesWalkingLeft[EnemySpriteNum];
	}
	else
	{
		fprintf(stderr, "Error with Wosu state : invalid value %d.\n", this->state);
		exit(-1);
	}
}

void Wosu::Scroll_Enemy(float ScrollDistanceX, float ScrollDistanceY)
{
	if (state == E_State_Walking)
	{
		if (direction == dir_left)
		{
			this->positionX -= ScrollDistanceX;
			//if wosu walked 6 or more tiles its time to turn back
			if (DIV_TILE_WIDTH(this->StartPosX - this->positionX) >= 6)
			{
				this->direction = dir_right;
			}
		}
		else
		{
			this->positionX += ScrollDistanceX;
			if (DIV_TILE_WIDTH(abs(this->StartPosX - this->positionX)) >= 6)
			{
				this->direction = dir_left;
			}
		}
	}
}

//End Wosu Class

//Start of Guma Class

Guma::Guma(int x, int y) : Enemy(x, y)
{

}

void Guma::Init_frames_bounding_boxes() 
{
	Rect* r;
	int i = 0;

	//FramesWalkingRight
	for (int i = 0; i < 2; i++) {
		r = new Rect;
		r->h = ENEMY_SPRITE_HEIGHT * 2;
		r->w = ENEMY_SPRITE_WIDTH;

		r->y = 816 + 5;
		if (i == 0)
			r->x = 368 + 10;
		else if (i == 1)
			r->x = 400 + 4;
		FramesWalkingRight.push_back(*r);
	}

	//FramesAttackingRight
	for (int i = 0; i < 2; i++) {
		r = new Rect;
		
		
			r->h = ENEMY_SPRITE_HEIGHT * 2;
			r->w = ENEMY_SPRITE_WIDTH + ENEMY_SPRITE_WIDTH / 4;
			r->y = 816 + 5;
			if (i == 0)
				r->x = 432 - 1;
			else
				r->x = 464 - 3;



			FramesAttackingRight.push_back(*r);
	}

	//FramesWalkingLeft
	for (int i = 0; i < 2; i++) {
		r = new Rect;
		r->h = ENEMY_SPRITE_HEIGHT * 2;
		r->w = ENEMY_SPRITE_WIDTH;

		r->y = 816 + 5;
		if (i == 0)
			r->x = 192 + 3;
		else if (i == 1)
			r->x = 208 + 12;
		FramesWalkingRight.push_back(*r);
	}

	//FramesAttackingLeft
	for (int i = 0; i < 2; i++) {
		r = new Rect;


		r->h = ENEMY_SPRITE_HEIGHT * 2;
		r->w = ENEMY_SPRITE_WIDTH + ENEMY_SPRITE_WIDTH / 4;
		r->y = 816 + 5;
		if (i == 0)
			r->x = 128 + 5;
		else
			r->x = 160 + 4;



		FramesAttackingLeft.push_back(*r);
	}
}

void Guma::Increment_Sprite_Counter()
{
	if (this->state == E_State_Walking || this->state == E_State_Attacking)
	{
		this->EnemySpriteNum = ++this->EnemySpriteNum % 2;	//2 frames for walking or attacking
	}
}

void Guma::Set_State(Enemy_State state) 
{
	if (this->state == E_State_Walking && state == E_State_Attacking) //Walking -> Attacking
	{
		this->state = state;
	}
	else if (this->state == E_State_Attacking && state == E_State_Walking) //Attacking -> Walking
	{
		this->state = state;
	}
}

Rect Guma::FrameToDraw() 
{
	if (state == E_State_Walking && direction == dir_right)
	{
		return FramesWalkingRight[EnemySpriteNum];
	}
	else if (state == E_State_Walking && direction == dir_left) {
		return FramesWalkingLeft[EnemySpriteNum];
	}
	else if (state == E_State_Attacking && direction == dir_right)
	{
		return FramesAttackingRight[EnemySpriteNum];
	}
	else if (state == E_State_Attacking && direction == dir_left)
	{
		return FramesAttackingLeft[EnemySpriteNum];
	}
	else
	{
		fprintf(stderr, "Error with Guma state : invalid value %d.\n", this->state);
		exit(-1);
	}
}

void Guma::Scroll_Enemy(float ScrollDistanceX, float ScrollDistanceY)
{
	if (state == E_State_Walking)
	{
		this->positionX += ScrollDistanceX;
		this->positionY += ScrollDistanceY;
	}
}

//End of Guma Class

//Start of Projectile Class

Projectile::Projectile(int posX, int posY)
{
	positionX = posX;
	positionY = posY;
}

Projectile::~Projectile()
{

}

void Projectile::Set_Speed_X(int speedX)
{
	this->scrollDistanceX = speedX;
}

int Projectile::Get_Speed_X()
{
	return this->scrollDistanceX;
}

void Projectile::Set_Speed_Y(int speedY)
{
	this->scrollDistanceY = speedY;
}

int Projectile::Get_Speed_Y()
{
	return this->scrollDistanceY;
}

void Projectile::Load_Projectile_Spritesheet()
{
	this->ProjectileSpriteSheet = al_load_bitmap(ENEMY1_SPRITES_PATH);
	if (this->ProjectileSpriteSheet == NULL)
	{
		fprintf(stderr, "\nFailed to initialize ProjectileSpriteSheet (al_load_bitmap() failed).\n");
		exit(-1);
	}
}


//End of Projectile Class

//Start of Axe Class

GumaAxe::GumaAxe(int x, int y) : Projectile(x, y)
{

}

void GumaAxe::Init_frames_bounding_boxes() 
{
	Rect* r;
	int i = 0;

	//FramesLunging
	for (int i = 0; i < 4; i++)
	{
		r = new Rect;
		
		r->h = ENEMY_SPRITE_HEIGHT;
		r->w = 2 * ENEMY_SPRITE_WIDTH;
		r->y = 816 + 13;
		if (i == 0)
			r->x = 496 - 1;
		else if (i == 1)
			r->x = 512 - 1;
		else if (i == 2)
			r->x = 528 + 1;
		else
			r->x = 544 + 4;
		
		LungeFrames.push_back(*r);
	}
}

Rect GumaAxe::FrameToDraw() 
{
	return LungeFrames[ProjectileSpriteNum];
}

void GumaAxe::Scroll_Projectile(float ScrollDistanceX, float ScrollDistanceY)
{
	this->positionX += ScrollDistanceX;
	this->positionY += ScrollDistanceY;
}

//End of Axe Class

//Start of Collectable

Collectable::Collectable(int posX, int posY)
{
	positionX = posX;
	positionY = posY;
}

Collectable::~Collectable()
{

}

//End of Class Collectable

//Start of RedPotion

RedPotion::RedPotion(int x, int y) : Collectable(x, y) 
{
	RedPotionFrame.h = 0;
	RedPotionFrame.w = 0;
	RedPotionFrame.x = 0;
	RedPotionFrame.y = 0;
}

void RedPotion::Init_frames_bounding_boxes() 
{
	Rect* r;

	//RedPotionFrames
	r = new Rect;

	r->h = OBJECT_SPRITE_HEIGHT;
	r->w = OBJECT_SPRITE_WIDTH;
	r->y = 16 - 5;
	r->x = 48 + 2;

	RedPotionFrame = *r;
}

void RedPotion::Init_frames_bounding_boxes(unsigned short id)
{
	Rect* r;

	//BluePotionFrame
	r = new Rect;

	r->h = OBJECT_SPRITE_HEIGHT;
	r->w = OBJECT_SPRITE_WIDTH;
	if (id == 1)
	{
		r->y = 16 - 5;
		r->x = 40 + 1;	// 6*8 with tile width 8
	}
	else if (id == 2)
	{
		r->y = 16 - 5;
		r->x = 48 + 2;
	}
	else
	{
		r->y = 16 - 5;
		r->x = 56 + 3;
	}

	RedPotionFrame = *r;
}

Rect RedPotion::FrameToDraw()
{
	return RedPotionFrame;
}

void RedPotion::Set_Restore_Amount(unsigned short amount)
{
	this->restore = amount;
}

unsigned short RedPotion::Get_Restore_Amount()
{
	return this->restore;
}

//End of RedPotion

//Start of BluePotion

BluePotion::BluePotion(int x, int y) : Collectable(x, y) 
{
	BluePotionFrame.h = 0;
	BluePotionFrame.w = 0;
	BluePotionFrame.x = 0;
	BluePotionFrame.y = 0;
}

void BluePotion::Init_frames_bounding_boxes()
{
	Rect* r;

	//BluePotionFrame
	r = new Rect;

	r->h = OBJECT_SPRITE_HEIGHT;
	r->w = OBJECT_SPRITE_WIDTH;
	r->y = 16 - 5;
	r->x = 16 + 5;

	BluePotionFrame = *r;
}

void BluePotion::Init_frames_bounding_boxes(unsigned short id)
{
	Rect* r;

	//BluePotionFrame
	r = new Rect;

	r->h = OBJECT_SPRITE_HEIGHT;
	r->w = OBJECT_SPRITE_WIDTH;
	if (id == 1)
	{
		r->y = 16 - 5;
		r->x = 8 + 4;
	}
	else if (id == 2)
	{
		r->y = 16 - 5;
		r->x = 16 + 5;
	}
	else
	{
		r->y = 16 - 5;
		r->x = 32 - 2;
	}

	BluePotionFrame = *r;
}

Rect BluePotion::FrameToDraw()
{
	return BluePotionFrame;
}

void BluePotion::Set_Restore_Amount(unsigned short amount)
{
	this->restore = amount;
}

unsigned short BluePotion::Get_Restore_Amount()
{
	return this->restore;
}

//End of BluePotion

//Start of PointBag

PointBag::PointBag(int x, int y) : Collectable(x, y) 
{
	PointBagFrame.h = 0;
	PointBagFrame.w = 0;
	PointBagFrame.x = 0;
	PointBagFrame.y = 0;
}

void PointBag::Init_frames_bounding_boxes() 
{
	Rect* r;

	//PointBagFrame
	r = new Rect;

	r->h = OBJECT_SPRITE_HEIGHT;
	r->w = OBJECT_SPRITE_WIDTH;
	r->y = 16 - 5;
	r->x = 0;

	PointBagFrame = *r;
}

Rect PointBag::FrameToDraw() 
{
	return PointBagFrame;
}

//End of PointBag

//Start of SimpleKey class

SimpleKey::SimpleKey(int x, int y) : Collectable(x, y) 
{
	KeyFrame.h = 0;
	KeyFrame.w = 0;
	KeyFrame.x = 0;
	KeyFrame.y = 0;
}

void SimpleKey::Init_frames_bounding_boxes() 
{
	Rect* r;

	//KeyFrame
	r = new Rect;

	r->h = OBJECT_SPRITE_HEIGHT;
	r->w = OBJECT_SPRITE_WIDTH;
	r->y = 16 - 5;
	r->x = 96 + 5;

	KeyFrame = *r;
}

Rect SimpleKey::FrameToDraw() 
{
	return KeyFrame;
}

//End of SimpleKey class

//Start of UpDoll class
UpDoll::UpDoll(int x, int y) : Collectable(x, y) 
{
	UpDollFrame.h = 0;
	UpDollFrame.w = 0;
	UpDollFrame.x = 0;
	UpDollFrame.y = 0;
}

void UpDoll::Init_frames_bounding_boxes() 
{
	Rect* r;

	//UpDollFrame
	r = new Rect;

	r->h = OBJECT_SPRITE_HEIGHT;
	r->w = OBJECT_SPRITE_WIDTH;
	r->y = 16 - 5;
	r->x = 64 + 6;

	UpDollFrame = *r;
}

Rect UpDoll::FrameToDraw() 
{
	return UpDollFrame;
}

//End of UpDoll class

//Start of global functions

void Init_Player(int PlayerX, int PlayerY)
{
	player = new Player(PlayerX, PlayerY);
	player->Init_frames_bounding_boxes();
	player->Load_Player_Spritesheet();
}

void Load_Player_Spritesheet()
{
	player->PlayerSpriteSheet = al_load_bitmap(LINK_SPRITES_PATH);
	if (player->PlayerSpriteSheet == NULL)
	{
		fprintf(stderr, "\nFailed to initialize PlayerSpriteSheet (al_load_bitmap() failed).\n");
		exit(-1);
	}
}

bool operator == (const ALLEGRO_COLOR c1, const ALLEGRO_COLOR c2)
{
	return (c1.r == c2.r && c1.g == c2.g && c1.b == c2.b);
}

bool operator != (const ALLEGRO_COLOR c1, const ALLEGRO_COLOR c2)
{
	return (c1.r != c2.r || c1.g != c2.g || c1.b != c2.b);
}

bool operator < (const ALLEGRO_COLOR c1, const ALLEGRO_COLOR c2)
{
	if (c1.r < c2.r)
		return true;
	if (c1.r == c2.r && c1.g < c2.g)
		return true;
	if (c1.r == c2.r && c1.g == c2.g && c1.b < c2.b)
		return true;
	return false;
}

bool IsTileColorEmpty(unsigned int emptyTileColor_num, ALLEGRO_COLOR c)
{
	return emptyTileColors[emptyTileColor_num].ColorIn(c);
}

bool IsTileIndexAssumedEmpty(unsigned int layer, Index index)
{
	return emptyTileColors[layer].IndexIn(index);
}

void Init_emptyTileColorsHolder(const char* filepath)
{
	ifstream file;
	file.open(filepath);
	if (!file.is_open())
	{
		cout << "Error Init_emptyTileColorsHolder could not open file.\n";
		exit(-1);
	}
	if (gameObj.level == NULL)
	{
		cout << "game object has a null pointer on level (maybe a level isn't loaded yet)... Exiting....\n";
		exit(-1);
	}


	TileColorsHolder* tch = new TileColorsHolder;
	ALLEGRO_BITMAP* bmp = al_create_bitmap(TILE_WIDTH, TILE_HEIGHT);
	al_set_target_bitmap(bmp);
	while (!file.eof())
	{
		string line;
		getline(file, line, ',');
		//cout << "Init_emptyTileColorsHolder testing : " << line << "\n";
		Index x = stoi(line);

		al_draw_bitmap_region(gameObj.level->TileSet, MUL_TILE_WIDTH(gameObj.level->modIndex[x]), MUL_TILE_HEIGHT(gameObj.level->divIndex[x]), TILE_WIDTH, TILE_HEIGHT, 0, 0, 0);
		tch->Insert(bmp, x);
	}

	emptyTileColors.push_back(*tch);
	al_set_target_bitmap(al_get_backbuffer(gameObj.display));
	al_destroy_bitmap(bmp);
	file.close();
}

void add_Grid(unsigned int layer, unsigned int Grid_Element_Width, unsigned int Grid_Element_Height, unsigned int bitmapNumTilesWidth, unsigned int bitmapNumTilesHeight)
{
	gameObj.level->grids.push_back(new Grid(layer, Grid_Element_Width, Grid_Element_Height, bitmapNumTilesWidth, bitmapNumTilesHeight));
}



void add_Stalfos(int x,int y) 
{
	Stalfos *stalfos = new Stalfos(x, y);
	stalfos->Init_frames_bounding_boxes();
	stalfos->Set_Health(32);
	stalfos->Set_Points(30);
	Enemies.push_back(stalfos);
}

void add_PalaceBot(int x, int y)
{
	PalaceBot *pbot = new PalaceBot(x, y);
	pbot->Init_frames_bounding_boxes();
	pbot->Set_Health(16);
	pbot->Set_Points(10);
	Enemies.push_back(pbot);
}

void add_Wosu(int x, int y)
{
	Wosu* wosu = new Wosu(x, y);
	wosu->Init_frames_bounding_boxes();
	wosu->Set_Health(8);
	wosu->Set_Points(0);
	Enemies.push_back(wosu);
}

void add_Guma(int x, int y)
{
	Guma* guma = new Guma(x, y);
	guma->Init_frames_bounding_boxes();
	guma->Set_Health(64);
	guma->Set_Points(50);
	Enemies.push_back(guma);
}



void add_RedPotion(int x,int y, unsigned short restore_amount, short id) 
{
	RedPotion* rpotion = new RedPotion(x,y);
	if (id == -1)
		rpotion->Init_frames_bounding_boxes();
	else
		rpotion->Init_frames_bounding_boxes(id);
	rpotion->Set_Restore_Amount(restore_amount);
	Collectables.push_back(rpotion);
}

void add_BluePotion(int x, int y, unsigned short restore_amount, short id)
{
	BluePotion* bpotion = new BluePotion(x,y);
	if(id == -1)
		bpotion->Init_frames_bounding_boxes();
	else
		bpotion->Init_frames_bounding_boxes(id);
	bpotion->Set_Restore_Amount(restore_amount);
	Collectables.push_back(bpotion);
}

void add_PointBag(int x, int y)
{
	PointBag* pbag = new PointBag(x, y);
	pbag->Init_frames_bounding_boxes();
	Collectables.push_back(pbag);
}

void add_SimpleKey(int x, int y)
{
	SimpleKey* k = new SimpleKey(x, y);
	k->Init_frames_bounding_boxes();
	Collectables.push_back(k);
}

void add_UpDoll(int x, int y)
{
	UpDoll* updoll = new UpDoll(x, y);
	updoll->Init_frames_bounding_boxes();
	Collectables.push_back(updoll);
}