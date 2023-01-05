#pragma once

#include <vector>


//Link sprites vary so its better to go with 16 pixels
#define LINK_SPRITE_WIDTH 16
#define LINK_SPRITE_HEIGHT 16
#define LINK_JUMP_HEIGHT 32


typedef unsigned short Dim;
typedef unsigned short Index;
struct Rect { int x, y, w, h; };
struct Point { int x, y; };

enum Game_State {StartingScreen, PlayingLevel1, Paused};
enum Player_Direction {dir_left, dir_right};
enum Player_State {State_Walking, State_Crounching, State_Attacking, State_CrounchAttacking};

class GameLogic
{

};

class Player //player might be in layer 3 for drawing and compare with layer 1 for block collisions? enemies are a different story
{
private:
	Player_State state = State_Walking;
	int scrollDistanceX = 2, scrollDistanceY = 3;
public:
	Player_Direction direction = dir_right;
	int positionX, positionY;
	int screenX, screenY;	//measures screens/rooms
	unsigned int LinkSpriteNum = 0;
	boolean jumping = false;
	std::vector<Rect>FramesWalkingLeft, FramesWalkingRight;	//the bounding box for each frame, x and y will be the position in the sprite sheet to help find the sprite we want
	std::vector<Rect>FramesCrounch;
	std::vector<Rect>FramesSlashLeft, FramesSlashRight;
	std::vector<Rect>FramesCrounchSlash;

	Player(float _positionX, float _positionY)
	{
		positionX = _positionX;
		positionY = _positionY;
		screenX = 0;
		screenY = 0;
		
	}

	~Player()
	{

	}

	void Set_Speed_X(int speedX)
	{
		this->scrollDistanceX = speedX;
	}

	void Increment_Speed_X()
	{
		this->scrollDistanceX++;
	}

	int Get_Speed_X()
	{
		return this->scrollDistanceX;
	}

	void Set_Speed_Y(int speedY)
	{
		this->scrollDistanceY = speedY;
	}

	void Increment_Speed_Y()
	{
		this->scrollDistanceY++;
	}

	void Decrement_Speed_Y()
	{
		this->scrollDistanceY--;
	}

	int Get_Speed_Y()
	{
		return this->scrollDistanceY;
	}

	void Set_State(Player_State state)
	{
		if (this->state == State_Walking && state == State_Crounching)	//Walking -> Crounching
		{
			this->Scroll_Player(5, 5);	//scroll because the sprite is a bit shorter than then rest, scroll before changing the state because in crounch state you can't scroll
			this->state = state;
		}
		else if (this->state == State_Walking && state == State_Attacking) //Walking -> Attacking
		{
			this->state = state;
		}
		else if (this->state == State_Crounching && state == State_Walking) //Crounching -> Walking
		{
			this->state = State_Walking;	//to enable scrolling
			this->Scroll_Player(-5, -5);
		}
		else if (this->state == State_Crounching && state == State_Attacking)	//Crounching -> CrounchAttacking
		{
			this->state = State_CrounchAttacking;	
			//this->state = state;
		}
		else if ((this->state == State_CrounchAttacking && state == State_Crounching)){ //CrounchAttacking -> Crouching
			this->state = State_Crounching;
		}	
		else if (this->state == State_Attacking && state == State_Walking) //Attacking -> Walking
		{
			this->state = state;
		}
		
	}

	Player_State Get_State()
	{
		return this->state;
	}

	void Scroll_Player(float ScrollDistanceX, float ScrollDistanceY)
	{
		if (state == State_Walking)
		{
			this->positionX += ScrollDistanceX;
			this->positionY += ScrollDistanceY;
		}
	}

