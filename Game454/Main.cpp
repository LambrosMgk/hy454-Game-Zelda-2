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

/*returns a vector<vector<int>> of indeces that corresponde to a TileSet*/
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

/*uses the indeces from the CSV file which are stored in the CSV vector and uses them to blit from the given Tileset*/
void Draw_BitMap_From_CSV(vector<vector<int>> CSV, ALLEGRO_BITMAP* Tileset)
{
	ALLEGRO_BITMAP* bitmap = al_create_bitmap(CSV[0].size(), CSV.size());	/*ligo prosoxh me to .size() gia seg fault*/
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
	ALLEGRO_BITMAP* bitmap = al_create_bitmap(CSV[0].size(), CSV.size());	/*ligo prosoxh me to .size() gia seg fault*/
	int TileSetIndex;

	//cout << "height : " << al_get_bitmap_height(bitmap) << " width : " << al_get_bitmap_width(bitmap) << "\n";
	//cout << "CSV[0].size() = " << CSV[0].size() << " CSV.size() = " << CSV.size() << "\n";
	for (auto y = 0; y < al_get_bitmap_height(bitmap); y++)
	{
		for (auto x = 0; x < al_get_bitmap_width(bitmap); x++)
		{
			TileSetIndex = CSV[y][x];
			al_draw_scaled_bitmap(Tileset, (TileSetIndex % TILESET_WIDTH) * TILE_WIDTH, (TileSetIndex / TILESET_WIDTH) * TILE_HEIGHT, TILE_WIDTH, TILE_HEIGHT, scaleWidth * x * TILE_WIDTH, scaleHeight * y * TILE_HEIGHT, scaleWidth * TILE_WIDTH, scaleHeight * TILE_HEIGHT, 0);
		}
	}
}

void Scroll()
{
	/*check if you want to go out of boundary*/
	ALLEGRO_KEYBOARD_STATE KbState;

	al_get_keyboard_state(&KbState);
	while (al_key_down(&KbState, ALLEGRO_KEY_DOWN))
	{

		al_get_keyboard_state(&KbState);
	}
}

int main(int argc, char* argv[])
{
	ALLEGRO_DISPLAY* display = NULL;
	ALLEGRO_BITMAP* bitmap = NULL, * TileSet = NULL;
	vector<vector<int>> TileMapCSV, TileMapCSV_l2;

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

	TileSet = load_tileset("UnitTests\\overworld_tileset_grass.png");


	TileMapCSV = ReadTextMap("UnitTests\\map1_Kachelebene 1.csv");
	TileMapCSV_l2 = ReadTextMap("UnitTests\\map1_Tile Layer 2.csv");
	if (TileMapCSV.size() == 0)
	{
		fprintf(stderr, "ReadTextMap(string TileMapFileName) returned empty vector.\n");
		return -1;
	}

	bitmap = al_create_bitmap(DISPLAY_W, DISPLAY_H);
	Draw_BitMap_From_CSV(TileMapCSV, TileSet);
	Draw_BitMap_From_CSV(TileMapCSV_l2, TileSet);	/*layer 2 of map*/
	//Draw_Scaled_BitMap_From_CSV(TileMapCSV, TileSet, 1.5, 1.5);
	//al_set_target_bitmap(TileSet);
	//al_draw_bitmap(TileSet, 0, 0, 0); /*Note: The current target bitmap must be a different bitmap*/
	//al_clear_to_color(al_map_rgb(61, 61, 61));





	al_flip_display();
	/*Copies or updates the front and back buffers so that what has been drawn previously on the currently selected display becomes visible
	on screen. Pointers to the special back and front buffer bitmaps remain valid and retain their semantics as back and front buffers
	respectively, although their contents may have changed.*/
	al_rest(10.0);
	al_destroy_display(display);

	return 0;
}