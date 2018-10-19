#include "Wall.h"

Wall::Wall(int x, int y, int w, int h, SDL_Color color, SDL_Renderer *gRenderer) : AnimateObj(OBJ_TYPE::MAZEWALL, NULL, color, w, h, NULL)
{
	this->gRenderer = gRenderer;

	setPos(x, y);
}

Wall::~Wall()
{
}


void Wall::render()
{
	SDL_Color color = getColor();

	SDL_SetRenderDrawColor(gRenderer, color.r, color.g, color.b, color.a);
	SDL_RenderFillRect(gRenderer, &mCollider);
}