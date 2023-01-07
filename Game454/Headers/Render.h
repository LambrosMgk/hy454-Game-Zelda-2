#pragma once

#include <stdio.h>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>
#include <set>

#include "al_init.h"
#include "User_Input.h"
#include "ProgressAnimations.h"

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

class TileColorsHolder final 
{
	private:
		std::set<Index> indices;
		set<ALLEGRO_COLOR> colors;
	public:
		void Insert(ALLEGRO_BITMAP *bmp, Index index) 
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
		bool ColorIn(ALLEGRO_COLOR c) const
		{
			set<ALLEGRO_COLOR>::iterator ptr = colors.begin();

			while(ptr != colors.end())
			{
				if (ptr->r == c.r && ptr->g == c.g && ptr->b == c.b)
				{
					return true;
				}
				ptr = next(ptr);
			}

			return false;
		}
		bool IndexIn(Index index)
		{
			return indices.find(index) != indices.end();
		}
};
// keeps colors that are assumed to be empty

std::vector<TileColorsHolder> emptyTileColors;

bool IsTileColorEmpty(unsigned int emptyTileColor_num,ALLEGRO_COLOR c)
{
	return emptyTileColors[emptyTileColor_num].ColorIn(c);
}

bool IsTileIndexAssumedEmpty(unsigned int layer, Index index)
{
	return emptyTileColors[layer].IndexIn(index);
}

