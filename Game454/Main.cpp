#include <stdio.h>
#include <allegro5/allegro.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_primitives.h>

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>


#define DISPLAY_W 640
#define DISPLAY_H 480

#define TILE_WIDTH 16
#define TILE_HEIGHT 16
#define COL_SHIFT 4
#define ROW_MASK 0x0C
#define COL_MASK 0xF0


using namespace std;

unsigned short TILESET_WIDTH = 0, TILESET_HEIGHT = 0;
float cameraX = 0, cameraY = 0;


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
	ALLEGRO_BITMAP* bitmap = al_create_bitmap(CSV[0].size()*TILE_WIDTH, CSV.size()*TILE_HEIGHT);
	int TileSetIndex;

	//cout << "Create_Bitmap_From_CSV() height : " << al_get_bitmap_height(bitmap) << " width : " << al_get_bitmap_width(bitmap) << "\n";
	al_set_target_bitmap(bitmap);	/*Select the bitmap to which all subsequent drawing operations in the calling thread will draw.*/
	for (auto y = 0; y < CSV.size(); y++)
	{
		for (auto x = 0; x < CSV[0].size(); x++)
		{
			TileSetIndex = CSV[y][x];
			if (TileSetIndex != -1)
				al_draw_bitmap_region(Tileset, (TileSetIndex % TILESET_WIDTH) * TILE_WIDTH, (TileSetIndex / TILESET_WIDTH) * TILE_HEIGHT, TILE_WIDTH, TILE_HEIGHT, x * TILE_WIDTH, y * TILE_HEIGHT, 0);
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
	for (auto y = 0; y < CSV.size(); y++)
	{
		for (auto x = 0; x < CSV[0].size(); x++)
		{
			TileSetIndex = CSV[y][x];
			if (TileSetIndex != -1)
				al_draw_bitmap_region(Tileset, (TileSetIndex % TILESET_WIDTH) * TILE_WIDTH, (TileSetIndex / TILESET_WIDTH) * TILE_HEIGHT, TILE_WIDTH, TILE_HEIGHT, x * TILE_WIDTH, y * TILE_HEIGHT, 0);
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
			if(TileSetIndex != -1)
				al_draw_bitmap_region(Tileset, (TileSetIndex % TILESET_WIDTH) * TILE_WIDTH, (TileSetIndex / TILESET_WIDTH) * TILE_HEIGHT, TILE_WIDTH, TILE_HEIGHT, x * TILE_WIDTH, y * TILE_HEIGHT, 0);
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
			al_draw_scaled_bitmap(Tileset, (TileSetIndex % TILESET_WIDTH) * TILE_WIDTH, (TileSetIndex / TILESET_WIDTH) * TILE_HEIGHT, TILE_WIDTH, TILE_HEIGHT, scaleWidth * x * TILE_WIDTH, scaleHeight * y * TILE_HEIGHT, scaleWidth * TILE_WIDTH, scaleHeight * TILE_HEIGHT, 0);
		}
	}
}

/*Checks for keyboard input and repaints the bitmap*/
void Scroll(ALLEGRO_BITMAP* bitmap, ALLEGRO_DISPLAY* display, float scrollDistance)
{
	assert(scrollDistance > 0);		/*scrollDistance <= 0 doesn't make sense*/
	ALLEGRO_KEYBOARD_STATE KbState;
	unsigned int TotalHeight = al_get_bitmap_height(bitmap), TotalWidth = al_get_bitmap_width(bitmap);

	al_get_keyboard_state(&KbState);
	if (al_key_down(&KbState, ALLEGRO_KEY_DOWN) && cameraY < TotalHeight-200) /*check if you want to go out of boundary*/
	{
		cameraY += scrollDistance;
	}
	if (al_key_down(&KbState, ALLEGRO_KEY_UP) && cameraY > -100)	/*"if" is not connected with "else" so if played presses up and down nothing will happen*/
	{
		cameraY -= scrollDistance;
	}
	if (al_key_down(&KbState, ALLEGRO_KEY_RIGHT) && cameraX < TotalWidth - 200)
	{
		cameraX += scrollDistance;
	}
	if (al_key_down(&KbState, ALLEGRO_KEY_LEFT) && cameraX > -100)
	{
		cameraX -= scrollDistance;
	}
	al_draw_bitmap(bitmap, -cameraX, -cameraY, 0);
	al_flip_display();
}

/*for now just checks if "esc" is pressed*/
void UserInput(unsigned char * done)
{
	ALLEGRO_KEYBOARD_STATE KbState;

	al_get_keyboard_state(&KbState);
	if (al_key_down(&KbState, ALLEGRO_KEY_ESCAPE))
	{
		*done = 1;
	}
}

int main(int argc, char* argv[])
{
	ALLEGRO_DISPLAY* display = NULL;
	ALLEGRO_BITMAP* bitmap = NULL, * TileSet = NULL;
	vector<vector<int>> TileMapCSV, TileMapCSV_l2;
	unsigned char done = 0;

	if (!al_init() || !al_init_image_addon() || !al_init_primitives_addon())
	{
		fprintf(stderr, "Cannot initialise the Allegro library");
		return -1;
	}
	if (!al_install_keyboard())
	{
		fprintf(stderr, "failed to install keyboard().\n");
		return -1;
	}

	display = al_create_display(DISPLAY_W, DISPLAY_H);
	if (!display)
	{
		fprintf(stderr, "failed to create display!\n");
		return -1;
	}
	al_set_window_title(display, "Zelda II: The Adventure of Link");


	TileSet = load_tileset("UnitTests\\Media\\overworld_tileset_grass.png");

	TileMapCSV = ReadTextMap("UnitTests\\Media\\map1_Kachelebene 1.csv");
	TileMapCSV_l2 = ReadTextMap("UnitTests\\Media\\map1_Tile Layer 2.csv");
	if (TileMapCSV.size() == 0)
	{
		fprintf(stderr, "ReadTextMap(string TileMapFileName) returned empty vector.\n");
		return -1;
	}

	bitmap = Create_Bitmap_From_CSV(TileMapCSV, TileSet, display);
	Paint_To_Bitmap(bitmap, TileMapCSV_l2, TileSet, display);
	

	al_flip_display();/*Copies or updates the front and back buffers so that what has been drawn previously on the currently selected display becomes visible
	on screen. Pointers to the special back and front buffer bitmaps remain valid and retain their semantics as back and front buffers
	respectively, although their contents may have changed.*/


	while (!done)
	{
		Sleep(25);	/*need to change this later for smoother scrolling*/
		UserInput(&done);
		Scroll(bitmap, display, 5);
	}

	
	al_rest(1.0);
	al_destroy_display(display);

	return 0;
}