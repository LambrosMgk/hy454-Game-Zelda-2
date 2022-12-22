#pragma once

#include <stdio.h>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>
#include <set>
#include <unordered_set>

#include <allegro5/allegro.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_primitives.h>

#include "User_Input.h"


#define DISPLAY_W 640
#define DISPLAY_H 480

#define TILE_WIDTH 16
#define TILE_HEIGHT 16

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


#define scrollDistanceX 5
#define scrollDistanceY 5


using namespace std;

typedef unsigned short Dim;
struct Rect { int x, y, w, h; };
struct Point { int x, y; };
typedef unsigned short Index;

ALLEGRO_DISPLAY* display = NULL;
ALLEGRO_BITMAP* bitmap = NULL, * TileSet = NULL;
vector<int> *EmptyTiles = NULL;
vector<vector<int>> TileMapCSV, TileMapCSV_l2;
unsigned char *divIndex;
unsigned char *modIndex;
unsigned short TILESET_WIDTH = 0, TILESET_HEIGHT = 0;
int cameraX = 0, cameraY = 0;
bool Toggle_Grid = false;

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
static TileColorsHolder emptyTileColors;

bool IsTileColorEmpty(ALLEGRO_COLOR c)
{
	return emptyTileColors.ColorIn(c);
} // return false to disable

bool IsTileIndexAssumedEmpty(Index index)
{
	return emptyTileColors.IndexIn(index);
}

