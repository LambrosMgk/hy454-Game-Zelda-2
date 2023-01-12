#pragma once

#include <stdio.h>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>
#include <set>
#include <vector>

#include "al_init.h"
#include "render.h"

#define START_SCREEN_PATH "UnitTests\\Media\\Start_Screen.png"
#define TILESET_PATH "UnitTests\\Media\\Level_1\\Zelda-II-Parapa-Palace-Tileset.png"
#define ASSUMED_EMPTY_LAYER_PATH "UnitTests\\Media\\Level_1\\Assumed_Empty_indices_Layer"
#define LINK_SPRITES_PATH "UnitTests\\Media\\link-sprites.png"

#define DISPLAY_W 640
#define DISPLAY_H 480

#define TILE_WIDTH 16
#define TILE_HEIGHT 16

#define StartPlayerPositionX 5*TILE_WIDTH
#define StartPlayerPositionY 11*TILE_HEIGHT-1

#define FPS 60.0
#define LAYERS 2

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

//Link sprites vary so its better to go with 16 pixels
#define LINK_SPRITE_WIDTH 16
#define LINK_SPRITE_HEIGHT 16
#define LINK_JUMP_HEIGHT 32

#define ELEVATORID1 12
#define ELEVATORID2 13


typedef unsigned short Dim;
typedef unsigned short Index;
struct Rect { int x, y, w, h; };
struct Point { int x, y; };

enum Game_State { StartingScreen, PlayingLevel1, Paused };
enum Player_Direction { dir_left, dir_right };
enum Player_State { State_Walking, State_Crounching, State_Attacking, State_CrounchAttacking, State_Elevator };
Player* player = NULL;

class GameLogic
{

};


class Elevator
{
private:
	unsigned int x, y;
	ALLEGRO_BITMAP* elevatorbitmap;
	
public:
	Elevator(unsigned int _x, unsigned int _y);
	void Paint_Elevator();
	void hide_og_elevator();
};

std::vector<Elevator> elevators;

void createElevators();

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

	Player(float _positionX, float _positionY);

	~Player();

	void Set_Speed_X(int speedX);

	void Increment_Speed_X();

	int Get_Speed_X();

	void Set_Speed_Y(int speedY);

	void Increment_Speed_Y();

	void Decrement_Speed_Y();

	int Get_Speed_Y();

	void Set_State(Player_State state);

	Player_State Get_State();

	void Scroll_Player(float ScrollDistanceX, float ScrollDistanceY);

	void Init_frames_bounding_boxes();

	Rect FrameToDraw();
};

void Init_Player(int PlayerX, int PlayerY);
