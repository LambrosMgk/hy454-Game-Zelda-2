#pragma once

#define scrollDistanceX 5
#define scrollDistanceY 5


ALLEGRO_EVENT_QUEUE *EventQueue;
bool User_input_done = false;
bool scrollUp = false, scrollDown = false, scrollLeft = false, scrollRight = false;
bool Toggle_Grid = false;

enum Game_State {StartingScreen, PlayingLevel1, Paused};
Game_State GameState;
enum Player_Direction {left, right};

class GameLogic
{

};

class Player //player might be in layer 3 for drawing and compare with layer 1 for block collisions? enemies are a different story
{
public:
	Player_Direction direction;
	int positionX, positionY; // position based on the screen or the tilemap?????
};

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
					//Load_Level(1);
					break;
				}
			}
		}
		else if (event.type == ALLEGRO_EVENT_KEY_DOWN)	//maybe add a check that you are currently playing a level
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
				scrollLeft = true;
				break;
			case ALLEGRO_KEY_RIGHT:
				scrollRight = true;
				break;
			case ALLEGRO_KEY_A:			// Action key. e.g. slash with sword
				break;
			case ALLEGRO_KEY_LCTRL:
				ALLEGRO_KEYBOARD_STATE KbState;
				std::cout << "LCTRL\n";
				//try a loop through the event queue till empty or find G
				al_get_keyboard_state(&KbState);
				if (al_key_down(&KbState, ALLEGRO_KEY_G))
				{
					Toggle_Grid = Toggle_Grid ? false : true;
					std::cout << "Grid troggled\n";
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
