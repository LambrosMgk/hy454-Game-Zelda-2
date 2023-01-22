#include "..\\Headers\\GameObjectsClasses.h"

//Start of global variables

std::vector<TileColorsHolder> emptyTileColors;
GameLogic gameObj;
Player* player = NULL;
std::vector<Elevator> elevators;
std::vector<Enemy*> Enemies;

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
	else if (player->positionY / DISPLAY_H > cameraY / DISPLAY_H)	//check if this statemnet if correct
	{

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
	
	add_Guma(5 * TILE_WIDTH + 20,11 * TILE_HEIGHT - 1);

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

	createElevators();
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
					if (elevators[k].getRow() == row - 4 && elevators[k].getCol() == col)	//if the upper part is already added don't create new elevator
					{
						isTopElev = true;
						break;
					}
				}

				if (isTopElev != true)
				{
					//cout << "Created elevator at row = " << row << "and col = " << col << "\n";
					elevators.push_back(Elevator(MUL_TILE_HEIGHT(row), MUL_TILE_WIDTH(col)));
				}

				isTopElev = false;
			}
		}
	}
}

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

void Player::set_Direction(Player_Direction direction)
{
	if (direction != dir_left && direction != dir_right)
	{
		cout << "Error : invalid direction \"" << direction << "\"\n";
		exit(-1);
	}

	this->direction = direction;
}

Player_Direction Player::get_Direction()
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
}

