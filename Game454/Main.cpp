#include <stdio.h>
#include <allegro5/allegro.h>

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>

using namespace std;
/*returns a vector<vector<int>> of indeces that corresponde to a TileSet*/
vector<vector<int>> ReadTextMap(string TileMapFileName)
{
	ifstream TileMap(TileMapFileName);
	vector<vector<int>> content;
	vector<int> row;
	string line, word;

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

	for (int i = 0; i < content.size(); i++)
	{
		for (int j = 0; j < content[i].size(); j++)
		{
			cout << content[i][j] << " ";
		}
		cout << "\n";
	}

	return content;
}

int main(int argc, char* argv[])
{
	ALLEGRO_DISPLAY* display = NULL;
	ALLEGRO_BITMAP* bitmap = NULL;
	vector<vector<int>> TileMap;

	if (!al_init())
	{
		fprintf(stderr, "failed to initialize allegro!\n");
		return -1;
	}

	display = al_create_display(640, 480);
	if (!display)
	{
		fprintf(stderr, "failed to create display!\n");
		return -1;
	}

	TileMap = ReadTextMap("UnitTests\\map1_Kachelebene 1.csv");
	if (TileMap.size() != 0)
	{
		bitmap = al_create_bitmap(640, 480);
		al_draw_bitmap(bitmap, 0, 0, 0);
		al_set_target_bitmap(bitmap);
	}
	//al_clear_to_color(al_map_rgb(61, 61, 61));
	al_flip_display();
	/*Copies or updates the front and back buffers so that what has been drawn previously on the currently selected display becomes visible 
	on screen. Pointers to the special back and front buffer bitmaps remain valid and retain their semantics as back and front buffers 
	respectively, although their contents may have changed.*/
	al_rest(10.0);
	al_destroy_display(display);

	return 0;
}