/*Opens the file at "filepath" for reading, reads the indices of tiles that are considered empty in the tileset.
The format of the file should be numbers seperated by commas*/
void Init_emptyTileColorsHolder(const char* filepath)
{
	ifstream file;
	file.open(filepath);
	if (!file.is_open())
	{
		cout << "Error Init_emptyTileColorsHolder could not open file.\n";
		exit(-1);
	}

	ALLEGRO_BITMAP* bmp = al_create_bitmap(TILE_WIDTH, TILE_HEIGHT);
	al_set_target_bitmap(bmp);
	while (!file.eof())
	{
		string line;
		getline(file, line, ',');
		//cout << "Init_emptyTileColorsHolder testing : " << line << "\n";
		Index x = stoi(line);
		
		al_draw_bitmap_region(TileSet, MUL_TILE_WIDTH(modIndex[x]), MUL_TILE_HEIGHT(divIndex[x]), TILE_WIDTH, TILE_HEIGHT, 0, 0, 0);
		emptyTileColors.Insert(bmp, x);
	}

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

	byte** grid;	// 2D Array that holds our grid

	/*Parameters _Grid_Element_Width and _Grid_Element_Height are for the size of each element in the grid.
	MAX_WIDTH and MAX_HEIGHT are the number of tiles of the tilemap you want to apply the grid*/
	Grid(unsigned int _Grid_Element_Width, unsigned int _Grid_Element_Height, unsigned int MAX_WIDTH, unsigned int MAX_HEIGHT)
	{
		Grid_Element_Width = _Grid_Element_Width;
		Grid_Element_Height = _Grid_Element_Height;


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
		Grid_Max_Height = MAX_HEIGHT * Grid_Block_Rows;		//MAX_HEIGHT of the tilemap

		/*cout << "Grid created with : Grid_Block_Columns = " << Grid_Block_Columns << " Grid_Block_Rows = " << Grid_Block_Rows << '\n';
		cout << "Grid_Elements_Per_Tile = " << Grid_Elements_Per_Tile << "\n";
		cout << "Grid_Max_Width = " << Grid_Max_Width << " Grid_Max_Height" << Grid_Max_Height << '\n';
		*/

		grid = new byte*[Grid_Max_Width];
		for (int i = 0; i < Grid_Max_Width; i++)
			grid[i] = new byte[Grid_Max_Height];

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

	
	void SetGridTile(unsigned short col, unsigned short row, byte index)
	{
		grid[row][col] = index;
	}

	byte GetGridTile(unsigned short col, unsigned short row)
	{
		return grid[row][col];
	}

	void SetSolidGridTile(unsigned short col, unsigned short row)
	{
		SetGridTile(col, row, GRID_SOLID_TILE);
	}

	void SetEmptyGridTile(unsigned short col, unsigned short row)
	{
		SetGridTile(col, row, GRID_EMPTY_TILE);
	}

	void SetGridTileFlags(unsigned short col, unsigned short row, byte flags)
	{
		SetGridTile(col, row, flags);
	}

	void SetGridTileTopSolidOnly(unsigned short col, unsigned short row)
	{
		SetGridTileFlags(row, col, GRID_TOP_SOLID_MASK);
	}

	bool CanPassGridTile(unsigned short col, unsigned short row, byte flags) // i.e. checks if flags set
	{
		return (GetGridTile(row, col) & flags) != 0;
	}

	void FilterGridMotion(const Rect& r, int* dx, int* dy)
	{
		assert(abs(*dx) <= Grid_Element_Width && abs(*dy) <= Grid_Element_Height);

		// try horizontal move
		if (*dx < 0)
			FilterGridMotionLeft(r, dx);
		else if (*dx > 0)
			FilterGridMotionRight(r, dx);

		// try vertical move
		if (*dy < 0)
			FilterGridMotionUp(r, dy);
		else if (*dy > 0)
			FilterGridMotionDown(r, dy);
	}

	void FilterGridMotionLeft(const Rect& r, int* dx) 
	{
		auto x1_next = r.x + *dx;
		if (x1_next < 0)
			*dx = -r.x;
		else 
		{
			auto newCol = DIV_GRID_ELEMENT_WIDTH(x1_next);
			auto currCol = DIV_GRID_ELEMENT_WIDTH(r.x);
			if (newCol != currCol) 
			{
				assert(newCol + 1 == currCol); // we really move left
				auto startRow = DIV_GRID_ELEMENT_HEIGHT(r.y);
				auto endRow = DIV_GRID_ELEMENT_HEIGHT(r.y + r.h - 1);
				for (auto row = startRow; row <= endRow; ++row)
					if (!CanPassGridTile(newCol, row, GRID_RIGHT_SOLID_MASK)) 
					{
						*dx = MUL_GRID_ELEMENT_WIDTH(currCol) - r.x;
						break;
					}
			}
		}
	}

	void FilterGridMotionRight(const Rect& r, int* dx) 
	{
		auto x2 = r.x + r.w - 1;
		auto x2_next = x2 + *dx;
		if (x2_next >= MAX_PIXEL_WIDTH)
			*dx = (MAX_PIXEL_WIDTH - 1) - x2;
		else 
		{
			auto newCol = DIV_GRID_ELEMENT_WIDTH(x2_next);
			auto currCol = DIV_GRID_ELEMENT_WIDTH(x2);
			if (newCol != currCol)
			{
				assert(newCol - 1 == currCol); // we really move right
				auto startRow = DIV_GRID_ELEMENT_HEIGHT(r.y);
				auto endRow = DIV_GRID_ELEMENT_HEIGHT(r.y + r.h - 1);
				for (auto row = startRow; row <= endRow; ++row)
					if (!CanPassGridTile(newCol, row, GRID_LEFT_SOLID_MASK)) 
					{
						*dx = (MUL_GRID_ELEMENT_WIDTH(newCol) - 1) - x2;
						break;
					}
			}
		}
	}

	void FilterGridMotionUp(const Rect& r, int* dx)
	{

	}

	void FilterGridMotionDown(const Rect& r, int* dx)
	{

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
				byte r = 0, g = 0, b = 0;//, a = 0; i don't think i need alpha
				byte rt = 0, gt = 0, bt = 0;//, at = 0;
				al_unmap_rgb(c, &r, &g, &b);
				/*al_unmap_rgb(transColor, &rt, &gt, &bt);*/

				if (/*(r != rt && g != gt && b != bt) && */ !IsTileColorEmpty(c))
					++n;
			}
		}cout << "n = " << n << '\n';
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
				//PutTile(tileElem, 0, 0, tileSet, index);//copy the tile
				al_set_target_bitmap(tileElem);	//set as target the tileElem and copy/paint the tile onto it
				al_draw_bitmap_region(tileSet, MUL_TILE_WIDTH(modIndex[index]), MUL_TILE_HEIGHT(divIndex[index]), TILE_WIDTH, TILE_HEIGHT, 0, 0, 0);
				al_set_target_bitmap(al_get_backbuffer(display));	//reset the target back to the display

				if (IsTileIndexAssumedEmpty(index))
				{
					this->grid[row][col] = GRID_EMPTY_TILE;
				}
				else /*subdivide the tile Grid_Elements_Per_Tile times and check the colors of each sub-tile to see which are solid*/
				{
					al_set_target_bitmap(gridElem);
					for (auto i = 0; i < Grid_Elements_Per_Tile; ++i)
					{
						auto x = i % Grid_Block_Rows;
						auto y = i / Grid_Block_Rows;
						
						al_draw_bitmap_region(tileSet, MUL_TILE_WIDTH(modIndex[index]) + x * Grid_Element_Width, MUL_TILE_HEIGHT(divIndex[index]) + y * Grid_Element_Height, Grid_Element_Width, Grid_Element_Height, 0, 0, 0);
						
						auto isEmpty = ComputeIsGridIndexEmpty(gridElem, solidThreshold);
						this->grid[row][col] = isEmpty ? GRID_EMPTY_TILE : GRID_SOLID_TILE;
						cout << "this->grid[" << row << "][" << col << "] = " << this->grid[row][col] << "    ";
					}
					cout << "\n";
					al_set_target_bitmap(al_get_backbuffer(display));
				}
			}
		
		al_destroy_bitmap(tileElem);
		al_destroy_bitmap(gridElem);
	}
};

