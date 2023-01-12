#pragma once

#include <stdio.h>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>
#include <set>

#include "GameObjectsClasses.h"

using namespace std;


ALLEGRO_DISPLAY* display = NULL;
ALLEGRO_BITMAP* bitmap = NULL, * TileSet = NULL, * Start_Screen_bitmap = NULL, * PlayerSpriteSheet = NULL;
vector<vector<vector<int>>>TileMapCSV;		//vector of layers, each layer made by 2d array of indices (vector<vector<int>>)
unsigned char* divIndex, * modIndex;
unsigned short TILESET_WIDTH = 0, TILESET_HEIGHT = 0;
bool draw = false;
ALLEGRO_TIMER* FPStimer;
ALLEGRO_EVENT_QUEUE* timerQueue;
int levelLoaded = -1;		//-1 == no level loaded

/*added these 3 overloards so that set<ALLEGRO_COLOR> could work*/
bool operator == (const ALLEGRO_COLOR c1, const ALLEGRO_COLOR c2);

bool operator != (const ALLEGRO_COLOR c1, const ALLEGRO_COLOR c2);

bool operator < (const ALLEGRO_COLOR c1, const ALLEGRO_COLOR c2);

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
// keeps colors that are assumed to be empty

std::vector<TileColorsHolder> emptyTileColors;

bool IsTileColorEmpty(unsigned int emptyTileColor_num, ALLEGRO_COLOR c);

bool IsTileIndexAssumedEmpty(unsigned int layer, Index index);

/*Opens the file at "filepath" for reading, reads the indices of tiles that are considered empty in the tileset.
The format of the file should be numbers (indices) seperated by commas*/
void Init_emptyTileColorsHolder(const char* filepath);

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

	void FilterGridMotion(Player *player, int* dx, int* dy);

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

std::vector<Grid*> grids;

/*class objlevel
{
	//maybe gather all of the global vars like the bitmap of the level, display position etc
	//maybe put the grid object in here
};*/

void add_Grid(unsigned int layer, unsigned int Grid_Element_Width, unsigned int Grid_Element_Height, unsigned int bitmapNumTilesWidth, unsigned int bitmapNumTilesHeight);

//pre-caching tileset indexes for better perfomance, this function should be called at the start of the programm
void Calculate_Tileset_Indexes();

/*loads a bitmap from the given filepath, sets the global variables TILESET_WIDTH and TILESET_HEIGHT then returns the loaded tileset as a bitmap
in case of bad file path exits program with -1*/
ALLEGRO_BITMAP* load_tileset(const char* filepath);

/*Given the path of a .csv file, returns a vector<vector<int>> of indices that correspondes to a TileSet*/
vector<vector<int>> ReadTextMap(string TileMapFileName);

/*Creates and returns a bitmap from an CSV and a Tileset, WILL NOT WORK IF A TILESET IS NOT LOADED (tileset width and height vars will have a value of 0)*/
ALLEGRO_BITMAP* Create_Bitmap_From_CSV(vector<vector<int>> CSV, ALLEGRO_BITMAP* Tileset, ALLEGRO_DISPLAY* display);

/*Paints over the given bitmap, useful for maps with multiple layers*/
void Paint_To_Bitmap(ALLEGRO_BITMAP* bitmap, vector<vector<int>> CSV, ALLEGRO_BITMAP* Tileset, ALLEGRO_DISPLAY* display);

/*uses the indeces from the CSV file which are stored in the CSV vector and uses them to blit from the given Tileset*/
void Draw_BitMap_From_CSV(vector<vector<int>> CSV, ALLEGRO_BITMAP* Tileset);

/*uses the indeces from the CSV file which are stored in the CSV vector and uses them to blit from the given Tileset with the given scaling*/
void Draw_Scaled_BitMap_From_CSV(vector<vector<int>> CSV, ALLEGRO_BITMAP* Tileset, float scaleWidth, float scaleHeight);

/*By screen i mean the target bitmap*/
void Paint_Player_to_Screen(Rect r);

void Load_Start_Screen();

void Load_Player_Spiresheet();

void Load_Level(unsigned short levelNum);

void Render_init();

void Scroll_Bitmap();

// use this to render grid (toggle on / off), used only for development time testing -
// a tile grid block is consecutive GRID_BLOCK_ROWS x GRID_BLOCK_COLUMNS block of grid indices
void DisplayGrid(unsigned int grid_num);

void Renderer();

/*might be useful to have this in the future, also add more stuff cuz i'll forget for sure*/
void Render_Clear();