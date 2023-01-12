#include "..\\Headers\\GameObjectsClasses.h"

//Class Elevator functions

Elevator::Elevator(unsigned int _x, unsigned int _y) 
{
	this->x = _x;
	this->y = _y;

	this->elevatorbitmap = al_create_bitmap(TILE_HEIGHT, 2 * TILE_WIDTH); //create a bitmap for the bottom part of the elevator with dimensions (16,2x16)
	al_set_target_bitmap(elevatorbitmap);

	al_draw_bitmap_region(TileSet, MUL_TILE_WIDTH(modIndex[ELEVATORID1]), MUL_TILE_HEIGHT(divIndex[ELEVATORID1]), TILE_WIDTH, TILE_HEIGHT, MUL_TILE_WIDTH(0), MUL_TILE_HEIGHT(0), 0);
	al_draw_bitmap_region(TileSet, MUL_TILE_WIDTH(modIndex[ELEVATORID2]), MUL_TILE_HEIGHT(divIndex[ELEVATORID2]), TILE_WIDTH, TILE_HEIGHT, MUL_TILE_WIDTH(1), MUL_TILE_HEIGHT(0), 0);

	al_set_target_bitmap(al_get_backbuffer(display));
}

void Elevator::Paint_Elevator() 
{
	al_draw_bitmap_region(this->elevatorbitmap, 0, 0, TILE_WIDTH, TILE_HEIGHT, x, y, 0);
	al_draw_bitmap_region(this->elevatorbitmap, MUL_TILE_WIDTH(1), 0, TILE_WIDTH, TILE_HEIGHT, x, y, 0);
}

void Elevator::hide_og_elevator() 
{
	al_set_target_bitmap(bitmap);
	for (int i = y; i < y + TILE_HEIGHT; i++) {
		for (int j = x; j < x + 2 * TILE_WIDTH; j++) {
			ALLEGRO_COLOR color = al_get_pixel(bitmap, i, j);
			unsigned char r, g, b;

			al_unmap_rgb(color, &r, &g, &b);


			al_put_pixel(i, j, al_map_rgba(r, g, b, 255));

		}
	}
	al_set_target_bitmap(al_get_backbuffer(display));
}

void createElevators() {
	boolean isTopElev = true;

	for (int i = 0; i < TileMapCSV[1].size(); i++) {
		for (int j = 0; j < TileMapCSV[1][i].size(); i++) {

			if ((TileMapCSV[1][i][j] == ELEVATORID1))
				isTopElev = false;

			else if ((TileMapCSV[1][i][j] == ELEVATORID1) && !isTopElev) {

				elevators.push_back(new Elevator(MUL_TILE_WIDTH(i), MUL_TILE_HEIGHT(j)));
			}
		}
	}
}

//Class Player functions

Player::Player(float _positionX, float _positionY)
{
	positionX = _positionX;
	positionY = _positionY;
	screenX = 0;
	screenY = 0;

}

Player::~Player()
{

}

void Player::Set_Speed_X(int speedX)
{
	this->scrollDistanceX = speedX;
}
void Player::Increment_Speed_X()
{
	this->scrollDistanceX++;
}
int Player::Get_Speed_X()
{
	return this->scrollDistanceX;
}
void Player::Set_Speed_Y(int speedY)
{
	this->scrollDistanceY = speedY;
}
void Player::Increment_Speed_Y()
{
	this->scrollDistanceY++;
}
void Player::Decrement_Speed_Y()
{
	this->scrollDistanceY--;
}
int Player::Get_Speed_Y()
{
	return this->scrollDistanceY;
}

void Player::Set_State(Player_State state)
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
	else if ((this->state == State_CrounchAttacking && state == State_Crounching)) { //CrounchAttacking -> Crouching
		this->state = State_Crounching;
	}
	else if (this->state == State_Attacking && state == State_Walking) //Attacking -> Walking
	{
		this->state = state;
	}
}

Player_State Player::Get_State()
{
	return this->state;
}

void Player::Scroll_Player(float ScrollDistanceX, float ScrollDistanceY)
{
	if (state == State_Walking)
	{
		this->positionX += ScrollDistanceX;
		this->positionY += ScrollDistanceY;
	}
}

void Player::Init_frames_bounding_boxes()
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

Rect Player::FrameToDraw()
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

void Init_Player(int PlayerX, int PlayerY)
{
	player = new Player(PlayerX, PlayerY);
	player->Init_frames_bounding_boxes();
}