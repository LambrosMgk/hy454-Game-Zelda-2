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

#define DISPLAY_W 640
#define DISPLAY_H 480

#define TILE_WIDTH 16
#define TILE_HEIGHT 16

#define StartPlayerPositionX 5*TILE_WIDTH
#define StartPlayerPositionY 11*TILE_HEIGHT-1

#define FPS 60.0
#define LAYERS 2

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

#define ELEVATORID1 12
#define ELEVATORID2 13

//used in render 
bool keyboardUp = false, scrollDown = true, scrollLeft = false, scrollRight = false;	//omit these later, maybe not left and right? useful for animation?
//used in physics and animations

typedef unsigned short Dim;
typedef unsigned short Index;
struct Rect { int x, y, w, h; };
struct Point { int x, y; };

enum Game_State { StartingScreen, PlayingLevel1, Paused };
enum Player_Direction { dir_left, dir_right };
enum Player_State { State_Walking, State_Crounching, State_Attacking, State_CrounchAttacking, State_Elevator };
Player *player = NULL;

//forward declaration
class TileColorsHolder;
class GameLogic;


// keeps colors that are assumed to be empty
std::vector<TileColorsHolder> emptyTileColors;
GameLogic gameLogic;	//object that holds the game state and other useful information

class Level
{
public:
	ALLEGRO_BITMAP* bitmap = NULL, * TileSet = NULL;
	std::vector<std::vector<std::vector<int>>>TileMapCSV;		//vector of layers, each layer made by 2d array of indices (vector<vector<int>>)
	unsigned char* divIndex, * modIndex;
	unsigned short TILESET_WIDTH = 0, TILESET_HEIGHT = 0;
	int cameraX = 0, cameraY = 0;
	std::vector<Grid*> grids;
};

class GameLogic
{
public:
	ALLEGRO_DISPLAY* display = NULL;
	ALLEGRO_BITMAP* Start_Screen_bitmap = NULL, * PlayerSpriteSheet = NULL;
	Level *level = NULL;
};

class Elevator
{
private:
	unsigned int x, y;
	ALLEGRO_BITMAP* elevatorbitmap;
	
public:
	Elevator(unsigned int _x, unsigned int _y);
	void Paint_Elevator();
	void hide_og_elevator();
};

std::vector<Elevator> elevators;

void createElevators();

class Player //player might be in layer 3 for drawing and compare with layer 1 for block collisions? enemies are a different story
{
private:
	Player_State state = State_Walking;
	int scrollDistanceX = 2, scrollDistanceY = 3;
public:
	Player_Direction direction = dir_right;
	int positionX, positionY;
	int screenX, screenY;	//measures screens/rooms
	unsigned int LinkSpriteNum = 0;
	boolean jumping = false;
	std::vector<Rect>FramesWalkingLeft, FramesWalkingRight;	//the bounding box for each frame, x and y will be the position in the sprite sheet to help find the sprite we want
	std::vector<Rect>FramesCrounch;
	std::vector<Rect>FramesSlashLeft, FramesSlashRight;
	std::vector<Rect>FramesCrounchSlash;

	Player(float _positionX, float _positionY);

	~Player();

	void Set_Speed_X(int speedX);

	void Increment_Speed_X();

	int Get_Speed_X();

	void Set_Speed_Y(int speedY);

	void Increment_Speed_Y();

	void Decrement_Speed_Y();

	int Get_Speed_Y();

	void Set_State(Player_State state);

	Player_State Get_State();

	void Scroll_Player(float ScrollDistanceX, float ScrollDistanceY);

	void Init_frames_bounding_boxes();

	Rect FrameToDraw();
};

class TileColorsHolder final
{
private:
	std::set<Index> indices;
	set<ALLEGRO_COLOR> colors;
public:
	void Insert(ALLEGRO_BITMAP* bmp, Index index);

	bool ColorIn(ALLEGRO_COLOR c) const;

	bool IndexIn(Index index);
};

void Init_Player(int PlayerX, int PlayerY);

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

	int getPlayerStartCol(Player* player);

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
	void ComputeTileGridBlocks2(vector<vector<int>> map, ALLEGRO_BITMAP* tileSet, byte solidThreshold);
};

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