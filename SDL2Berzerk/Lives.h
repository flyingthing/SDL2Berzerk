// Lives.h

#ifndef __LIVES__
#define __LIVES__
#include <SDL.h>
#include "SDL2Berzerk.h"
#include "AnimateObj.h"

class Lives : public AnimateObj
{
public:
	static const int LIVES_WIDTH = 20;  // 12;
	static const int LIVES_HEIGHT = 24; // 16;

	Lives(Texture *, SDL_Color);
	~Lives();

private:
};
#endif
