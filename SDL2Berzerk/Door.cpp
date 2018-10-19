#include "Door.h"

Door::Door(bool state, MAZE_EXIT exit, int x, int y, int w, int h, SDL_Color color, SDL_Renderer *gRenderer) : Wall(x, y, w, h, color, gRenderer)
{
	obj_type = OBJ_TYPE::MAZEDOOR;
	this->state = state;
	this->exit = exit;
}

Door::~Door()
{
}