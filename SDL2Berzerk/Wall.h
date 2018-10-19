// Wall.h

#ifndef __WALL__
#define __WALL__
#include <SDL.h>
#include "SDL2Berzerk.h"
#include "AnimateObj.h"

class Wall : public AnimateObj
{
public:

	Wall(int, int, int, int, SDL_Color, SDL_Renderer *);
	~Wall();

	void render();

private:
	SDL_Renderer *gRenderer = NULL;
};
#endif

