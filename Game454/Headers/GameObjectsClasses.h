#pragma once

#include <stdio.h>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>
#include <set>
#include <vector>

#include "al_init.h"

#define START_SCREEN_PATH "UnitTests\\Media\\Start_Screen.png"
#define TILESET_PATH "UnitTests\\Media\\Level_1\\Zelda-II-Parapa-Palace-Tileset.png"
#define ASSUMED_EMPTY_LAYER_PATH "UnitTests\\Media\\Level_1\\Assumed_Empty_indices_Layer"
#define LINK_SPRITES_PATH "UnitTests\\Media\\link-sprites.png"
#define ENEMY1_SPRITES_PATH "UnitTests\\Media\\enemies-sprites-1.gif"
#define ENEMY2_SPRITES_PATH "UnitTests\\Media\\enemies-sprites-2.gif"
#define ITEMS_OBJECTS_PATH "UnitTests\\Media\\items-objects.png"
#define START_SCREEN_MUSIC "UnitTests\\Media\\Zelda II The Adventure of Link OST\\01.-Title-Screen-Prologue.ogg"
#define LEVEL_1_MUSIC "UnitTests\\Media\\Zelda II The Adventure of Link OST\\02.-Overworld.ogg"

#define DISPLAY_W 640
#define DISPLAY_H 480

#define TILE_WIDTH 16
#define TILE_HEIGHT 16

#define StartPlayerPositionX 5*TILE_WIDTH
#define StartPlayerPositionY 11*TILE_HEIGHT-1



#define FPS 60.0
#define LEVEL_LAYERS 2

#define MUL_TILE_WIDTH(i) ((i)<<4)
#define MUL_TILE_HEIGHT(i)((i)<<4)
#define DIV_TILE_WIDTH(i) ((i)>>4)
#define DIV_TILE_HEIGHT(i)((i)>>4)
#define MOD_TILE_WIDTH(i) ((i)&15)
#define MOD_TILE_HEIGHT(i)((i)&15)


#define GRID_THIN_AIR_MASK 0x0000 // element is ignored
#define GRID_LEFT_SOLID_MASK 0x0001 // bit 0
#define GRID_RIGHT_SOLID_MASK 0x0002 // bit 1
#define GRID_TOP_SOLID_MASK 0x0004 // bit 2
#define GRID_BOTTOM_SOLID_MASK 0x0008 // bit 3
#define GRID_GROUND_MASK 0x0010 // bit 4, keep objects top / bottom (gravity)
#define GRID_FLOATING_MASK 0x0020 // bit 5, keep objects anywhere inside (gravity)
#define GRID_EMPTY_TILE GRID_THIN_AIR_MASK
#define GRID_SOLID_TILE \
(GRID_LEFT_SOLID_MASK | GRID_RIGHT_SOLID_MASK | GRID_TOP_SOLID_MASK | GRID_BOTTOM_SOLID_MASK)

#define DIV_GRID_ELEMENT_WIDTH(i) ((i)>>2)
#define DIV_GRID_ELEMENT_HEIGHT(i) ((i)>>2)
#define MUL_GRID_ELEMENT_WIDTH(i) ((i)<<2)
#define MUL_GRID_ELEMENT_HEIGHT(i) ((i)<<2)

//Link sprites vary so its better to go with 16 pixels
#define LINK_SPRITE_WIDTH 16
#define LINK_SPRITE_HEIGHT 16
#define LINK_JUMP_HEIGHT 32

//Enemy sprites vary so its better to go with 16 pixels
#define ENEMY_SPRITE_WIDTH 16
#define ENEMY_SPRITE_HEIGHT 16

#define ELEVATORID1 12
#define ELEVATORID2 13

using namespace std;


typedef unsigned short Dim;
typedef unsigned short Index;
struct Rect { int x, y, w, h; };
struct Point { int x, y; };

enum Game_State { StartingScreen, PlayingLevel1, Paused , GameFinished};
enum Player_Direction { dir_left, dir_right };
enum Player_State { State_Walking, State_Crounching, State_Attacking, State_CrounchAttacking, State_Elevator };
enum Enemy_Direction {	e_dir_left, e_dir_right };
enum Enemy_State { E_State_Walking, E_State_Attacking,E_State_Falling,E_State_Jumping };


