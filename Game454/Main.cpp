#include <stdio.h>
#include <allegro5/allegro.h>

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>

using namespace std;

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

	ReadTextMap("UnitTests\\map1_Kachelebene 1.csv");
	al_clear_to_color(al_map_rgb(61, 61, 61));
	al_flip_display();
	al_rest(10.0);
	al_destroy_display(display);

	return 0;
}