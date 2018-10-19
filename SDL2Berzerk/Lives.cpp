#include "Lives.h"

Lives::Lives(Texture *pTexture, SDL_Color color) : AnimateObj(OBJ_TYPE::LIFE, pTexture, color, LIVES_WIDTH, LIVES_HEIGHT, NULL)
{
	//Initialize the offsets
	//mPosX = 40;
	//mPosY = 40;

	addPattern("", { 0 });
	setPattern("");
}

Lives::~Lives()
{
}