//forward declaration
class TileColorsHolder;
class GameLogic;
class Player;
class Enemy;
class Grid;
class Elevator;
class Stalfos;
class PalaceBot;



// keeps colors that are assumed to be empty
extern std::vector<TileColorsHolder> emptyTileColors;
extern GameLogic gameObj;	//object that holds the game state and other useful information
extern Player* player;
extern std::vector<Elevator> elevators;
extern std::vector<Enemy*> Enemies;

//used in render
extern bool keyboardUp, scrollDown, scrollLeft, scrollRight; //omit these later, maybe not left and right? useful for animation?
//used in physics and animations

/*useful for drawing stuff between layers*/
class DrawOrder
{
public:
	ALLEGRO_BITMAP* bitmap;
	unsigned int sx, sy, w, h;
	int xPos, yPos;

	DrawOrder(ALLEGRO_BITMAP* bmp, unsigned int sx, unsigned int sy, unsigned int w, unsigned int h, int xPos, int yPos);
	//setters getters
};


class Level
{
public:
	ALLEGRO_BITMAP* TileSet = NULL;
	std::vector<ALLEGRO_BITMAP*> bitmaps;	//bitmaps vector has the bitmaps of all the layers of the map
	std::vector<std::vector<std::vector<int>>>TileMapCSV;		//vector of layers, each layer made by 2d array of indices (vector<vector<int>>)
	std::vector<Grid*> grids;
	unsigned int active_elevator = -1;	//index for the elevators vector showing which elevator is being used by the player

	bool Toggle_Grid = false;
	unsigned char* divIndex = NULL, * modIndex = NULL;
	unsigned short TILESET_WIDTH = 0, TILESET_HEIGHT = 0;
	int cameraX = 0, cameraY = 0;
	int ScreenX = 0, ScreenY = 0;	/*like the player class cameras will represent pixels and Screens will represent how many screen from the start(upper left corner)*/

	/*loads a bitmap from the given filepath, sets the global variables TILESET_WIDTH and TILESET_HEIGHT then returns the loaded tileset as a bitmap
in case of bad file path exits program with -1*/
	void load_tileset(const char* filepath);

	//pre-caching tileset indexes for better perfomance, this function should be called at the start of the programm
	void Calculate_Tileset_Indexes();

	/*Given the path of a .csv file, returns a vector<vector<int>> of indices that correspondes to a TileSet*/
	vector<vector<int>> ReadTextMap(string TileMapFileName);

	/*Creates and returns a bitmap from an CSV and a Tileset, WILL NOT WORK IF A TILESET IS NOT LOADED (tileset width and height vars will have a value of 0)*/
	ALLEGRO_BITMAP* Create_Bitmap_From_CSV(vector<vector<int>> CSV, ALLEGRO_BITMAP* Tileset, ALLEGRO_DISPLAY* display);

	void Paint_To_Bitmap(ALLEGRO_BITMAP* bitmap, vector<vector<int>> CSV, ALLEGRO_BITMAP* Tileset, ALLEGRO_DISPLAY* display);

	void Scroll_Bitmap();
};

class GameLogic
{
private:
	Game_State GameState = StartingScreen;
	ALLEGRO_SAMPLE* song = NULL;
	ALLEGRO_SAMPLE_INSTANCE* songInstance = NULL;
public:
	/*Allows me to draw in order stuff from the same layer e.g. i want to draw layer 2 but layer 2 has a moving elevator which must
	be drawn without changing the layer 2 bitmap, so it must be drawn after layer 2 but before layer 3 (if we add a layer 3)
	its like getting a priority, layer 2 is basically 2.1 (first) and the elevator is 2.2 (second)*/
	std::vector<DrawOrder*> DrawingOrder[LEVEL_LAYERS];
	ALLEGRO_DISPLAY* display = NULL;
	ALLEGRO_BITMAP* Start_Screen_bitmap = NULL;
	Level *level = NULL;

	void Play_Music(const char* path);

	void Stop_Music();

	void Clear();

	Game_State Get_State();

	void Set_State(Game_State state);

	void End_Game();

	void Load_Level(unsigned short levelNum);

	void insert_DrawingOrder(DrawOrder *dro, unsigned int layer);
};

