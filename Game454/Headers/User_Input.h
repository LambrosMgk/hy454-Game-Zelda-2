#pragma once

bool done = false;

void User_Input_init()
{
	if (!al_install_keyboard())
	{
		fprintf(stderr, "failed to install keyboard().\n");
		exit(-1);
	}
}

/*Checks for keyboard input and repaints the bitmap*/
void Scroll(ALLEGRO_BITMAP* bitmap, ALLEGRO_DISPLAY* display, float scrollDistance)
{
	assert(scrollDistance > 0);		/*scrollDistance <= 0 doesn't make sense*/
	ALLEGRO_KEYBOARD_STATE KbState;
	unsigned int TotalHeight = al_get_bitmap_height(bitmap), TotalWidth = al_get_bitmap_width(bitmap);

	al_get_keyboard_state(&KbState);
	if (al_key_down(&KbState, ALLEGRO_KEY_DOWN) && cameraY < TotalHeight - 200) /*check if you want to go out of boundary*/
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

bool isDone()
{
	return done;
}

/*for now just checks if "esc" is pressed*/
void UserInput(void)
{
	ALLEGRO_KEYBOARD_STATE KbState;

	al_get_keyboard_state(&KbState);
	if (al_key_down(&KbState, ALLEGRO_KEY_ESCAPE))
	{
		done = true;
	}
}