Grid* grid = NULL;

void init_Grid(unsigned int Grid_Element_Width, unsigned int Grid_Element_Height, unsigned int bitmapNumTilesWidth, unsigned int bitmapNumTilesHeight)
{
	assert(grid == NULL);
	grid = new Grid(Grid_Element_Width, Grid_Element_Height, bitmapNumTilesWidth, bitmapNumTilesHeight);
}

//pre-caching tileset indexes for better perfomance, this function should be called at the start of the programm
void Calculate_Tileset_Indexes(unsigned short TILESET_WIDTH, unsigned short TILESET_HEIGHT)
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

void Load_Start_Screen(const char* filepath)
{
	ALLEGRO_BITMAP* Start_bitmap = al_load_bitmap(filepath);
	if (Start_bitmap == NULL)
	{
		fprintf(stderr, "\nFailed to initialize Start_bitmap (al_load_bitmap() failed).\n");
		exit(-1);
	}
	al_draw_bitmap(Start_bitmap, 0, 0, 0);
	al_flip_display();
	//cout << "Drawing " << filepath << '\n';
	//maybe have a global var to set to the state "START SCREEN" or something
}

void Render_init()
{
	if (!al_init() || !al_init_image_addon() || !al_init_primitives_addon())
	{
		fprintf(stderr, "Cannot initialise the Allegro library");
		exit(-1);
	}

	al_set_new_display_flags(ALLEGRO_WINDOWED);
	display = al_create_display(DISPLAY_W, DISPLAY_H);
	if (!display)
	{
		fprintf(stderr, "failed to create display!\n");
		exit(-1);
	}
	al_set_window_title(display, "Zelda II: The Adventure of Link");


	TileSet = load_tileset("UnitTests\\Media\\overworld_tileset_grass.png");
	Calculate_Tileset_Indexes(TILESET_WIDTH, TILESET_HEIGHT);//pre-caching
	//add initalize for the colors of the tileset
	Init_emptyTileColorsHolder("UnitTests\\Media\\Assumed_Empty_Tiles_indices.txt");

	TileMapCSV = ReadTextMap("UnitTests\\Media\\map1_Kachelebene 1.csv");
	TileMapCSV_l2 = ReadTextMap("UnitTests\\Media\\map1_Tile Layer 2.csv");
	if (TileMapCSV.size() == 0)
	{
		fprintf(stderr, "ReadTextMap(string TileMapFileName) returned empty vector.\n");
		exit(-1);
	}

	bitmap = Create_Bitmap_From_CSV(TileMapCSV, TileSet, display);
	Paint_To_Bitmap(bitmap, TileMapCSV_l2, TileSet, display);
	
	init_Grid(TILE_WIDTH, TILE_HEIGHT, TileMapCSV.size(), TileMapCSV[0].size());		//initialize the grid for the tilemap
	grid->ComputeTileGridBlocks2(TileMapCSV, TileSet, 30);
	//al_draw_bitmap(bitmap, -cameraX, -cameraY, 0);
	al_flip_display();/*Copies or updates the front and back buffers so that what has been drawn previously on the currently selected display becomes visible
	on screen. Pointers to the special back and front buffer bitmaps remain valid and retain their semantics as back and front buffers
	respectively, although their contents may have changed.*/
}