class Elevator
{
private:
	unsigned int row, col, curr_row;	//row and col count pixels (take caution when drawing because it might be out of the screens width or height and will not draw), curr_row is the height of the elevatorbitmap
	ALLEGRO_BITMAP* elevatorbitmap;
	DrawOrder* DrawObj;	/*pointers to update the object directly inside the drawing queue*/
	
public:
	Elevator(unsigned int _row, unsigned int _col);
	void Add_to_draw_queue();
	void Update_draw_obj();
	void hide_og_elevator();

	unsigned int getRow();
	unsigned int getCurrRow();
	unsigned int getCol();
	void setRow(unsigned int X);
	void setCurrRow(unsigned int X);
	void setCol(unsigned int Y);

	void addToCurrRow(unsigned int X);
};

class Player //player might be in layer 3 for drawing and compare with layer 1 for block collisions? enemies are a different story
{
private:
	Player_State state = State_Walking;
	Player_Direction direction = dir_right;
	int scrollDistanceX = 2, scrollDistanceY = 3;
	float Health = 10;
	int SwordX = -1, SwordY = -1;
public:
	ALLEGRO_BITMAP* PlayerSpriteSheet = NULL;
	int positionX, positionY;
	int screenX, screenY;	//measures screens/rooms
	unsigned int LinkSpriteNum = 0;
	bool jumping = false;
	std::vector<Rect>FramesWalkingLeft, FramesWalkingRight;	//the bounding box for each frame, x and y will be the position in the sprite sheet to help find the sprite we want
	std::vector<Rect>FramesCrounch;
	std::vector<Rect>FramesSlashLeft, FramesSlashRight;
	std::vector<Rect>FramesCrounchSlash;

	Player(int _positionX, int _positionY);

	~Player();

	void Set_Speed_X(int speedX);

	void Increment_Speed_X();

	int Get_Speed_X();

	void Set_Speed_Y(int speedY);

	void Increment_Speed_Y();

	void Decrement_Speed_Y();

	int Get_Speed_Y();

	void set_Direction(Player_Direction direction);

	Player_Direction get_Direction();

	void Set_State(Player_State state);

	Player_State Get_State();

	void Scroll_Player(float ScrollDistanceX, float ScrollDistanceY);

	void Init_frames_bounding_boxes();

	void Load_Player_Spritesheet();

	Rect FrameToDraw();

	float Get_Health();

	void Set_Health(float health);

	void Take_Damage(float health_damage);

	void Heal(float health_gain);

	void SetSwordX(int x);

	int GetSwordX();

	void SetSwordY(int y);

	int GetSwordY();
};

class TileColorsHolder final
{
private:
	std::set<Index> indices;
	std::set<ALLEGRO_COLOR> colors;
public:
	void Insert(ALLEGRO_BITMAP* bmp, Index index);

	bool ColorIn(ALLEGRO_COLOR c) const;

	bool IndexIn(Index index);
};

class Grid
{
public:
	unsigned int Grid_Element_Width, Grid_Element_Height;	//subdivisions of our tiles (TILE_WIDTH and TILE_HEIGHT)
	unsigned int Grid_Block_Columns, Grid_Block_Rows;
	unsigned int Grid_Elements_Per_Tile;
	unsigned int Grid_Max_Width, Grid_Max_Height;

	unsigned int MAX_PIXEL_WIDTH, MAX_PIXEL_HEIGHT;
	unsigned int layer;

	byte** grid;	// 2D Array that holds our grid

	/*Parameters _Grid_Element_Width and _Grid_Element_Height are for the size of each element in the grid.
	MAX_WIDTH and MAX_HEIGHT are the number of tiles of the tilemap you want to apply the grid*/
	Grid(unsigned int layer_num, unsigned int _Grid_Element_Width, unsigned int _Grid_Element_Height, unsigned int MAX_WIDTH, unsigned int MAX_HEIGHT);

	~Grid();	//Destructor


	void SetGridTile(unsigned short row, unsigned short col, byte index);

	byte GetGridTile(unsigned short row, unsigned short col);

	void SetSolidGridTile(unsigned short row, unsigned short col);

	void SetEmptyGridTile(unsigned short row, unsigned short col);

	void SetGridTileFlags(unsigned short row, unsigned short col, byte flags);

	void SetGridTileTopSolidOnly(unsigned short row, unsigned short col);

	bool CanPassGridTile(unsigned short row, unsigned short col, byte flags); // i.e. checks if flags set

	int getPlayerBottomRow(Player* player);

	int getPlayerLeftCol(Player* player);

	int getPlayerRightCol(Player* player);