/*Opens the file at "filepath" for reading, reads the indices of tiles that are considered empty in the tileset.
The format of the file should be numbers (indices) seperated by commas*/
void Init_emptyTileColorsHolder(const char* filepath)
{
	ifstream file;
	file.open(filepath);
	if (!file.is_open())
	{
		cout << "Error Init_emptyTileColorsHolder could not open file.\n";
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
		
		al_draw_bitmap_region(TileSet, MUL_TILE_WIDTH(modIndex[x]), MUL_TILE_HEIGHT(divIndex[x]), TILE_WIDTH, TILE_HEIGHT, 0, 0, 0);
		tch->Insert(bmp, x);
	}
	
	emptyTileColors.push_back(*tch);
	al_set_target_bitmap(al_get_backbuffer(display));
	al_destroy_bitmap(bmp);
	file.close();
}

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
	Grid(unsigned int layer_num,unsigned int _Grid_Element_Width, unsigned int _Grid_Element_Height, unsigned int MAX_WIDTH, unsigned int MAX_HEIGHT)
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

		grid = new byte*[Grid_Max_Height];
		for (int i = 0; i < Grid_Max_Height; i++)
			grid[i] = new byte[Grid_Max_Width];

		MAX_PIXEL_WIDTH = MUL_TILE_WIDTH(MAX_WIDTH);
		MAX_PIXEL_HEIGHT = MUL_TILE_HEIGHT(MAX_HEIGHT);
	}

	~Grid()	//Destructor
	{
		//Free the memory for the grid array
		for (int i = 0; i < Grid_Max_Height; i++)
			delete [] grid[i];
		delete [] grid;
	}

	
	void SetGridTile(unsigned short row, unsigned short col, byte index)
	{
		grid[row][col] = index;
	}

	byte GetGridTile(unsigned short row, unsigned short col)
	{
		return grid[row][col];
	}

	void SetSolidGridTile(unsigned short row, unsigned short col)
	{
		SetGridTile(row, col, GRID_SOLID_TILE);
	}

	void SetEmptyGridTile(unsigned short row, unsigned short col)
	{
		SetGridTile(row, col, GRID_EMPTY_TILE);
	}

	void SetGridTileFlags(unsigned short row, unsigned short col, byte flags)
	{
		SetGridTile(row, col, flags);
	}

	void SetGridTileTopSolidOnly(unsigned short row, unsigned short col)
	{
		SetGridTileFlags(row, col, GRID_TOP_SOLID_MASK);
	}

	bool CanPassGridTile(unsigned short row, unsigned short col, byte flags) // i.e. checks if flags set
	{
		return (GetGridTile(row, col) & flags) == 0;	//changed from != 0
	}

	void FilterGridMotion(Player *player, int* dx, int* dy)
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

	void FilterGridMotionLeft(Player* player, int* dx)
	{
		auto x1_next = player->positionX + player->screenX*DISPLAY_W + *dx;
		if (x1_next < 0)
			*dx = -player->positionX;
		else
		{
			auto newCol = (x1_next)/Grid_Element_Width;
			auto currCol = (player->positionX + player->screenX * DISPLAY_W)/ Grid_Element_Width;
			if (newCol != currCol) 
			{
				assert(newCol + 1 == currCol); // we really move left
				auto startRow = (player->positionY + player->screenY * DISPLAY_H)/Grid_Element_Height;
				auto endRow = (player->positionY + player->screenY * DISPLAY_H + LINK_SPRITE_HEIGHT*2 - 1)/Grid_Element_Height;
				//cout << "newCol : " << newCol << ", currCol : " << currCol << ", startRow : " << startRow << ", endrow : " << endRow << '\n';
				for (auto row = startRow; row <= endRow; ++row)
					if (!CanPassGridTile(row, newCol, GRID_LEFT_SOLID_MASK))
					{
						*dx = Grid_Element_Width *(currCol) - (player->positionX + player->screenX * DISPLAY_W);
						//cout << "*dx = " << *dx << "\n";
						break;
					}
			}
		}
	}

	void FilterGridMotionRight(Player* player, int* dx)
	{
		auto x2 = player->positionX + player->screenX * DISPLAY_W + LINK_SPRITE_WIDTH*2 - 1;
		auto x2_next = x2 + *dx;
		if (x2_next >= MAX_PIXEL_WIDTH)
			*dx = (MAX_PIXEL_WIDTH - 1) - x2;
		else 
		{
			auto newCol = (x2_next)/Grid_Element_Width;
			auto currCol = (x2)/Grid_Element_Width;
			if (newCol != currCol)
			{
				assert(newCol - 1 == currCol); // we really move right
				auto startRow = (player->positionY + player->screenY * DISPLAY_H)/Grid_Element_Height;
				auto endRow = (player->positionY + player->screenY * DISPLAY_H + LINK_SPRITE_HEIGHT*2 - 1)/Grid_Element_Height;
				//cout << "newCol : " << newCol << ", currCol : " << currCol << ", startRow : " << startRow << ", endrow : " << endRow << '\n';
				for (auto row = startRow; row <= endRow; ++row)
					if (!CanPassGridTile(row, newCol, GRID_RIGHT_SOLID_MASK))
					{
						*dx = (Grid_Element_Width*(newCol) - 1) - (x2);
						//cout << "*dx = " << *dx << "\n";
						break;
					}
			}
		}
	}

	void FilterGridMotionUp(Player* player, int* dy)
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
				auto endCol = (player->positionX + player->screenX * DISPLAY_W + LINK_SPRITE_WIDTH*2 - 1) / Grid_Element_Width;
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

	void FilterGridMotionDown(Player* player, int* dy)
	{
		auto x2 = player->positionY + player->screenY * DISPLAY_H + LINK_SPRITE_HEIGHT*2 - 1;
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
				auto endCol = (player->positionX + player->screenX * DISPLAY_W + LINK_SPRITE_WIDTH*2 - 1) / Grid_Element_Width;
				//cout << "newRow : " << newRow << ", currRow : " << currRow << ", startCol : " << startCol << ", endCol : " << endCol << '\n';
				for (auto col = startCol; col <= endCol; ++col)
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
	
	bool ComputeIsGridIndexEmpty(ALLEGRO_BITMAP* gridElement, byte solidThreshold)
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

				if (/*(r != rt && g != gt && b != bt) && */ !IsTileColorEmpty(layer,c))
					++n;
			}
		}//cout << "\nn = " << n << '\n';
		al_unlock_bitmap(gridElement);

		return n <= solidThreshold;
	}

	/*Computes the grid elements (solid or empty) based on the given map. (map argument is a vector with the values of the csv of the tilemap,
	those values will be used to get the corresponding tile from the tileset)*/
	void ComputeTileGridBlocks2(vector<vector<int>> map, ALLEGRO_BITMAP* tileSet, byte solidThreshold)
	{
		if (map.size() == 0)
		{
			cout << "ComputeTileGridBlocks2 got called with bad argument \"map\".";
			return;
		}
		auto tileElem = al_create_bitmap(TILE_WIDTH, TILE_HEIGHT);		//e.g. 16x16
		auto gridElem = al_create_bitmap(Grid_Element_Width, Grid_Element_Height);	//e.g. 8x8

		for (auto row = 0; row < map.size(); ++row)
			for (auto col = 0; col < map[0].size(); ++col)
			{
				auto index = map[row][col];
				al_set_target_bitmap(tileElem);	//set as target the tileElem and copy/paint the tile onto it
				al_draw_bitmap_region(tileSet, MUL_TILE_WIDTH(modIndex[index]), MUL_TILE_HEIGHT(divIndex[index]), TILE_WIDTH, TILE_HEIGHT, 0, 0, 0);
				al_set_target_bitmap(al_get_backbuffer(display));	//reset the target back to the display

				if (IsTileIndexAssumedEmpty(layer,index) || index == -1)
				{
					//printf("this->grid[%d][%d] = %d\n", row, col, this->grid[row][col]);
					this->grid[row][col] = GRID_EMPTY_TILE;
				}
				else /*subdivide the tile Grid_Elements_Per_Tile times and check the colors of each sub-tile to see which are solid*/
				{
					al_set_target_bitmap(gridElem);
					for (auto i = 0; i < Grid_Elements_Per_Tile; i++)
					{
						auto x = i % Grid_Block_Rows;
						auto y = i / Grid_Block_Rows;
						
						al_draw_bitmap_region(tileSet, MUL_TILE_WIDTH(modIndex[index]) + x * Grid_Element_Width, MUL_TILE_HEIGHT(divIndex[index]) + y * Grid_Element_Height, Grid_Element_Width, Grid_Element_Height, 0, 0, 0);
						
						auto isEmpty = ComputeIsGridIndexEmpty(gridElem, solidThreshold);
						this->grid[row][col] = isEmpty ? GRID_EMPTY_TILE : GRID_SOLID_TILE;
						//printf("\nthis->grid[%d][%d] = %d", row, col, this->grid[row][col]);
					}
					al_set_target_bitmap(al_get_backbuffer(display));
				}
			}
		
		al_destroy_bitmap(tileElem);
		al_destroy_bitmap(gridElem);
	}
};