//might be useful
/*int allegro_startup(void)
{
	if (al_init())
	{
		if (al_init_primitives_addon())
		{
			if (al_install_keyboard())
			{
				if (al_install_mouse())
				{
					if (al_init_image_addon())
					{
						al_init_font_addon();   //Void
						if (al_init_ttf_addon())
						{
							if (al_install_audio())
							{
								if (al_init_acodec_addon())
								{
									if (al_reserve_samples(1))
									{

										return AL_STARTUP_SUCCESS;


									}
									else
										fprintf(stderr, "ERROR: Failed to reserve samples:(\n");
									//al_shutdown_acodec_addon(); Does not exist
								}
								else
									fprintf(stderr, "ERROR: Failed to initialize acodec addon\n");
								al_uninstall_audio();
							}
							else
								fprintf(stderr, "ERROR: Failed to install audio\n");
							al_shutdown_ttf_addon();
						}
						else
							fprintf(stderr, "ERROR: Failed to initialize ttf addon\n");
						al_shutdown_font_addon();
						al_shutdown_image_addon();
					}
					else
						fprintf(stderr, "ERROR: Failed to initialize image addon\n");
					al_uninstall_mouse();
				}
				else
					fprintf(stderr, "ERROR: Failed to install mouse\n");
				al_uninstall_keyboard();
			}
			else
				fprintf(stderr, "ERROR: Failed to load primitives addon\n");
			al_shutdown_primitives_addon();
		}
		else
			fprintf(stderr, "ERROR: Failed to install keyboard\n");
		al_uninstall_system();
	}
	else
		fprintf(stderr, "ERROR: Failed to initialize allegro system\n");
	return AL_STARTUP_ERROR;
}

void allegro_shut_down(ALLEGRO_DISPLAY* display, ALLEGRO_EVENT_QUEUE* event_queue)
{
	al_destroy_display(display);
	al_destroy_event_queue(event_queue);
	al_uninstall_audio();
	al_shutdown_ttf_addon();
	al_shutdown_font_addon();
	al_shutdown_image_addon();
	al_uninstall_mouse();
	al_uninstall_keyboard();
	al_shutdown_primitives_addon();
	al_uninstall_system();

}*/

void Scroll(float ScrollDistanceX, float ScrollDistanceY)
{	
	unsigned int TotalHeight = al_get_bitmap_height(bitmap);
	unsigned int TotalWidth = al_get_bitmap_width(bitmap);

	/*check if you want to go out of boundary*/
	if ((ScrollDistanceY > 0 && cameraY < TotalHeight - 200) || (ScrollDistanceY < 0 && cameraY > -100))
	{
		cameraY += ScrollDistanceY;
	}
	if ((ScrollDistanceX > 0 && cameraX < TotalWidth - 200) || (ScrollDistanceX < 0 && cameraX > -100))
	{
		cameraX += ScrollDistanceX;
	}

	al_draw_bitmap(bitmap, -cameraX, -cameraY, 0);
	//al_flip_display();
}

