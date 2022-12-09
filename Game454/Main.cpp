#include <stdio.h>


#include "Headers\Render.h"
#include "Headers\User_Input.h"

int main(int argc, char* argv[])
{
	ALLEGRO_DISPLAY* display = NULL;
	ALLEGRO_BITMAP* bitmap = NULL, * TileSet = NULL;
	vector<vector<int>> TileMapCSV, TileMapCSV_l2;
	


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


	while (!isDone())
	{
		Sleep(25);	/*need to change this later for smoother scrolling*/
		UserInput();
		Scroll(bitmap, display, 5);
	}

	
	al_rest(1.0);
	al_destroy_display(display);

	return 0;
}