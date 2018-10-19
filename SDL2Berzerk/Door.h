// Door.h

#ifndef __DOOR__
#define __DOOR__
#include "Wall.h"

class Door : public Wall
{
public:

	Door(bool, MAZE_EXIT, int, int, int, int, SDL_Color, SDL_Renderer *);
	~Door();

	bool GetState() { return state; }
	MAZE_EXIT GetExit() { return exit; }

private:
	bool state;
	MAZE_EXIT exit;
};
#endif