std::vector<Grid*> grids;

/*class objlevel
{
	//maybe gather all of the global vars like the bitmap of the level, display position etc
	//maybe put the grid object in here
};*/

void add_Grid(unsigned int layer,unsigned int Grid_Element_Width, unsigned int Grid_Element_Height, unsigned int bitmapNumTilesWidth, unsigned int bitmapNumTilesHeight)
{
	
	grids.push_back(new Grid(layer,Grid_Element_Width, Grid_Element_Height, bitmapNumTilesWidth, bitmapNumTilesHeight));
}

//pre-caching tileset indexes for better perfomance, this function should be called at the start of the programm
void Calculate_Tileset_Indexes()
{
	divIndex = new unsigned char[TILESET_WIDTH*TILESET_HEIGHT];
	modIndex = new unsigned char[TILESET_WIDTH * TILESET_HEIGHT];
	if (divIndex == NULL || modIndex == NULL)
	{
		cout << "Failed to allocate memory for tileset indexes\n";
		exit(-1);
	}
	for (unsigned short i = 0; i < TILESET_WIDTH* TILESET_HEIGHT; ++i)
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

/*Given the path of a .csv file, returns a vector<vector<int>> of indeces that correspondes to a TileSet*/
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

	add_Grid(0,TILE_WIDTH, TILE_HEIGHT, TileMapCSV[0][0].size(), TileMapCSV[0].size());		//initialize the grid for layer 1
	grids[0]->ComputeTileGridBlocks2(TileMapCSV[0], TileSet, 128);
	cout << "Created the whole grid for layer 1\n";

	add_Grid(1, TILE_WIDTH, TILE_HEIGHT, TileMapCSV[1][0].size(), TileMapCSV[1].size());		//initialize the grid for layer 2
	grids[1]->ComputeTileGridBlocks2(TileMapCSV[1], TileSet, 64);
	cout << "Created the whole grid for layer 2\n";

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
	if(cameraX < 0)
		startCol += DIV_TILE_WIDTH(abs(cameraX* DISPLAY_W)) + 1;
	if (cameraY < 0)
		startRow += DIV_TILE_HEIGHT(abs(cameraY* DISPLAY_H)) + 1;

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
						auto x = MUL_TILE_WIDTH(colTile) + (colElem)* grids[grid_num]->Grid_Element_Width;	//if i want better perfomance note : try pre-caching the power of 2 of the grid elem width so that i can do something like colelem >> MUL_GRID_WIDTH
						auto y = MUL_TILE_HEIGHT(rowTile) + (rowElem)* grids[grid_num]->Grid_Element_Height;
						auto w = grids[grid_num]->Grid_Element_Width - 1;
						auto h = grids[grid_num]->Grid_Element_Height - 1;
						
						al_draw_filled_rectangle(x + cameraX, y + cameraY, x+w + cameraX, y+h + cameraY, al_map_rgba(255, 0, 0, 64));
					}
				}
			}
				
		}
	}

	al_draw_rectangle(player->positionX,player->positionY, player->positionX + player->FrameToDraw().w, player->positionY + player->FrameToDraw().h,al_map_rgba(0,255,0,64),1.0);
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
	for (int i = 0; i < grids.size();i++) {
		delete grids[i];
	}	
	delete divIndex;
	delete modIndex;
}