// use this to render grid (toggle on / off), used only for development time testing -
// a tile grid block is consecutive GRID_BLOCK_ROWS x GRID_BLOCK_COLUMNS block of grid indices
void DisplayGrid()
{
	if (grid == NULL)
	{
		return;
	}

	auto startCol = DIV_TILE_WIDTH(cameraX);
	auto startRow = DIV_TILE_HEIGHT(cameraY);
	auto endCol = DIV_TILE_WIDTH(cameraX + DISPLAY_W - 1);
	auto endRow = DIV_TILE_HEIGHT(cameraY + DISPLAY_H - 1);

	//in case of a bitmap smaller than the screen (e.g. when testing the engine)
	if (endCol > grid->Grid_Max_Width)
		endCol = grid->Grid_Max_Width/ grid->Grid_Block_Columns;	//divide with block cols and rows to get the max width and height for tiles
	if (endRow > grid->Grid_Max_Height)
		endRow = grid->Grid_Max_Height/ grid->Grid_Block_Rows;

	for (Dim rowTile = startRow; rowTile <= endRow; ++rowTile)
	{
		for (Dim colTile = startCol; colTile <= endCol; ++colTile)
		{
			auto sx = colTile - startCol;
			auto sy = rowTile - startRow;
			
			for (auto rowElem = 0; rowElem < grid->Grid_Block_Rows; ++rowElem)
			{
				for (auto colElem = 0; colElem < grid->Grid_Block_Columns; ++colElem)
				{
					if (grid->grid[rowTile + rowElem][colTile + colElem] & GRID_SOLID_TILE)
					{
						auto x = (rowTile + rowElem)* grid->Grid_Element_Width;
						auto y = (colTile + colElem)* grid->Grid_Element_Height;
						auto w = grid->Grid_Element_Width;
						auto h = grid->Grid_Element_Height;
						al_draw_filled_rectangle(x, y, x+w, y+h, al_map_rgb(255, 0, 0));
					}
				}
			}
				
		}
	}
}

void Renderer()
{
	int scrollx = 0, scrolly = 0;
	static bool scrollUp = false, scrollDown = false, scrollLeft = false, scrollRight = false;
	while (!al_is_event_queue_empty(EventQueue)) //Check all the user input in a frame
	{
		ALLEGRO_EVENT event;
		if (!al_get_next_event(EventQueue, &event))
		{
			cout << "Error : EventQueue empty when !al_is_event_queue_empty() returned false\n";
			exit(-1);
		}
		
		
		if (event.type == ALLEGRO_EVENT_KEY_DOWN)
		{
			switch (event.keyboard.keycode)
			{
				case ALLEGRO_KEY_ESCAPE:
					User_input_done = true;		//ends the game for now
					break;
				case ALLEGRO_KEY_UP :
					scrollUp = true;
					break;
				case ALLEGRO_KEY_DOWN:
					scrollDown = true;
					break;
				case ALLEGRO_KEY_LEFT:
					scrollLeft = true;
					break;
				case ALLEGRO_KEY_RIGHT:
					scrollRight = true;
					break;
				case ALLEGRO_KEY_ENTER:			//"ENTER" will be the action key for now. check if im at the starting screen and if i need to load other bitmaps
					al_clear_to_color(al_map_rgb(0, 0, 0));	//Clear the complete target bitmap, but confined by the clipping rectangle.
					al_draw_bitmap(bitmap, -cameraX, -cameraY, 0);
					break;
				case ALLEGRO_KEY_LCTRL:
					ALLEGRO_KEYBOARD_STATE KbState;
					cout << "LCTRL\n";
					//try a loop through the event queue till empty or find G
					al_get_keyboard_state(&KbState);
					if (al_key_down(&KbState, ALLEGRO_KEY_G))
					{
						Toggle_Grid = Toggle_Grid ? false : true;
						cout << "Grid troggled\n";
					}
					break;
			}
		}
		else if (event.type == ALLEGRO_EVENT_KEY_UP)
		{
			switch (event.keyboard.keycode)
			{
				case ALLEGRO_KEY_UP:
					scrollUp = false;
					break;
				case ALLEGRO_KEY_DOWN:
					scrollDown = false;
					break;
				case ALLEGRO_KEY_LEFT:
					scrollLeft = false;
					break;
				case ALLEGRO_KEY_RIGHT:
					scrollRight = false;
					break;			
			}
		}
		
		if (Toggle_Grid)
		{
			DisplayGrid();
		}
	}


	if(scrollUp == true) 
		scrolly -= scrollDistanceY;
	if(scrollDown == true)
		scrolly += scrollDistanceY;
	if(scrollLeft == true )
		scrollx -= scrollDistanceX;
	if(scrollRight == true)
		scrollx += scrollDistanceX;
		
	if(scrollx != 0 || scrolly != 0)
		Scroll(scrollx, scrolly);
	al_flip_display();
}

/*might be useful to have this in the future*/
void Render_Clear()
{
	delete grid;
	delete divIndex;
	delete modIndex;
}