void Player::Init_frames_bounding_boxes()
{
	Rect* r;
	int i = 0, k = 0;

	//FramesWalkingLeft
	for (i = 0, k = 0; i < 4; i++)
	{
		r = new Rect;
		r->h = LINK_SPRITE_HEIGHT * 2;
		r->w = i % 2 == 1 ? LINK_SPRITE_WIDTH : LINK_SPRITE_WIDTH * 2;
		k = i % 2 == 1 ? k + i * 16 + 32 : k + i * 16; 
		r->y = 0;
		if (i == 3)
			k -= 32;
		r->x = k;
		FramesWalkingLeft.push_back(*r);
	}
	std::reverse(FramesWalkingLeft.begin(), FramesWalkingLeft.end());

	//FramesWalkingRight
	for (i = 0, k = 160; i < 4; i++)
	{
		r = new Rect;
		r->h = LINK_SPRITE_HEIGHT * 2;
		r->w = i % 2 == 1 ? LINK_SPRITE_WIDTH : LINK_SPRITE_WIDTH * 2;
		k = i % 2 == 1 ? k + i * 16 + 32 : k + i * 16;
		r->y = 0;
		if (i == 3)
			k -= 32;
		r->x = k;
		FramesWalkingRight.push_back(*r);
	}

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

void Player::SetSwordX(int x)
{
	this->SwordX = x;
}

int Player::GetSwordX()
{
	return this->SwordX;
}

void Player::SetSwordY(int y)
{
	this->SwordY = y;
}

int Player::GetSwordY()
{
	return this->SwordY;
}

//end of class Player functions

//start of class Enemy functions

Enemy::Enemy(int posX, int posY)
{
	positionX = posX;
	positionY = posY;
}

Enemy::~Enemy()
{

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

void Enemy::Set_State(Enemy_State state)
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

Enemy_State Enemy::Get_State()
{
	return this->state;
}

void Enemy::Load_Enemy_Spritesheet()
{
	this->EnemySpriteSheet = al_load_bitmap(ENEMY1_SPRITES_PATH);
	if (this->EnemySpriteSheet == NULL)
	{
		fprintf(stderr, "\nFailed to initialize PlayerSpriteSheet (al_load_bitmap() failed).\n");
		exit(-1);
	}
}


void Enemy::Scroll_Enemy(float ScrollDistanceX, float ScrollDistanceY)
{
	if (state == E_State_Walking)
	{
		this->positionX += ScrollDistanceX;
		this->positionY += ScrollDistanceY;
	}
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
	
}

void Stalfos::Set_State(Enemy_State state) {
	
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

Rect Stalfos::FrameToDraw() {


	if (state == E_State_Walking && direction == e_dir_right)
	{
		return FramesWalkingRight[EnemySpriteNum];
	}
	else if (state == E_State_Walking && direction == e_dir_left) {
		return FramesWalkingLeft[EnemySpriteNum];
	}
	else if (state == E_State_Falling ) {
		return FramesFalling[EnemySpriteNum];
	}
	else if (state == E_State_Attacking && direction == e_dir_right)
	{
		return FramesSlashRight[EnemySpriteNum];
	}
	else if (state == E_State_Attacking && direction == e_dir_left)
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

void PalaceBot::Init_frames_bounding_boxes() {
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

void PalaceBot::Set_State(Enemy_State state) {

	if (this->state == E_State_Walking && state == E_State_Jumping) //Walking -> Jumping
	{
		this->state = state;
	}
	else if (this->state == E_State_Jumping && state == E_State_Walking) //Jumping -> Walking
	{
		this->state = state;
	}

}

Rect PalaceBot::FrameToDraw() {

	
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

}

void Wosu::Init_frames_bounding_boxes() {
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
	

}

void Wosu::Set_State(Enemy_State state) {

	if (this->state == E_State_Walking) //Walking 
	{
		this->state = state;
	}

}

Rect Wosu::FrameToDraw() {

	
	if (state == E_State_Walking && direction == e_dir_right)
	{
		return FramesWalkingRight[EnemySpriteNum];
	}
	else if (state == E_State_Walking && direction == e_dir_left) {
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
		this->positionX += ScrollDistanceX;
		this->positionY += ScrollDistanceY;
	}
}

//End Wosu Class

//Start of Guma Class

Guma::Guma(int x, int y) : Enemy(x, y)
{

}

void Guma::Init_frames_bounding_boxes() {
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
	for (int i = 0; i < 4; i++) {
		r = new Rect;
		
		if (i > 1) {
			r->h = ENEMY_SPRITE_HEIGHT;
			r->w = 2*ENEMY_SPRITE_WIDTH;
			r->y = 816 + 13;
			if (i == 2)
				r->x = 496-1;
			else
				r->x = 528;
		}
		else {
			r->h = ENEMY_SPRITE_HEIGHT * 2;
			r->w = ENEMY_SPRITE_WIDTH + ENEMY_SPRITE_WIDTH / 4;
			r->y = 816 + 5;
			if (i == 0)
				r->x = 432 - 1;
			else 
				r->x = 464 - 3;
		}	

		
		FramesAttackingRight.push_back(*r);
	}


}

void Guma::Set_State(Enemy_State state) {

	if (this->state == E_State_Walking && state == E_State_Attacking) //Walking -> Attacking
	{
		this->state = state;
	}
	else if (this->state == E_State_Attacking && state == E_State_Walking) //Attacking -> Walking
	{
		this->state = state;
	}

}

Rect Guma::FrameToDraw() {

	return FramesAttackingRight[3];
	/*if (state == E_State_Walking && direction == e_dir_right)
	{
		return FramesWalkingRight[EnemySpriteNum];
	}
	else if (state == E_State_Walking && direction == e_dir_left) {
		return FramesWalkingLeft[EnemySpriteNum];
	}
	else if (state == E_State_Attacking && direction == e_dir_right)
	{
		return FramesAttackingRight[EnemySpriteNum];
	}
	else if (state == E_State_Attacking && direction == e_dir_left)
	{
		return FramesAttackingLeft[EnemySpriteNum];
	}
	else
	{
		fprintf(stderr, "Error with Guma state : invalid value %d.\n", this->state);
		exit(-1);
	}*/
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


//Class TileColorsHolder

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

//Class Grid

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
	return (player->positionX + player->screenX * DISPLAY_W) / TILE_WIDTH;
}

int Grid::getPlayerRightCol(Player* player) {
	return (player->positionX + player->screenX * DISPLAY_W + LINK_SPRITE_WIDTH * 2 - 1) / TILE_WIDTH;
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

void Init_Player(int PlayerX, int PlayerY)
{
	player = new Player(PlayerX, PlayerY);
	player->Init_frames_bounding_boxes();
	player->Load_Player_Spritesheet();
	player->HurtTimer =  al_create_timer(1.0);	//1 sec of invicibility, useful for timing incoming damage
}

void Load_Player_Spiresheet()
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
	stalfos->Load_Enemy_Spritesheet();
	stalfos->Set_Health(32);
	stalfos->Set_Points(30);
	Enemies.push_back(stalfos);
}

void add_PalaceBot(int x, int y)
{
	PalaceBot *pbot = new PalaceBot(x, y);
	pbot->Init_frames_bounding_boxes();
	pbot->Load_Enemy_Spritesheet();
	pbot->Set_Health(16);
	pbot->Set_Points(10);
	Enemies.push_back(pbot);
}

void add_Wosu(int x, int y)
{
	Wosu* wosu = new Wosu(x, y);
	wosu->Init_frames_bounding_boxes();
	wosu->Load_Enemy_Spritesheet();
	wosu->Set_Health(8);
	wosu->Set_Points(0);
	Enemies.push_back(wosu);
}

void add_Guma(int x, int y)
{
	Guma* guma = new Guma(x, y);
	guma->Init_frames_bounding_boxes();
	guma->Load_Enemy_Spritesheet();
	guma->Set_Health(64);
	guma->Set_Points(50);
	Enemies.push_back(guma);
}