#pragma once

#include <vector>


//Link sprites vary so its better to go with 16 pixels
#define LINK_SPRITE_WIDTH 16
#define LINK_SPRITE_HEIGHT 16

#define scrollDistanceX 2
#define scrollDistanceY 2

typedef unsigned short Dim;
typedef unsigned short Index;
struct Rect { int x, y, w, h; };
struct Point { int x, y; };

enum Game_State {StartingScreen, PlayingLevel1, Paused};
enum Player_Direction {dir_left, dir_right};

class GameLogic
{

};

class Player //player might be in layer 3 for drawing and compare with layer 1 for block collisions? enemies are a different story
{
public:
	Player_Direction direction = dir_left;
	int positionX, positionY;
	int screenX, screenY;	//measures screens/rooms
	unsigned int LinkSpriteNum = 0;
	std::vector<Rect>FramesWalkingLeft, FramesWalkingRight;	//the bounding box for each frame, x and y will be the position in the sprite sheet to help find the sprite we want

	Player(int _positionX, int _positionY)
	{
		positionX = _positionX;
		positionY = _positionY;
		screenX = 0;
		screenY = 0;
		
	}

	~Player()
	{

	}

	void Scroll_Player(float ScrollDistanceX, float ScrollDistanceY)
	{
		this->positionX += ScrollDistanceX;
		this->positionY += ScrollDistanceY;
	}

	void Init_frames_bounding_boxes()
	{
		Rect* r;
		int i = 0, k = 0;

		for (i = 0, k = 0; i < 4; i++)
		{
			r = new Rect;
			r->h = LINK_SPRITE_HEIGHT * 2;
			r->w = i % 2 == 1 ? LINK_SPRITE_WIDTH : LINK_SPRITE_WIDTH * 2;
			k = i % 2 == 1 ? k + i * 16 + 32 : k + i * 16;
			r->y = 0;
			if (i == 3)
				k -= 32;
			r->x = k;
			FramesWalkingLeft.push_back(*r);
		}

		for (i = 0, k = 160; i < 4; i++)
		{
			r = new Rect;
			r->h = LINK_SPRITE_HEIGHT * 2;
			r->w = i % 2 == 1 ? LINK_SPRITE_WIDTH : LINK_SPRITE_WIDTH * 2;
			k = i % 2 == 1 ? k + i * 16 + 32 : k + i * 16;
			r->y = 0;
			if (i == 3)
				k -= 32;
			r->x = k;
			FramesWalkingRight.push_back(*r);
		}
	}
};

ALLEGRO_EVENT_QUEUE* EventQueue;
Game_State GameState;
Player *player = NULL;
bool User_input_done = false;
bool scrollUp = false, scrollDown = false, scrollLeft = false, scrollRight = false;	//omit these later, maybe not left and right? useful for animation?
int cameraX = 0, cameraY = 0;
bool Toggle_Grid = false;

void Init_Player(int PlayerX, int PlayerY)
{
	player = new Player(PlayerX, PlayerY);
	player->Init_frames_bounding_boxes();
}

void User_Input_init()
{
	EventQueue = al_create_event_queue();
	al_register_event_source(EventQueue, al_get_keyboard_event_source());
	GameState = StartingScreen;
}

bool isDone()
{
	return User_input_done;
}

/*Handles all user input*/
void UserInput(void)
{
	while (!al_is_event_queue_empty(EventQueue)) //Check all the user input
	{
		ALLEGRO_EVENT event;
		if (!al_get_next_event(EventQueue, &event))
		{
			std::cout << "Error : EventQueue empty when !al_is_event_queue_empty() returned false\n";
			exit(-1);
		}

		if (GameState == StartingScreen)
		{
			if (event.type == ALLEGRO_EVENT_KEY_DOWN)
			{
				switch (event.keyboard.keycode)
				{
				case ALLEGRO_KEY_ENTER:			//Enter == continue, load first level
					GameState = PlayingLevel1;
					break;
				}
			}
		}
		else if (event.type == ALLEGRO_EVENT_KEY_DOWN)
		{
			switch (event.keyboard.keycode)
			{
			case ALLEGRO_KEY_ESCAPE:
				User_input_done = true;		//ends the game for now
				break;
			case ALLEGRO_KEY_UP:
				scrollUp = true;
				break;
			case ALLEGRO_KEY_DOWN:
				scrollDown = true;
				break;
			case ALLEGRO_KEY_LEFT:
				player->direction = dir_left;
				scrollLeft = true;
				break;
			case ALLEGRO_KEY_RIGHT:
				player->direction = dir_right;
				scrollRight = true;
				break;
			case ALLEGRO_KEY_A:			// Action key. e.g. slash with sword
				break;
			case ALLEGRO_KEY_G:			// L_Ctrl + G combination
				ALLEGRO_KEYBOARD_STATE KbState;
				
				//try a loop through the event queue till empty or find G
				al_get_keyboard_state(&KbState);
				if (al_key_down(&KbState, ALLEGRO_KEY_LCTRL))
				{
					Toggle_Grid = Toggle_Grid ? false : true;
					std::cout << "Grid toggled\n";
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
		else
		{
			//isws piasei san constant char event alla meta isws einai polu grhgoro to scroll
			//cout << "Event type : " << event.type << "\n";
		}
	}
}