	int GetIndexFromLayer(int gridRow, int gridCol);

	void FilterGridMotion(Player* player, int* dx, int* dy);

	void FilterGridMotionLeft(Player* player, int* dx);

	void FilterGridMotionRight(Player* player, int* dx);

	void FilterGridMotionUp(Player* player, int* dy);

	void FilterGridMotionDown(Player* player, int* dy);

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

	bool ComputeIsGridIndexEmpty(ALLEGRO_BITMAP* gridElement, byte solidThreshold);

	/*Computes the grid elements (solid or empty) based on the given map. (map argument is a vector with the values of the csv of the tilemap,
	those values will be used to get the corresponding tile from the tileset)*/
	void ComputeTileGridBlocks2(std::vector<std::vector<int>> map, ALLEGRO_BITMAP* tileSet, byte solidThreshold);
};

class Enemy
{
protected:
	Enemy_State state = E_State_Walking;
	unsigned int EnemySpriteNum = 0;	//counter for animation
	int scrollDistanceX = 2, scrollDistanceY = 3;
	Enemy_Direction direction = e_dir_right;

	float Health = 1.0;
public:
	int positionX, positionY;
	ALLEGRO_BITMAP* EnemySpriteSheet = NULL;
	

	Enemy(int posX, int posY);

	~Enemy();

	virtual void Init_frames_bounding_boxes() = 0;	//pure virtual

	virtual Rect FrameToDraw() = 0;

	void Set_Speed_X(int speedX);

	void Increment_Speed_X();

	int Get_Speed_X();

	void Set_Speed_Y(int speedY);

	void Increment_Speed_Y();

	void Decrement_Speed_Y();

	int Get_Speed_Y();

	virtual void Set_State(Enemy_State state) = 0;

	Enemy_State Get_State();

	void Load_Enemy_Spritesheet();

	void Scroll_Enemy(float ScrollDistanceX, float ScrollDistanceY);

	float Get_Health();

	void Set_Health(float health);

	void Take_Damage(float health_damage);

	void Heal(float health_gain);
	virtual void Scroll_Enemy(float ScrollDistanceX, float ScrollDistanceY) = 0;
};

class Stalfos : public Enemy
{
private:
	std::vector<Rect> FramesWalkingRight;
	std::vector<Rect> FramesSlashRight;
	std::vector<Rect> FramesFalling;
	
public :
	Stalfos(int x, int y);
	
	Enemy_State Get_State();

	void Init_frames_bounding_boxes();

	Rect FrameToDraw();

	void Set_State(Enemy_State state);

	void Scroll_Enemy(float ScrollDistanceX, float ScrollDistanceY);
};

class PalaceBot : public Enemy
{
private:
	std::vector<Rect> FramesWalking;
	std::vector<Rect> FramesJumping;

public:
	PalaceBot(int x, int y);

	Enemy_State Get_State();

	void Init_frames_bounding_boxes();

	Rect FrameToDraw();

	void Set_State(Enemy_State state);

	void Scroll_Enemy(float ScrollDistanceX, float ScrollDistanceY);
};



void createElevators();

void Init_Player(int PlayerX, int PlayerY);

void Load_Player_Spiresheet();

void add_Grid(unsigned int layer, unsigned int Grid_Element_Width, unsigned int Grid_Element_Height, unsigned int bitmapNumTilesWidth, unsigned int bitmapNumTilesHeight);

/*added these 3 overloards so that set<ALLEGRO_COLOR> could work*/
bool operator == (const ALLEGRO_COLOR c1, const ALLEGRO_COLOR c2);

bool operator != (const ALLEGRO_COLOR c1, const ALLEGRO_COLOR c2);

bool operator < (const ALLEGRO_COLOR c1, const ALLEGRO_COLOR c2);

bool IsTileColorEmpty(unsigned int emptyTileColor_num, ALLEGRO_COLOR c);

bool IsTileIndexAssumedEmpty(unsigned int layer, Index index);

/*Opens the file at "filepath" for reading, reads the indices of tiles that are considered empty in the tileset.
The format of the file should be numbers (indices) seperated by commas*/
void Init_emptyTileColorsHolder(const char* filepath);

void add_Stalfos(int EnemyX, int EnemyY);

void add_PalaceBot(int EnemyX, int EnemyY);

void add_Wosu(int EnemyX, int EnemyY);