	void Init_frames_bounding_boxes()
	{
		Rect* r;
		int i = 0, k = 0;

		//FramesWalkingLeft
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
		std::reverse(FramesWalkingLeft.begin(), FramesWalkingLeft.end());

		//FramesWalkingRight
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

		//FramesCrounch (left)
		r = new Rect;
		r->h = LINK_SPRITE_HEIGHT + LINK_SPRITE_HEIGHT;
		r->w = LINK_SPRITE_WIDTH;
		r->y = LINK_SPRITE_HEIGHT * 2 + 10;	//10 pixels offset for the next row of sprites
		r->x = LINK_SPRITE_WIDTH * 3;
		FramesCrounch.push_back(*r);

		//FramesCrounch (right)
		r = new Rect;
		r->h = LINK_SPRITE_HEIGHT + LINK_SPRITE_HEIGHT;
		r->w = LINK_SPRITE_WIDTH;
		r->y = LINK_SPRITE_HEIGHT * 2 + 10;	//0 pixels offset for the next row of sprites
		r->x = LINK_SPRITE_WIDTH * 15 + LINK_SPRITE_WIDTH / 2;
		FramesCrounch.push_back(*r);

		//FramesSlashLeft
		r = new Rect;
		r->h = LINK_SPRITE_HEIGHT * 2;
		r->w = LINK_SPRITE_WIDTH * 2;
		r->y = LINK_SPRITE_HEIGHT * 2 + 10;	//10 pixels offset for the next row of sprites
		r->x = LINK_SPRITE_WIDTH * 7 + 10;
		FramesSlashLeft.push_back(*r);
		
		r = new Rect;
		r->h = LINK_SPRITE_HEIGHT * 2;
		r->w = LINK_SPRITE_WIDTH * 2;
		r->y = LINK_SPRITE_HEIGHT * 2 + 10;	//10 pixels offset for the next row of sprites
		r->x = LINK_SPRITE_WIDTH * 5;
		FramesSlashLeft.push_back(*r);

		//FramesSlashRight
		r = new Rect;
		r->h = LINK_SPRITE_HEIGHT * 2;
		r->w = LINK_SPRITE_WIDTH * 2;
		r->y = LINK_SPRITE_HEIGHT * 2 + 10;	//10 pixels offset for the next row of sprites
		r->x = LINK_SPRITE_WIDTH * 10;
		FramesSlashRight.push_back(*r);

		r = new Rect;
		r->h = LINK_SPRITE_HEIGHT * 2;
		r->w = LINK_SPRITE_WIDTH * 2;
		r->y = LINK_SPRITE_HEIGHT * 2 + 10;	//10 pixels offset for the next row of sprites
		r->x = LINK_SPRITE_WIDTH * 12 + 10;
		FramesSlashRight.push_back(*r);

		//FramesCrounchSlashLeft
		r = new Rect;
		r->h = LINK_SPRITE_HEIGHT * 2;
		r->w = LINK_SPRITE_WIDTH * 2;
		r->y = LINK_SPRITE_HEIGHT * 2 + 10;
		r->x = 0;
		FramesCrounchSlash.push_back(*r);

		//FramesCrounchSlashRight
		r = new Rect;
		r->h = LINK_SPRITE_HEIGHT * 2;
		r->w = LINK_SPRITE_WIDTH * 2;
		r->y = LINK_SPRITE_HEIGHT * 2 + 10;
		r->x = LINK_SPRITE_WIDTH * 18;
		FramesCrounchSlash.push_back(*r);

	}

	Rect FrameToDraw()
	{
		if (state == State_Walking && direction == dir_left)
		{
			return FramesWalkingLeft[LinkSpriteNum];
		}
		else if (state == State_Walking && direction == dir_right)
		{
			return FramesWalkingRight[LinkSpriteNum];
		}
		else if (state == State_Crounching)
		{
			return FramesCrounch[direction];
		}
		else if (state == State_Attacking && direction == dir_left)
		{
			return FramesSlashLeft[LinkSpriteNum];
		}
		else if (state == State_Attacking && direction == dir_right)
		{
			return FramesSlashRight[LinkSpriteNum];
		} 
		else if (state == State_CrounchAttacking)
		{
			return FramesCrounchSlash[direction];
		}
		else
		{
			fprintf(stderr, "Error with player direction : invalid value %d.\n", direction);
			exit(-1);
		}
	}
};

ALLEGRO_EVENT_QUEUE* EventQueue;
Game_State GameState;
Player *player = NULL;
bool User_input_done = false;
bool keyboardUp = false, scrollDown = true, scrollLeft = false, scrollRight = false;	//omit these later, maybe not left and right? useful for animation?
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
			case ALLEGRO_KEY_DOWN:
				player->Set_State(State_Crounching);
				break;
			case ALLEGRO_KEY_LEFT:
				player->direction = dir_left;
				scrollLeft = true;
				break;
			case ALLEGRO_KEY_RIGHT:
				player->direction = dir_right;
				scrollRight = true;
				break;
			case ALLEGRO_KEY_A:			// Jump
				keyboardUp = true;
				break;
			case ALLEGRO_KEY_B:			// Slash
				player->LinkSpriteNum = 0;
				player->Set_State(State_Attacking);
				break;
			case ALLEGRO_KEY_G:			// L_Ctrl + G combination
				ALLEGRO_KEYBOARD_STATE KbState;
				
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
			case ALLEGRO_KEY_DOWN:
				player->Set_State(State_Walking);
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
